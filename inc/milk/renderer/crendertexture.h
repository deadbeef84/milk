#ifndef MILK_CRENDERTEXTURE_H_
#define MILK_CRENDERTEXTURE_H_

#include "milk/renderer/irendertarget.h"
#include "milk/renderer/ctexture.h"

namespace milk
{
	class IRenderTexture2D : public IRenderTarget
	{
	public:
		IRenderTexture2D(CTexture2D& texture)
			: IRenderTarget(texture.size()), m_texture(texture)
		{ m_texture.addRef(); }

		virtual ~IRenderTexture2D()
		{ m_texture.release(); }

		static IRenderTexture2D* create(CTexture2D& texture);

		CTexture2D& getTexture()
		{ return m_texture; }

	protected:
		MILK_NO_COPY(IRenderTexture2D);

		CTexture2D& m_texture;
	};

	class CRenderTexture2D_fbo : public IRenderTexture2D
	{
	public:
		CRenderTexture2D_fbo(CTexture2D& texture);
		virtual ~CRenderTexture2D_fbo();

		bool beginRender();
		void endRender();

	private:
		void checkFrameBufferStatus();

		GLuint m_frameBufferIndex;
		GLuint m_depthBufferIndex;
		GLuint m_stencilBufferIndex;
	};

	class CRenderTexture2D_pbuffer : public IRenderTexture2D
	{
	public:
		CRenderTexture2D_pbuffer(CTexture2D& texture);
		virtual ~CRenderTexture2D_pbuffer();

		bool beginRender();
		void endRender();

	private:
		void initPixelBuffer();
		void freePixelBuffer();

		struct PixelBuffer
		{
			PixelBuffer()
				: hpbuffer(0), hdc(0), hglrc(0), width(0), height(0)
			{ }

			HPBUFFERARB  hpbuffer;      // Handle to a pbuffer.
			HDC          hdc;           // Handle to a device context.
			HGLRC        hglrc;         // Handle to a GL rendering context.
			int          width;         // Width of the pbuffer
			int          height;        // Height of the pbuffer
		};

		PixelBuffer m_pb;
		HDC m_hdc;
		HGLRC m_hglrc;
		bool m_bound;
	};

	class CRenderTexture2D_ogl : public IRenderTexture2D
	{
	public:
		CRenderTexture2D_ogl(CTexture2D& texture);
		virtual ~CRenderTexture2D_ogl()
		{ }

		bool beginRender();
		void endRender();
	};
}

#endif
