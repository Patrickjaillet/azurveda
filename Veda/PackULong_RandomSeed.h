// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackULong_RandomSeed_H
#define COM_M4NKIND_PackULong_RandomSeed_H

#include "PackULong.h"

class PackULong_RandomSeed : public PackULong
{

public:
#ifdef _ENGINE_EDITABLE_

	PackULong_RandomSeed(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual ~PackULong_RandomSeed(void);
#endif

	inline void	Reset(){ m_CurrentRandomValue= m_value ;  };

	unsigned int	GetRandomNumber();

protected:
	unsigned int	m_CurrentRandomValue;
};

#endif
