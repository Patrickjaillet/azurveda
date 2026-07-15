// SPDX-License-Identifier: LGPL-2.1-only

#include "RenderTargetVirtual.h"

BASEOBJECT_DECLARE_VIRTUALCLASS( "reta", RenderTargetVirtual, BaseObject );

RenderTargetVirtual::RenderTargetVirtual() : BaseObject()
	,m_pInternalViewPort(0L)
{

}

#ifdef _ENGINE_EDITABLE_

void	RenderTargetVirtual::CloseInternal(void)
{
	GetMachine()->DeleteViewPort( &m_pInternalViewPort );
}
#endif
