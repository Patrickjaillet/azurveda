/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_RGBAImage_H
#define COM_M4NKIND_RGBAImage_H

#include "VirtualImage.h"
#include "PackObjectReference.h"
#include "PackULong_Flags.h"
#include "PackLong_WithLimits.h"
#include "PackULong_Enums.h"
#include "PackList.h"
/*!
	\class	RGBAImage
	\ingroup BaseObjectInherited_VedaLibImage BaseObjectInheritedDocGroup
	\brief	Image that mix images of any size or color depth into a new RGBA one. 
			it uses a layer list by source image. Each layers can affect a color channel,
			it can use a HSV->RGB mixer and manages alpha channel. 
*/

class RGBAImage : public VirtualImage
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	RGBAImage(void);
	
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(RGBAImage);

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
	/*!
		\brief	Tool: make a conversion from HSV to RGB. 
		\param	t tint (hue) 0->255
		\param	s saturation 0->255 (grey->colorfull)
		\param	l luminance (Value) 0->255
		\param	R return written on 3 consecutive char.
	*/
	static inline void TslToRgb(  unsigned int t,unsigned int  s,unsigned int  l, unsigned char *R );
	/*!
		\brief	 Nested class that describes a reference to an object3D, used as element in the list.
	*/
	class ImageLayer : public PackStruct
	{
	public:
//!  true in mSer_Flags if the image use HSV->RGB mixer.
#define LayerFlag_Red		1 
#define LayerFlag_Green		2
#define LayerFlag_Blue		4 
#define LayerFlag_Alpha		8 
#define LayerFlag_HSV		16 
#define LayerFlag_AddMul	32		// true if mul, false if add. 
#define LayerFlag_MulAlpha	64	
		//! the type of layer:
		PackULong_Flags		mSer_LayerFlag;
		//! the reference to an image:
		PackObjectReference	mSerRef_ComponentImage;
		//! reference to a color-binding curve:
		PackObjectReference mSerRef_Curve;
		//! \brief constructor . 
		ImageLayer(void);  
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new object.
	*/
	static BaseType *NewImageLayer(){ return new ImageLayer(); };

/*==================================================================
								PROTECTED
==================================================================*/
protected:

	//! the pixel size:
	PackLong_WithLimits		mSer_PixelWidth;
	//! the pixel size:
	PackLong_WithLimits		mSer_PixelHeight;
	//! list of the layers used to build the image:
	PackList		mSer_LayerList;

/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
			Mix one or more GreyImage into a RGBA bitmap.
		\return false if creation failed.
*/
	virtual bool CreateInternal(void);

};

#endif
