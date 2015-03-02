#ifndef MILK_IVERTEXBUFFER_H_
#define MILK_IVERTEXBUFFER_H_

// Includes
//////////////////////////////////////////

#include "milk/includes.h"
#include "milk/types.h"
#include "milk/math/cvector.h"
#include "milk/ccolor.h"
#include "milk/cdatacontainer.h"
#include "milk/math/cmatrix.h"
#include "milk/renderer/cvertexformat.h"

namespace milk
{
	enum BufferUsage
	{
		STATIC = 1,
		DYNAMIC = 2
	};

	enum BufferAccess
	{
		READ = 1,
		WRITE = 2
	};

	inline BufferAccess operator|(BufferAccess a, BufferAccess b)
	{ return static_cast<BufferAccess>(static_cast<int>(a)|static_cast<int>(b)); }

	/// Base class for vertex buffers
	/**
		Base class for vertex-buffers. Use IVertexBuffer::create() to create the
		most appropriate vertex-buffer type.
	*/
	class IVertexBuffer
	{
		friend class CIndexBuffer_GL;
		friend class CIndexBuffer_VBO;
	public:

		virtual ~IVertexBuffer() { }

		void writeToStream(std::ostream& os);
		static IVertexBuffer* create(std::istream& is);
		static IVertexBuffer* create(CVertexFormat format, uint size, BufferUsage usage = STATIC);

		/// Set default color.
		void setDefaultColor(const CColor4f& color)
		{ m_defaultColor = color; }

		/// Get color to use when no color-array is set.
		const CColor4f& getDefaultColor() const
		{ return m_defaultColor; }

		/// Lock the vertex-buffer data
		/**
			Locking the buffer will allow you to modify it's data. Make sure you
			unlock it after you're done modifying it or rendering operations may crash.
		*/
		virtual void lock(BufferAccess = READ|WRITE) { }

		/// Unlock the vertex-buffer data
		/**
			Unlock the vertex data when you are done modifying it.
		*/
		virtual void unlock() { }

		/// Render primitives
		/**
			mode = OpenGL-primitive-type, eg. GL_TRIANGLES
		*/
		void draw(GLenum mode)
		{ draw(mode, 0, m_numVertices); }

		/// Render primitives from specified vertices
		/**
			mode = OpenGL-primitive-type, eg. GL_TRIANGLES. Uses vertices [start] to [start+num].
		*/
		virtual void draw(GLenum mode, uint start, uint num) = 0;

		/// Render multiple times
		virtual void beginMultipleDraw() = 0;
		virtual void endMultipleDraw() = 0;
		virtual void multipleDraw(GLenum mode, uint start, uint num) = 0;
		void multipleDraw(GLenum mode)
		{ multipleDraw(mode, 0, m_numVertices); }

		/// Get the number of vertices
		uint numVertices() const
		{ return m_numVertices; }

		/// Get the vertex-format
		const CVertexFormat& format() const
		{ return m_format; }

		/// Get vertex-positions (2 components)
		CDataContainer<CVector2f> getVertices2f()
		{ return getComponent<CVector2f>(POSITION); }
		/// Get vertex-positions (3 components)
		CDataContainer<CVector3f> getVertices3f()
		{ return getComponent<CVector3f>(POSITION); }
		/// Get vertex-positions (4 components)
		CDataContainer<CVector4f> getVertices4f()
		{ return getComponent<CVector4f>(POSITION); }

		/// Get vertex-normals (3 components)
		CDataContainer<CVector3f> getNormals3f()
		{ return getComponent<CVector3f>(NORMAL); }

		/// Get vertex-texture coordinates (2 components)
		CDataContainer<CVector2f> getTexCoords2f(size_t unit = 0)
		{ return getComponent<CVector2f>(TEXCOORD, unit); }
		/// Get vertex-texture coordinates (3 components)
		CDataContainer<CVector3f> getTexCoords3f(size_t unit = 0)
		{ return getComponent<CVector3f>(TEXCOORD, unit); }
		/// Get vertex-texture coordinates (4 components)
		CDataContainer<CVector4f> getTexCoords4f(size_t unit = 0)
		{ return getComponent<CVector4f>(TEXCOORD, unit); }

		/// Get vertex-colors (4 float components)
		CDataContainer<CColor4f> getColors4f()
		{ return getComponent<CColor4f>(COLOR); }
		/// Get vertex-colors (4 unsigned byte components)
		CDataContainer<CColor4ub> getColors4ub()
		{ return getComponent<CColor4ub>(COLOR); }
		/// Get vertex-colors (3 float components)
		CDataContainer<CColor3f> getColors3f()
		{ return getComponent<CColor3f>(COLOR); }
		/// Get vertex-colors (3 unsigned byte components)
		CDataContainer<CColor3ub> getColors3ub()
		{ return getComponent<CColor3ub>(COLOR); }

		/// Get vertex-shader attribute
		CDataContainer<GLshort> getAttrib1s(size_t index)
		{ return getComponent<GLshort>(SHADERATTRIBUTE, index); }
		CDataContainer<GLfloat> getAttrib1f(size_t index)
		{ return getComponent<GLfloat>(SHADERATTRIBUTE, index); }
		CDataContainer<CVector2f> getAttrib2f(size_t index)
		{ return getComponent<CVector2f>(SHADERATTRIBUTE, index); }
		CDataContainer<CVector3f> getAttrib3f(size_t index)
		{ return getComponent<CVector3f>(SHADERATTRIBUTE, index); }
		CDataContainer<CVector4f> getAttrib4f(size_t index)
		{ return getComponent<CVector4f>(SHADERATTRIBUTE, index); }
		CDataContainer<CMatrix3f> getAttrib9f(size_t index)
		{ return getComponent<CMatrix3f>(SHADERATTRIBUTE, index); }
		CDataContainer<CMatrix4f> getAttrib16f(size_t index)
		{ return getComponent<CMatrix4f>(SHADERATTRIBUTE, index); }

		/// Get vertex component-list
		template<class C>
		CDataContainer<C> getComponent(VertexComponent vc, size_t subcomp=0)
		{
			size_t stride = 0;
			void *pPtr = getComponentPtr(vc, subcomp, stride);
			if(pPtr)
				return CDataContainer<C>(reinterpret_cast<C*>(pPtr), m_numVertices, stride);
			throw error::opengl("Vertex-buffer component error."); // FIXME: TODO, make SEH-class for this
		}

	protected:
		/// Protected to prevent construction, use create() instead.
		IVertexBuffer(CVertexFormat format, uint size, BufferUsage usage);

		virtual void privBufferBeginDraw() { };
		virtual void privBufferEndDraw() { };

		virtual void* getComponentPtr(VertexComponent vc, size_t subcomp, size_t& stride) = 0;

		uint			m_numVertices;
		CVertexFormat	m_format;
		BufferUsage		m_usage;
		CColor4f		m_defaultColor;

	private:
		/// Private to prevent copy
		IVertexBuffer(const IVertexBuffer&);
		IVertexBuffer& operator=(const IVertexBuffer&);
	};
}

#endif
