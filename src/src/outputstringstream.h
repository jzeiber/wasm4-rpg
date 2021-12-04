#pragma once

#include <stdint.h>

class OutputStringStream
{
public:
    OutputStringStream();
    ~OutputStringStream();

    void Clear();

    char *Buffer();

    OutputStringStream &operator<<(const char *str);
    OutputStringStream &operator<<(const char c);
    OutputStringStream &operator<<(const uint32_t val);
    OutputStringStream &operator<<(const int32_t val);
    OutputStringStream &operator<<(const uint64_t val);
    OutputStringStream &operator<<(const int64_t val);
    OutputStringStream &operator<<(const float val);

private:
    char m_buffer[256];
    int16_t m_pos;

    void Concat(const char *str);
};