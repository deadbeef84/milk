#include "milk/iapplication.h"
#include "milk/includes.h"
#include "milk/timer.h"
#include "milk/input.h"
#include "milk/obsolete/milk_profiler.h"
#include "milk/renderer.h"
#include "milk/audio/sound.h"
#include "milk/net/net.h"
#include "milk/obsolete/milk_cdrom.h"
#include "milk/random.h"

using namespace milk;
using namespace std;

IApplication *IApplication::ms_instance = 0;

//ovveride the default behavior for std::terminate
void terminator()
{
	try
	{
		CLog* log = error::milk::getLog();
		if (log)
		{
			*log << LERROR << "Corrupt stack-unwinding or uncaught exception." << endl;
		}
	}
	catch (...)
	{
	}

	abort();
}

IApplication::IApplication(int argc, char *argv[], Parts parts, uint nsteps)
	: m_arguments(argv+1, argv+argc), m_quitState(false), m_parts(parts), m_steps(nsteps)
{
#ifndef NDEBUG
	set_terminate(terminator);
#endif

	BOOST_ASSERT(!ms_instance);
	ms_instance = this;

#ifdef MILK_EH_TRANSLATOR
	_set_se_translator(my_translator);
#endif

	ExceptionReporter::init();

	// Initialize SDL
	SDL_Init(SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE);

	Timer::init();
	Input::init();
	UseMilkProfiler::Profiler::init();

	if (m_parts&RENDERER)
		Renderer::init();
	if (m_parts&SOUND)
		Sound::init();
	if (m_parts&NET)
		Net::init();
	if (m_parts&CDROM)
		CDRom::init();
}

IApplication::~IApplication()
{
	BOOST_ASSERT(ms_instance);
	ms_instance = 0;

	if (m_parts&CDROM)
		CDRom::free();
	if (m_parts&NET)
		Net::free();
	if (m_parts&SOUND)
		Sound::free();
	if (m_parts&RENDERER)
		Renderer::free();

	UseMilkProfiler::Profiler::free();
	Input::free();
	Timer::free();

	// Quit SDL
	SDL_Quit();

	ExceptionReporter::free();
}
