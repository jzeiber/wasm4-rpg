#include <stdint.h>
#include <stddef.h>
#include "wasmmemcpy.h"

#include "wasm4.h"

void _putchar(char d)
{
	//char o=d;
}

int __cxa_atexit(void (*func) (void *), void * arg, void * dso_handle)
{
	/*
	trace("__cxa_atexit()");
	func;
	arg;
	dso_handle;
	*/
	return 0;
}

void __cxa_pure_virtual()
{
	//trace("__cxa_pure_virtual()");
}
