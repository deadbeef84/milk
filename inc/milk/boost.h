#ifndef MILK_BOOST_H_
#define MILK_BOOST_H_

#ifdef _MSC_VER
#	pragma warning(push, 1) // set warning level 1
#	pragma warning(disable: 4701)
#endif

#ifdef NDEBUG
#	define BOOST_DISABLE_ASSERTS
#else
#	define BOOST_ENABLE_ASSERT_HANDLER
#endif

#include <boost/lexical_cast.hpp>
#include <boost/static_assert.hpp>
#include <boost/current_function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>

#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#endif
