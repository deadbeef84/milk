#ifndef MILK_ASSERT_H_
#define MILK_ASSERT_H_

#ifdef NDEBUG
	
	/// the argument is only evaluated in debug-builds
	#define milk_on_debug(x)

	/// the argument is only evaluated in release-builds
	#define milk_on_release(x) x

	// only the first argument is evaluated in release-builds
	#define milk_release_debug(r, d) r

#else

	/// the argument is only evaluated in debug-builds
	#define milk_on_debug(x) x

	/// the argument is only evaluated in release-builds
	#define milk_on_release(x)

	// only the second argument is evaluated in debug-builds
	#define milk_release_debug(r, d) d

#endif

#endif
