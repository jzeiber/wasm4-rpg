#include <stdint.h>
#include "wasm4.h"

void operator delete(void * p) // or delete(void *, std::size_t)
{
  //trace("operator delete(void *)");
}
