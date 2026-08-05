#include "types.h"
#include "PowerPC_EABI_Support/Runtime/NMWException.h"
#include "PowerPC_EABI_Support/Runtime/__ppc_eabi_linker.h"
#include "PowerPC_EABI_Support/Runtime/__init_cpp_exceptions.h"

static int fragmentID = -2;

extern char *GetR2() ;

void __init_cpp_exceptions()
{
	if ((s32)fragmentID == -2) {
		char* R2   = GetR2();
		fragmentID = __register_fragment(&_eti_init_info, R2);
	}
}

// clang-format on
void __fini_cpp_exceptions()
{
	if ((s32)fragmentID != -2) {
		__unregister_fragment(fragmentID);
		fragmentID = -2;
	}
}

// clang-format off
asm char* GetR2() 
{ 
	nofralloc 
	mr r3, r2 
	blr 
}

__declspec(section ".ctors") extern void* const __init_cpp_exceptions_reference  = __init_cpp_exceptions;
__declspec(section ".dtors") extern void* const __destroy_global_chain_reference = __destroy_global_chain;
__declspec(section ".dtors") extern void* const __fini_cpp_exceptions_reference  = __fini_cpp_exceptions;