// SPDX-License-Identifier: LGPL-2.1-only

#include "RenderTargetRectangle.h"

BASEOBJECT_DECLARE_CLASS( "r", RenderTargetRectangle, RenderTargetVirtual,"2D RenderTarget Rectangle","RenderRect","It stands for a drawable screen and can be set in a hierarchy of rectangles." );

RenderTargetRectangle::RenderTargetRectangle() : RenderTargetVirtual()
{
	REGISTER_MEMBER_REFERENCE(	mSer_PositionFather,"Aspect Father",RenderTargetVirtual::m_Description );

}

bool RenderTargetRectangle::CreateInternal(void)
{
	VirtualMachine::InternalViewPort *pFatherVP;
	RenderTargetVirtual *pFatherObject = (RenderTargetVirtual *)mSer_PositionFather.GetObjectPointer();
	if( pFatherObject )
	{
		pFatherVP = pFatherObject->GetViewPort();

		GetMachine()->NewViewPortChild( &m_pInternalViewPort,pFatherVP );
		if( !m_pInternalViewPort ) return false;
	}

	return(true);
}
#ifdef _ENGINE_EDITABLE_

#endif
