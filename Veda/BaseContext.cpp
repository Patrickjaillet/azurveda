// SPDX-License-Identifier: LGPL-2.1-only

#include "BaseContext.h"
#include "ClassNode.h"
#include "BaseObject.h"
#include "PackObjectReference.h"
#include "PackResource.h"
#include "PackString.h"

BaseContext::BaseContext()

	: ClassNode( BaseObject::m_Description )
	,m_pFirstObjectReferenceInContextList(0L)
	,m_pMachine(0L)
#ifdef _ENGINE_EDITABLE_
	,m_GUIUpdateFunctionList(UpdateFunctionListCell::NewCell)
	,m_UndoStack( BaseContext::UndoCell::NewCell)
	,m_pCurrentUndoCell(0L)
	,m_MaximumNumberOfUndoableEvents(0L)
	,m_CurrentNumberOfUndoableEvents(0L)
#endif
{

	m_pBaseContext = this;
}
#ifdef _ENGINE_EDITABLE_
BaseContext::~BaseContext(void)
{

	FlushUndoStack();

	while( m_GUIUpdateFunctionList.GetFirstCell()  )
	{
		RemoveGUIUpdateFunction(
			((UpdateFunctionListCell *)(m_GUIUpdateFunctionList.GetFirstCell()->GetManagedObject()))
			->m_func
			);
	}

	ProtectedDestroyAllObjects();
}
#endif
#ifdef _ENGINE_EDITABLE_

BaseObject *BaseContext::NewObject( const BaseObject::ClassDescription & _classDescription ,char *  _ObjectName )
{

	ClassNode *pClassNode = GetSubClassNodeByDescription( &_classDescription);
	if( !pClassNode ) return(0L);

	return( pClassNode->NewObject(_ObjectName) );

}
#endif
#ifdef _ENGINE_EDITABLE_

void	BaseContext::DestroyObject( BaseObject * _objectToDelete )
{

	ClassNode *pClassNode = GetSubClassNodeByDescription(  &_objectToDelete->GetClassDescription() );
	if( !pClassNode ) return;

	m_OwnerThreadIsCurrentlyManagingObjects =true;
	m_TESTDEBUG_CONTEXTLOCK_ID = 5;

	while(m_NumberOfThreadsCurrentlyLockingTheContext != 0){
		if( m_pMachine) m_pMachine->Sleep(20);
		}

		pClassNode->DestroyManagedObject( _objectToDelete );

	m_OwnerThreadIsCurrentlyManagingObjects =false;

	FlushUndoStack();

}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned int BaseContext:: GetSerializedDescriptionSize(void)
{

	SetObjectsUniqueReferenceIndex();

	UpdateAllReferencesSerializationID();

	return( ClassNode::GetSerializedDescriptionSize() );
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * BaseContext::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{

	SetObjectsUniqueReferenceIndex();

	UpdateAllReferencesSerializationID();

	return( ClassNode::Serialize_Out( _pDescriptionChunkToFill ) );
}
#endif

const unsigned char * BaseContext::Serialize_In( const unsigned char * _pDescriptionChunk)
{
#ifdef _ENGINE_EDITABLE_

	const unsigned char *pdummyTest = _pDescriptionChunk;
	PackULong	dummyUL;
	PackString	dummyString;
	pdummyTest = dummyUL.Serialize_In(pdummyTest);
	pdummyTest = dummyString.Serialize_In(pdummyTest);
	if( PackString::Compare(dummyString.Get(),m_ClassDescription->m_ClassName ) !=0)
	{
		m_SerializationState = eSerError_BadChunkHead;
		return _pDescriptionChunk;
	}
#endif

#ifdef _ENGINE_EDITABLE_
	m_OwnerThreadIsCurrentlyManagingObjects = true;
	m_TESTDEBUG_CONTEXTLOCK_ID = 6;

	while(m_NumberOfThreadsCurrentlyLockingTheContext != 0){if( m_pMachine) m_pMachine->Sleep(20);}
#endif

#ifdef _ENGINE_EDITABLE_

	FlushUndoStack();
	ProtectedDestroyAllObjects();
#endif

	_pDescriptionChunk = ClassNode::Serialize_In( _pDescriptionChunk );

	PackObjectReference *pRef = m_pFirstObjectReferenceInContextList;
	while( pRef )
	{
		BaseObject *pObjRefered = GetBaseObjectBySerializedIndex( pRef->Get() );

		if(pObjRefered) pRef->SetObjectPointer( pObjRefered );

		pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
	}
#ifdef _ENGINE_EDITABLE_
	m_OwnerThreadIsCurrentlyManagingObjects = false;
#endif
	return(_pDescriptionChunk);
}

void	BaseContext::LinkObjectReference( PackObjectReference *_pObjectReferenceToLink )
{

	*(_pObjectReferenceToLink->GetPointerOnNextObjectReferenceInContextList()) =
		m_pFirstObjectReferenceInContextList;
	m_pFirstObjectReferenceInContextList = _pObjectReferenceToLink;

}
#ifdef _ENGINE_EDITABLE_

void BaseContext::UnLinkObjectReference( PackObjectReference *_pReferenceToUnlink )
{

	PackObjectReference **ppref = &m_pFirstObjectReferenceInContextList;
	while( *ppref )
	{
		if( (*ppref) == _pReferenceToUnlink )
		{

			*ppref =  *((*ppref)->GetPointerOnNextObjectReferenceInContextList()) ;

		}
		else
		{

			ppref = (*ppref)->GetPointerOnNextObjectReferenceInContextList();
		}
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

bool BaseContext::CanUndo()
{
	return(m_pCurrentUndoCell != 0L);
}
#endif
#ifdef _ENGINE_EDITABLE_

bool BaseContext::Undo()
{

	if(!m_pCurrentUndoCell) return false;

	UndoCell *pUndoCell = (UndoCell *)m_pCurrentUndoCell->GetManagedObject();

	NotifyObjectChange( BaseContext::eVedaUpdate_MemberIsGoingToChange,
							pUndoCell->GetMemberToChange()->GetObjectThatManagesThis(),
							pUndoCell->GetMemberToChange()  );

		pUndoCell->GetMemberToChange()->Serialize_In(
			pUndoCell->GetPreviousValueChunk()
			);

		m_pCurrentUndoCell = m_pCurrentUndoCell->GetPreviousBrotherCell();

	NotifyObjectChange( BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo,
							pUndoCell->GetMemberToChange()->GetObjectThatManagesThis(),
							pUndoCell->GetMemberToChange()  );

	return true;
}
#endif
#ifdef _ENGINE_EDITABLE_

bool BaseContext::CanRedo()
{
	if(!m_pCurrentUndoCell) return( m_UndoStack.GetFirstCell() != 0L );
	else return(   m_pCurrentUndoCell->GetNextBrotherCell() != 0L  );
}
#endif
#ifdef _ENGINE_EDITABLE_

bool BaseContext::Redo()
{

	if(!m_pCurrentUndoCell)
	{
		m_pCurrentUndoCell = m_UndoStack.GetFirstCell();
		if( !m_pCurrentUndoCell ) return(false);
	}
	else
	{
		if(  m_pCurrentUndoCell->GetNextBrotherCell() == 0L ) return(false);
		m_pCurrentUndoCell = m_pCurrentUndoCell->GetNextBrotherCell() ;
	}

	UndoCell *pRedoCell =(UndoCell *)m_pCurrentUndoCell->GetManagedObject();

	NotifyObjectChange( BaseContext::eVedaUpdate_MemberIsGoingToChange,
							pRedoCell->GetMemberToChange()->GetObjectThatManagesThis(),
							pRedoCell->GetMemberToChange()  );

		pRedoCell->GetMemberToChange()->Serialize_In(
			pRedoCell->GetAfterValueChunk()
			);

	NotifyObjectChange( BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo,
							pRedoCell->GetMemberToChange()->GetObjectThatManagesThis(),
							pRedoCell->GetMemberToChange()  );

	return true;
}
#endif
#ifdef _ENGINE_EDITABLE_

void	BaseContext::DestroyAllObjects( )
{

	ProtectedDestroyAllObjects();
}
#endif

#ifdef _ENGINE_EDITABLE_

void BaseContext::NotifyObjectChange( eVedaUpdate _UpDateType,BaseObject *_pObjectEdited, BaseType *_ObjectMember,
							const unsigned char *_PreviousShapeChunk,
							unsigned int		 _PreviousShapeChunkLength,
							const unsigned char *_NewShapeChunk,
							unsigned int		 _NewShapeChunkLength
							)
{

	if( m_MaximumNumberOfUndoableEvents >0 &&
		_UpDateType == eVedaUpdate_MemberChanged &&
		_ObjectMember->GetClassID() &&
			(PackString::Compare(_ObjectMember->GetClassID(),"PackList") != 0) &&
			(PackString::Compare(	_ObjectMember->GetClassID(),"PackTreeCell") != 0) &&
		_PreviousShapeChunk &&
		_NewShapeChunk
		)
	{

		if( m_pCurrentUndoCell )
		{
			while( m_pCurrentUndoCell->GetNextBrotherCell() )
			{
				m_UndoStack.DeleteElement( m_pCurrentUndoCell->GetNextBrotherCell()->GetManagedObject() );
				m_CurrentNumberOfUndoableEvents--;
			}
		} else
		{
			FlushUndoStack();
		}

		PackStruct::Cell *plistcell = m_UndoStack.AddElement();
		m_CurrentNumberOfUndoableEvents++;

		UndoCell *pUndoCell = (UndoCell *)plistcell->GetManagedObject();
		pUndoCell->Init(
				eVedaUpdate_MemberChanged, _ObjectMember,
				_PreviousShapeChunk,
				_PreviousShapeChunkLength,
				_NewShapeChunk,
				_NewShapeChunkLength );

		m_pCurrentUndoCell = plistcell ;

		if( m_CurrentNumberOfUndoableEvents > m_MaximumNumberOfUndoableEvents )
		{
			if( m_UndoStack.GetFirstCell() )
				m_UndoStack.DeleteElement(
					m_UndoStack.GetFirstCell()->GetManagedObject() );

			m_CurrentNumberOfUndoableEvents--;
		}
	}

	if( _UpDateType == eVedaUpdate_MemberIsGoingToChange )
	{

		m_OwnerThreadIsCurrentlyManagingObjects = true;
		m_TESTDEBUG_CONTEXTLOCK_ID = 4;

		while(m_NumberOfThreadsCurrentlyLockingTheContext != 0){if( m_pMachine) m_pMachine->Sleep(20);}

	}

	if( _UpDateType == eVedaUpdate_MemberChanged ||
		_UpDateType == eVedaUpdate_MemberChangedBecauseOfUndo)
	{

		_pObjectEdited->Close(true);

		m_OwnerThreadIsCurrentlyManagingObjects =false;
	}

	if( _UpDateType == eVedaUpdate_MemberDestroyed ) FlushUndoStack();

	UpdateAllGUI( _UpDateType, _pObjectEdited , _ObjectMember );

}
#endif
#ifdef _ENGINE_EDITABLE_

void BaseContext::FlushUndoStack()
{
	while( m_UndoStack.GetFirstCell() )
		m_UndoStack.DeleteElement((unsigned int)0);
	m_CurrentNumberOfUndoableEvents = 0;
	m_pCurrentUndoCell = 0L;
}
#endif
#ifdef _ENGINE_EDITABLE_

void BaseContext::SetMaximumNumberOfUndoableEvents( unsigned int _maxNumber )
{

	if( m_MaximumNumberOfUndoableEvents != 0 ) return;
	m_MaximumNumberOfUndoableEvents = _maxNumber;

}
#endif
#ifdef _ENGINE_EDITABLE_

void BaseContext::AddGUIUpdateFunction(	void (*_func)(void *_GUIObjectToUpdateReturned,eVedaUpdate _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated) ,
								void *_GUIObjectToUpdate
								)
{

	if( !_func ) return;
	PackStruct::Cell *pNewCEll = m_GUIUpdateFunctionList.AddElement();
	if(!pNewCEll) return;
	UpdateFunctionListCell *pUpdateCell = (UpdateFunctionListCell *)pNewCEll->GetManagedObject();

	pUpdateCell->m_func			= _func;
	pUpdateCell->m_UserObject	=  _GUIObjectToUpdate ;

}
#endif
#ifdef _ENGINE_EDITABLE_

bool BaseContext::RemoveGUIUpdateFunction(	void (*_func)(void *_GUIObjectToUpdateReturned,eVedaUpdate _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated) )
{

	PackStruct::Cell *pCell = m_GUIUpdateFunctionList.GetFirstCell();
	while( pCell )
	{
		if(
			((UpdateFunctionListCell *)pCell->GetManagedObject())->m_func
			== 	_func
			)
		{

			m_GUIUpdateFunctionList.DeleteElement( pCell->GetManagedObject() );
			return(true);
		}
		pCell = pCell->GetNextBrotherCell() ;
	}
	return(false);
}
#endif

#ifdef _ENGINE_EDITABLE_
BaseContext::UndoCell::UndoCell() : PackStruct()
	,m_EditionType(0L)
	,m_pObjectMemberEdited(0L)
	,m_pPreviousValueChunk(0L)
	,m_pAfterValueChunk(0L)
{

}
void BaseContext::UndoCell::Init(unsigned int _editionType,
					BaseType *_pObjectMemberEdited,
					const unsigned char *_PreviousValueChunk,
					unsigned int _PreviousValueChunkLength,
					const unsigned char *_AfterValueChunk,
					unsigned int _AfterValueChunkLength )
{
	unsigned int ii;
	m_EditionType			= _editionType ;
	m_pObjectMemberEdited	= _pObjectMemberEdited ;

	if( _PreviousValueChunkLength > 0)
	{
		m_pPreviousValueChunk = new unsigned char[_PreviousValueChunkLength];
		ii=0;
		while( m_pPreviousValueChunk != 0L && ii < _PreviousValueChunkLength)
		{
			m_pPreviousValueChunk[ii] = _PreviousValueChunk[ii];
			ii++;
		}
	}

	if( _AfterValueChunkLength > 0)
	{
		m_pAfterValueChunk = new unsigned char[ _AfterValueChunkLength ];
		ii=0;
		while( m_pAfterValueChunk != 0L && ii < _AfterValueChunkLength)
		{
			m_pAfterValueChunk[ii] = _AfterValueChunk[ii];
			ii++;
		}
	}

}
BaseContext::UndoCell::~UndoCell()
{
	if( m_pPreviousValueChunk ) delete [] m_pPreviousValueChunk;
	if( m_pAfterValueChunk ) delete [] m_pAfterValueChunk;

}
#endif

#ifdef _ENGINE_EDITABLE_

void BaseContext::UpdateAllGUI(  eVedaUpdate _updateType,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated)
{
	PackStruct::Cell *pCell = m_GUIUpdateFunctionList.GetFirstCell();
	while( pCell )
	{
		UpdateFunctionListCell *PupdateCell = (UpdateFunctionListCell *)pCell->GetManagedObject();
		PupdateCell->m_func( PupdateCell->m_UserObject,
								_updateType,
								this,
								_ObjectUpdated,
								_MemberUpdated);
		pCell = pCell->GetNextBrotherCell() ;
	}

}
#endif
#ifdef _ENGINE_EDITABLE_

void BaseContext::DestroyAllIndependantObjects( BaseObject *_pObjectWhichAllOtherShouldDependOn )
{

	if( _pObjectWhichAllOtherShouldDependOn->GetContext() != this ) return;

	ProtectedDestroyAllIndependantObjects(_pObjectWhichAllOtherShouldDependOn);

}
#endif
#ifdef _ENGINE_EDITABLE_

const unsigned char * BaseContext::MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_endName)
{

	SetObjectsUniqueReferenceIndex(0,0);

	register PackObjectReference *pRef = m_pFirstObjectReferenceInContextList;
	while( pRef )
	{
		pRef->Set(0UL);
		pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
	}

	_pDescriptionChunk = ClassNode::MergeSerialize_In( _pDescriptionChunk , _endName );

	pRef = m_pFirstObjectReferenceInContextList;
	while( pRef )
	{
		unsigned int refnum = pRef->Get();

		if(refnum != 0)
		{
			BaseObject *pObjRefered = GetBaseObjectBySerializedIndex(refnum );
			if(pObjRefered) pRef->SetObjectPointer( pObjRefered );
		}

		pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
	}

	return(_pDescriptionChunk);

}
#endif
#ifdef _ENGINE_EDITABLE_

bool BaseContext::AddOneLockForThreadUse()
{

	m_NumberOfThreadsCurrentlyLockingTheContext++;

	if(m_OwnerThreadIsCurrentlyManagingObjects)
	{
		m_NumberOfThreadsCurrentlyLockingTheContext--;
		return false;
	}
	return true;
}
#endif
#ifdef _ENGINE_EDITABLE_

void BaseContext::RemoveOneLockForThreadUse()
{
	m_NumberOfThreadsCurrentlyLockingTheContext--;

}
#endif
#ifdef _ENGINE_EDITABLE_

void BaseContext::CloneClassesAndObjects(BaseContext *_pContextToModify)
{
	if(!_pContextToModify) return;

	_pContextToModify->DestroyAllObjects();

	ProtectedAddSameClasses(_pContextToModify);

	unsigned int	serSize =GetSerializedDescriptionSize();
	if(serSize==0) return;
	unsigned char	*pChunk = new unsigned char[serSize];
	if(!pChunk) return;
	Serialize_Out(pChunk);
	_pContextToModify->Serialize_In(pChunk);
	delete [] pChunk;

}
#endif
#ifdef _ENGINE_EDITABLE_

void BaseContext::ExportAsCPPTableFile(const char *_pfileToExport,const char *_pGobalTableName,bool _optimize)
{
	BaseContext	*ObjToSerialize=this;
	BaseContext	dummyClone;

	if(_optimize)
	{
		CloneClassesAndObjects( &dummyClone );
		dummyClone.FlushNames();
		ObjToSerialize = &dummyClone;
	}

	unsigned int serializedSize = ObjToSerialize->GetSerializedDescriptionSize();
	if(serializedSize == 0) return;
	unsigned char	*pchunk = new unsigned char[serializedSize];
	if( pchunk )
	{
		unsigned char	*pChunkVector=pchunk;

		ObjToSerialize->Serialize_Out(pchunk);

		PackString	fileText;
		PackResource	fileres;
		fileres.SetFile(_pfileToExport);
		fileres.SetLinkMode( false);

		fileText.Set("class BaseObject { public: class ClassDescription; };\n");

		register const ClassNode *pNextSon = m_pFirstSonClassNode ;
		while( pNextSon )
		{
			pNextSon->ExportCPPClassDescription(fileText);
			pNextSon = pNextSon->GetNextBrotherClassNode() ;
		}

		fileText.AddString("const BaseObject::ClassDescription * ContextClassTable[]=\n{\n");
		pNextSon = m_pFirstSonClassNode ;
		while( pNextSon )
		{
			pNextSon->ExportCPPClassDescriptionLine(fileText);
			pNextSon = pNextSon->GetNextBrotherClassNode() ;
		}
		fileText.AddString("0L\n};\n");

		fileText.AddString("\nunsigned char ");
		if(_pGobalTableName == 0L) _pGobalTableName="ContextDescription";
		fileText.AddString(_pGobalTableName);
		fileText.AddString("[");
		fileText.AddInt(serializedSize);
		fileText.AddString("]=\n{\n");

		fileres.SetBinary( (const char *) fileText.Get(), fileText.length() );
		fileText.Set("");

		unsigned int	ii=0;
		unsigned int	intheline=0;
		fileText.AddInt( (unsigned int) (*(pChunkVector++)) );
		ii++;
		while(ii<serializedSize)
		{
			fileText.AddString(",");
			fileText.AddInt((unsigned int) (*pChunkVector++));

			intheline++;
			ii++;
			if( intheline >=50)
			{
				intheline = 0;
				fileText.AddString("\n");
				fileres.SetBinary( (const char *) fileText.Get(), fileText.length(),true );
				fileText.Set("");

			}
		}

		fileText.AddString("\n};\n");
		fileres.SetBinary( (const char *) fileText.Get(), fileText.length(),true );

		delete [] pchunk;
	}
}
#endif

#ifdef _ENGINE_EDITABLE_

void BaseContext::UpdateAllReferencesSerializationID()
{

	PackObjectReference *pRef = GetFirstObjectReferenceInContextList();
	while( pRef )
	{
		pRef->UpdateSerializedID();

		pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
	}
}
#endif
