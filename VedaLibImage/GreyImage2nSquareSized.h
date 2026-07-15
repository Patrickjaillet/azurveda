/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_GreyImage2nSquareSized_H
#define COM_M4NKIND_GreyImage2nSquareSized_H

#include "GreyImage.h"
#include "PackLong_WithLimits.h"
/*!
	\class	GreyImage2nSquareSized
	\ingroup BaseObjectInherited_VedaLibImage BaseObjectInheritedDocGroup
	\brief	Virtual Class for Grey Image that exactly size a 2-Power-N square sized image.
			Some image generation Algorythms implies to use such images.
*/
class GreyImage2nSquareSized : public GreyImage
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	GreyImage2nSquareSized(void);
	
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(GreyImage2nSquareSized);

	/*!
		\brief	Return the pixel width of the image
	*/
	virtual unsigned int	GetPixelWidth(){ return( 1<<mSer_2nPixelSize.Get() );  };
	/*!
		\brief	Return the pixel height of the image
	*/
	virtual unsigned int	GetPixelHeight(){ return( 1<<mSer_2nPixelSize.Get() );  };

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the square 2n pixel size:
	PackLong_WithLimits	mSer_2nPixelSize;
};

#endif
