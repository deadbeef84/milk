#ifndef MILK_IVERTEXBUFFERINTERLEAVED_H_
#define MILK_IVERTEXBUFFERINTERLEAVED_H_

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
	/// Interleaved Vertex Buffer class, stores vertex data in an interleaved format
	/**
	Base class for vertex-buffers. Use CreateVertexBuffer() to create the
	most appropriate vertex-buffer type.
	*/
	class IVertexBufferInterleaved : public IVertexBuffer
	{
	public:
		virtual ~IVertexBufferInterleaved() { }

		static IVertexBufferInterleaved* create(CVertexFormat format, uint size, BufferUsage usage);

		void draw(GLenum mode, uint start, uint num);

		void beginMultipleDraw();
		void endMultipleDraw();
		void multipleDraw(GLenum mode, uint start, uint num);

	protected:
		/// Get pointer to vertex component-list
		virtual void* getComponentPtr(VertexComponent vc, size_t subComponent, size_t& stride);

		/// Protected to prevent construction
		IVertexBufferInterleaved(CVertexFormat format, uint size, BufferUsage usage);

		uchar	*m_pVertices;
		uint	m_interleavedFormat;
		size_t	m_vOfs;		// vertex-offset
		size_t	m_cOfs;		// color-offset
		size_t	m_nOfs;		// normal-offset
		size_t	m_tOfs[8];	// texcoord-offset

	private:
		/// Private to prevent copy
		IVertexBufferInterleaved(const IVertexBufferInterleaved&);
		IVertexBufferInterleaved& operator=(const IVertexBufferInterleaved&);
	};

	/// Interleaved "Regular GL" vertexbuffer
	class CVertexBufferInterleaved_GL : public IVertexBufferInterleaved
	{
	public:
		CVertexBufferInterleaved_GL(CVertexFormat format, uint size, BufferUsage usage);
		virtual ~CVertexBufferInterleaved_GL();

		virtual void lock(BufferAccess access = READ|WRITE);
		virtual void unlock();

	protected:
		void privBufferBeginDraw();

	private:
		// Prevent copy
		CVertexBufferInterleaved_GL(const CVertexBufferInterleaved_GL&);
		CVertexBufferInterleaved_GL& operator=(const CVertexBufferInterleaved_GL&);
	};

	/// Interleaved "Vertex Buffer Object" (VBO)
	class CVertexBufferInterleaved_VBO : public IVertexBufferInterleaved
	{
	public:
		CVertexBufferInterleaved_VBO(CVertexFormat format, uint size, BufferUsage usage);
		virtual ~CVertexBufferInterleaved_VBO();

		virtual void lock(BufferAccess access = READ|WRITE);
		virtual void unlock();

	protected:
		void privBufferBeginDraw();
		void privBufferEndDraw();

	private:
		CVertexBufferInterleaved_VBO(const CVertexBufferInterleaved_VBO&);
		CVertexBufferInterleaved_VBO& operator=(const CVertexBufferInterleaved_VBO&);

		GLuint m_id;
	};
}
#endif
