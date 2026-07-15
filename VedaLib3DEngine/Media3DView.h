// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_Media3DView_H
#define COM_M4NKIND_Media3DView_H

#include "VirtualMedia.h"
#include "PackList.h"
#include "PackDynamicFloat.h"
#include "PackULong_Flags.h"

class Media3DView : public VirtualMedia
{

public:

	Media3DView(void);

	BASEOBJECT_DEFINE_CLASS(Media3DView);

	virtual void ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort );

protected:
	typedef enum{
		scfl_NoClear=1
	} RenderFlag ;

	PackULong_Flags		mSer_Flags;

	PackFloat			mSer_TimeLength;

	PackObjectReference	mSer_MainObject;

	PackObjectReference	mSer_RenderTargetRef;

	PackDynamicFloat	mSer_PositionRef;

	PackDynamicFloat	mSer_TargetPositionRef;

	PackDynamicFloat	mSer_FlipRot;

	PackDynamicFloat	mSer_FOV;

	void	AtanO1O2( float *po1, float xd,float yd,float zd );

	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_

#endif

};

#endif
