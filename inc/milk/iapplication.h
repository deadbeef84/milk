#ifndef MILK_IAPPLICATION_H_
#define MILK_IAPPLICATION_H_

#include "milk/types.h"
#include "milk/helper.h"
#include <vector>

namespace UseMilkProfiler { class Profiler; }

namespace milk
{
	
	class IApplication
	{
		friend class IWindow;
	public:
		enum Parts
		{
			SOUND	= 1<<0,
			NET		= 1<<1,
			CDROM	= 1<<2,
			INPUT	= 1<<3,
			TIMER	= 1<<4,
			RENDERER= 1<<5
		};

		IApplication(int argc, char *argv[], Parts parts = Parts(0), uint nsteps=30);
		virtual ~IApplication();

		virtual void run() = 0;

		bool quitState() const				{ return m_quitState; }
		void setQuitState(bool quit=true)	{ m_quitState=quit; }

		static IApplication *getInstance()
		{ return ms_instance; }

		const std::vector<char*>& getArguments() const
		{ return m_arguments; }

	private:
		IApplication(IApplication&);
		IApplication& operator=(IApplication&);

		std::vector<char*> m_arguments;
		bool m_quitState;
		Parts m_parts;
		uint m_steps; //used by Profiler and Timer

		friend class Timer;
		friend class UseMilkProfiler::Profiler;

		static IApplication *ms_instance;
	};
	MILK_FIX_ENUM(IApplication::Parts);
}

#endif
