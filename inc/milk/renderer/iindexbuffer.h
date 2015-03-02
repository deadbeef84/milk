#ifndef MILK_IINDEXBUFFER_H_
#define MILK_IINDEXBUFFER_H_

// Includes
//////////////////////////////////////////

#include <algorithm>
#include "milk/includes.h"
#include "milk/types.h"
#include "milk/math/cvector.h"
#include "milk/ccolor.h"
#include "milk/cdatacontainer.h"
#include "milk/renderer/ivertexbuffer.h"

namespace milk
{
	/// Index buffer class.
	/**
		
	*/
	class IIndexBuffer
	{
	public:
		virtual ~IIndexBuffer() { }

		static IIndexBuffer* create(std::istream& is);
		static IIndexBuffer* create(GLenum format, uint size, BufferUsage usage = STATIC);
		void write(std::ostream& os, bool smart = true);

		virtual void lock(BufferAccess = BufferAccess(READ|WRITE)) { }
		virtual void unlock() { }

		virtual void draw(IVertexBuffer* vb, GLenum mode, uint start, uint num) = 0;
		void draw(IVertexBuffer* vb, GLenum mode)
		{ draw(vb, mode, 0, m_numIndices); }

		virtual void beginMultipleDraw(IVertexBuffer* vb) = 0;
		virtual void endMultipleDraw(IVertexBuffer* vb) = 0;
		virtual void multipleDraw(GLenum mode, uint start, uint num) = 0;
		void multipleDraw(GLenum mode)
		{ multipleDraw(mode, 0, m_numIndices); }

		void optimize();
		void optimize(uint start, uint end);

		//////////////

		template<class T>
		CDataContainer<T> getIndices()
		{ return CDataContainer<T>(reinterpret_cast<T*>(m_pIndices), sizeOfFormat(), m_numIndices); }

		uint* getIndicesui()
		{ return reinterpret_cast<uint*>(m_pIndices); }

		ushort* getIndicesus()
		{ return reinterpret_cast<ushort*>(m_pIndices); }

		uchar* getIndicesub()
		{ return reinterpret_cast<uchar*>(m_pIndices); }

		uint numIndices() const
		{ return m_numIndices; }

		size_t numDegenerateTriangles();

		size_t sizeOfFormat() const;

	protected:
		IIndexBuffer(GLenum format, uint size, BufferUsage usage);

		GLvoid* getIndexPtr(uint index);

		GLenum			m_format;
		void			*m_pIndices;
		uint			m_numIndices;
		BufferUsage		m_usage;
		bool			m_optimized;
		uint			m_firstVertex, m_lastVertex;
	};

	class CIndexBuffer_GL : public IIndexBuffer
	{
	public:
		CIndexBuffer_GL(GLenum format, uint size, BufferUsage usage);
		virtual ~CIndexBuffer_GL();

		void draw(IVertexBuffer* vb, GLenum mode, uint start, uint num);

		void beginMultipleDraw(IVertexBuffer* vb);
		void endMultipleDraw(IVertexBuffer* vb);
		void multipleDraw(GLenum mode, uint start, uint num);
	};

	class CIndexBuffer_VBO : public IIndexBuffer
	{
	public:
		CIndexBuffer_VBO(GLenum format, uint size, BufferUsage usage);
		virtual ~CIndexBuffer_VBO();

		void lock(BufferAccess = READ|WRITE);
		void unlock();

		void draw(IVertexBuffer* vb, GLenum mode, uint start, uint num);

		void beginMultipleDraw(IVertexBuffer* vb);
		void endMultipleDraw(IVertexBuffer* vb);
		void multipleDraw(GLenum mode, uint start, uint num);

	private:
		GLuint m_id;
	};
}
#endif
