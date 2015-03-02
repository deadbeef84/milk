#include "milk/renderer/ivertexbufferinterleaved.h"
#include "milk/renderer/cmaterial.h"
#include "milk/renderer.h"
#include "milk/error.h"
#include "milk/helper.h"
#include "milk/includes.h"
#include "milk/glhelper.h"

using namespace milk;

/*
	These are the different types of interleaved arrays	

	GL_V2F,
	GL_V3F,
	GL_C4UB_V2F,
	GL_C4UB_V3F,
	GL_C3F_V3F,
	GL_N3F_V3F,
	GL_C4F_N3F_V3F,
	GL_T2F_V3F,
	GL_T4F_V4F,
	GL_T2F_C4UB_V3F,
	GL_T2F_C3F_V3F,
	GL_T2F_N3F_V3F,
	GL_T2F_C4F_N3F_V3F, 
	GL_T4F_C4F_N3F_V4F.
*/

IVertexBufferInterleaved* IVertexBufferInterleaved::create(CVertexFormat format, uint size, BufferUsage usage)
{
	CHECK_FOR_OPENGL_ERRORS("There were errors prior to this function.");

	if(GLEW_ARB_vertex_buffer_object)
	{
		try
		{
			IVertexBufferInterleaved *pVertexBufferInterleaved = new CVertexBufferInterleaved_VBO(format, size, usage);
			Renderer::checkForErrors();
			return pVertexBufferInterleaved;
		}
		catch(error::opengl&)
		{ }
	}

	try
	{
		IVertexBufferInterleaved *pVertexBufferInterleaved = new CVertexBufferInterleaved_GL(format, size, usage);
		Renderer::checkForErrors();
		return pVertexBufferInterleaved;
	}
	catch(error::opengl&)
	{ }

	return 0;
}

IVertexBufferInterleaved::IVertexBufferInterleaved(CVertexFormat format, uint size, BufferUsage usage)
: IVertexBuffer(format, size, usage), m_pVertices(0)
{
	// Save interleaved format
	m_interleavedFormat = m_format.interleavedFormat();

	// Calculate offsets
	m_vOfs = m_format.getTexCoordSize() + m_format.m_c.size() + m_format.m_n.size();
	m_nOfs = m_format.getTexCoordSize() + m_format.m_c.size();
	m_cOfs = m_format.getTexCoordSize();

	for(int i = 0; i < 8; ++i)
		m_tOfs[i] = m_format.getTexCoordSize(i);
}

void IVertexBufferInterleaved::draw(GLenum mode, uint start, uint num)
{
	Renderer::setStatistics().triangles += (mode==GL_TRIANGLE_STRIP) ? num-2 : num/3;
	Renderer::setStatistics().calls++;

	BOOST_ASSERT(m_numVertices > 0);
	BOOST_ASSERT((start+num) <= m_numVertices);
	privBufferBeginDraw();
	glDrawArrays(mode, start, num);
	privBufferEndDraw();
}

void IVertexBufferInterleaved::beginMultipleDraw()
{
	privBufferBeginDraw();
}

void IVertexBufferInterleaved::endMultipleDraw()
{
	privBufferEndDraw();
}

void IVertexBufferInterleaved::multipleDraw(GLenum mode, uint start, uint num)
{
	Renderer::setStatistics().triangles += (mode==GL_TRIANGLE_STRIP) ? num-2 : num/3;
	Renderer::setStatistics().calls++;

	// Draw primitives
	glDrawArrays(mode, start, num);
}

void* IVertexBufferInterleaved::getComponentPtr(VertexComponent vc, size_t subComponent, size_t& stride)
{
	stride = m_format.size();
	switch(vc)
	{
	case POSITION: return reinterpret_cast<void*>(&m_pVertices[m_vOfs]);
	case NORMAL: return reinterpret_cast<void*>(&m_pVertices[m_nOfs]);
	case COLOR: return reinterpret_cast<void*>(&m_pVertices[m_cOfs]);
	case TEXCOORD: return reinterpret_cast<void*>(&m_pVertices[m_tOfs[subComponent]]);
	//case MATRIXINDEX: return reinterpret_cast<void*>(m_pVertices+m_iOfs);
	//case WEIGHT: return reinterpret_cast<void*>(m_pVertices+m_wOfs);
	//case SHADERATTRIBUTE: return reinterpret_cast<void*>(m_pVertices+m_aOfs[subComponent]);
	default: return 0;
	}
}

// Regular GL Buffer ////////////////////////////////////////////

CVertexBufferInterleaved_GL::CVertexBufferInterleaved_GL(CVertexFormat format, uint size, BufferUsage usage)
: IVertexBufferInterleaved(format, size, usage)
{
	// Allocate vertices
	m_pVertices = new uchar[m_format.size() * m_numVertices];
}

CVertexBufferInterleaved_GL::~CVertexBufferInterleaved_GL()
{
	safeDelete(m_pVertices);
}

void CVertexBufferInterleaved_GL::lock(BufferAccess)
{
	if(GLEW_EXT_compiled_vertex_array)
	{
		privBufferBeginDraw();
		glUnlockArraysEXT();
	}
}

void CVertexBufferInterleaved_GL::unlock()
{
	if(GLEW_EXT_compiled_vertex_array)
	{
		privBufferBeginDraw();
		glLockArraysEXT(0, m_numVertices);
	}
}

void CVertexBufferInterleaved_GL::privBufferBeginDraw()
{
	BOOST_ASSERT(m_pVertices);
	glInterleavedArrays(m_interleavedFormat, static_cast<GLsizei>(m_format.size()), m_pVertices);

	// set default color if needed
	if(Material::ms_vertexColor && !m_format.m_c.size())
		glColor4fv(m_defaultColor);
}


// Vertex Buffer Object ///////////////////////////////////////////

CVertexBufferInterleaved_VBO::CVertexBufferInterleaved_VBO(CVertexFormat format, uint size, BufferUsage usage)
: IVertexBufferInterleaved(format, size, usage), m_id(0)
{
	glGenBuffersARB(1, &m_id);
	BOOST_ASSERT(m_id);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_id);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, static_cast<GLsizei>(m_format.size() * m_numVertices), 0, m_usage == STATIC ? GL_STATIC_DRAW_ARB : GL_DYNAMIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

CVertexBufferInterleaved_VBO::~CVertexBufferInterleaved_VBO()
{
	BOOST_ASSERT(m_pVertices == 0);
	if(m_id)
		glDeleteBuffersARB(1, &m_id);
	m_id = 0;
}

void CVertexBufferInterleaved_VBO::lock(BufferAccess access)
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

void CVertexBufferInterleaved_VBO::unlock()
{
	BOOST_ASSERT(m_id && m_pVertices);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_id);
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	m_pVertices = 0;
}

void CVertexBufferInterleaved_VBO::privBufferBeginDraw()
{
	BOOST_ASSERT(m_id);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_id);
	glInterleavedArrays(m_interleavedFormat, static_cast<GLsizei>(m_format.size()), 0);

	// set default color if needed
	if(Material::ms_vertexColor && !m_format.m_c.size())
		glColor4fv(m_defaultColor);
}


void CVertexBufferInterleaved_VBO::privBufferEndDraw()
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}
