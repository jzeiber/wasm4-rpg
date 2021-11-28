#include "randommt.h"

RandomMT::RandomMT()
{
	tinymt64_init(&m_state,0x1);
}

RandomMT::RandomMT(const uint64_t seed)
{
	tinymt64_init(&m_state,seed);
}

RandomMT::~RandomMT()
{

}

RandomMT &RandomMT::Instance()
{
	static RandomMT r;
	return r;
}
	
void RandomMT::Seed(const uint64_t seed)
{
	tinymt64_init(&m_state,seed);
}

uint64_t RandomMT::Next()
{
	return tinymt64_generate_uint64(&m_state);
}

double RandomMT::NextDouble()
{
	return tinymt64_generate_double(&m_state);
}
