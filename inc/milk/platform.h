#ifndef MILK_PLATFORM_H_
#define MILK_PLATFORM_H_

// stricmp does not exist in unix, fix
#ifndef WIN32
#	define stricmp strcasecmp
#endif

#endif
