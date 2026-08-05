#ifndef _NMWEXCEPTION
#define _NMWEXCEPTION

#include "types.h"
#include "exception" //#include <exception>

#ifdef __cplusplus
extern "C" {
#endif

typedef s16 vbase_ctor_arg_type;
typedef char local_cond_type;

typedef struct CatchInfo {
	void* location;
	void* typeinfo;
	void* dtor;
	void* sublocation;
	s32 pointercopy;
	void* stacktop;
} CatchInfo;

void __unregister_fragment(int fragmentID);
// struct __eti_init_info* info
int __register_fragment(void* info, char* TOC);
void* __register_global_object(void* object, void* destructor, void* regmem);
void __destroy_global_chain(void);
extern char __throw_catch_compare(const char* throwtype, const char* catchtype, s32* offset_result);

#ifdef __cplusplus
}
#endif

#endif // _NMWEXCEPTION
