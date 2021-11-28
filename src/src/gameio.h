#pragma once

#include <stdint.h>

namespace GameIO
{

uint8_t read_uint8_t(const uint8_t *data);
void write_uint8_t(uint8_t *data, uint8_t val);

uint16_t read_uint16_t(const uint8_t *data);
void write_uint16_t(uint8_t *data, uint16_t val);
int16_t read_int16_t(const uint8_t *data);
void write_int16_t(uint8_t *data, int16_t val);

uint32_t read_uint32_t(const uint8_t *data);
void write_uint32_t(uint8_t *data, uint32_t val);

uint64_t read_uint64_t(const uint8_t *data);
void write_uint64_t(uint8_t *data, uint64_t val);
int64_t read_int64_t(const uint8_t *data);
void write_int64_t(uint8_t *data, int64_t val);

};