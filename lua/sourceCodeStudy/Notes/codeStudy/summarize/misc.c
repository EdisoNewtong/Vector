//////////////////////////////////////////////////
// ?? lu_int32 => unsigned int
//////////////////////////////////////////////////

typedef LUAI_UINT32 lu_int32;     // llimits.h:18
#define LUAI_UINT32	unsigned int  // luaconf.h:414

lu_int32 <==> unsigned int


//////////////////////////////////////////////////
// ?? lu_byte => unsigned char
//////////////////////////////////////////////////

/* chars used as small naturals (so that `char' is reserved for characters) */
typedef unsigned char lu_byte;  // llimits.h:27
lu_byte  => unsigned char
