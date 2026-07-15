// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_GreyImage2nSquareSized_H
#define COM_M4NKIND_GreyImage2nSquareSized_H

#include "GreyImage.h"
#include "PackLong_WithLimits.h"

class GreyImage2nSquareSized : public GreyImage
{

public:

	GreyImage2nSquareSized(void);

	BASEOBJECT_DEFINE_VIRTUALCLASS(GreyImage2nSquareSized);

	virtual unsigned int	GetPixelWidth(){ return( 1<<mSer_2nPixelSize.Get() );  };

	virtual unsigned int	GetPixelHeight(){ return( 1<<mSer_2nPixelSize.Get() );  };

protected:

	PackLong_WithLimits	mSer_2nPixelSize;
};

#endif
