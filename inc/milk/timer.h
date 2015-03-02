#ifndef MILK_TIMER_H_
#define MILK_TIMER_H_

#include "milk/types.h"
#include "milk/iapplication.h"
#include <numeric>
#include <SDL.h>

namespace milk
{
	/// class for timing stuff
	class CTimer
	{
	public:
		// c-tor
		CTimer()
		: m_sum(0), m_scale(1), m_step(0), m_running(true)
		{ m_startStep = m_start = getTime(); }

		// Returns the time (in seconds)
		double time() const
		{ return m_sum + (m_running ? getTime() - m_start : 0.0) * m_scale; }

		// Start timer
		void start()
		{ if(m_running) { m_sum += (getTime() - m_start) * m_scale; } m_start = getTime(); m_running = true;}

		// Stop timer
		void stop()
		{ if(m_running) { m_sum += (getTime() - m_start) * m_scale; m_running = false; } }

		// Reset time
		void reset()
		{ m_sum = 0; m_start = getTime(); }

		bool isRunning() const
		{ return m_running; }

		void setScale(double scale)
		{ stop(); m_scale = scale; start(); }

		double getScale() const
		{ return m_scale; }

		// Set time
		void set(double time)
		{ m_sum = time; m_start = getTime(); }

		// "interval step"
		double step()
		{ m_step = time() - m_startStep; m_startStep = time(); return m_step; }

		// Get last "interval time"
		double dt() const
		{ return m_step; }

	private:
		double getTime() const
		{ return (double)SDL_GetTicks() / 1000.0; }

		double	m_sum;
		double	m_start;
		double	m_scale;
		double	m_startStep;
		double	m_step;
		bool	m_running;
	};

	/// static class for getting the current time, fps etc
	class Timer
	{
	public:
		static double time()
		{ return m_pTimer->time(); }

		static double avarage_fps()
		{ return m_fps; }

		static double fps()
		{ return 1.0/frametime(); }

		static double frametime()
		{ return m_avgTime[m_avgPos % m_steps]; }

		static void update()
		{
			++m_avgPos;
			m_avgTime[m_avgPos % m_steps] = m_pTimer->step();
			m_fps = m_steps / std::accumulate(m_avgTime, m_avgTime+m_steps, 0.0);
		}

	private:
		static uint		m_avgPos;
		static double	*m_avgTime;
		static double	m_fps;
		static CTimer*	m_pTimer;
		static int		m_steps;

		// Prevent construction
		Timer() { }

		static void init()
		{
			m_steps = IApplication::getInstance()->m_steps;
			m_pTimer = new CTimer();
			m_avgTime = new double[m_steps];
			std::fill_n(m_avgTime, m_steps, 0);
		}

		static void free()
		{ delete m_pTimer; delete [] m_avgTime; }

		friend class IWindow;
		friend class IApplication;
	};
}

#endif
