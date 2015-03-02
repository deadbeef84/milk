#ifndef MILK_IVERTEXBUFFERNORMAL_H_
#define MILK_IVERTEXBUFFERNORMAL_H_

// Includes
//////////////////////////////////////////

#include <algorithm>
#include "milk/renderer/ivertexbuffer.h"
#include "milk/includes.h"
#include "milk/types.h"
#include "milk/math/cvector.h"
#include "milk/ccolor.h"
#include "milk/cdatacontainer.h"


// Vertexbuffer class declaration
//////////////////////////////////////////

namespace milk
{
	/// Vertex Buffer class, stores vertex data in separate arrays (ie. not interleaved)
	/**
	Base class for vertex-buffers. Use IVertexBuffer::create() to create the
	most appropriate vertex-buffer type.
	*/
	class IVertexBufferNormal : public IVertexBuffer
	{
	public:
		virtual ~IVertexBufferNormal() { }

		static IVertexBufferNormal* create(CVertexFormat format, uint size, BufferUsage usage);

		void draw(GLenum mode, uint start, uint num);

		void beginMultipleDraw();
		void endMultipleDraw();
		void multipleDraw(GLenum mode, uint start, uint num);

	protected:
		/// Protected to prevent creation.
		IVertexBufferNormal(CVertexFormat format, uint size, BufferUsage usage);

		virtual void* getComponentPtr(VertexComponent vc, size_t subComponent, size_t& stride);

		virtual void privBufferBeginDraw();
		virtual void privBufferEndDraw();

		typedef std::vector<size_t> attribOffsetList;

		uchar	*m_pVertices;
		size_t	m_nOfs;		// normal-offset
		size_t	m_cOfs;		// color-offset
		size_t	m_tOfs[8];	// texturecoords-offset
		size_t	m_iOfs;		// index-offset
		size_t	m_wOfs;		// weight-offset
		attribOffsetList m_aOfs; // attribute-offset

	private:
		/// Private to prevent copy
		IVertexBufferNormal(const IVertexBufferNormal&);
		IVertexBufferNormal& operator=(const IVertexBufferNormal&);
	};

	/// Regular GL-buffer
	/**
	Vertex buffer class which uses standard OpenGL vertex-buffers.
	*/
	class CVertexBufferNormal_GL : public IVertexBufferNormal
	{
	public:
		CVertexBufferNormal_GL(CVertexFormat format, uint size, BufferUsage usage);
		virtual ~CVertexBufferNormal_GL();

	private:
		// Prevent copy
		CVertexBufferNormal_GL(const CVertexBufferNormal_GL&);
		CVertexBufferNormal_GL& operator=(const CVertexBufferNormal_GL&);
	};

	/// Vertex Buffer VBO
	/**
	Vertex buffer class which uses the VBO (Vertex Buffer Object) extension.
	Only use this class if you want a buffer using VBO. Otherwise I suggest
	you use IVertexBuffer::create() to create the most appropriate
	vertex-buffer.
	*/
	class CVertexBufferNormal_VBO
		: public IVertexBufferNormal
	{
	public:
		CVertexBufferNormal_VBO(CVertexFormat format, uint size, BufferUsage usage);
		virtual ~CVertexBufferNormal_VBO();

		void lock(BufferAccess access = READ|WRITE);
		void unlock();

	protected:
		void privBufferBeginDraw();
		void privBufferEndDraw();

	private:
		GLuint m_id;

		/// private to prevent copy
		CVertexBufferNormal_VBO(const CVertexBufferNormal_VBO&);
		CVertexBufferNormal_VBO& operator=(const CVertexBufferNormal_VBO&);
	};
}
#endif
