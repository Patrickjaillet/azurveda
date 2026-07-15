/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_BubbleGreyImage_H
#define COM_M4NKIND_BubbleGreyImage_H

#include "ProceduralImage.h"
#include "PackULong_Flags.h"
#include "PackObjectReference.h"
/*!
	\class	BubbleGreyImage
	\ingroup BaseObjectInherited_VedaLibImage BaseObjectInheritedDocGroup
	\brief	Class able to build some grey, 2n-pixelSized, bubble Image...
*/

class BubbleGreyImage : public ProceduralImage
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	BubbleGreyImage(void);
	
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(BubbleGreyImage);


/*==================================================================
								PROTECTED
==================================================================*/
protected:
	enum {
		BUBFLAG_OneColorByRegion=1,
		BUBFLAG_Multiply=2
	};
	//! property flags for modes:
	PackULong_Flags		mSer_Flags;
	//! reference to a sub-image to use.
	PackObjectReference	mSer_SubImage;
	/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
			It uses a 9-neighbour-case-test algorithm to make bubbles.
			1 bitmap is used.
		\return false if creation failed.
*/
	virtual bool CreateInternal(void);

	int					m_RandImageSeed;
	int					FindDistance( int distx, int disty );

	unsigned int		FindShiftX( int distx, int disty );
	unsigned int		FindShiftY( int distx, int disty );

};

#endif
