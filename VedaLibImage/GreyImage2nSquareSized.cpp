/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "GreyImage2nSquareSized.h"

BASEOBJECT_DECLARE_VIRTUALCLASS("2n", GreyImage2nSquareSized, GreyImage );

GreyImage2nSquareSized::GreyImage2nSquareSized() : GreyImage()
{
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_2nPixelSize , "2PowerN Size",8,1,9 );

}