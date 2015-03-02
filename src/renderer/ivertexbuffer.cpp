#include "milk/renderer/ivertexbuffer.h"
#include "milk/renderer/ivertexbufferinterleaved.h"
#include "milk/renderer/ivertexbuffernormal.h"
#include "milk/renderer/ishader.h"
#include "milk/renderer.h"
#include "milk/error.h"
#include "milk/helper.h"
#include "milk/glhelper.h"
#include "milk/includes.h"
#include "milk/io.h"
using namespace milk;

template<class CONTAINER>
static void writegeneric(std::ostream& os, const CONTAINER& c)
{
	for(CONTAINER::const_iterator it = c.begin(); it != c.end(); ++it)
		io::writepod(os, *it);
}

template<class VCI>
static void writecomponent(std::ostream& os, IVertexBuffer *pVertBuf, VertexComponent vc, VCI& vci)
{
	// format == float
	if(vci.type() == GL_FLOAT)
	{
		if(vci.components() == 1)
			writegeneric(os, pVertBuf->getComponent<GLfloat>(vc));
		else if(vci.components() == 2)
			writegeneric(os, pVertBuf->getComponent<CVector2f>(vc));
		else if(vci.components() == 3)
			writegeneric(os, pVertBuf->getComponent<CVector3f>(vc));
		else if(vci.components() == 4)
			writegeneric(os, pVertBuf->getComponent<CVector4f>(vc));
	}
	else if(vci.type() == GL_UNSIGNED_BYTE)
	{
		if(vci.components() == 3)
			writegeneric(os, pVertBuf->getComponent<CColor3ub>(vc));
		else if(vci.components() == 4)
			writegeneric(os, pVertBuf->getComponent<CColor4ub>(vc));
	}
}

void IVertexBuffer::writeToStream(std::ostream& os)
{
	m_format.write(os);
	io::writepod(os, ulong(m_numVertices));
	io::writepod(os, ulong(m_usage));

	lock(READ);
	// Vertices
	writecomponent(os, this, POSITION, m_format.m_v);
	// Normals
	writecomponent(os, this, NORMAL, m_format.m_n);
	// Tex-coords
	for(int i = 0; i < 8 && m_format.m_t[i].size(); ++i)
		writecomponent(os, this, TEXCOORD, m_format.m_t[i]);
	// Color
	writecomponent(os, this, COLOR, m_format.m_c);
	// Attributes
	for(CVertexFormat::attribList::iterator it = m_format.m_a.begin(); it != m_format.m_a.end(); ++it)
		writecomponent(os, this, SHADERATTRIBUTE, *it); // FIXME: TODO, can't write CAttribHandle
	unlock();
}

IVertexBuffer* IVertexBuffer::create(std::istream& is)
{
	return 0;
}

IVertexBuffer* IVertexBuffer::create(CVertexFormat format, uint size, BufferUsage usage)
{
	CHECK_FOR_OPENGL_ERRORS("There were errors prior to this function.");

	if(GLEW_ARB_vertex_buffer_object)
	{
		/* TODO: This causes some weird visual [texture?] error when interleave-format fail.
		 * dont know why, really weird.

		try
		{
			IVertexBufferInterleaved *pVertexBufferInterleaved = new CVertexBufferInterleaved_VBO(format, size, usage);
			Renderer::checkForErrors();
			return pVertexBufferInterleaved;
		}
		catch(error::opengl&)
		{ }
		*/

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
		IVertexBufferInterleaved *pVertexBufferInterleaved = new CVertexBufferInterleaved_GL(format, size, usage);
		Renderer::checkForErrors();
		return pVertexBufferInterleaved;
	}
	catch(error::opengl&)
	{ }

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

IVertexBuffer::IVertexBuffer(CVertexFormat format, uint size, BufferUsage usage)
: m_numVertices(size), m_format(format), m_usage(usage), m_defaultColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
