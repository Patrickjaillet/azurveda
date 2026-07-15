/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_RenderTargetRectangle_H
#define COM_M4NKIND_RenderTargetRectangle_H

#include "RenderTargetVirtual.h"
#include "PackObjectReference.h"

/*!
	\class	RenderTargetRectangle
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Define a screen rectangle to draw on, as the
			son of another drawing rectangle, in a hierarchy
			of rectangles.
*/
class RenderTargetRectangle : public RenderTargetVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	RenderTargetRectangle(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(RenderTargetRectangle);
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the RenderTargetRectangle that define the origin of the position.
	PackObjectReference	mSer_PositionFather;
	//!  the RenderTargetRectangle that define the clipping position of the clipping.
	//PackObjectReference	mSer_ClippingFather;

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by Create().
		\returnfalse if creation failed.
	*/
	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	//virtual void CloseInternal(void);
#endif

};
#endif
