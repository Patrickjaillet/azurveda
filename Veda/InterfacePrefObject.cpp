/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "InterfacePrefObject.h"
#include "BaseContext.h"

BASEOBJECT_DECLARE_CLASS( "IPref", InterfacePrefObject, BaseObject,".Interface Prefs","IPref","Keep Interface preferences (preview,selection)." );

InterfacePrefObject::InterfacePrefObject() : BaseObject()
	,mSer_PreviewIndexList(NewPreviewIndexList)
	,mSer_EditorIndexList(NewEditorIndexList)
	,m_isBeingDestroyed(false)
{
	REGISTER_MEMBER_FLAG(	mSer_Flags,"Flags",0,"Active");

	REGISTER_MEMBER_ENUMS( mSer_PreviewShapeConfiguration,/*"PreviewConf"*/0L,0,"1x1.1x2.2x1.2x2.1+3");
	REGISTER_MEMBER(mSer_PreviewIndexList,/*"Preview"*/0L);

	REGISTER_MEMBER( mSer_ObjectEditorShapeConfiguration,0L );
	REGISTER_MEMBER(mSer_EditorIndexList,/*"Edited"*/0L);

	REGISTER_MEMBER_PACKFLOATBYTE_0_1(mSer_SoundMachineMainVolume,"Volume",1.0f);

}
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	change the value:
		\param	_value 28b unsigned int value.
	*/
void	InterfacePrefObject::PackULong_Flags_Activate::
	Set(unsigned int _value)
{
	if( _value == m_value ) return;
	if( GetObjectThatManagesThis() &&
		(GetObjectThatManagesThis()->GetContext()) &&
		(_value & ipof_Active) )
	{
		// unactive all other IPO of the context, except us :-)
		BaseContext *pContext = GetObjectThatManagesThis()->GetContext();
		ClassNode *pClassNode = pContext->GetSubClassNodeByDescription(
			&(InterfacePrefObject::m_Description) );

		const PackList	*pObjectList = pClassNode->GetObjectList(); // can't be NULL.
		PackStruct::Cell *pcell = pObjectList->GetFirstCell();
		while(pcell )
		{			
			InterfacePrefObject *pObj = (InterfacePrefObject *) pcell->GetManagedObject();
			if( pObj != GetObjectThatManagesThis() )
			{	// it occurs an update: don't !!
				//pObj->mSer_Flags.Set( pObj->mSer_Flags.Get() & (~ipof_Active) );
				pObj->mSer_Flags.m_value &= (~ipof_Active);

			}
			pcell = pcell->GetNextBrotherCell();
		}
	
	}
	PackULong_Flags::Set( _value );
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Only one preference object can have the active flag at the same time,
			in the context. This one is returned if it exists.
			If there is no preference object or no active one, it return 0L.
	\param	_pContext the context where to search the active preference..
	\return a preference object or NULL.
*/
InterfacePrefObject *InterfacePrefObject::GetContextActivePref(BaseContext *_pContext,bool _createOneIfNone)
{
	ClassNode *pClassNode = _pContext->GetSubClassNodeByDescription(
		&(InterfacePrefObject::m_Description) );
	if( !pClassNode ) return 0L;

	const PackList	*pObjectList = pClassNode->GetObjectList(); // can't be NULL.
	PackStruct::Cell *pcell = pObjectList->GetFirstCell();
	while(pcell )
	{			
		InterfacePrefObject *pObj = (InterfacePrefObject *) pcell->GetManagedObject();
		if( pObj->mSer_Flags.Get() & ipof_Active ) return pObj;
		pcell = pcell->GetNextBrotherCell();
	}
	// no active object found: get the first and activate it:
	pcell = pObjectList->GetFirstCell();
	if(pcell )
	{
		InterfacePrefObject *pObj = (InterfacePrefObject *) pcell->GetManagedObject();
		pObj->mSer_Flags.Set(
			pObj->mSer_Flags.Get() | ipof_Active );
		return( pObj );
	}
	// else: create one and activate it:
	if(!_createOneIfNone) return 0L;
	InterfacePrefObject *ppref = (InterfacePrefObject *)
			_pContext->NewObject( (InterfacePrefObject::m_Description) );
	if( ppref )
	{	
		ppref->mSer_Flags.Set(
			ppref->mSer_Flags.Get() | ipof_Active );
	}
	return ppref; // can be null.
}
#endif
#ifdef _ENGINE_EDITABLE_
/*
	\brief protected: if not done, create a preview history list for an indexed view.
	\param	optionnal index of the view.
	\return	the list, which itself manages PreviewEdited objects.
*/
PackList	*InterfacePrefObject::GetOrCreatePreviewIndexList( unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return 0L;
	PackList *pPreviewList=0L;
	while(!pPreviewList)
	{
		pPreviewList = (PackList *)mSer_PreviewIndexList.Get(_RenderViewIndex);
		if(pPreviewList) return pPreviewList;
		PackList::Cell *pCell =  mSer_PreviewIndexList.AddElement(); //after
		if(!pCell) break;
	}
	 return 0L; // mem error ?
}
#endif
#ifdef _ENGINE_EDITABLE_
/*
	\brief protected: if not done, create a preview history list for an indexed view.
	\param	optionnal index of the view.
	\return	the list, which itself manages PreviewEdited objects.
*/
PackList	*InterfacePrefObject::GetOrCreateEditorIndexList( unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return 0L;
	PackList *pEditorList=0L;
	while(!pEditorList)
	{
		pEditorList = (PackList *)mSer_EditorIndexList.Get(_RenderViewIndex);
		if(pEditorList) return pEditorList;
		PackList::Cell *pCell =  mSer_EditorIndexList.AddElement(); //after
		if(!pCell) break;
	}
	 return 0L; // mem error ?
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Set the Preview object. It affect's the context object,
			and the current active Preference object in the context.
			It is then noted in the current active preference object.

	\param	_ObjectToPreview the object to preview.
	\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
*/
void	InterfacePrefObject::SetContextPreviewObject(
						BaseObject *_ObjectToPreview,
						unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return ;
	// get or create the list for this _RenderViewIndex.
	PackList *pPreviewList = GetOrCreatePreviewIndexList(_RenderViewIndex);
	if(!pPreviewList) return;

	PackList::Cell *pCell = pPreviewList->GetFirstCell();
	PreviewEdited	lastconfiguration;
	bool				wasInListBefore=false;
	while(pCell)
	{
		PreviewEdited *pEdited = (PreviewEdited *) pCell->GetManagedObject();		
		BaseObject *ppreview = pEdited->mSer_Reference.GetObjectPointer();
		if( ppreview == _ObjectToPreview )
		{
			// already in the list: keep preview configuration:
			// note the object pointer is not cloned.
			lastconfiguration.MakeCloneOf(*pEdited);
			wasInListBefore = true;
			// destroy old item:
			pPreviewList->DeleteElement(pEdited);
		}
		
		if(wasInListBefore) break;
		// next	
		pCell = pCell->GetNextBrotherCell();		
	}
	// insert a new preview conf at the top, using a new conf or the old:
	pCell = pPreviewList->AddElement(0);
	if(!pCell) return;
	PreviewEdited *pnEdited = 
		(PreviewEdited *) pCell->GetManagedObject();
	if(wasInListBefore) pnEdited->MakeCloneOf(lastconfiguration);
	// clear all history flags:
	pCell = pPreviewList->GetFirstCell();
	while(pCell)
	{
		PreviewEdited *pCleanEdited = (PreviewEdited *) pCell->GetManagedObject();
		pCleanEdited->mSer_Flag.RemoveFlags(PreviewEdited::m_PF_HistoryFlag);
		pCell = pCell->GetNextBrotherCell();
	}
	// this call would occur a refresh:
	pnEdited->mSer_Reference.SetObjectPointer(_ObjectToPreview);

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Get a current Preview object for a context.
				If there are none, return 0L.
	\param	_pContext the context which has a Preview object.
	\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
	\param _ppPreviewConfiguration if not nul, return a pointer to the configuration preview to it.
	\return the current Preview object or 0L.
*/
BaseObject *InterfacePrefObject::GetContextPreviewObject(unsigned int _RenderViewIndex,InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration **_ppPreviewConfiguration)
{
	if(m_isBeingDestroyed) return 0L;
	// get or create the list for this _RenderViewIndex.
	PackList *pPreviewList = GetOrCreatePreviewIndexList(_RenderViewIndex);
	if(!pPreviewList) return 0L;

	PackList::Cell *pCell = pPreviewList->GetFirstCell();
	while(pCell)
	{
		PreviewEdited *pEdited = 
			(PreviewEdited *) pCell->GetManagedObject();
		if( ! pEdited->mSer_Flag.TestFlags(PreviewEdited::m_PF_HistoryFlag) ) // history flag
		{
			// return the first in the list:
			BaseObject *ppreview = pEdited->mSer_Reference.GetObjectPointer();
			if( _ppPreviewConfiguration )
				*_ppPreviewConfiguration = &(pEdited->m_PreviewConfiguration);
			return ppreview; // can be 0L.
		}
		// next	
		pCell = pCell->GetNextBrotherCell();		
	}
	// not found:
	return 0L;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Preview Previous: scroll down the history of previewed object. May Change result of GetContextPreviewObject().
	\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
*/
void InterfacePrefObject::PreviewPrevious(unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return ;
	// get or create the list for this _RenderViewIndex.
	PackList *pPreviewList = GetOrCreatePreviewIndexList(_RenderViewIndex);
	if(!pPreviewList) return;

	PackList::Cell *pCell = pPreviewList->GetFirstCell();
	while(pCell)
	{
		PreviewEdited *pEdited = (PreviewEdited *) pCell->GetManagedObject();
		if( ! pEdited->mSer_Flag.TestFlags(PreviewEdited::m_PF_HistoryFlag) ) // history flag
		{
			pEdited->mSer_Flag.AddFlags(PreviewEdited::m_PF_HistoryFlag);
			return;
		}
		pCell = pCell->GetNextBrotherCell();	
	}
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Preview Next: scroll up the history of previewed object. May Change result of GetContextPreviewObject().
	\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
*/
void InterfacePrefObject::PreviewNext(unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return ;
	// get or create the list for this _RenderViewIndex.
	PackList *pPreviewList = GetOrCreatePreviewIndexList(_RenderViewIndex);
	if(!pPreviewList) return;

	PackList::Cell *pCell = pPreviewList->GetFirstCell();
	PreviewEdited *pPrevEdited = 0L;
	while(pCell)
	{
		PreviewEdited *pEdited = (PreviewEdited *) pCell->GetManagedObject();
		if(pEdited->mSer_Flag.TestFlags(PreviewEdited::m_PF_HistoryFlag)) 
		{
			pPrevEdited = pEdited;
		}
		else
			if(pPrevEdited) // if current history ==0 and previous ==1
			{
				// prev =0;
				pPrevEdited->mSer_Flag.RemoveFlags(PreviewEdited::m_PF_HistoryFlag);
				return;
			}
		pCell = pCell->GetNextBrotherCell();	
	}
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Set the edited object in interface. It affects the current active 
			Preference object in the context. (it creates one if there wasnt one).
			The _ObjectToEdit must belong to _pContext.
			if _ObjectToEdit is NULL, the edited object is set to NULL 
			(no edit.)
	\param	_ObjectToEdit the object to preview.
	\param _EditViewIndex if the editor support multiple object edition, an abstract index of the view affected.
*/
void	InterfacePrefObject::SetContextEditedObject(BaseObject *_ObjectToEdit,unsigned int _EditViewIndex)
{
	if(m_isBeingDestroyed) return ;
	// get or create the list for this _RenderViewIndex.
	PackList *pEditorList = GetOrCreateEditorIndexList(_EditViewIndex);
	if(!pEditorList) return;

	PackList::Cell *pCell = pEditorList->GetFirstCell();
	bool	wasInListBefore=false;
	while(pCell)
	{
		Edited *pEdited = (Edited *) pCell->GetManagedObject();		
		BaseObject *pObEdited = pEdited->mSer_Reference.GetObjectPointer();
		if( pObEdited == _ObjectToEdit ||
			( _ObjectToEdit == this &&
			pEdited->mSer_Flag.TestFlags(Edited::m_EF_SelfReference)) )
		{
			wasInListBefore = true;
			// destroy old item:
			pEditorList->DeleteElement(pEdited);
		}
		if(wasInListBefore) break; // contine after delete is impossible.
		// next	
		pCell = pCell->GetNextBrotherCell();		
	}
	// insert a new preview conf at the top, using a new conf or the old:
	pCell = pEditorList->AddElement(0);
	if(!pCell) return;
	Edited *pnEdited = 
		(Edited *) pCell->GetManagedObject();

	// clear all history flags:
	pCell = pEditorList->GetFirstCell();
	while(pCell)
	{
		Edited *pCleanEdited = (Edited *) pCell->GetManagedObject();
		pCleanEdited->mSer_Flag.RemoveFlags(Edited::m_EF_HistoryFlag);
		pCell = pCell->GetNextBrotherCell();
	}
	// this call would occur a refresh:
	// special case if _ObjectToEdit is "this" to manage self-reference !!!
	if( _ObjectToEdit == this )
	{
		pnEdited->mSer_Flag.Set(Edited::m_EF_SelfReference); //  means edited is self.
	} else
	{	// normal case:
		pnEdited->mSer_Reference.SetObjectPointer( _ObjectToEdit );	
	}

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Get a current edited object for a context.
				If there are none, return 0L.
	\param _EditViewIndex if there is support for multiple editors, the index of the editor affected.
	\return the current edtited object or 0L.
*/
BaseObject *InterfacePrefObject::GetContextEditedObject(unsigned int _EditViewIndex)
{
	if(m_isBeingDestroyed) return 0L;
	// get or create the list for this _RenderViewIndex.
	PackList *pEditorList = GetOrCreateEditorIndexList(_EditViewIndex);
	if(!pEditorList) return 0L;

	PackList::Cell *pCell = pEditorList->GetFirstCell();
	while(pCell)
	{
		Edited *pEdited = 	(Edited *) pCell->GetManagedObject();
		if( !pEdited->mSer_Flag.TestFlags(Edited::m_EF_HistoryFlag ) ) // history flag
		{
			// return the first in the list:
			if( pEdited->mSer_Flag.TestFlags(Edited::m_EF_SelfReference )) return this;
			else return pEdited->mSer_Reference.GetObjectPointer(); // can be 0L.
		}
		// next	
		pCell = pCell->GetNextBrotherCell();		
	}
	// not found:
	return 0L;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Edit Previous: scroll down the history of edited object. May Change result of GetContextPreviewObject().
	\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
*/
void InterfacePrefObject::EditPrevious(unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return;
	// get or create the list for this _RenderViewIndex.
	PackList *pEditorList = GetOrCreateEditorIndexList(_RenderViewIndex);
	if(!pEditorList) return;

	PackList::Cell *pCell = pEditorList->GetFirstCell();
	while(pCell)
	{
		Edited *pEdited = 	(Edited *) pCell->GetManagedObject();
		if( ! pEdited->mSer_Flag.TestFlags(Edited::m_EF_HistoryFlag)) // history flag
		{
			pEdited->mSer_Flag.AddFlags(Edited::m_EF_HistoryFlag);
			return;
		}
		pCell = pCell->GetNextBrotherCell();	
	}
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Edit Next: scroll up the history of edited object. May Change result of GetContextPreviewObject().
	\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
*/
void InterfacePrefObject::EditNext(unsigned int _RenderViewIndex)
{
	// get or create the list for this _RenderViewIndex.
	PackList *pEditorList = GetOrCreateEditorIndexList(_RenderViewIndex);
	if(!pEditorList) return;

	PackList::Cell *pCell = pEditorList->GetFirstCell();
	Edited *pPrevEdited = 0L;
	while(pCell)
	{
		Edited *pEdited = (Edited *) pCell->GetManagedObject();
		if(pEdited->mSer_Flag.TestFlags(Edited::m_EF_HistoryFlag))
		{
			pPrevEdited = pEdited;
		}
		else
			if(pPrevEdited) // if current history ==0 and previous ==1
			{
				// prev =0;
				pPrevEdited->mSer_Flag.RemoveFlags(Edited::m_EF_HistoryFlag);
				return;
			}
		pCell = pCell->GetNextBrotherCell();	
	}
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	see PackObjectReference::Set()
*/
 PackObjectReference::enum_SetReferenceResult
InterfacePrefObject::PackObjectReferenceSpecial::SetObjectPointer( BaseObject * _Object )
{
	// classic behaviour:
	 PackObjectReference::enum_SetReferenceResult result = PackObjectReference::SetObjectPointer(_Object);
	// +, if null,means object deleted, sorry, bt it deletes this preference row.
	if( !_Object )
	{
		((InterfacePrefObject *)GetObjectThatManagesThis())
			->PurgePreferenceToDeadObjects();
	}
	// important: last operation: auto-deletion.
	return result;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*
	\brief protected tool, delete all edition or preview preference that are linked to nothing.
*/
void	InterfacePrefObject::PurgePreferenceToDeadObjects()
{
	// scan all renderviewx index, then all pref/object.
	PackList::Cell *pViewIndex= mSer_PreviewIndexList.GetFirstCell();
	while(pViewIndex)
	{
		PackList *pPreviewList = (PackList *)pViewIndex->GetManagedObject();
		
		PackList::Cell *pNext,*pCell = pPreviewList->GetFirstCell();		
		while(pCell)
		{
			pNext= pCell->GetNextBrotherCell();
			PreviewEdited *pEdited = (PreviewEdited *) pCell->GetManagedObject();		
			if( pEdited->mSer_Reference.GetObjectPointer() == 0L ) 
				pPreviewList->DeleteElement(pEdited);
			pCell = pNext;
		}

		pViewIndex = pViewIndex->GetNextBrotherCell();
	}
	// editors now:
	PackList::Cell *pEditIndex= mSer_EditorIndexList.GetFirstCell();
	while(pEditIndex)
	{
		// scan all pref/objects:
		PackList *pEditorList = (PackList *)pEditIndex->GetManagedObject();
		PackList::Cell *pNext,*pCell = pEditorList->GetFirstCell();		
		while(pCell)
		{
			pNext= pCell->GetNextBrotherCell();
			Edited *pEdited = (Edited *) pCell->GetManagedObject();		
			if( pEdited->mSer_Reference.GetObjectPointer() == 0L &&
				(!pEdited->mSer_Flag.TestFlags(Edited::m_EF_SelfReference)) )
				pEditorList->DeleteElement(pEdited);
			pCell = pNext;
		}
		pEditIndex = pEditIndex->GetNextBrotherCell();
	}

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	all reference to this object will be closed. used by deletion done by ClassNode.
			special close.
*/
void InterfacePrefObject::CloseReferences()
{
	m_isBeingDestroyed = true;
	BaseObject::CloseReferences();
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
			Special process: purge all null pointer links in the object before serialization.
	\return	byte size of the serialisation to do.
*/
unsigned int  InterfacePrefObject::GetSerializedDescriptionSize(void)
{
	PurgePreferenceToDeadObjects();
	return BaseObject::GetSerializedDescriptionSize();
}
#endif
