/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_ImplicitEquationImage_H
#define COM_M4NKIND_ImplicitEquationImage_H

#include "VirtualImage.h"
#include "PackLong_WithLimits.h"
#include "PackObjectReference.h"
#include "PackULong_Flags.h"
#include "PackULong_Enums.h"
/*!
	\class	ImplicitEquationImage
	\ingroup BaseObjectInherited_VedaLibImage BaseObjectInheritedDocGroup
	\brief	virtual base Class able to define an image defined by an equation:
*/

class ImplicitEquationImage : public VirtualImage
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	ImplicitEquationImage(void);
	
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(ImplicitEquationImage);
	/*!
		\brief	Return the pixel width of the image
	*/
	virtual unsigned int	GetPixelWidth();
	/*!
		\brief	Return the pixel height of the image
	*/
	virtual unsigned int	GetPixelHeight();
	/*!
		\brief	Return the number of byte component for one pixel.
			This one is no more a virtual method.
	*/
	virtual unsigned int	GetByteDepth();
	/*
		\brief	If IsTimeDynamic() is true, this image can change with time.
				You have to use UpdateToFrame()to make the image change to a given date.
		\param	_framedate time in seconds.
	*/
	virtual	void	UpdateToFrame(float _framedate);

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	enum {
		eflag_TimeDynamic=1
	}; // it's a 1,2,4,8,16,32 bitfield enum !!!
	//! preference flags about the way the quation is used.
	PackULong_Flags			mSer_EquaFlags;
	enum {
		etype_XToGrey=0,
		etype_XYZToRGBA, // no RGBA special enum anymore: depends on equation, and 1.0 is default.
	}; //
	//! preference flags about the way the quation is used.
	PackULong_Enums			mSer_ResultTypeEnum;
	//! the pixel size:
	PackLong_WithLimits		mSer_PixelWidth;
	//! the pixel size:
	PackLong_WithLimits		mSer_PixelHeight;
	//! reference to an equation that can use X,Y:
	PackObjectReference		mSerRef_Equation;
	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by Create().
				Create the whole bitmap using one equation.
			\return false if creation failed.
	*/
	virtual bool CreateInternal(void);
	/*
		\brief the real draw method is kept protected, because used by construction and update:
		\param	_framedate
	*/
	void	ProtectedDraw(float _framedate);

};

#endif
