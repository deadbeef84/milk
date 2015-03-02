#include "milk/renderer/iindexbuffer.h"
#include "milk/renderer.h"
#include "milk/error.h"
#include "milk/helper.h"
#include "milk/includes.h"
#include "milk/io.h"

using namespace milk;

// IndexBuffer ///////////////////////////////////

template<class T>
static size_t degen(const T *pIndices, uint size)
{
	uint count = 0;
	uint csize = 3;
	pIndices += 3;
	while(csize < size)
	{
		if(*(pIndices - 2) == *(pIndices - 1) ||
		   *(pIndices - 2) == *pIndices ||
		   *(pIndices - 1) == *pIndices)
			++count;
		++pIndices;
		++csize;
	}
	return count;
}

template<class T>
static void indexrange(const T *pIndices, uint size, uint &min, uint &max)
{
	min = *std::min_element(pIndices, pIndices+size);
	max = *std::max_element(pIndices, pIndices+size);
}

IIndexBuffer* IIndexBuffer::create(std::istream& is)
{
	GLenum format = io::readpod<ulong>(is);
	uint numIndices = io::readpod<ulong>(is);
	BufferUsage usage = BufferUsage(io::readpod<ulong>(is));
	bool optimized = io::readpod<bool>(is);
	uint firstVertex = io::readpod<ulong>(is);
	uint lastVertex = io::readpod<ulong>(is);

	IIndexBuffer *pIndexBuffer = IIndexBuffer::create(format, numIndices, usage);

	pIndexBuffer->lock(WRITE);
	is.read((char*)pIndexBuffer->m_pIndices, numIndices * pIndexBuffer->sizeOfFormat());
	pIndexBuffer->unlock();

	if(optimized)
		pIndexBuffer->optimize(firstVertex, lastVertex);

	return pIndexBuffer;
}

IIndexBuffer* IIndexBuffer::create(GLenum format, uint size, BufferUsage usage)
{
	IIndexBuffer* ib = 0;
	/*if(GLEW_ARB_vertex_buffer_object)
		ib = new CIndexBuffer_VBO(format, size, usage);
	else*/
		ib = new CIndexBuffer_GL(format, size, usage);
	return ib;
}

void IIndexBuffer::write(std::ostream& os, bool smart)
{
	io::writepod(os, ulong(m_format));
	io::writepod(os, ulong(m_numIndices));
	io::writepod(os, ulong(m_usage));
	io::writepod(os, m_optimized);
	io::writepod(os, ulong(m_firstVertex));
	io::writepod(os, ulong(m_lastVertex));

	lock(READ);
	os.write((char*)m_pIndices, m_numIndices*sizeOfFormat());
	unlock();
}

IIndexBuffer::IIndexBuffer(GLenum format, uint size, BufferUsage usage)
: m_format(format), m_pIndices(0), m_numIndices(size), m_usage(usage), m_optimized(false), m_firstVertex(0), m_lastVertex(0)
{
}

void IIndexBuffer::optimize()
{
	if(!m_numIndices)
		return;

	lock(READ);
	if(m_format == GL_UNSIGNED_INT)
		indexrange(getIndicesui(), m_numIndices, m_firstVertex, m_lastVertex);
	else if(m_format == GL_UNSIGNED_SHORT)
		indexrange(getIndicesus(), m_numIndices, m_firstVertex, m_lastVertex);
	else if(m_format == GL_UNSIGNED_BYTE)
		indexrange(getIndicesub(), m_numIndices, m_firstVertex, m_lastVertex);
	m_optimized = true;
	unlock();
}

void IIndexBuffer::optimize(uint start, uint end)
{
	m_firstVertex = start;
	m_lastVertex = end;
	m_optimized = true;
}

GLvoid* IIndexBuffer::getIndexPtr(uint index)
{
	if(m_format == GL_UNSIGNED_INT)
		return reinterpret_cast<void*>(&getIndicesui()[index]);
	else if(m_format == GL_UNSIGNED_SHORT)
		return reinterpret_cast<void*>(&getIndicesus()[index]);
	else if(m_format == GL_UNSIGNED_BYTE)
		return reinterpret_cast<void*>(&getIndicesub()[index]);
	return 0;
}

size_t IIndexBuffer::numDegenerateTriangles()
{
	size_t retval = 0;
	lock(READ);
	if(m_format == GL_UNSIGNED_INT)
		retval = degen(getIndicesui(), m_numIndices);
	else if(m_format == GL_UNSIGNED_SHORT)
		retval = degen(getIndicesus(), m_numIndices);
	else if(m_format == GL_UNSIGNED_BYTE)
		retval = degen(getIndicesub(), m_numIndices);
	unlock();
	return retval;
}

size_t IIndexBuffer::sizeOfFormat() const
{
	if(m_format == GL_UNSIGNED_INT)
		return sizeof(uint);
	else if(m_format == GL_UNSIGNED_SHORT)
		return sizeof(ushort);
	else if(m_format == GL_UNSIGNED_BYTE)
		return sizeof(uchar);
	return 0;
}

// GL Index Buffer ///////////////////////////////

CIndexBuffer_GL::CIndexBuffer_GL(GLenum format, uint size, BufferUsage usage)
: IIndexBuffer(format, size, usage)
{
	if(m_format == GL_UNSIGNED_INT)
		m_pIndices = new uint[m_numIndices];
	else if(m_format == GL_UNSIGNED_SHORT)
		m_pIndices = new ushort[m_numIndices];
	else if(m_format == GL_UNSIGNED_BYTE)
		m_pIndices = new uchar[m_numIndices];
}

CIndexBuffer_GL::~CIndexBuffer_GL()
{
	delete [] m_pIndices;
	m_pIndices = 0;
}

void CIndexBuffer_GL::draw(IVertexBuffer* vb, GLenum mode, uint start, uint num)
{
	Renderer::setStatistics().triangles += (mode==GL_TRIANGLE_STRIP) ? num : num/3;
	Renderer::setStatistics().calls++;

	vb->privBufferBeginDraw();
	//glLockArraysEXT(0, m_numIndices);
	if(m_optimized && GLEW_EXT_draw_range_elements)
		glDrawRangeElements(mode, m_firstVertex, m_lastVertex, num, m_format, getIndexPtr(start));
	else
		glDrawElements(mode, num, m_format, getIndexPtr(start));
	//glUnlockArraysEXT();
	vb->privBufferEndDraw();
}

void CIndexBuffer_GL::beginMultipleDraw(IVertexBuffer* vb)
{
	vb->privBufferBeginDraw();
}

void CIndexBuffer_GL::endMultipleDraw(IVertexBuffer* vb)
{
	vb->privBufferEndDraw();
}

void CIndexBuffer_GL::multipleDraw(GLenum mode, uint start, uint num)
{
	Renderer::setStatistics().triangles += (mode==GL_TRIANGLE_STRIP) ? num : num/3;
	Renderer::setStatistics().calls++;

	if(m_optimized && GLEW_EXT_draw_range_elements)
		glDrawRangeElements(mode, m_firstVertex, m_lastVertex, num, m_format, getIndexPtr(start));
	else
		glDrawElements(mode, num, m_format, getIndexPtr(start));
}

// VBO Index Buffer ///////////////////////////////

CIndexBuffer_VBO::CIndexBuffer_VBO(GLenum format, uint size, BufferUsage usage)
: IIndexBuffer(format, size, usage), m_id(0)
{
	glGenBuffersARB(1, &m_id);
	BOOST_ASSERT(m_id);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_id);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeOfFormat() * m_numIndices, 0, m_usage == STATIC ? GL_STATIC_DRAW_ARB : GL_DYNAMIC_DRAW_ARB);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

CIndexBuffer_VBO::~CIndexBuffer_VBO()
{
	if(m_id)
		glDeleteBuffersARB(1, &m_id);
	m_id = 0;
}

void CIndexBuffer_VBO::lock(BufferAccess access)
{
	BOOST_ASSERT(m_id && !m_pIndices);
	GLenum accessgl = GL_READ_ONLY_ARB;
	if((access & READ) && (access & WRITE))
		accessgl = GL_READ_WRITE_ARB;
	else if(access & WRITE)
		accessgl = GL_WRITE_ONLY_ARB;
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_id);
	m_pIndices = reinterpret_cast<uint*>(glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, accessgl));
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void CIndexBuffer_VBO::unlock()
{
	BOOST_ASSERT(m_id && m_pIndices);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_id);
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	m_pIndices = 0;
}

void CIndexBuffer_VBO::draw(IVertexBuffer* vb, GLenum mode, uint start, uint num)
{
	Renderer::setStatistics().triangles += (mode==GL_TRIANGLE_STRIP) ? num : num/3;
	Renderer::setStatistics().calls++;

	BOOST_ASSERT(m_id);
	BOOST_ASSERT(m_numIndices > 0);
	BOOST_ASSERT(vb);
	BOOST_ASSERT(m_pIndices == 0);
	vb->privBufferBeginDraw();
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_id);
	if(m_optimized && GLEW_EXT_draw_range_elements)
		glDrawRangeElements(mode, m_firstVertex, m_lastVertex, num, m_format, (char*)0 + start);
	else
		glDrawElements(mode, num, m_format, (char*)0 + start); // fixme... 0+start even with integer data?
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	vb->privBufferEndDraw();
}

void CIndexBuffer_VBO::beginMultipleDraw(IVertexBuffer* vb)
{
	BOOST_ASSERT(m_id);
	BOOST_ASSERT(m_numIndices > 0);
	BOOST_ASSERT(vb);
	vb->privBufferBeginDraw();
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_id);
}

void CIndexBuffer_VBO::endMultipleDraw(IVertexBuffer* vb)
{
	BOOST_ASSERT(vb);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	vb->privBufferEndDraw();
}

void CIndexBuffer_VBO::multipleDraw(GLenum mode, uint start, uint num)
{
	Renderer::setStatistics().triangles += (mode==GL_TRIANGLE_STRIP) ? num : num/3;
	Renderer::setStatistics().calls++;

	if(m_optimized && GLEW_EXT_draw_range_elements)
		glDrawRangeElements(mode, m_firstVertex, m_lastVertex, num, m_format, (char*)0 + start);
	else
		glDrawElements(mode, num, m_format, (char*)0 + start);
}
