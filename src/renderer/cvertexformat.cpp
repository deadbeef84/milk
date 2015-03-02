#include "milk/renderer/ivertexbuffer.h"
#include "milk/renderer/ishader.h"
#include "milk/renderer.h"
#include "milk/error.h"
#include "milk/helper.h"
#include "milk/glhelper.h"
#include "milk/includes.h"
#include "milk/io.h"
#include <numeric> // std::accumulate

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

GLint CVertexComponentInfo::sizeOfType(GLenum f)
{
	if(f == GL_BYTE)
		return sizeof(GLbyte);
	else if(f == GL_UNSIGNED_BYTE)
		return sizeof(GLubyte);
	else if(f == GL_SHORT)
		return sizeof(GLshort);
	else if(f == GL_UNSIGNED_SHORT)
		return sizeof(GLushort);
	else if(f == GL_INT)
		return sizeof(GLint);
	else if(f == GL_UNSIGNED_INT)
		return sizeof(GLuint);
	else if(f == GL_FLOAT)
		return sizeof(GLfloat);
	else if(f == GL_DOUBLE)
		return sizeof(GLdouble);
	else
		throw error::opengl("Unknown format in CVertexComponentInfo::sizeOfFormat");
}

CVertexFormat::CVertexFormat(GLint vCount, GLint nCount, GLint tCount, GLint cCount, GLint iCount, GLint wCount)
: m_v(vCount, GL_FLOAT),
m_n(nCount, GL_FLOAT),
m_c(cCount, GL_FLOAT),
m_i(iCount, GL_UNSIGNED_BYTE),
m_w(wCount, GL_FLOAT)
{
	setTexCoordFormat(tCount);
}

CVertexFormat::CVertexFormat(GLint vCount, GLint nCount, GLint tCount, std::pair<GLint, GLenum> color, GLint iCount, GLint wCount)
: m_v(vCount, GL_FLOAT),
m_n(nCount, GL_FLOAT),
m_c(color.first, color.second),
m_i(iCount, GL_UNSIGNED_BYTE),
m_w(wCount, GL_FLOAT)
{
	setTexCoordFormat(tCount);
}

CVertexFormat::CVertexFormat(GLenum glformat)
{
	if(glformat == GL_V2F)
	{
		m_v.setFormat(2, GL_FLOAT);
	}
	else if(glformat == GL_V3F)
	{
		m_v.setFormat(3, GL_FLOAT);
	}
	else if(glformat == GL_C4UB_V2F)
	{
		m_v.setFormat(2, GL_FLOAT);
		m_c.setFormat(4, GL_UNSIGNED_BYTE);
	}
	else if(glformat == GL_C4UB_V3F)
	{
		m_v.setFormat(3, GL_FLOAT);
		m_c.setFormat(4, GL_UNSIGNED_BYTE);
	}
	else if(glformat == GL_C3F_V3F)
	{
		m_v.setFormat(3, GL_FLOAT);
		m_c.setFormat(3, GL_FLOAT);
	}
	else if(glformat == GL_N3F_V3F)
	{
		m_v.setFormat(3, GL_FLOAT);
		m_n.setFormat(3, GL_FLOAT);
	}
	else if(glformat == GL_C4F_N3F_V3F)
	{
		m_v.setFormat(3, GL_FLOAT);
		m_n.setFormat(3, GL_FLOAT);
		m_c.setFormat(4, GL_FLOAT);
	}
	else if(glformat == GL_T2F_V3F)
	{
		m_v.setFormat(3, GL_FLOAT);
		setTexCoordFormat(2);
	}
	else if(glformat == GL_T4F_V4F)
	{
		m_v.setFormat(4, GL_FLOAT);
		setTexCoordFormat(4);
	}
	else if(glformat == GL_T2F_C4UB_V3F)
	{
		m_v.setFormat(3, GL_FLOAT);
		m_c.setFormat(4, GL_UNSIGNED_BYTE);
		setTexCoordFormat(2);
	}
	else if(glformat == GL_T2F_C3F_V3F)
	{
		m_v.setFormat(3, GL_FLOAT);
		m_c.setFormat(3, GL_FLOAT);
		setTexCoordFormat(2);
	}
	else if(glformat == GL_T2F_N3F_V3F)
	{
		m_v.setFormat(3, GL_FLOAT);
		m_n.setFormat(3, GL_FLOAT);
		setTexCoordFormat(2);
	}
	else if(glformat == GL_T2F_C4F_N3F_V3F) 
	{
		m_v.setFormat(3, GL_FLOAT);
		m_n.setFormat(3, GL_FLOAT);
		m_c.setFormat(4, GL_FLOAT);
		setTexCoordFormat(2);
	}
	else if(glformat == GL_T4F_C4F_N3F_V4F)
	{
		m_v.setFormat(4, GL_FLOAT);
		m_n.setFormat(3, GL_FLOAT);
		m_c.setFormat(4, GL_FLOAT);
		setTexCoordFormat(4);
	}
	else
		throw error::vertexbuffer_format("CVertexFormat::CVertexFormat(GLenum glformat): Unable to find matching format");
}

bool CVertexFormat::operator==(const CVertexFormat& rhs) const
{
	if(m_v == rhs.m_v && m_n == rhs.m_n && m_c == rhs.m_c && m_i == rhs.m_i && m_w == rhs.m_w)
	{
		for(int i = 0; i < 8; ++i)
			if(!(m_t[i] == rhs.m_t[i]))
				return false;
		if(m_a.size() != rhs.m_a.size())
			return false;
		for(attribList::const_iterator it = m_a.begin(), rit = rhs.m_a.begin(); it != m_a.end(); ++it, ++rit)
			if(!(*it == *rit))
				return false;
		return true;
	}
	return false;
}

GLenum CVertexFormat::interleavedFormat() const
{
	if(*this == CVertexFormat(2, 0, 0))
		return GL_V2F;
	else if(*this == CVertexFormat(3, 0, 0))
		return GL_V3F;
	else if(*this == CVertexFormat(2, 0, 0, std::make_pair(4, GL_UNSIGNED_BYTE)))
		return GL_C4UB_V2F;
	else if(*this == CVertexFormat(3, 0, 0, std::make_pair(4, GL_UNSIGNED_BYTE)))
		return GL_C4UB_V3F;
	else if(*this == CVertexFormat(3, 0, 0, 3))
		return GL_C3F_V3F;
	else if(*this == CVertexFormat(3, 3, 0))
		return GL_N3F_V3F;
	else if(*this == CVertexFormat(3, 3, 0, 4))
		return GL_C4F_N3F_V3F;
	else if(*this == CVertexFormat(3, 0, 2))
		return GL_T2F_V3F;
	else if(*this == CVertexFormat(4, 0, 4))
		return GL_T4F_V4F;
	else if(*this == CVertexFormat(3, 0, 2, std::make_pair(4, GL_UNSIGNED_BYTE)))
		return GL_T2F_C4UB_V3F;
	else if(*this == CVertexFormat(3, 0, 2, 3))
		return GL_T2F_C3F_V3F;
	else if(*this == CVertexFormat(3, 3, 2))
		return GL_T2F_N3F_V3F;
	else if(*this == CVertexFormat(3, 3, 2, 4))
		return GL_T2F_C4F_N3F_V3F;
	else if(*this == CVertexFormat(4, 3, 4, 4))
		return GL_T4F_C4F_N3F_V4F;

	// No matching format found, so throw an exception
	throw error::vertexbuffer_format("CVertexFormat::interleavedFormat(), no matching interleaved format could be found.");
}

void CVertexFormat::setTexCoordFormat(ulong tCount)
{
	for(int i = 0; i < 8; i++)
		m_t[i].setFormat((GLint)math::clamp<ulong>((tCount >> i*3) & 7, 0, 4), GL_FLOAT);
}

GLint CVertexFormat::getTexCoordSize(int tc) const
{
	tc = math::clamp<int>(tc, 0, 8);
	GLint sum = 0;
	for(int i = 0; i < tc; ++i)
		sum += m_t[i].size();
	return sum;
}

GLint CVertexFormat::getAttribSize() const
{
	size_t sum = 0;
	for(attribList::const_iterator it = m_a.begin(); it != m_a.end(); ++it)
		sum += it->size();
	return (GLint)sum;
}

GLint CVertexFormat::size() const
{
	return m_v.size() + m_n.size() + getTexCoordSize() + m_c.size() + m_i.size() + m_w.size() + getAttribSize();
}

void CVertexFormat::read(std::istream &is)
{
	uchar version = io::readpod<uchar>(is);
	if(version == 1)
	{
		CVertexComponentInfo *pComponents[] =
		{
			&m_v, &m_n,
			&m_t[0], &m_t[1], &m_t[2], &m_t[3], &m_t[4], &m_t[5], &m_t[6], &m_t[7],
			&m_c, &m_i, &m_w, 0
		};
		for(int i = 0; pComponents[i]; ++i)
		{
			GLint c = io::readpod<GLint>(is);
			GLenum f = io::readpod<GLenum>(is);
			pComponents[i]->setFormat(c, f);
		}
		m_a.resize(io::readpod<ulong>(is));
		for(attribList::iterator it = m_a.begin(); it != m_a.end(); ++it)
		{
			GLint index = io::readpod<GLint>(is); /// FIXME: uhh. this is no good!
			GLint components = io::readpod<GLint>(is);
			GLenum type = io::readpod<GLenum>(is);
			size_t size = io::readpod<ulong>(is);
			*it = CAttribHandle(index, components, type, size);
		}
	}
}

void CVertexFormat::write(std::ostream &os) const
{
	io::writepod<uchar>(os, 1);
	const CVertexComponentInfo *pComponents[] =
	{
		&m_v, &m_n,
		&m_t[0], &m_t[1], &m_t[2], &m_t[3], &m_t[4], &m_t[5], &m_t[6], &m_t[7],
		&m_c, &m_i, &m_w, 0
	};
	for(int i = 0; pComponents[i]; ++i)
	{
		io::writepod(os, pComponents[i]->components());
		io::writepod(os, pComponents[i]->type());
	}
	io::writepod(os, ulong(m_a.size()));
	for(attribList::const_iterator it = m_a.begin(); it != m_a.end(); ++it)
	{
		io::writepod(os, it->index()); /// FIXME: this is connected to the problem problem above (read)
		io::writepod(os, it->components());
		io::writepod(os, it->type());
		io::writepod(os, ulong(it->size()));
	}
}

ulong milk::makeTexCoordFormat(uint tc1, uint tc2, uint tc3, uint tc4, uint tc5, uint tc6, uint tc7, uint tc8)
{
	uint *tc[8] = {&tc1, &tc2, &tc3, &tc4, &tc5, &tc6, &tc7, &tc8};
	ulong ret = 0;
	for(int i = 0; i < 8; ++i)
		ret |= math::clamp<ulong>(*tc[i], 0, 4) << (i*3);
	return ret;
}
