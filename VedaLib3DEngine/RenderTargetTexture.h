// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_RenderTargetTexture_H
#define COM_M4NKIND_RenderTargetTexture_H

#include "RenderTargetVirtual.h"
#include "PackObjectReference.h"
#include "PackLong_WithLimits.h"
#include "PackULong_Enums.h"
#include "VirtualMachine.h"

class RenderTargetTexture : public RenderTargetVirtual
{

public:

	RenderTargetTexture(void);

	BASEOBJECT_DEFINE_CLASS(RenderTargetTexture);

protected:

	PackObjectReference	mSer_TextureToRender;

	PackULong_Enums	mSer_TargetLayer;

	PackLong_WithLimits	mSer_2nPixelWidth;

	PackLong_WithLimits	mSer_2nPixelHeight;

	VirtualMachine::InternalViewPort	*m_pInternalViewPort;

	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_

#endif

};
#endif
