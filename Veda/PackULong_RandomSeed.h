/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackULong_RandomSeed_H
#define COM_M4NKIND_PackULong_RandomSeed_H

#include "PackULong.h"

/*!
	\class	PackULong_RandomSeed
	\ingroup	BaseSerializableClass
	\brief	 PackULong_RandomSeed specializes PackULong to express a random number seed generator,
			and provide a random function, for procedural purposes.
			PackULong is a base type managing "28 bit unsigned long", which serializes a number
			between [0,512Mb-1], which can be written on 1, 2,3 or 4 bytes when needed.			
*/
class PackULong_RandomSeed : public PackULong
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Constructor. 
	*/
	PackULong_RandomSeed(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackULong_RandomSeed(void);
#endif
	/*!
		\brief	Reset the random list to its begining as defined by its seed number.
	*/
	inline void	Reset(){ m_CurrentRandomValue= m_value ;  };

	/*!
		\brief get a random number, continuing the suit of random number shaped by the seed.
	*/
	unsigned int	GetRandomNumber();	
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	unsigned int	m_CurrentRandomValue;
};

#endif
