#ifndef MILK_RENDERER_H_
#define MILK_RENDERER_H_

#include <string>
#include "milk/types.h"
#include "milk/includes.h"

namespace milk
{
	struct RendererStatistics
	{
		RendererStatistics()
			: triangles(0), calls(0)
		{ }

		size_t triangles;
		size_t bytes;
		size_t calls;
	};

	/// class for controlling the rendering
	class Renderer
	{
		friend class IApplication;
		friend class IWindow;
	public:

		/// checks if a certain extension is supported
		static bool isSuported(std::string ext);

		/// throws an error::opengl on opengl-errors
		static void checkForErrors();

		static void update();

		static RendererStatistics& setStatistics()
		{ return ms_statistics; }

		static const RendererStatistics& statistics()
		{ return ms_lastStatistics; }

		static std::string vendor()
		{ return (char*)glGetString(GL_VENDOR); }
		static std::string version()
		{ return (char*)glGetString(GL_VERSION); }
		static std::string renderer()
		{ return (char*)glGetString(GL_RENDERER); }
		static std::string extensions()
		{ return (char*)glGetString(GL_EXTENSIONS); }
		static std::string shadingVersion()
		{ return (char*)glGetString(GL_SHADING_LANGUAGE_VERSION); }

	private:
		Renderer() { }

		static RendererStatistics ms_statistics, ms_lastStatistics;

		static void init();
		static void initWindow();
		static void free();
		static void freeWindow();
	};
}

#endif
