#include "milk/iwindow.h"
#include "milk/renderer.h"
#include "milk/includes.h"
#include "milk/input.h"
#include "milk/audio/sound.h"
#include "milk/iapplication.h"
#include "milk/timer.h"
#include "milk/error.h"
#include "milk/helper.h"
#include <algorithm>

using namespace milk;
using namespace std;

IWindow *IWindow::ms_instance = 0;

///////////////////////////////////////////////////////////

IWindow::IWindow(IApplication& owner, const string& title, int width, int height,
				 Flags flags, int depth, int zDepth, int sDepth, int msb, int mss)
	: IRenderTarget(width, height),
	m_owner(owner), m_depth(depth), m_zDepth(zDepth),
	m_sDepth(sDepth), m_fullscreen(flags & FULLSCREEN),
	m_isRendering(0), m_frame(0)
{
	BOOST_ASSERT(ms_instance == 0);
	ms_instance = this;

	addRef(); // TODO: We need this here because this is usually a static object...
			  // otherwize CRenderPass will try to delete it when not using RenderTarget

	if(msb)
		if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, msb) == -1)
			throw error::milk("Couldn't set multi-sample buffers");
	if(mss)
		if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, mss) == -1)
			throw error::milk("Couldn't set multi-sample samples");

	if (SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, m_depth) == -1)
		throw error::milk("Couldn't set buffer depth");
	if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_zDepth) == -1)
		throw error::milk("Couldn't set z-buffer depth");

	if (m_sDepth)
		if(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, m_sDepth) == -1)
			throw error::milk("Couldn't set stencil-buffer depth");

	Uint32 sdl_flags=SDL_OPENGL;

	if (flags & FULLSCREEN)
		sdl_flags |= SDL_FULLSCREEN;

	if (flags & RESIZABLE)
		sdl_flags |= SDL_RESIZABLE;

	if (flags & NOFRAME)
		sdl_flags |= SDL_NOFRAME;

	// Try to set up the videomode
	if (!SDL_SetVideoMode(size().x, size().y, 0, sdl_flags))
		throw error::milk((string)"Couldn't set video mode, SDL-error: "+SDL_GetError());

	SDL_WM_SetCaption(title.c_str(), NULL);

	// Set Up Our Perspective GL Screen
	onResizeWindow(size().x, size().y);

	// Init glew
	int err = glewInit();
	if (err != GLEW_OK)
		throw error::milk((string)"GLEW Error: " + reinterpret_cast<const char*>(glewGetErrorString(err)));

	// Check if the extension EXT_clamp_to_edge exists (FIXME?)
	m_clamp = GLEW_ARB_texture_border_clamp ? GL_CLAMP_TO_EDGE : GL_CLAMP;

	// TEMPORARY! MULUX
	SDL_SetModState(KMOD_NONE);

	Renderer::initWindow();
}

IWindow::~IWindow()
{
	ms_instance = 0;

	Renderer::freeWindow();

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
		throw error::milk((string)"Couldn't init SDL: " + SDL_GetError());
}

void IWindow::setWindowTitle(const string& title)
{
	SDL_WM_SetCaption(title.c_str(), NULL);
}

// FIXME: hmm. Resize window?
void IWindow::onResizeWindow(int width, int height)
{
	if (Input::boundary() && Input::getBoundary() == CRect<int>(0, 0, size().x, size().y))
		Input::setBoundary(CRect<int>(0, 0, width, height));

	m_size.x = width;
	m_size.y = max(height, 1);
}

void IWindow::toggleFullscreen()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);

	if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
		throw error::milk((string)"Couldn't init SDL: " + SDL_GetError());

	// -------------

	m_fullscreen = !m_fullscreen;

	if(SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, m_depth) == -1)
		throw error::milk("Couldn't set buffer depth");
	if(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_zDepth) == -1)
		throw error::milk("Couldn't set z-buffer depth");
	if(m_sDepth)
		if(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, m_sDepth) == -1)
			throw error::milk("Couldn't set stencil-buffer depth");

	Uint32 flags = SDL_OPENGL | (m_fullscreen ? SDL_FULLSCREEN : 0);

	// Try to set up the videomode
	if(!SDL_SetVideoMode(size().x, size().y, 0, flags))
		throw error::milk("Couldn't set video mode");

	// Set Up Our Perspective GL Screen
	onResizeWindow(size().x, size().y);
}

bool IWindow::isActive() const
{
	return Input::m_active;
}

/////////////////////////////////////

bool IWindow::beginRender()
{
	++m_isRendering;
	return true;
}

void IWindow::endRender()
{
	if(!--m_isRendering)
		SDL_GL_SwapBuffers();
}

/////////////////////////////////////

void IWindow::setVerticalSync(bool enabled)
{
#ifdef WGL_EXT_swap_control
	if(WGLEW_EXT_swap_control)
		wglSwapIntervalEXT(enabled ? 1 : 0);
#endif
}


bool IWindow::getVerticalSync() const
{
#ifdef WGL_EXT_swap_control
	if(WGLEW_EXT_swap_control)
		return wglGetSwapIntervalEXT() ? true : false;
#endif
	return false;
}

/////////////////////////////////////

int IWindow::getClamp()
{
	BOOST_ASSERT(ms_instance);
	return ms_instance->m_clamp;
}

void IWindow::update()
{
	if(m_owner.m_parts & IApplication::SOUND)
		Sound::checkForErrors();
	if(m_owner.m_parts & IApplication::RENDERER)
		Renderer::checkForErrors();

	if(m_owner.m_parts & IApplication::TIMER)
		Timer::update();
	if(m_owner.m_parts & IApplication::INPUT)
		Input::update();
	if(m_owner.m_parts & IApplication::SOUND)
		Sound::update();
	if(m_owner.m_parts & IApplication::RENDERER)
		Renderer::update();
}

void IWindow::run()
{
	// Main loop
	while(!m_owner.quitState())
	{
		update();

		if(beginRender())
		{
			render();
			endRender();
		}

		++m_frame;
	}
}
