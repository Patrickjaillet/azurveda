/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "MediaAccess.h"
#include "VirtualMedia.h"
#include "BaseContext.h"
#include "TextLocalized.h"
BASEOBJECT_DECLARE_CLASS( "->", MediaAccess, BaseObject,"Media Access","Access","Define which media object is the main media,used as the entry for executables. Also, a boot media can be used to be played while creating the main. You need only one Media Access per context." );

MediaAccess::MediaAccess() : BaseObject()
{
	// Contexts are also PackStructs ! We can serialize special extra members by context here:
	REGISTER_MEMBER_REFERENCE(mSer_MainMedia ,"MainMedia", VirtualMedia::m_Description );
	REGISTER_MEMBER_REFERENCE(mSer_BootMedia ,"BootMedia", VirtualMedia::m_Description );
	REGISTER_MEMBER_PACKSTRING(mSer_ExportName,"ExportName");
#ifdef _ENGINE_EDITABLE_
	// set default name:
	mSer_ExportName.Set("Demo");
#endif
	REGISTER_MEMBER_ENUMS(mSer_VGADefaultResolution,/*"DefaultRes."*/0L,0,"MachineDefault.320x240.640x480.800x600.1280x1024");
	REGISTER_MEMBER_REFERENCE(mSer_QuittingInfoTextRef,0L,TextLocalized::m_Description);

}
/*!
	\brief	Get the media object defined as the main, from the context, 
			and eventually a boot script.
	\param	_pContext the context which has a main script.
	\param _ppbootScript a pointer to a pointer to a bootscript to return.
	\param _ppMediaExportName a pointer to an executable name.
	\return the main media object or null if none selected.
*/
VirtualMedia *MediaAccess::GetMainMedia(BaseContext *_pContext,VirtualMedia **_ppbootScript,const char **_ppMediaExportName )
{
	// get classnode
	* _ppbootScript = 0L;
	ClassNode *pClassNode = _pContext->GetSubClassNodeByDescription(
		&(MediaAccess::m_Description) );
	if( !pClassNode ) return 0L;
	// get first object in the context:
	const PackList	*pObjectList = pClassNode->GetObjectList(); // can't be NULL.
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
/*!
	\brief	Extend this method to draw your preview !
			a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. Inherited classes can implement it in
			any way, to explicit the current shape of an object.

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
	\param	_pPreviewConfiguration Preview Configuration object. Never 0L. Do not keep a pointer to it.
*/
void MediaAccess::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	VirtualMedia *pMainMedia = (VirtualMedia *)mSer_MainMedia.GetObjectPointer();
	if(!pMainMedia) return;
	pMainMedia->ProcessPreview(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);
}
#endif