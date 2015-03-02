#ifdef MILK_PACK8ENABLE_H_
#	undef MILK_PACK8ENABLE_H_
#	undef MILK_PACK_STRUCT

#	if defined( _MSC_VER ) || defined( __BORLANDC__ )
#		pragma pack(pop)
#		pragma warning(disable:4103)
#	elif defined( __GNUC__ )
#	else
#		error Do not know how to reset struct alignment, unsupported compiler
#	endif

#else
#	error packing was NOT enabled
#endif
