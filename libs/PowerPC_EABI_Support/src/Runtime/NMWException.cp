#include "PowerPC_EABI_Support/Runtime/NMWException.h"
#include "PowerPC_EABI_Support/Runtime/MWCPlusPlusLib.h"

#pragma exceptions on

extern "C"
{
	void abort();
}

namespace std
{
	static void dthandler()
	{
		abort();
	}

	terminate_handler thandler = dthandler;

	static void duhandler()
	{
		terminate();
	}

	unexpected_handler uhandler = duhandler;

	extern void terminate()
	{
		thandler();
	}	

	extern void unexpected() {
		uhandler();
	}
}

extern "C" char __throw_catch_compare(const char* throwtype, const char* catchtype, s32* offset_result) {

}

extern "C" void *__construct_new_array(void *block, ConstructorDestructor ctor, ConstructorDestructor dtor_arg, size_t size, size_t n)
{

}

class __partial_array_destructor
{
private:
	void *p;
	size_t size;
	size_t n;
	void *dtor;

public:
	size_t i;

	__partial_array_destructor(void *array, size_t elementsize, size_t nelements, void *destructor)
	{
		p = array;
		size = elementsize;
		n = nelements;
		dtor = destructor;
		i = n;
	}

	~__partial_array_destructor()
	{
		char *ptr;

		if (i < n && dtor)
		{
			for (ptr = (char *)p + size * i; i > 0; i--)
			{
				ptr -= size;
				DTORCALL_COMPLETE(dtor, ptr);
			}
		}
	}
};


extern "C" void __construct_array(void *ptr, void *ctor, void *dtor, size_t size, size_t n)
{

}

extern "C" void __destroy_arr(void *block, ConstructorDestructor *dtor, size_t size, size_t n)
{

}
