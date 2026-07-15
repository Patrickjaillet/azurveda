/*! \file 
	\author victorien ferry & www.m4nkind.com 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_ImageJPEG_H
#define COM_M4NKIND_ImageJPEG_H

#include "VirtualImage.h"
#include "PackResource.h"
#include "PackULong_Flags.h"
/*!
	\class	ImageJPEG
	\ingroup BaseObjectInherited_VedaLibImageJPEG
	\brief	Use JPEG file format to manage an image.
*/

class ImageJPEG : public VirtualImage
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	ImageJPEG(void);
	
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(ImageJPEG);

	/*!
		\brief	Return the number of byte component for one pixel.
			This one is no more a virtual method.
	*/
	virtual unsigned int	GetByteDepth(){ return(4); };
	/*!
		\brief	Return the pixel width of the image
	*/
	virtual unsigned int	GetPixelWidth();
	/*!
		\brief	Return the pixel height of the image
	*/
	virtual unsigned int	GetPixelHeight();

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! resource file:
	PackResource	mSer_JPEGFile;
	//! flags for alpha management.
	enum {
		b_GreyToAlpha=1
	};
	//! flags for alpha management.
	PackULong_Flags	mSer_Flags;

	//! the pixel size, computed from file:
	unsigned int		m_PixelWidth;
	//! the pixel size, computed from file:
	unsigned int		m_PixelHeight;
/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
			Mix one or more GreyImage into a RGBA bitmap.
		\return false if creation failed.
*/
	virtual bool CreateInternal(void);

};

#endif
