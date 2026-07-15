// SPDX-License-Identifier: LGPL-2.1-only

#include "ProceduralImage.h"

BASEOBJECT_DECLARE_VIRTUALCLASS( "Proc", ProceduralImage, GreyImage2nSquareSized );

ProceduralImage::ProceduralImage() : GreyImage2nSquareSized()
{
	REGISTER_MEMBER( mSer_RandomSeed , "RandomSeed" );
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_Recursion , "Recursion",2,0,8 );

}

bool ProceduralImage::CreateInternal(void)
{

	if( !GreyImage2nSquareSized::CreateInternal() ) return(false);

	mSer_RandomSeed.Reset();
	return(true);
}
