#include "outputstringstream.h"
#include "printf.h"

OutputStringStream::OutputStringStream():m_buffer{""},m_pos(0)
{

}

OutputStringStream::~OutputStringStream()
{

}

void OutputStringStream::Clear()
{
    m_buffer[0]='\0';
    m_pos=0;
}

char *OutputStringStream::Buffer()
{
    return m_buffer;
}

void OutputStringStream::Concat(const char *str)
{
    if(str)
    {
        int16_t i=0;
        while(m_pos<255 && str[i])
        {
            m_buffer[m_pos++]=str[i++];
        }
        m_buffer[m_pos]='\0';
    }
}

OutputStringStream &OutputStringStream::operator<<(const char *str)
{
    Concat(str);
    return *this;
}

OutputStringStream &OutputStringStream::operator<<(const char c)
{
    if(m_pos<255)
    {
        m_buffer[m_pos++]=c;
    }
    m_buffer[m_pos]='\0';
    return *this;
}

OutputStringStream &OutputStringStream::operator<<(const uint32_t val)
{
    return *this << static_cast<uint64_t>(val);
}

OutputStringStream &OutputStringStream::operator<<(const int32_t val)
{
    return *this << static_cast<int64_t>(val);
}

OutputStringStream &OutputStringStream::operator<<(const uint64_t val)
{
    if(m_pos<255)
    {
        char buff[32];
        snprintf(buff,31,"%llu",val);
        buff[31]='\0';
        Concat(buff);
    }
    return *this;
}

OutputStringStream &OutputStringStream::operator<<(const int64_t val)
{
    if(m_pos<255)
    {
        char buff[32];
        snprintf(buff,31,"%lli",val);
        buff[31]='\0';
        Concat(buff);
    }
    return *this;
}

OutputStringStream &OutputStringStream::operator<<(const float val)
{
    if(m_pos<255)
    {
        char buff[32];
        snprintf(buff,31,"%f",val);
        buff[31]='\0';
        Concat(buff);
    }
    return *this;
}
