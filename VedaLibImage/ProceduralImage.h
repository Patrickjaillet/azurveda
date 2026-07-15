// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_ProceduralImage_H
#define COM_M4NKIND_ProceduralImage_H

#include "GreyImage2nSquareSized.h"
#include "PackULong_RandomSeed.h"
#include "PackLong_WithLimits.h"

class ProceduralImage : public GreyImage2nSquareSized
{

public:

	ProceduralImage(void);

	BASEOBJECT_DEFINE_VIRTUALCLASS(ProceduralImage);

protected:

	PackULong_RandomSeed	mSer_RandomSeed;

	PackLong_WithLimits		mSer_Recursion;

	virtual bool CreateInternal(void);

};

#endif
