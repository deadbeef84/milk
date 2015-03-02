#include "milk/renderer/ivertexbuffernormal.h"
#include "milk/renderer/ishader.h"
#include "milk/renderer/cmaterial.h"
#include "milk/renderer.h"
#include "milk/error.h"
#include "milk/helper.h"
#include "milk/glhelper.h"
#include "milk/includes.h"
#include "milk/boost.h"

using namespace milk;

IVertexBufferNormal* IVertexBufferNormal::create(CVertexFormat format, uint size, BufferUsage usage)
{
	CHECK_FOR_OPENGL_ERRORS("There were errors prior to this function.");

	if(GLEW_ARB_vertex_buffer_object)
	{
		try
		{
			IVertexBufferNormal *pVertexBufferNormal = new CVertexBufferNormal_VBO(format, size, usage);
			Renderer::checkForErrors();
			return pVertexBufferNormal;
		}
		catch(error::opengl&)
		{ }
	}

	try
	{
		IVertexBufferNormal *pVertexBufferNormal = new CVertexBufferNormal_GL(format, size, usage);
		Renderer::checkForErrors();
		return pVertexBufferNormal;
	}
	catch(error::opengl&)
	{ }

	return 0;
}

IVertexBufferNormal::IVertexBufferNormal(CVertexFormat format, uint size, BufferUsage usage)
: IVertexBuffer(format, size, usage),
  m_pVertices(0),
  m_nOfs(0), m_cOfs(0)
{
	// Calculate offsets
	m_nOfs = m_numVertices * m_format.m_v.size();
	m_cOfs = m_nOfs + m_numVertices * m_format.m_n.size();
	m_tOfs[0] = m_cOfs + m_numVertices * m_format.m_c.size();
	for(int i = 1; i < 8; ++i)
		m_tOfs[i] = m_tOfs[i-1] + m_numVertices * m_format.m_t[i-1].size();
	m_iOfs = m_tOfs[7] + m_numVertices * m_format.m_t[7].size();
	m_wOfs = m_iOfs + m_numVertices * m_format.m_i.size();

	size_t prevOfs = m_wOfs;
	GLint prevSize = m_format.m_i.size();
	m_aOfs.resize(m_format.m_a.size());
	attribOffsetList::iterator ao = m_aOfs.begin();
	for(CVertexFormat::attribList::const_iterator it = m_format.m_a.begin(); it != m_format.m_a.end(); ++it, ++ao)
	{
		*ao = prevOfs + m_numVertices * prevSize;
		prevOfs = *ao;
		prevSize = (GLint)it->size();
	}
}

void IVertexBufferNormal::draw(GLenum mode, uint start, uint num)
{
	privBufferBeginDraw();

	Renderer::setStatistics().triangles += (mode==GL_TRIANGLE_STRIP) ? num-2 : num/3;
	Renderer::setStatistics().calls++;

	// Draw primitives
	glDrawArrays(mode, start, num);

	privBufferEndDraw();
}

void IVertexBufferNormal::beginMultipleDraw()
{
	privBufferBeginDraw();
}

void IVertexBufferNormal::endMultipleDraw()
{
	privBufferEndDraw();
}

void IVertexBufferNormal::multipleDraw(GLenum mode, uint start, uint num)
{
	Renderer::setStatistics().triangles += (mode==GL_TRIANGLE_STRIP) ? num-2 : num/3;
	Renderer::setStatistics().calls++;

	// Draw primitives
	glDrawArrays(mode, start, num);
}

void IVertexBufferNormal::privBufferBeginDraw()
{
	// Set vertex pointer
	if(m_format.m_v.size())
	{
		glVertexPointer(m_format.m_v.components(), m_format.m_v.type(), 0, m_pVertices);
		glEnableClientState(GL_VERTEX_ARRAY);
	}
	else
		throw error::vertexbuffer_format("Vertex-size cannot be 0.");

	// Set normal pointer
	if(m_format.m_n.size())
	{
		glNormalPointer(m_format.m_n.type(), 0, m_pVertices+m_nOfs);
		glEnableClientState(GL_NORMAL_ARRAY);
	}
	else
		glDisableClientState(GL_NORMAL_ARRAY);

	// Set texturecoord pointers
	for(int i = 0; i < 8; ++i)
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
		if(m_format.m_t[i].size())
		{
			glTexCoordPointer(m_format.m_t[i].components(), m_format.m_t[i].type(), 0, m_pVertices+m_tOfs[i]);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	// Set color pointer
	if(m_format.m_c.size())
	{
		glColorPointer(m_format.m_c.components(), m_format.m_c.type(), 0, m_pVertices+m_cOfs);
		glEnableClientState(GL_COLOR_ARRAY);
	}
	else
	{
		glDisableClientState(GL_COLOR_ARRAY);
		// set default color
		if(Material::ms_vertexColor)
			glColor4fv(m_defaultColor);
	}

	attribOffsetList::const_iterator ao = m_aOfs.begin();
	for(CVertexFormat::attribList::const_iterator it = m_format.m_a.begin(); it != m_format.m_a.end(); ++it, ++ao)
	{
		it->vertexPointer(0, m_pVertices + *ao);
		it->enableVertexArray();
	}

	if(GLEW_ARB_matrix_palette)
	{
		// Set index pointer
		if(m_format.m_i.size())
		{
			glEnable(GL_MATRIX_PALETTE_ARB);
			glMatrixIndexPointerARB(m_format.m_i.components(), m_format.m_i.type(), 0, m_pVertices+m_iOfs);
			glEnableClientState(GL_MATRIX_INDEX_ARRAY_ARB);
		}
		else
			glDisableClientState(GL_MATRIX_INDEX_ARRAY_ARB);
	}

	if(GLEW_ARB_vertex_blend)
	{
		// Set weight pointer
		if(m_format.m_w.size())
		{
			glEnable(GL_VERTEX_BLEND_ARB);
			glVertexBlendARB(m_format.m_i.components());
			glWeightPointerARB(m_format.m_w.components(), m_format.m_w.type(), 0, m_pVertices+m_wOfs);
			glEnableClientState(GL_WEIGHT_ARRAY_ARB);
		}
		else
			glDisableClientState(GL_WEIGHT_ARRAY_ARB);
	}
}

void IVertexBufferNormal::privBufferEndDraw()
{
	if(m_format.m_i.size() && GLEW_ARB_matrix_palette)
		glDisable(GL_MATRIX_PALETTE_ARB);
	if(m_format.m_w.size() && GLEW_ARB_vertex_blend)
		glDisable(GL_VERTEX_BLEND_ARB);

	for(CVertexFormat::attribList::const_iterator it = m_format.m_a.begin(); it != m_format.m_a.end(); ++it)
		it->disableVertexArray();
}

void* IVertexBufferNormal::getComponentPtr(VertexComponent vc, size_t subComponent, size_t& stride)
{
	switch(vc)
	{
	case POSITION: stride = m_format.m_v.size(); return reinterpret_cast<void*>(m_pVertices);
	case NORMAL: stride = m_format.m_n.size(); return reinterpret_cast<void*>(m_pVertices+m_nOfs);
	case COLOR: stride = m_format.m_c.size(); return reinterpret_cast<void*>(m_pVertices+m_cOfs);
	case TEXCOORD: stride = m_format.m_t[subComponent].size(); return reinterpret_cast<void*>(m_pVertices+m_tOfs[subComponent]);
	case MATRIXINDEX: stride = m_format.m_i.size(); return reinterpret_cast<void*>(m_pVertices+m_iOfs);
	case WEIGHT: stride = m_format.m_w.size(); return reinterpret_cast<void*>(m_pVertices+m_wOfs);
	case SHADERATTRIBUTE: stride = m_format.m_a[subComponent].size(); return reinterpret_cast<void*>(m_pVertices+m_aOfs[subComponent]);
	default: return 0;
	}
}

// Regular GL Buffer ////////////////////////////////////////////

CVertexBufferNormal_GL::CVertexBufferNormal_GL(CVertexFormat format, uint size, BufferUsage usage)
: IVertexBufferNormal(format, size, usage)
{
	// Allocate vertices
	m_pVertices = new uchar[m_format.size() * m_numVertices];
}

CVertexBufferNormal_GL::~CVertexBufferNormal_GL()
{
	delete [] m_pVertices;
	m_pVertices = 0;
}

// Vertex Buffer Object ///////////////////////////////////////////

CVertexBufferNormal_VBO::CVertexBufferNormal_VBO(CVertexFormat format, uint size, BufferUsage usage)
: IVertexBufferNormal(format, size, usage), m_id(0)
{
	glGenBuffersARB(1, &m_id);
	BOOST_ASSERT(m_id);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_id);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, static_cast<GLsizei>(m_format.size() * m_numVertices), 0, m_usage == STATIC ? GL_STATIC_DRAW_ARB : GL_DYNAMIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

CVertexBufferNormal_VBO::~CVertexBufferNormal_VBO()
{
	if(m_id)
		glDeleteBuffersARB(1, &m_id);
	m_id = 0;
}

void CVertexBufferNormal_VBO::lock(BufferAccess access)
{
	BOOST_ASSERT(m_id && !m_pVertices);
	GLenum accessgl = GL_READ_ONLY_ARB;
	if((access & READ) && (access & WRITE))
		accessgl = GL_READ_WRITE_ARB;
	else if(access & WRITE)
		accessgl = GL_WRITE_ONLY_ARB;
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_id);
	m_pVertices = reinterpret_cast<uchar*>(glMapBufferARB(GL_ARRAY_BUFFER_ARB, accessgl));
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void CVertexBufferNormal_VBO::unlock()
{
	BOOST_ASSERT(m_id && m_pVertices);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_id);
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	m_pVertices = 0;
}

void CVertexBufferNormal_VBO::privBufferBeginDraw()
{
	BOOST_ASSERT(m_id);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_id);
	IVertexBufferNormal::privBufferBeginDraw();
}

void CVertexBufferNormal_VBO::privBufferEndDraw()
{
	IVertexBufferNormal::privBufferEndDraw();
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}
