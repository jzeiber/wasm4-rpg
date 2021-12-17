#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memmove(void *dst, const void *src, size_t len);

#ifdef __cplusplus
}
#endif
