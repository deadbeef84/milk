#ifndef MILK_RANDOM_H_
#define MILK_RANDOM_H_

#include <cstdlib>
#include <ctime>
#include "milk/boost.h"
#include "milk/types.h"

namespace milk
{
	/// class for creating random numbers
	class CRandom
	{
	public:
		CRandom(Sint32 s = time(0)) : m_distributor(m_generator)
		{
			seed(s);
		}

		/// seeds the randomness
		void seed(Sint32 s = time(0))
		{
			m_generator.seed(boost::int32_t(s));
		}

		void seed(Uint64 s)
		{
			m_generator.seed(boost::uint64_t(s));
		}

		/// returns a rational number in the interval [0, 1)
		double rational()
		{
			return m_distributor();
		}

		/// gives you a number in the range [first, last)
		template<class F>
		F range(F first, F last)
		{
			return static_cast<F>(first + rational()*(last-first));
		}

	private:
		typedef boost::rand48 generator;
		generator m_generator;
		boost::uniform_01<generator> m_distributor;
	};
}

#endif
