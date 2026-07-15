// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_RenderTargetRectangle_H
#define COM_M4NKIND_RenderTargetRectangle_H

#include "RenderTargetVirtual.h"
#include "PackObjectReference.h"

class RenderTargetRectangle : public RenderTargetVirtual
{

public:

	RenderTargetRectangle(void);

	BASEOBJECT_DEFINE_CLASS(RenderTargetRectangle);

protected:

	PackObjectReference	mSer_PositionFather;

	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_

#endif

};
#endif
