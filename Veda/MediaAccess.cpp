// SPDX-License-Identifier: LGPL-2.1-only

#include "MediaAccess.h"
#include "VirtualMedia.h"
#include "BaseContext.h"
#include "TextLocalized.h"
BASEOBJECT_DECLARE_CLASS( "->", MediaAccess, BaseObject,"Media Access","Access","Define which media object is the main media,used as the entry for executables. Also, a boot media can be used to be played while creating the main. You need only one Media Access per context." );

MediaAccess::MediaAccess() : BaseObject()
{

	REGISTER_MEMBER_REFERENCE(mSer_MainMedia ,"MainMedia", VirtualMedia::m_Description );
	REGISTER_MEMBER_REFERENCE(mSer_BootMedia ,"BootMedia", VirtualMedia::m_Description );
	REGISTER_MEMBER_PACKSTRING(mSer_ExportName,"ExportName");
#ifdef _ENGINE_EDITABLE_

	mSer_ExportName.Set("Demo");
#endif
	REGISTER_MEMBER_ENUMS(mSer_VGADefaultResolution,0L,0,"MachineDefault.320x240.640x480.800x600.1280x1024");
	REGISTER_MEMBER_REFERENCE(mSer_QuittingInfoTextRef,0L,TextLocalized::m_Description);

}

VirtualMedia *MediaAccess::GetMainMedia(BaseContext *_pContext,VirtualMedia **_ppbootScript,const char **_ppMediaExportName )
{

	* _ppbootScript = 0L;
	ClassNode *pClassNode = _pContext->GetSubClassNodeByDescription(
		&(MediaAccess::m_Description) );
	if( !pClassNode ) return 0L;

	const PackList	*pObjectList = pClassNode->GetObjectList();
	PackList::Cell *pcell = pObjectList->GetFirstCell();
	if(!pcell) return 0L;

	MediaAccess *pAccess = (MediaAccess *) pcell->GetManagedObject();
	if(_ppbootScript)
		*_ppbootScript = (VirtualMedia *) pAccess->mSer_BootMedia.GetObjectPointer();
	if(_ppMediaExportName)
		*_ppMediaExportName = pAccess->GetExportName();
	return (VirtualMedia *) pAccess->mSer_MainMedia.GetObjectPointer();
}
#ifdef _ENGINE_EDITABLE_

void MediaAccess::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	VirtualMedia *pMainMedia = (VirtualMedia *)mSer_MainMedia.GetObjectPointer();
	if(!pMainMedia) return;
	pMainMedia->ProcessPreview(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);
}
#endif
