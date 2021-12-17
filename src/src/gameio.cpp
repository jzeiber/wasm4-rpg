#include "gameio.h"

namespace GameIO
{

uint8_t read_uint8_t(const uint8_t *data)
{
    return data[0];
}

void write_uint8_t(uint8_t *data, uint8_t val)
{
    data[0]=val;
}

int8_t read_int8_t(const uint8_t *data)
{
    return (int8_t)data[0];
}

void write_int8_t(uint8_t *data, int8_t val)
{
    data[0]=(uint8_t)val;
}

uint16_t read_uint16_t(const uint8_t *data)
{
    uint16_t val=0;
    val=static_cast<uint64_t>(data[0]) << 8;
    val|=static_cast<uint64_t>(data[1]);
    return val;
}

void write_uint16_t(uint8_t *data, uint16_t val)
{
    data[0]=(val >> 8) & 0xff;
    data[1]=val & 0xff;
}

int16_t read_int16_t(const uint8_t *data)
{
    int16_t val=0;
    val=static_cast<int64_t>(data[0]) << 8;
    val|=static_cast<int64_t>(data[1]);
    return val;
}

void write_int16_t(uint8_t *data, int16_t val)
{
    data[0]=(val >> 8) & 0xff;
    data[1]=val & 0xff;
}

uint32_t read_uint32_t(const uint8_t *data)
{
    uint32_t val=0;
    val=static_cast<uint32_t>(data[0]) << 24;
    val|=static_cast<uint32_t>(data[1]) << 16;
    val|=static_cast<uint32_t>(data[2]) << 8;
    val|=static_cast<uint32_t>(data[3]);
    return val;
}

void write_uint32_t(uint8_t *data, uint32_t val)
{
    data[0]=(val >> 24) & 0xff;
    data[1]=(val >> 16) & 0xff;
    data[2]=(val >> 8) & 0xff;
    data[3]=val & 0xff;
}

int32_t read_int32_t(const uint8_t *data)
{
    uint32_t val=0;
    val=static_cast<int32_t>(data[0]) << 24;
    val|=static_cast<int32_t>(data[1]) << 16;
    val|=static_cast<int32_t>(data[2]) << 8;
    val|=static_cast<int32_t>(data[3]);
    return val;
}

void write_int32_t(uint8_t *data, int32_t val)
{
    data[0]=(val >> 24) & 0xff;
    data[1]=(val >> 16) & 0xff;
    data[2]=(val >> 8) & 0xff;
    data[3]=val & 0xff;
}

uint64_t read_uint64_t(const uint8_t *data)
{
    uint64_t val=0;
    val=static_cast<uint64_t>(data[0]) << 56;
    val|=static_cast<uint64_t>(data[1]) << 48;
    val|=static_cast<uint64_t>(data[2]) << 40;
    val|=static_cast<uint64_t>(data[3]) << 32;
    val|=static_cast<uint64_t>(data[4]) << 24;
    val|=static_cast<uint64_t>(data[5]) << 16;
    val|=static_cast<uint64_t>(data[6]) << 8;
    val|=static_cast<uint64_t>(data[7]);
    return val;
}

void write_uint64_t(uint8_t *data, uint64_t val)
{
    data[0]=(val >> 56) & 0xff;
    data[1]=(val >> 48) & 0xff;
    data[2]=(val >> 40) & 0xff;
    data[3]=(val >> 32) & 0xff;
    data[4]=(val >> 24) & 0xff;
    data[5]=(val >> 16) & 0xff;
    data[6]=(val >> 8) & 0xff;
    data[7]=val & 0xff;
}

int64_t read_int64_t(const uint8_t *data)
{
    int64_t val=0;
    val=static_cast<int64_t>(data[0]) << 56;
    val|=static_cast<int64_t>(data[1]) << 48;
    val|=static_cast<int64_t>(data[2]) << 40;
    val|=static_cast<int64_t>(data[3]) << 32;
    val|=static_cast<int64_t>(data[4]) << 24;
    val|=static_cast<int64_t>(data[5]) << 16;
    val|=static_cast<int64_t>(data[6]) << 8;
    val|=static_cast<int64_t>(data[7]);
    return val;
}

void write_int64_t(uint8_t *data, int64_t val)
{
    data[0]=(val >> 56) & 0xff;
    data[1]=(val >> 48) & 0xff;
    data[2]=(val >> 40) & 0xff;
    data[3]=(val >> 32) & 0xff;
    data[4]=(val >> 24) & 0xff;
    data[5]=(val >> 16) & 0xff;
    data[6]=(val >> 8) & 0xff;
    data[7]=val & 0xff;
}

}