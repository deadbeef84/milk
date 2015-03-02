#include "milk/renderer/crendertexture.h"
#include "milk/iwindow.h"
#include "milk/glhelper.h"
#include "milk/cimage.h"
#include "milk/renderer.h"
#include <SDL.h>
using namespace milk;

CRenderTexture2D_fbo::CRenderTexture2D_fbo(CTexture2D& texture)
: IRenderTexture2D(texture),
  m_frameBufferIndex(0), m_depthBufferIndex(0), m_stencilBufferIndex(0)
{
	CHECK_FOR_OPENGL_ERRORS("There were errors prior to this function.");

	if(!GLEW_EXT_framebuffer_object)
		throw error::milk("GL_EXT_framebuffer_object not supported");

	glGenFramebuffersEXT(1, &m_frameBufferIndex);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_frameBufferIndex);

	/*
	// generate mip-maps?
	if(texture.getImageType().mipmap)
	{
		binder bound(texture);
		glGenerateMipmapEXT(GL_TEXTURE_2D); // TODO: move this to ctexture2d?
	}
	*/

	if(!texture.isDepthTexture())
	{
		// FIXME: allow users to set depth?
		GLuint depth = GL_DEPTH_COMPONENT24;
		if(depth)
		{
			glGenRenderbuffersEXT(1, &m_depthBufferIndex);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthBufferIndex);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, depth, m_size.x, m_size.y);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthBufferIndex);
		}
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_texture.getFaceTarget(), m_texture.getTextureId(), 0);
	}
	else
	{
		// TEMP/FIXME: Why do we need a color-renderbuffer?
		/**/
		glGenRenderbuffersEXT(1, &m_depthBufferIndex);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthBufferIndex);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA, m_size.x, m_size.y);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, m_depthBufferIndex);
		/**/

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, m_texture.getFaceTarget(), m_texture.getTextureId(), 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	/*/
	//if(format & FBO_STENCIL)
	{
		glGenRenderbuffersEXT(1, &m_stencilBufferIndex);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_stencilBufferIndex);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, m_size.x, m_size.y);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
			GL_RENDERBUFFER_EXT, m_stencilBufferIndex);
	}
	/**/

	checkFrameBufferStatus();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	CHECK_FOR_OPENGL_ERRORS("TODO");
}

CRenderTexture2D_fbo::~CRenderTexture2D_fbo()
{
	if(m_stencilBufferIndex)
		glDeleteRenderbuffersEXT(1, &m_stencilBufferIndex);

	if(m_depthBufferIndex)
		glDeleteRenderbuffersEXT(1, &m_depthBufferIndex);

	if(m_frameBufferIndex)
		glDeleteFramebuffersEXT(1, &m_frameBufferIndex);
}

void CRenderTexture2D_fbo::checkFrameBufferStatus()
{
	GLenum status = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			return;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			throw error::opengl("Framebuffer incomplete, incomplete attachment");
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			throw error::opengl("Unsupported framebuffer format");
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			throw error::opengl("Framebuffer incomplete, missing attachment");
		case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
			throw error::opengl("Framebuffer incomplete, duplicate attachment");
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			throw error::opengl("Framebuffer incomplete, attached images must have same dimensions");
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			throw error::opengl("Framebuffer incomplete, attached images must have same format");
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			throw error::opengl("Framebuffer incomplete, missing draw buffer");
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			throw error::opengl("Framebuffer incomplete, missing read buffer");
		default:
			throw error::opengl("Unknown Framebuffer status");
	}
}

/*
void CRenderTexture2D::switchTarget(Texture &texture, int attachIndex, int newTarget)
{
	attachIndex = clamp(attachIndex, 0, 15);
	newTarget   = (newTarget == -1) ? texture.getTarget() : newTarget;

	if(m_frameBufferIndex)
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + attachIndex,
		newTarget, texture.getID(), 0);
}
*/

bool CRenderTexture2D_fbo::beginRender()
{
	if(m_frameBufferIndex)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_frameBufferIndex);
	else
		return false;
		//throw error::opengl("Invalid FrameBufferObject index");

	// TODO: remove this?
	if(m_texture.isDepthTexture())
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	return true;
}

void CRenderTexture2D_fbo::endRender()
{
	// TODO: remove this as well?
	if(m_texture.isDepthTexture())
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	if(m_frameBufferIndex)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	/*
	if(m_imgType.mipmap)
	{
		binder bound(*this);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
	}
	*/
}

//////////////////////////////////////////////////////////////////////////

CRenderTexture2D_pbuffer::CRenderTexture2D_pbuffer(CTexture2D& texture)
: IRenderTexture2D(texture), m_bound(false)
{
	if(!WGLEW_ARB_pbuffer)
		throw error::opengl("WGL_ARB_pbuffer");
	if(!WGLEW_ARB_pixel_format)
		throw error::opengl("WGL_ARB_pixel_format");

	binder bound(m_texture);

	// TODO: verify this code!
	/*
	if(!WGLEW_ARB_render_texture || m_texture.isDepthTexture())
	{
		m_texture.private_ctor(0, size, imgtype.type, imgtype);
	}
	else
	{
		m_size = size;
		m_imgType.mipmap = false;
		setQuality(Quality(Clamp, Clamp));
	}
	*/
	m_texture.setQuality(Quality(Clamp, Clamp));

	initPixelBuffer();
}

CRenderTexture2D_pbuffer::~CRenderTexture2D_pbuffer()
{
	freePixelBuffer();
}

void CRenderTexture2D_pbuffer::initPixelBuffer()
{
	m_hdc = wglGetCurrentDC();
	m_hglrc = wglGetCurrentContext();

	// Query for a suitable pixel format based on the specified mode.
	int format;
	int formats[100];
	unsigned int nformats;

	int pixelFormatAttr[] =
	{
		//WGL_SUPPORT_OPENGL_ARB, TRUE,       // P-buffer will be used with OpenGL
		WGL_DRAW_TO_PBUFFER_ARB, GL_TRUE,      // Enable render to p-buffer
		WGL_BIND_TO_TEXTURE_RGBA_ARB, GL_TRUE, // P-buffer will be used as a texture
		//WGL_RED_BITS_ARB, 8,                // At least 8 bits for RED channel
		//WGL_GREEN_BITS_ARB, 8,              // At least 8 bits for GREEN channel
		//WGL_BLUE_BITS_ARB, 8,               // At least 8 bits for BLUE channel
		//WGL_ALPHA_BITS_ARB, 8,              // At least 8 bits for ALPHA channel
		//WGL_DEPTH_BITS_ARB, 16,             // At least 16 bits for depth buffer
		//WGL_STENCIL_BITS_ARB, 0,
		//WGL_DOUBLE_BUFFER_ARB, FALSE,       // We don't require double buffering
		0                                   // Zero terminates the list
	};

	if(!wglChoosePixelFormatARB(m_hdc, pixelFormatAttr, 0, 100, formats, &nformats))
		throw error::opengl("pbuffer creation error: wglChoosePixelFormatARB() failed.");

	format = formats[0];

	if(nformats <= 0)
		throw error::opengl("pbuffer creation error:  Couldn't find a suitable pixel format.");

	int pixelBufferAttr[] =
	{
		WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGBA_ARB, // Our p-buffer will have a texture format of RGBA
		WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB,   // Of texture target will be GL_TEXTURE_2D
		//WGL_MIPMAP_TEXTURE_ARB, TRUE,
		//WGL_PBUFFER_LARGEST_ARB, FALSE,
		0                                             // Zero terminates the list
	};

	// Create the p-buffer.
	m_pb.hpbuffer = wglCreatePbufferARB(m_hdc, format, m_size.x, m_size.y, pixelBufferAttr);
	if(m_pb.hpbuffer == 0)
		throw error::opengl("pbuffer creation error:  wglCreatePbufferARB() failed");

	// Get the device context.
	m_pb.hdc = wglGetPbufferDCARB(m_pb.hpbuffer);
	if(m_pb.hdc == 0)
		throw error::opengl("pbuffer creation error:  wglGetPbufferDCARB() failed");

	// Create a gl context for the p-buffer.
	m_pb.hglrc = wglCreateContext(m_pb.hdc);
	if(m_pb.hglrc == 0)
		throw error::opengl("pbuffer creation error:  wglCreateContext() failed");

	// Determine the actual width and height we were able to create.
	wglQueryPbufferARB(m_pb.hpbuffer, WGL_PBUFFER_WIDTH_ARB, &m_pb.width);
	wglQueryPbufferARB(m_pb.hpbuffer, WGL_PBUFFER_HEIGHT_ARB, &m_pb.height);

	/*
	GLint texFormat = WGL_NO_TEXTURE_ARB;
	if(!wglQueryPbufferARB(m_pb.hpbuffer, WGL_TEXTURE_FORMAT_ARB, &texFormat))
	throw error::milk("wglQueryPbufferARB() failed");

	bool istex = false;
	if (texFormat != WGL_NO_TEXTURE_ARB)
	istex = true;
	*/

	if(!wglShareLists(m_hglrc, m_pb.hglrc))
		throw error::opengl("wglShareLists() failed.");
}

void CRenderTexture2D_pbuffer::freePixelBuffer()
{
	// release the pbuffer from the render texture object
	if(m_bound)
	{
		binder bound(m_texture);
		if(!wglReleaseTexImageARB(m_pb.hpbuffer, WGL_FRONT_LEFT_ARB))
			throw error::opengl("wglReleaseTexImageARB() failed");
		m_bound = false;
	}

	if(m_pb.hpbuffer)
	{
		// delete the pbuffer rendering context
		wglDeleteContext(m_pb.hglrc);
		m_pb.hglrc = 0;

		// Release DC
		wglReleasePbufferDCARB(m_pb.hpbuffer, m_pb.hdc);
		m_pb.hdc = 0;

		// destroy the pbuffer
		wglDestroyPbufferARB(m_pb.hpbuffer);
		m_pb.hpbuffer = 0;
	}
}

bool CRenderTexture2D_pbuffer::beginRender()
{
	CHECK_FOR_OPENGL_ERRORS("There were errors prior to this function.");

	// release the pbuffer from the render texture object
	if(m_bound && WGLEW_ARB_render_texture)
	{
		binder bound(m_texture);
		if(!wglReleaseTexImageARB(m_pb.hpbuffer, WGL_FRONT_LEFT_ARB))
			throw error::opengl("wglReleaseTexImageARB() failed");
		m_bound = false;
	}

	// make the pbuffer's rendering context current.
	if(!wglMakeCurrent(m_pb.hdc, m_pb.hglrc))
		throw error::opengl("wglMakeCurrent() failed");

	return true;
}

void CRenderTexture2D_pbuffer::endRender()
{
	CHECK_FOR_OPENGL_ERRORS("There were errors prior to this function.");

	if(!WGLEW_ARB_render_texture || m_texture.isDepthTexture())
	{
		binder bound(m_texture);
		glCopyTexSubImage2D(m_texture.getFaceTarget(), 0, 0, 0, 0, 0, m_pb.width, m_pb.height);
		CHECK_FOR_OPENGL_ERRORS("glCopyTexSubImage2D()");
	}

	// make the window's rendering context current
	if(!wglMakeCurrent(m_hdc, m_hglrc))
		throw error::opengl("wglMakeCurrent() failed");

	if(WGLEW_ARB_render_texture && !m_bound && !m_texture.isDepthTexture())
	{
		// bind the pbuffer to the render texture object
		binder bound(m_texture);
		if(!wglBindTexImageARB(m_pb.hpbuffer, WGL_FRONT_LEFT_ARB))
			throw error::opengl("wglBindTexImageARB() failed");
		m_bound = true;
	}
}

//////////////////////////////////////////////////////////////////////////

CRenderTexture2D_ogl::CRenderTexture2D_ogl(CTexture2D& texture)
: IRenderTexture2D(texture)
{
	// TODO: check for SGIX_depth_texture ?
	IWindow *pWindow = IWindow::getInstance();
	if(!pWindow)
		throw error::opengl("CRenderTexture2D_ogl requires a window.");
	if(m_size.x > pWindow->size().x || m_size.y > pWindow->size().y)
		throw error::opengl("CRenderTexture2D_ogl: window too small");

	//private_ctor(0, size, imgtype.type, imgtype);
}

bool CRenderTexture2D_ogl::beginRender()
{
	CHECK_FOR_OPENGL_ERRORS("There were errors prior to this function.");
	return true;
}

void CRenderTexture2D_ogl::endRender()
{
	CHECK_FOR_OPENGL_ERRORS("There were errors prior to this function.");

	binder bound(m_texture);
	glCopyTexSubImage2D(m_texture.getFaceTarget(), 0, 0, 0, 0, 0, m_size.x, m_size.y);
	CHECK_FOR_OPENGL_ERRORS("glCopyTexImage2D()");
}

//////////////////////////////////////////////////////////////////////////

IRenderTexture2D* IRenderTexture2D::create(CTexture2D& texture)
{
	CHECK_FOR_OPENGL_ERRORS("There were errors prior to this function.");

	try
	{
		if(GLEW_EXT_framebuffer_object)
			return new CRenderTexture2D_fbo(texture);
	}
	catch(error::opengl&)
	{ }

	/*
	try
	{
		if(WGLEW_ARB_pbuffer && WGLEW_ARB_pixel_format)
			return new CRenderTexture2D_pbuffer(texture);
	}
	catch(error::opengl&)
	{ }
	*/

	try
	{
		return new CRenderTexture2D_ogl(texture);
	}
	catch(error::opengl&)
	{ }

	throw error::opengl("unable to create render texture");
}
