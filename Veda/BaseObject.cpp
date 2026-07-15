// SPDX-License-Identifier: LGPL-2.1-only

#include "BaseObject.h"
#include "PackObjectReference.h"
#include "BaseContext.h"

const BaseObject::ClassDescription BaseObject::m_Description=
{
	0L
	,"Ob"
	,0L
#ifdef _ENGINE_EDITABLE_
	,"BaseObject"
	,0L
	,0L
	,0L
#endif
};
#ifdef _ENGINE_EDITABLE_

const char *BaseObject::m_pErrorString_Memory="Memory Allocation Error";
const char *BaseObject::m_pErrorString_CantOpenFile="Can't Open File";
#endif

BaseObject::BaseObject() : NamedObject()
	,m_bIsCreatedAccordingToAllUsedObject(false)
#ifdef _ENGINE_EDITABLE_
	,m_pFirstReferenceToThisObject(0L)
	,m_CreationRate(1.0f)
	,m_pFirstToolMethodCell(0L)
	,m_LastCreationFailed(false)
#endif
{
	RegisterSerializableMember( mSer_UniqueReference );

}
#ifdef _ENGINE_EDITABLE_
BaseObject::~BaseObject(void)
{

	Close();

	ToolMethodCell *pNextCell,*pCell = m_pFirstToolMethodCell;
	while(pCell )
	{
		pNextCell =  pCell->m_pNextCell ;
		delete pCell;
		pCell = pNextCell;
	}
	m_pFirstToolMethodCell = 0L ;
}
#endif

#ifdef _ENGINE_EDITABLE_

int BaseObject::Create(bool _immediate)
{

	if( m_bIsCreatedAccordingToAllUsedObject ) return 0;
	BaseContext *pContext = GetContext();
	if(!pContext) return -1;

	mCreationStackCell_pPrevCell = 0L;
	mCreationStackCell_pNextCell = 0L;
	BaseObject	*pLastInList = this ;
	BaseObject	*pObjectToBeCreated = this;
	unsigned int buildingTotalWeight = 1;
	while( pObjectToBeCreated )
	{

		PackObjectReference	*pRef =	pContext->GetFirstObjectReferenceInContextList();
		while( pRef )
		{
			if( pRef->GetObjectThatManagesThis() == pObjectToBeCreated )
			{
				BaseObject *pUsedObject = pRef->GetObjectPointer();
				if( pUsedObject && (pUsedObject != pLastInList) )
				{

						BaseObject	*pVerifyUp = pLastInList;
						while( pVerifyUp )
						{
							if( pVerifyUp == pUsedObject )
							{

								pVerifyUp->mCreationStackCell_pPrevCell->mCreationStackCell_pNextCell =
									pVerifyUp->mCreationStackCell_pNextCell;
								pVerifyUp->mCreationStackCell_pNextCell->mCreationStackCell_pPrevCell =
									pVerifyUp->mCreationStackCell_pPrevCell;
								buildingTotalWeight--;
								break;
							}
							pVerifyUp = pVerifyUp->mCreationStackCell_pPrevCell;
						}

						pLastInList->mCreationStackCell_pNextCell = pUsedObject;
						pUsedObject->mCreationStackCell_pPrevCell = pLastInList;
						pUsedObject->mCreationStackCell_pNextCell = 0L;
						pLastInList = pUsedObject ;
						buildingTotalWeight++;

				}
			}

			pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
		}

		pObjectToBeCreated = pObjectToBeCreated->mCreationStackCell_pNextCell ;
	}

	unsigned int buildingDoneWeight = 0;

	while( pLastInList  )
	{
		buildingDoneWeight++;
		m_CreationRate = 1.0f- ((float)buildingDoneWeight)/((float)buildingTotalWeight);

		if( !(pLastInList->m_bIsCreatedAccordingToAllUsedObject) )
		{

			bool lasterrorstate = pLastInList->DidLastCreationFailed();
			if( !(pLastInList->CreateInternal()) )
			{

				pLastInList->CloseInternal();
				pLastInList->m_LastCreationFailed = true;
				if(!lasterrorstate)
				{
					pContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectClosed,pLastInList,0L);
				}

				BaseObject	*pFirstObjectToFailOnDependencyChain = pLastInList;
				pLastInList = pFirstObjectToFailOnDependencyChain->mCreationStackCell_pPrevCell ;
				while(pLastInList)
				{

					if(pFirstObjectToFailOnDependencyChain->isDependantOf(pLastInList))
					{

						if(! pLastInList->DidLastCreationFailed())
						{
							pLastInList->m_LastCreationFailed = true;
							pLastInList->m_LastCreationErrorString.Set("Failed because Object:");
							pLastInList->m_LastCreationErrorString.AddString(
								pFirstObjectToFailOnDependencyChain->GetName() );
							pLastInList->m_LastCreationErrorString.AddString(" also failed");
							pContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectClosed,pLastInList,0L);

						}
					}
					pLastInList = pLastInList->mCreationStackCell_pPrevCell ;
				}
				return(-2);
			}
			m_LastCreationErrorString.Set("");
			pLastInList->m_LastCreationFailed = false;

			pLastInList->m_bIsCreatedAccordingToAllUsedObject = true;
			pContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectReCreated ,pLastInList,0L);

			if( !_immediate ) return buildingTotalWeight-buildingDoneWeight;
		}

		pLastInList = pLastInList->mCreationStackCell_pPrevCell ;
	}
	m_CreationRate = 0.0f;
	return 0;
}
#endif
#ifndef _ENGINE_EDITABLE_

int BaseObject::Create(bool _immediate)
{

	if( m_bIsCreatedAccordingToAllUsedObject ) return 0;
	BaseContext *pContext = GetContext();
	if(!pContext) return -1;

	mCreationStackCell_pPrevCell = 0L;
	mCreationStackCell_pNextCell = 0L;
	BaseObject	*pLastInList = this ;
	BaseObject	*pObjectToBeCreated = this;
	unsigned int buildingWeight = 1;
	while( pObjectToBeCreated )
	{

		PackObjectReference	*pRef =	pContext->GetFirstObjectReferenceInContextList();
		while( pRef )
		{
			if( pRef->GetObjectThatManagesThis() == pObjectToBeCreated )
			{
				BaseObject *pUsedObject = pRef->GetObjectPointer();
				if( pUsedObject && (pUsedObject != pLastInList) )
				{
					if( !(pUsedObject->m_bIsCreatedAccordingToAllUsedObject) )
					{

						BaseObject	*pVerifyUp = pLastInList;
						while( pVerifyUp )
						{
							if( pVerifyUp == pUsedObject )
							{

								pVerifyUp->mCreationStackCell_pPrevCell->mCreationStackCell_pNextCell =
									pVerifyUp->mCreationStackCell_pNextCell;
								pVerifyUp->mCreationStackCell_pNextCell->mCreationStackCell_pPrevCell =
									pVerifyUp->mCreationStackCell_pPrevCell;
								buildingWeight--;
								break;
							}
							pVerifyUp = pVerifyUp->mCreationStackCell_pPrevCell;
						}

						pLastInList->mCreationStackCell_pNextCell = pUsedObject;
						pUsedObject->mCreationStackCell_pPrevCell = pLastInList;
						pUsedObject->mCreationStackCell_pNextCell = 0L;
						pLastInList = pUsedObject ;
						buildingWeight++;
					}
				}
			}

			pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
		}

		pObjectToBeCreated = pObjectToBeCreated->mCreationStackCell_pNextCell ;
	}

	while( pLastInList  )
	{

		if( !(pLastInList->CreateInternal()) ) return(-2);

		pLastInList->m_bIsCreatedAccordingToAllUsedObject = true;
		buildingWeight--;
		if( !_immediate ) return buildingWeight;

		pLastInList = pLastInList->mCreationStackCell_pPrevCell ;
	}
	return 0;
}
#endif

#ifdef _ENGINE_EDITABLE_

void BaseObject::Close(bool _AndCloseAllDependantObjects)
{
	if( !m_bIsCreatedAccordingToAllUsedObject ) return;

	CloseInternal();

	m_bIsCreatedAccordingToAllUsedObject = false;
	m_LastCreationFailed = false;
	m_CreationRate =1.0f ;

	if(_AndCloseAllDependantObjects)
	{
		PackObjectReference *pref = m_pFirstReferenceToThisObject;
		while( pref )
		{
			BaseObject *pObject = pref->GetObjectThatManagesThis() ;

			pObject->Close(true);
			pref = pref->GetNextSelfReferenceNode() ;
		}
	}

	if( m_pBaseContext ) m_pBaseContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectClosed, this,0L);
}
#endif

#ifdef _ENGINE_EDITABLE_

const BaseObject::ClassDescription &BaseObject::GetClassDescription( void ) const
{
	return( BaseObject::m_Description );
}
#endif

#ifdef _ENGINE_EDITABLE_

void BaseObject::CloseReferences()
{

	while( m_pFirstReferenceToThisObject )
	{
		m_pFirstReferenceToThisObject->SetObjectPointer(0L);
	}

}
#endif
#ifdef _ENGINE_EDITABLE_

bool	BaseObject::isDependantOf( BaseObject *_pObjectSuspectedOfOwnage )
{

	if( _pObjectSuspectedOfOwnage == this ) return(true);

	PackObjectReference *pref = m_pFirstReferenceToThisObject;
	while( pref )
	{
		BaseObject *pObject = pref->GetObjectThatManagesThis() ;
		if( pObject == _pObjectSuspectedOfOwnage ) return( true );

		if( pObject->isDependantOf(_pObjectSuspectedOfOwnage) ) return( true);
		pref = pref->GetNextSelfReferenceNode() ;
	}

	return(false);
}
#endif

void	BaseObject::SetBaseContext(BaseContext *_pBaseContext)
{
	m_pBaseContext = _pBaseContext ;

	Cell *pCell = GetFirstCell();
	while( pCell )
	{

		pCell->GetManagedObject()->SetObjectThatManagesThis(this);
		pCell = pCell->GetNextBrotherCell();
	}
}

#ifdef _ENGINE_EDITABLE_

void BaseObject::ProcessPreview(double ,VirtualMachine::InternalViewPort *,const PreviewConfiguration *)
{
}
#endif
#ifdef _ENGINE_EDITABLE_

BaseObject *BaseObject::Clone(const char *_cloneName)
{

	if( !GetContext() ) return(0L);

	BaseObject *pnewOb = GetContext()->NewObject( GetClassDescription() );
	if(!pnewOb)	return(0L);

	GetContext()->SetObjectsUniqueReferenceIndex();

	GetContext()->UpdateAllReferencesSerializationID();

	unsigned int serializedsize = GetSerializedDescriptionSize();
	unsigned char *pSerChunk = new unsigned char[ serializedsize ];
	if( !pSerChunk ) return(0L);

	Serialize_Out(pSerChunk);
	pnewOb->Serialize_In(pSerChunk);
	delete [] pSerChunk;

	if(_cloneName)	pnewOb->SetName(_cloneName);
	else
	{
		PackString	str;
		str.Set( pnewOb->GetName() );
		str.AddString("_c");
		pnewOb->SetName(str.Get());
	}

	PackObjectReference *pRef = GetContext()->GetFirstObjectReferenceInContextList();
	while( pRef )
	{
		unsigned int sernum = pRef->Get();
		BaseObject *pObjRefered = GetContext()->GetBaseObjectBySerializedIndex( sernum );
		pRef->SetObjectPointer( pObjRefered );

		pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
	}

	return(pnewOb);
}
#endif

VirtualMachine	*BaseObject::GetMachine()
{
	return m_pBaseContext->GetMachine() ;
}

#ifdef _ENGINE_EDITABLE_

void BaseObject::ExecuteToolMethod( unsigned int  )
{
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned int BaseObject::GetNextToolMethod( unsigned int _PreviousMethodID,unsigned int &_NextMethodflagInfo ,const char *&_pNextShortDisplayName,const char *&_pNextDisplayHelp )
{
	ToolMethodCell *pCell = m_pFirstToolMethodCell;

	while(pCell )
	{
		if(_PreviousMethodID == 0) break;
		unsigned int previousid = pCell->m_ToolMethodID ;
		pCell = pCell->m_pNextCell ;
		if(previousid == _PreviousMethodID) break;
	}
	if(pCell)
	{
		_NextMethodflagInfo = pCell->m_FlagInfo ;
		_pNextShortDisplayName = pCell->m_ShortDisplayName.Get() ;
		_pNextDisplayHelp = pCell->m_DisplayHelp.Get() ;
		return pCell->m_ToolMethodID ;
	}
	return 0;
}
#endif
#ifdef _ENGINE_EDITABLE_

void BaseObject::RegisterToolMethod( unsigned int _MethodID,unsigned int _MethodflagInfo ,const char *_pShortDisplayName,const char *_pDisplayHelp )
{
	ToolMethodCell *pCell = new ToolMethodCell;
	if(!pCell) return;
	pCell->m_pNextCell = m_pFirstToolMethodCell;
	m_pFirstToolMethodCell = pCell ;

	pCell->m_ToolMethodID	= _MethodID;
	pCell->m_FlagInfo		= _MethodflagInfo;
	pCell->m_ShortDisplayName.Set( _pShortDisplayName );
	pCell->m_DisplayHelp.Set( _pDisplayHelp );

}
#endif
