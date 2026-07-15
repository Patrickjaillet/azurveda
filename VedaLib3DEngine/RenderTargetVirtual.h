// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_RenderTargetVirtual_H
#define COM_M4NKIND_RenderTargetVirtual_H

#include "BaseObject.h"
#include "PackObjectReference.h"
#include "VirtualMachine.h"

class RenderTargetVirtual : public BaseObject
{

public:

	RenderTargetVirtual(void);

	BASEOBJECT_DEFINE_VIRTUALCLASS(RenderTargetVirtual);

	inline  VirtualMachine::InternalViewPort	*GetViewPort(){ return m_pInternalViewPort; };

protected:

	VirtualMachine::InternalViewPort	*m_pInternalViewPort;

#ifdef _ENGINE_EDITABLE_

	virtual void CloseInternal(void);
#endif

};
#endif
