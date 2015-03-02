#include "milk/renderer.h"
#include "milk/includes.h"
#include "milk/input.h"
#include "milk/error.h"
#include "milk/helper.h"
#include "milk/boost.h"
#include "milk/renderer/cmaterial.h"
#include "milk/renderer/cappearance.h"
#include "milk/renderer/imodel.h"
#include "milk/scenegraph/ccamera.h"
#include <algorithm>
using namespace milk;
using namespace std;

RendererStatistics Renderer::ms_statistics;
RendererStatistics Renderer::ms_lastStatistics;

/////////////////////////////////////////////////////////

void Renderer::init()
{
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
		throw error::milk((string)"Couldn't init SDL: " + SDL_GetError());
}

void Renderer::initWindow()
{
	checkForErrors();

	CAppearance::init();

	IModel::init();
}

void Renderer::freeWindow()
{
	checkForErrors();

	IModel::free();
	checkForErrors();

	CAppearance::free();
	checkForErrors();
}

void Renderer::free()
{
	// Quit SDL
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Renderer::update()
{
	ms_lastStatistics = ms_statistics;
	ms_statistics = RendererStatistics();
}

bool Renderer::isSuported(std::string ext)
{
	//return glewGetExtension(reinterpret_cast<const GLubyte*>(ext.c_str())) ? true : false;
	return glewGetExtension(ext.c_str()) ? true : false;
}

void Renderer::checkForErrors()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		const char *errorString = reinterpret_cast<const char*>(gluErrorString(error));
		throw error::opengl("OpenGL-error " + boost::lexical_cast<string>(static_cast<uint>(error)) + ": " + string(errorString?errorString:""), static_cast<uint>(error));
	}
}
