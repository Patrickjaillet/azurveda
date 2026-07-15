/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "ProceduralImage.h"

BASEOBJECT_DECLARE_VIRTUALCLASS( "Proc", ProceduralImage, GreyImage2nSquareSized );

ProceduralImage::ProceduralImage() : GreyImage2nSquareSized()
{
	REGISTER_MEMBER( mSer_RandomSeed , "RandomSeed" );
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_Recursion , "Recursion",2,0,8 );

}

/*!
	\brief	Method that really build the object using the serializable parameters.
			For the virtual ProceduralImage class, it reset a random seed number which defines a shape.
			the superclass VirtualImage allocates the image memory.
			Close() should close everything opened by Create().
*/
bool ProceduralImage::CreateInternal(void)
{
	// important: do that for every objects:
	if( !GreyImage2nSquareSized::CreateInternal() ) return(false);
	// reset the random seed to start before bitmap creation:
	mSer_RandomSeed.Reset();
	return(true);
}