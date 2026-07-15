/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_MediaAccess_H
#define COM_M4NKIND_MediaAccess_H

#include "BaseObject.h"
#include "PackObjectReference.h"
#include "VirtualMedia.h"
#include "PackString.h"
#include "PackULong_Enums.h"
/*!
	\class	MediaAccess
	\ingroup BaseObjectInheritedDocGroup BaseObjectInherited_Veda
	\brief	Define which Media objects are the main media and the boot media
			in a context. Then static method MediaAccess::GetMainMedia()
			is a simple entry to a media in the context.
			There should be only one MediaAccess object per context.
*/
class MediaAccess : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
	*/
	MediaAccess(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp 
	*/
	BASEOBJECT_DEFINE_CLASS(MediaAccess);
	/*!
		\brief	Get the media object defined as the main, from the context, 
				and eventually a boot script.
		\param	_pContext the context which has a main script.
		\param _ppbootScript a pointer to a pointer to a bootscript to return.
		\param _ppMediaExportName a pointer to an executable name.
		\return the main media object or null if none selected.
	*/
	static	VirtualMedia *GetMainMedia(BaseContext *_pContext,VirtualMedia **_ppbootScript=0L,const char **_ppMediaExportName=0L);
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
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
#ifdef _ENGINE_EDITABLE_
	/*
		\brief	Return the main media object. Then use ->Create() to build it.
		\param	_pMediaObjectToLink Media Object To set as main.
		\return a possible error enum, like in PackObjectReference::Set() .
	*/
	inline PackObjectReference::enum_SetReferenceResult
						SetMainMedia( VirtualMedia *_pMediaObjectToLink ) 
	{ return mSer_MainMedia.SetObjectPointer( _pMediaObjectToLink ); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*
		\brief	Return the main media object. Then use ->Create() to build it.
		\param	_pMediaObjectToLink Media Object To set as main.
		\return a possible error enum, like in PackObjectReference::Set() .
	*/
	inline PackObjectReference::enum_SetReferenceResult
						SetBootMedia( VirtualMedia *_pMediaObjectToLink ) 
	{ return mSer_BootMedia.SetObjectPointer( _pMediaObjectToLink ); };
#endif

	/*
		\brief	returns a name for the whole media, suitable as Executable name.

		\return a character string of the name.
	*/
	inline	const char *GetExportName(){ return mSer_ExportName.Get(); };
	//! \typedef define a default resolution preference for executable export.
	typedef enum {
		eRes_320x240=0,
		eRes_640x480=0,
		eRes_800x600=0,
	} eVGAResolution;
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the selector of the main media, as a serializable type.
	PackObjectReference		mSer_MainMedia;
	//! the selector of the boot media, as a serializable type.
	PackObjectReference		mSer_BootMedia;
	//! name of the media, can be used as executable name:
	PackString				mSer_ExportName;
	//! default eVGAResolution enum for exporting media executable.
	PackULong_Enums			mSer_VGADefaultResolution;
	//! additional text 
	PackObjectReference		mSer_QuittingInfoTextRef;
};

#endif
