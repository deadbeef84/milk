#ifndef MILK_PACK8ENABLE_H_
#	define MILK_PACK8ENABLE_H_
#	if defined( _MSC_VER ) || defined( __BORLANDC__ )
#		pragma warning(disable:4103)
#		pragma pack(push)
#		pragma pack(1)
#		define MILK_PACK_STRUCT
#	elif defined( __GNUC__ )
#		define MILK_PACK_STRUCT	__attribute__((packed))
#	else
#		error Do not know how to set struct alignment, unsupported compiler
#	endif

#else
#	error packing already enabled
#endif