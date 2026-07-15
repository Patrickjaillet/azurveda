// SPDX-License-Identifier: LGPL-2.1-only

#include "GreyImage2nSquareSized.h"

BASEOBJECT_DECLARE_VIRTUALCLASS("2n", GreyImage2nSquareSized, GreyImage );

GreyImage2nSquareSized::GreyImage2nSquareSized() : GreyImage()
{
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_2nPixelSize , "2PowerN Size",8,1,9 );

}
