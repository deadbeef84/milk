#include "milk/timer.h"
using namespace milk;

uint	Timer::m_avgPos		= 0;
double*	Timer::m_avgTime	= 0;
double	Timer::m_fps		= 0;
CTimer*	Timer::m_pTimer		= 0;
int		Timer::m_steps		= 0;
