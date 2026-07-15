#include "PackULong_RandomSeed.h"
#ifdef _ENGINE_EDITABLE_
PackULong_RandomSeed::PackULong_RandomSeed(void): PackULong()
{

}
#endif
#ifdef _ENGINE_EDITABLE_
PackULong_RandomSeed::~PackULong_RandomSeed(void)
{
}
#endif

unsigned int	PackULong_RandomSeed::GetRandomNumber()
{
	unsigned int nn = m_CurrentRandomValue;
	m_CurrentRandomValue = (unsigned int) ((0x79a55cb5 + m_CurrentRandomValue) * 0x3ab597a7) ;
	return(nn>>8);
}
