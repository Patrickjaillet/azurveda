// SPDX-License-Identifier: LGPL-2.1-only

#include "InterfacePrefObject.h"
#include "BaseContext.h"

BASEOBJECT_DECLARE_CLASS( "IPref", InterfacePrefObject, BaseObject,".Interface Prefs","IPref","Keep Interface preferences (preview,selection)." );

InterfacePrefObject::InterfacePrefObject() : BaseObject()
	,mSer_PreviewIndexList(NewPreviewIndexList)
	,mSer_EditorIndexList(NewEditorIndexList)
	,m_isBeingDestroyed(false)
{
	REGISTER_MEMBER_FLAG(	mSer_Flags,"Flags",0,"Active");

	REGISTER_MEMBER_ENUMS( mSer_PreviewShapeConfiguration,0L,0,"1x1.1x2.2x1.2x2.1+3");
	REGISTER_MEMBER(mSer_PreviewIndexList,0L);

	REGISTER_MEMBER( mSer_ObjectEditorShapeConfiguration,0L );
	REGISTER_MEMBER(mSer_EditorIndexList,0L);

	REGISTER_MEMBER_PACKFLOATBYTE_0_1(mSer_SoundMachineMainVolume,"Volume",1.0f);

}
#ifdef _ENGINE_EDITABLE_

void	InterfacePrefObject::PackULong_Flags_Activate::
	Set(unsigned int _value)
{
	if( _value == m_value ) return;
	if( GetObjectThatManagesThis() &&
		(GetObjectThatManagesThis()->GetContext()) &&
		(_value & ipof_Active) )
	{

		BaseContext *pContext = GetObjectThatManagesThis()->GetContext();
		ClassNode *pClassNode = pContext->GetSubClassNodeByDescription(
			&(InterfacePrefObject::m_Description) );

		const PackList	*pObjectList = pClassNode->GetObjectList();
		PackStruct::Cell *pcell = pObjectList->GetFirstCell();
		while(pcell )
		{
			InterfacePrefObject *pObj = (InterfacePrefObject *) pcell->GetManagedObject();
			if( pObj != GetObjectThatManagesThis() )
			{

				pObj->mSer_Flags.m_value &= (~ipof_Active);

			}
			pcell = pcell->GetNextBrotherCell();
		}

	}
	PackULong_Flags::Set( _value );
}
#endif
#ifdef _ENGINE_EDITABLE_

InterfacePrefObject *InterfacePrefObject::GetContextActivePref(BaseContext *_pContext,bool _createOneIfNone)
{
	ClassNode *pClassNode = _pContext->GetSubClassNodeByDescription(
		&(InterfacePrefObject::m_Description) );
	if( !pClassNode ) return 0L;

	const PackList	*pObjectList = pClassNode->GetObjectList();
	PackStruct::Cell *pcell = pObjectList->GetFirstCell();
	while(pcell )
	{
		InterfacePrefObject *pObj = (InterfacePrefObject *) pcell->GetManagedObject();
		if( pObj->mSer_Flags.Get() & ipof_Active ) return pObj;
		pcell = pcell->GetNextBrotherCell();
	}

	pcell = pObjectList->GetFirstCell();
	if(pcell )
	{
		InterfacePrefObject *pObj = (InterfacePrefObject *) pcell->GetManagedObject();
		pObj->mSer_Flags.Set(
			pObj->mSer_Flags.Get() | ipof_Active );
		return( pObj );
	}

	if(!_createOneIfNone) return 0L;
	InterfacePrefObject *ppref = (InterfacePrefObject *)
			_pContext->NewObject( (InterfacePrefObject::m_Description) );
	if( ppref )
	{
		ppref->mSer_Flags.Set(
			ppref->mSer_Flags.Get() | ipof_Active );
	}
	return ppref;
}
#endif
#ifdef _ENGINE_EDITABLE_

PackList	*InterfacePrefObject::GetOrCreatePreviewIndexList( unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return 0L;
	PackList *pPreviewList=0L;
	while(!pPreviewList)
	{
		pPreviewList = (PackList *)mSer_PreviewIndexList.Get(_RenderViewIndex);
		if(pPreviewList) return pPreviewList;
		PackList::Cell *pCell =  mSer_PreviewIndexList.AddElement();
		if(!pCell) break;
	}
	 return 0L;
}
#endif
#ifdef _ENGINE_EDITABLE_

PackList	*InterfacePrefObject::GetOrCreateEditorIndexList( unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return 0L;
	PackList *pEditorList=0L;
	while(!pEditorList)
	{
		pEditorList = (PackList *)mSer_EditorIndexList.Get(_RenderViewIndex);
		if(pEditorList) return pEditorList;
		PackList::Cell *pCell =  mSer_EditorIndexList.AddElement();
		if(!pCell) break;
	}
	 return 0L;
}
#endif
#ifdef _ENGINE_EDITABLE_

void	InterfacePrefObject::SetContextPreviewObject(
						BaseObject *_ObjectToPreview,
						unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return ;

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

			lastconfiguration.MakeCloneOf(*pEdited);
			wasInListBefore = true;

			pPreviewList->DeleteElement(pEdited);
		}

		if(wasInListBefore) break;

		pCell = pCell->GetNextBrotherCell();
	}

	pCell = pPreviewList->AddElement(0);
	if(!pCell) return;
	PreviewEdited *pnEdited =
		(PreviewEdited *) pCell->GetManagedObject();
	if(wasInListBefore) pnEdited->MakeCloneOf(lastconfiguration);

	pCell = pPreviewList->GetFirstCell();
	while(pCell)
	{
		PreviewEdited *pCleanEdited = (PreviewEdited *) pCell->GetManagedObject();
		pCleanEdited->mSer_Flag.RemoveFlags(PreviewEdited::m_PF_HistoryFlag);
		pCell = pCell->GetNextBrotherCell();
	}

	pnEdited->mSer_Reference.SetObjectPointer(_ObjectToPreview);

}
#endif
#ifdef _ENGINE_EDITABLE_

BaseObject *InterfacePrefObject::GetContextPreviewObject(unsigned int _RenderViewIndex,InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration **_ppPreviewConfiguration)
{
	if(m_isBeingDestroyed) return 0L;

	PackList *pPreviewList = GetOrCreatePreviewIndexList(_RenderViewIndex);
	if(!pPreviewList) return 0L;

	PackList::Cell *pCell = pPreviewList->GetFirstCell();
	while(pCell)
	{
		PreviewEdited *pEdited =
			(PreviewEdited *) pCell->GetManagedObject();
		if( ! pEdited->mSer_Flag.TestFlags(PreviewEdited::m_PF_HistoryFlag) )
		{

			BaseObject *ppreview = pEdited->mSer_Reference.GetObjectPointer();
			if( _ppPreviewConfiguration )
				*_ppPreviewConfiguration = &(pEdited->m_PreviewConfiguration);
			return ppreview;
		}

		pCell = pCell->GetNextBrotherCell();
	}

	return 0L;
}
#endif
#ifdef _ENGINE_EDITABLE_

void InterfacePrefObject::PreviewPrevious(unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return ;

	PackList *pPreviewList = GetOrCreatePreviewIndexList(_RenderViewIndex);
	if(!pPreviewList) return;

	PackList::Cell *pCell = pPreviewList->GetFirstCell();
	while(pCell)
	{
		PreviewEdited *pEdited = (PreviewEdited *) pCell->GetManagedObject();
		if( ! pEdited->mSer_Flag.TestFlags(PreviewEdited::m_PF_HistoryFlag) )
		{
			pEdited->mSer_Flag.AddFlags(PreviewEdited::m_PF_HistoryFlag);
			return;
		}
		pCell = pCell->GetNextBrotherCell();
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

void InterfacePrefObject::PreviewNext(unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return ;

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
			if(pPrevEdited)
			{

				pPrevEdited->mSer_Flag.RemoveFlags(PreviewEdited::m_PF_HistoryFlag);
				return;
			}
		pCell = pCell->GetNextBrotherCell();
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

void	InterfacePrefObject::SetContextEditedObject(BaseObject *_ObjectToEdit,unsigned int _EditViewIndex)
{
	if(m_isBeingDestroyed) return ;

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

			pEditorList->DeleteElement(pEdited);
		}
		if(wasInListBefore) break;

		pCell = pCell->GetNextBrotherCell();
	}

	pCell = pEditorList->AddElement(0);
	if(!pCell) return;
	Edited *pnEdited =
		(Edited *) pCell->GetManagedObject();

	pCell = pEditorList->GetFirstCell();
	while(pCell)
	{
		Edited *pCleanEdited = (Edited *) pCell->GetManagedObject();
		pCleanEdited->mSer_Flag.RemoveFlags(Edited::m_EF_HistoryFlag);
		pCell = pCell->GetNextBrotherCell();
	}

	if( _ObjectToEdit == this )
	{
		pnEdited->mSer_Flag.Set(Edited::m_EF_SelfReference);
	} else
	{
		pnEdited->mSer_Reference.SetObjectPointer( _ObjectToEdit );
	}

}
#endif
#ifdef _ENGINE_EDITABLE_

BaseObject *InterfacePrefObject::GetContextEditedObject(unsigned int _EditViewIndex)
{
	if(m_isBeingDestroyed) return 0L;

	PackList *pEditorList = GetOrCreateEditorIndexList(_EditViewIndex);
	if(!pEditorList) return 0L;

	PackList::Cell *pCell = pEditorList->GetFirstCell();
	while(pCell)
	{
		Edited *pEdited = 	(Edited *) pCell->GetManagedObject();
		if( !pEdited->mSer_Flag.TestFlags(Edited::m_EF_HistoryFlag ) )
		{

			if( pEdited->mSer_Flag.TestFlags(Edited::m_EF_SelfReference )) return this;
			else return pEdited->mSer_Reference.GetObjectPointer();
		}

		pCell = pCell->GetNextBrotherCell();
	}

	return 0L;
}
#endif
#ifdef _ENGINE_EDITABLE_

void InterfacePrefObject::EditPrevious(unsigned int _RenderViewIndex)
{
	if(m_isBeingDestroyed) return;

	PackList *pEditorList = GetOrCreateEditorIndexList(_RenderViewIndex);
	if(!pEditorList) return;

	PackList::Cell *pCell = pEditorList->GetFirstCell();
	while(pCell)
	{
		Edited *pEdited = 	(Edited *) pCell->GetManagedObject();
		if( ! pEdited->mSer_Flag.TestFlags(Edited::m_EF_HistoryFlag))
		{
			pEdited->mSer_Flag.AddFlags(Edited::m_EF_HistoryFlag);
			return;
		}
		pCell = pCell->GetNextBrotherCell();
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

void InterfacePrefObject::EditNext(unsigned int _RenderViewIndex)
{

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
			if(pPrevEdited)
			{

				pPrevEdited->mSer_Flag.RemoveFlags(Edited::m_EF_HistoryFlag);
				return;
			}
		pCell = pCell->GetNextBrotherCell();
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

 PackObjectReference::enum_SetReferenceResult
InterfacePrefObject::PackObjectReferenceSpecial::SetObjectPointer( BaseObject * _Object )
{

	 PackObjectReference::enum_SetReferenceResult result = PackObjectReference::SetObjectPointer(_Object);

	if( !_Object )
	{
		((InterfacePrefObject *)GetObjectThatManagesThis())
			->PurgePreferenceToDeadObjects();
	}

	return result;
}
#endif
#ifdef _ENGINE_EDITABLE_

void	InterfacePrefObject::PurgePreferenceToDeadObjects()
{

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

	PackList::Cell *pEditIndex= mSer_EditorIndexList.GetFirstCell();
	while(pEditIndex)
	{

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

void InterfacePrefObject::CloseReferences()
{
	m_isBeingDestroyed = true;
	BaseObject::CloseReferences();
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned int  InterfacePrefObject::GetSerializedDescriptionSize(void)
{
	PurgePreferenceToDeadObjects();
	return BaseObject::GetSerializedDescriptionSize();
}
#endif
