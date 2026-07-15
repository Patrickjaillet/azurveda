/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "RenderTargetRectangle.h"

BASEOBJECT_DECLARE_CLASS( "r", RenderTargetRectangle, RenderTargetVirtual,"2D RenderTarget Rectangle","RenderRect","It stands for a drawable screen and can be set in a hierarchy of rectangles." );

RenderTargetRectangle::RenderTargetRectangle() : RenderTargetVirtual()
{
	REGISTER_MEMBER_REFERENCE(	mSer_PositionFather,"Aspect Father",RenderTargetVirtual::m_Description );
//	REGISTER_MEMBER_REFERENCE(	mSer_ClippingFather,"Clip Father",RenderTargetRectangle::m_Description );

}

/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
		\return false if creation failed.
*/
bool RenderTargetRectangle::CreateInternal(void)
{	
	VirtualMachine::InternalViewPort *pFatherVP;
	RenderTargetVirtual *pFatherObject = (RenderTargetVirtual *)mSer_PositionFather.GetObjectPointer();
	if( pFatherObject )
	{
		pFatherVP = pFatherObject->GetViewPort(); // can be null if no obj selected.
		// if no father rectangle, make son of the default rectangle.
		//no: done by NewViewPortChild
		//if( !pFatherVP ) pFatherVP = GetMachine()->GetDefaultViewPort(); // couldn't be NULL.

		//create son:
		GetMachine()->NewViewPortChild( &m_pInternalViewPort,pFatherVP );
		if( !m_pInternalViewPort ) return false; // failed !
	}
	// OK:
	return(true);
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
/*void	RenderTargetRectangle::CloseInternal(void)
{
	GetMachine()->DeleteInternalViewPort( &m_pInternalViewPort );
}*/
#endif
