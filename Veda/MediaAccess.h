// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_MediaAccess_H
#define COM_M4NKIND_MediaAccess_H

#include "BaseObject.h"
#include "PackObjectReference.h"
#include "VirtualMedia.h"
#include "PackString.h"
#include "PackULong_Enums.h"

class MediaAccess : public BaseObject
{

public:

	MediaAccess(void);

	BASEOBJECT_DEFINE_CLASS(MediaAccess);

	static	VirtualMedia *GetMainMedia(BaseContext *_pContext,VirtualMedia **_ppbootScript=0L,const char **_ppMediaExportName=0L);
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
#ifdef _ENGINE_EDITABLE_

	inline PackObjectReference::enum_SetReferenceResult
						SetMainMedia( VirtualMedia *_pMediaObjectToLink )
	{ return mSer_MainMedia.SetObjectPointer( _pMediaObjectToLink ); };
#endif
#ifdef _ENGINE_EDITABLE_

	inline PackObjectReference::enum_SetReferenceResult
						SetBootMedia( VirtualMedia *_pMediaObjectToLink )
	{ return mSer_BootMedia.SetObjectPointer( _pMediaObjectToLink ); };
#endif

	inline	const char *GetExportName(){ return mSer_ExportName.Get(); };

	typedef enum {
		eRes_320x240=0,
		eRes_640x480=0,
		eRes_800x600=0,
	} eVGAResolution;

protected:

	PackObjectReference		mSer_MainMedia;

	PackObjectReference		mSer_BootMedia;

	PackString				mSer_ExportName;

	PackULong_Enums			mSer_VGADefaultResolution;

	PackObjectReference		mSer_QuittingInfoTextRef;
};

#endif
