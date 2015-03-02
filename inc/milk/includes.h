#ifndef MILK_INCLUDES_H_
#define MILK_INCLUDES_H_

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	define WIN32_EXTRA_LEAN
#	define NOMINMAX		// don't let windows.h define min and max macros.
#	include <winsock2.h>
#	include <windows.h>
/*
#   if defined(_MSC_VER)
#	  include <eh.h> // exception handling
#   endif
#	if defined(_MSC_VER) && !defined(MILK_BUILD_LIB)
#		pragma comment(lib, "glew32.lib")		// OpenGL-libraries
#		pragma comment(lib, "opengl32.lib")
#		pragma comment(lib, "glu32.lib")
#		pragma comment(lib, "SDL.lib")			// SDL-libraries
#		pragma comment(lib, "SDLmain.lib")
#		pragma comment(lib, "SDL_image.lib")
#		pragma comment(lib, "wsock32.lib")		// for milk::Net
#		pragma comment(lib, "ALut.lib")			// OpenAL-libraries
#		pragma comment(lib, "OpenAL32.lib")
#	endif
*/
#	define GLPREFIX "wgl"
#	define GLEWOSFILE <GL/wglew.h>
#else
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <unistd.h>
#	include <netdb.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	define GLPREFIX "glX"
#	define GLEWOSFILE <GL/glxew.h>
#endif

///////////////////////////////////////////////////

// OpenGL includes
#include <GL/glew.h>
#include GLEWOSFILE

///////////////////////////////////////////////////

#endif
