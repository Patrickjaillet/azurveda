/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_GreyImage_H
#define COM_M4NKIND_GreyImage_H

#include "VirtualImage.h"

/*!
	\class	GreyImage
	\ingroup BaseObjectInherited_VedaLibImage BaseObjectInheritedDocGroup
	\brief	1 byte component-per-pixel Images. Can then be used or mixed by RGBAImage for a color image.
			Still, it is virtual because size can then be given on classes that manage the type of size of the Bitmap.
*/

class GreyImage : public VirtualImage
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	//GreyImage(void);
	
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(GreyImage);

	/*!
		\brief	Return the number of byte component for one pixel.
			This one is no more a virtual method.
	*/
	virtual unsigned int	GetByteDepth(){ return(1); };

/*==================================================================
								PROTECTED
==================================================================*/
protected:

};

#endif
