#ifndef MILK_CVERTEXFORMAT_H_
#define MILK_CVERTEXFORMAT_H_

// Includes
//////////////////////////////////////////

#include "milk/includes.h"
#include "milk/types.h"
#include "milk/renderer/ishader.h"

namespace milk
{
	enum VertexComponent
	{
		POSITION = 1,
		NORMAL,
		COLOR,
		TEXCOORD,
		MATRIXINDEX,
		WEIGHT,
		SHADERATTRIBUTE
	};

	class CAttribHandle;

	/// Describes the format of one vertex component.
	class CVertexComponentInfo
	{
	public:
		CVertexComponentInfo()
			: m_components(0), m_type(0), m_size(0)
		{ }

		CVertexComponentInfo(GLint c, GLenum t)
			: m_components(c), m_type(t), m_size(c*sizeOfType(t))
		{ }

		/// Compare component
		bool operator==(const CVertexComponentInfo& rhs) const
		{
			if(!m_components && !rhs.m_components)
				return true;
			return m_components == rhs.m_components && m_type == rhs.m_type && m_size == rhs.m_size;
		}

		template<class T>
		void setFormatByType()
		{
			m_components = glType<T>::components;
			m_type = glType<T>::type;
			m_size = glType<T>::size;
		}

		void setFormat(GLint c, GLenum t)
		{
			m_components = c;
			m_type = t;
			m_size = c*sizeOfType(t);
		}

		GLint components() const
		{ return m_components; }

		GLenum type() const
		{ return m_type; }

		GLint size() const
		{ return m_size; }

	private:
		GLint sizeOfType(GLenum f);

		GLint m_components;
		GLenum m_type;
		GLint m_size;
	};

	/// Class to describe vertex format when creating vertex-buffers
	class CVertexFormat
	{
	public:
		/// Create vertex format
		/**
		Create a vertex format descriptor. This describes the component-count for each
		part of one vertex. The parts are vector, normal, texture-coords and color.

		\param	vCount	number of vector components (2,3,4) (float)
		\param	nCount	number of normal components (must be 3) (float)
		\param	tCount	number of texture-coord components (2,4) (float)
		\param	cCount	number of color components (3,4) (float/ubyte)
		\param	colorFormat	format of the color component (COLORFLOAT, COLORUBYTE)
		*/
		CVertexFormat(GLint vCount, GLint nCount, GLint tCount, GLint cCount=0, GLint iCount=0, GLint wCount=0);
		CVertexFormat(GLint vCount, GLint nCount, GLint tCount, std::pair<GLint, GLenum> color, GLint iCount=0, GLint wCount=0);

		/// Create vertex format from OpenGL interleaved format
		CVertexFormat(GLenum glformat);

		/// Compare vertexformats
		bool operator==(const CVertexFormat& rhs) const;

		/// Get the corresponding opengl format for interleaved buffers
		GLenum interleavedFormat() const;

		/// Add shader attribute
		size_t addShaderAttrib(CAttribHandle attribHandle)
		{ m_a.push_back(attribHandle); return m_a.size()-1; }

		/// Get the size (in bytes) of one vertex
		GLint size() const;

		/// read format from stream
		void read(std::istream &is);

		/// write format to stream
		void write(std::ostream &os) const;

		void setTexCoordFormat(ulong tCount);
		GLint getTexCoordSize(int tc = 8) const;

		GLint getAttribSize() const;

		typedef std::vector<CAttribHandle> attribList;

		CVertexComponentInfo m_v;	// vertex position
		CVertexComponentInfo m_n;	// normal
		CVertexComponentInfo m_t[8];// texture coords
		CVertexComponentInfo m_c;	// color size
		CVertexComponentInfo m_i;	// matrix indices
		CVertexComponentInfo m_w;	// matrix weights
		attribList m_a;				// shader attributes
	};

	//////////////////////////////////////////////////////////////////////////

	extern ulong makeTexCoordFormat(uint tc1=0, uint tc2=0, uint tc3=0, uint tc4=0, uint tc5=0, uint tc6=0, uint tc7=0, uint tc8=0);
}

#endif

