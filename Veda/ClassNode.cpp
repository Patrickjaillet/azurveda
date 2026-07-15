// SPDX-License-Identifier: LGPL-2.1-only

#include "ClassNode.h"
#include "BaseObject.h"
#include "BaseContext.h"
ClassNode::ClassNode( BaseObject::ClassDescription const &_classDescription  )
						: NamedObject()
	,m_ClassDescription( &_classDescription)
	,m_pNextBrotherClassNode(0L)
	,m_pFirstSonClassNode(0L)
	,m_BaseObjectList((BaseTypeCreatorCallBackFunction)_classDescription.m_NewMethod)
#ifdef _ENGINE_EDITABLE_
	,m_NumberOfObjectInstancied(0)
#endif
#ifdef _ENGINE_EDITABLE_
	,m_OwnerThreadIsCurrentlyManagingObjects(false)
	,m_TESTDEBUG_CONTEXTLOCK_ID(0)
	,m_NumberOfThreadsCurrentlyLockingTheContext(0L)
#endif
#ifdef _ENGINE_EDITABLE_
	,m_SerializationState(eSerError_OK)
#endif
#ifdef _ENGINE_EDITABLE_
	,m_pLastClassDescWithFaultySerializer(0L)
#endif
{

#ifdef _ENGINE_EDITABLE_
	mSerStr_ObjectName.Set( _classDescription.m_ClassName );
#endif
}
#ifdef _ENGINE_EDITABLE_
ClassNode::~ClassNode(void)
{

	m_OwnerThreadIsCurrentlyManagingObjects = true;
  m_TESTDEBUG_CONTEXTLOCK_ID = 1;

	while(m_NumberOfThreadsCurrentlyLockingTheContext != 0){ }

	DestroyAllManagedObjects();

	ClassNode *pNextSon;
	ClassNode *pSon = m_pFirstSonClassNode ;

	m_pFirstSonClassNode = 0L;
	while( pSon )
	{	pNextSon = pSon->m_pNextBrotherClassNode ;
		delete pSon;
		pSon = pNextSon;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

void	ClassNode::CloseAllObjects( )
{

	BaseObject *pObject;
	PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		pObject = (BaseObject *)pCell->GetManagedObject();
		pObject->Close(true);
		pCell = pCell->GetNextBrotherCell();
	}

	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		pNextSon->CloseAllObjects();
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}

}
#endif

const unsigned char * ClassNode::Serialize_In( const unsigned char * _pDescriptionChunk )
{
	NamedObject oObject;
	unsigned int chunksize;
	const unsigned char *pNextChunk = _pDescriptionChunk ;

	_pDescriptionChunk = NamedObject::Serialize_In(_pDescriptionChunk);

	chunksize =  GetChunkLength();
	pNextChunk += chunksize ;
#ifdef _ENGINE_EDITABLE_

	if( PackString::Compare(mSerStr_ObjectName.Get(),m_ClassDescription->m_ClassName ) !=0)
	{
		m_SerializationState = eSerError_BadChunkHead;
		return pNextChunk;
	}
#endif

	_pDescriptionChunk = m_BaseObjectList.Serialize_In(_pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_
	m_NumberOfObjectInstancied=0;
#endif

	PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while(pCell)
	{

		BaseObject *pOb = (BaseObject *)pCell->GetManagedObject();
		if(pOb) pOb->SetBaseContext(m_pBaseContext);
#ifdef _ENGINE_EDITABLE_
		m_NumberOfObjectInstancied++;

		m_pBaseContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectNew,pOb,0L);
#endif
		pCell = pCell->GetNextBrotherCell();
	}

	while( _pDescriptionChunk < pNextChunk )
	{

		oObject.Serialize_In(_pDescriptionChunk);

		const char * className =  oObject.GetName() ;

		ClassNode *pNextSon = m_pFirstSonClassNode ;
		while( pNextSon )
		{

			if( PackString::Compare( pNextSon->m_ClassDescription->m_ClassName ,  className ) == 0 )
			{

				pNextSon->Serialize_In(_pDescriptionChunk);

				break;
			}
			pNextSon = pNextSon->m_pNextBrotherClassNode ;
		}

		_pDescriptionChunk += oObject.GetChunkLength() ;

	}
#ifdef _ENGINE_EDITABLE_
	m_SerializationState = eSerError_OK;
#endif

	return(_pDescriptionChunk);
}

#ifdef _ENGINE_EDITABLE_

unsigned int ClassNode:: GetSerializedDescriptionSize(void)
{

	unsigned int length = NamedObject::GetSerializedDescriptionSize();

	length += m_BaseObjectList.GetSerializedDescriptionSize();

	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		if( pNextSon->IsUseful() )
			length += pNextSon->GetSerializedDescriptionSize();
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}

	unsigned int previousLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() ;
	mSerUlong_ChunkLength.Set( length );
	previousLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() - previousLength ;
	if( previousLength > 0 )
	{
		length += previousLength;

		previousLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() ;
		mSerUlong_ChunkLength.Set( length );
		previousLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() - previousLength ;
		if( previousLength > 0 )
		{
			length += previousLength;
			mSerUlong_ChunkLength.Set( length );
		}
	}
	return( length );
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * ClassNode::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{

	_pDescriptionChunkToFill = NamedObject::Serialize_Out(_pDescriptionChunkToFill);

	_pDescriptionChunkToFill = m_BaseObjectList.Serialize_Out(_pDescriptionChunkToFill);

	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		if( pNextSon->IsUseful() )
			_pDescriptionChunkToFill = pNextSon->Serialize_Out(_pDescriptionChunkToFill);
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}

	return(_pDescriptionChunkToFill);
}
#endif

BaseObject *ClassNode::GetBaseObject( const char *  _ObjectName)
{
	BaseObject *pNextObject=0L;
	PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while(pCell)
	{
		pNextObject = (BaseObject *)pCell->GetManagedObject();
		if( PackString::Compare( pNextObject->GetName(), _ObjectName ) == 0  ) return(pNextObject);
		pCell = pCell->GetNextBrotherCell();
	}

	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		pNextObject = pNextSon->GetBaseObject(_ObjectName);
		if( pNextObject ) return(pNextObject);
		pNextSon	= pNextSon->m_pNextBrotherClassNode ;
	}

	return(0L);
}

ClassNode *ClassNode::GetSubClassNodeByDescription( const BaseObject::ClassDescription * _classDescription)
{

	if( _classDescription ==  m_ClassDescription ) return(this);

	ClassNode *pSon = m_pFirstSonClassNode ;
	while( pSon )
	{

		ClassNode *pclass = pSon->GetSubClassNodeByDescription( _classDescription );
		if( pclass ) return( pclass );

		pSon = pSon->m_pNextBrotherClassNode ;
	}
	return(0L);
}

void	ClassNode::RegisterClassList( const BaseObject::ClassDescription * const * _pclassDescriptionList )
{
	ClassNode *pPossibleDouble;
	ClassNode *pSuperClass;

	while( *_pclassDescriptionList )
	{
		pPossibleDouble = GetSubClassNodeByDescription( *_pclassDescriptionList );
		pSuperClass		= GetSubClassNodeByDescription( (*_pclassDescriptionList)->m_SuperClassDescription );

		if( (!pSuperClass) || (pPossibleDouble) ){ _pclassDescriptionList++; continue; }
#ifdef _ENGINE_EDITABLE_

		ClassNode *pSon = pSuperClass->m_pFirstSonClassNode ;
		while( pSon )
		{
			if(PackString::Compare( pSon->m_ClassDescription->m_ClassName,
				(*_pclassDescriptionList)->m_ClassName)==0L)
			{

				m_pLastClassDescWithFaultySerializer = *_pclassDescriptionList ;
			}

			pSon = pSon->m_pNextBrotherClassNode ;
		}
#endif

			ClassNode *pNewClassNode = new ClassNode( **_pclassDescriptionList );

			pNewClassNode->m_pNextBrotherClassNode = pSuperClass->m_pFirstSonClassNode ;
			pSuperClass->m_pFirstSonClassNode = pNewClassNode ;

			pNewClassNode->m_pBaseContext = m_pBaseContext;

		_pclassDescriptionList++;
	}
}

#ifdef _ENGINE_EDITABLE_

BaseObject *ClassNode::NewObject( const char *  _ObjectName )
{
	PackStruct::Cell *pCell= m_BaseObjectList.AddElement();
	if( !pCell ) return(0L);
	BaseObject *pnewObj = (BaseObject *)pCell->GetManagedObject();
	if( !pnewObj ) return(0L);

	PackString	oPackString;
	const char *pClassName = m_ClassDescription->m_DefaultObjectName;
	unsigned int objectnumber = m_NumberOfObjectInstancied ;
	if( _ObjectName == 0L )
	{
		oPackString.Set( pClassName );
		oPackString.AddInt( objectnumber );
	} else
	{
		oPackString.Set(_ObjectName);
		objectnumber=0;
	}

	while( m_pBaseContext->GetBaseObject( oPackString.Get() ) )
	{
		objectnumber++;
		if( _ObjectName == 0L )

			oPackString.Set( pClassName );
		else
			oPackString.Set(_ObjectName);

		oPackString.AddInt( objectnumber );
	}

	pnewObj->SetName( oPackString.Get() );

	pnewObj->SetBaseContext( m_pBaseContext );

	m_NumberOfObjectInstancied++;

	m_pBaseContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectNew,pnewObj,0L);

	return( pnewObj );
}
#endif

#ifdef _ENGINE_EDITABLE_

#endif

#ifdef _ENGINE_EDITABLE_

void	ClassNode::DestroyManagedObject( BaseObject * _objectToDelete )
{

	PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		if( pCell->GetManagedObject() == _objectToDelete )
		{

			_objectToDelete->CloseReferences();

			m_pBaseContext->UpdateAllGUI( BaseContext::eVedaUpdate_ObjectDestroyed,_objectToDelete,0L);

			m_BaseObjectList.DeleteElement(_objectToDelete);

			m_NumberOfObjectInstancied--;

			return;
		}
		pCell = pCell->GetNextBrotherCell();
	}

}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned int ClassNode::SetObjectsUniqueReferenceIndex( unsigned int _FirstnumberToSet,int _increment )
{

	PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		((BaseObject *)pCell->GetManagedObject())->SetUniqueReference( _FirstnumberToSet );
		pCell = pCell->GetNextBrotherCell();
		_FirstnumberToSet+=_increment;
	}
	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		_FirstnumberToSet = pNextSon->SetObjectsUniqueReferenceIndex( _FirstnumberToSet, _increment );
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}
	return(_FirstnumberToSet);
}
#endif

BaseObject *ClassNode::GetBaseObjectBySerializedIndex( unsigned int _index)
{

	BaseObject *pObject;
	PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		pObject = (BaseObject *)pCell->GetManagedObject();
		if( pObject->GetUniqueReference() == _index ) return( pObject );
		pCell = pCell->GetNextBrotherCell();
	}

	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		pObject = pNextSon->GetBaseObjectBySerializedIndex( _index );
		if( pObject ) return( pObject );
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}

	return(0L);
}
#ifdef _ENGINE_EDITABLE_

void ClassNode::DestroyAllManagedObjects()
{

	while( m_BaseObjectList.GetFirstCell() )
	{

		DestroyManagedObject(  (BaseObject *) (m_BaseObjectList.GetFirstCell()->GetManagedObject() ) );
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

void ClassNode::ProtectedDestroyAllObjects()
{
	DestroyAllManagedObjects();

	ClassNode *pSon = m_pFirstSonClassNode ;
	while( pSon )
	{
		pSon->ProtectedDestroyAllObjects();
		pSon = pSon->m_pNextBrotherClassNode ;
	}

}
#endif
#ifdef _ENGINE_EDITABLE_

void ClassNode::ProtectedDestroyAllIndependantObjects( BaseObject *_pObjectWhichAllOtherShouldDependOn )
{

	BaseObject *pObject;
	PackStruct::Cell *pNextCell,*pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		pNextCell = pCell->GetNextBrotherCell();
		pObject = (BaseObject *)pCell->GetManagedObject();
		if( !(pObject->isDependantOf( _pObjectWhichAllOtherShouldDependOn ) ) )
			m_pBaseContext->DestroyObject( pObject );

		pCell = pNextCell;
	}

	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		pNextSon->ProtectedDestroyAllIndependantObjects( _pObjectWhichAllOtherShouldDependOn );
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

const unsigned char * ClassNode::MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_pendName)
{
	NamedObject oObject;
	unsigned int chunksize;
	const unsigned char *pNextChunk = _pDescriptionChunk ;

	_pDescriptionChunk = NamedObject::Serialize_In(_pDescriptionChunk);

	PackStruct::Cell *pLastCellBeforeMerge = m_BaseObjectList.GetLastCell();

	_pDescriptionChunk = m_BaseObjectList.MergeSerialize_In(_pDescriptionChunk);

	chunksize =  GetChunkLength();
	pNextChunk += chunksize ;

	PackStruct::Cell *pCell;
	if( pLastCellBeforeMerge ) pCell = pLastCellBeforeMerge->GetNextBrotherCell();
	else  pCell = m_BaseObjectList.GetFirstCell();

	while(pCell)
	{

		BaseObject *pOb = (BaseObject *)pCell->GetManagedObject();
		if(pOb) pOb->SetBaseContext(m_pBaseContext);
		m_NumberOfObjectInstancied++;

		m_pBaseContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectNew,pOb,0L);
		pCell = pCell->GetNextBrotherCell();
	}

	while( _pDescriptionChunk < pNextChunk )
	{

		oObject.Serialize_In(_pDescriptionChunk);

		const char * className =  oObject.GetName() ;
		ClassNode *pNextSon = m_pFirstSonClassNode ;
		while( pNextSon )
		{

			if( PackString::Compare( pNextSon->GetName() ,  className ) == 0 )
			{
				pNextSon->MergeSerialize_In(_pDescriptionChunk,_pendName);
				break;
			}
			pNextSon = pNextSon->m_pNextBrotherClassNode ;
		}

		_pDescriptionChunk += oObject.GetChunkLength() ;

	}

	return(_pDescriptionChunk);
}
#endif

#ifdef _ENGINE_EDITABLE_

void ClassNode::FlushNames()
{

	BaseObject *pObject;
	PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		pObject = (BaseObject *)pCell->GetManagedObject();
		pObject->SetName("");
		pCell = pCell->GetNextBrotherCell();
	}

	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		pNextSon->FlushNames();
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

bool ClassNode::IsUseful() const
{
	PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	if( pCell ) return true;

	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		if( pNextSon->IsUseful() ) return true;
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}

	return false;
}
#endif
#ifdef _ENGINE_EDITABLE_

void ClassNode::ProtectedAddSameClasses( BaseContext *_pContextToAddManagement )
{

	const BaseObject::ClassDescription *pclassDescriptionList[2]=
	{0L,0L};
	pclassDescriptionList[0] = m_ClassDescription;

	_pContextToAddManagement->RegisterClassList( pclassDescriptionList );

	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		pNextSon->ProtectedAddSameClasses(_pContextToAddManagement);
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

void ClassNode::ExportCPPClassDescription( PackString &_fileTextImage ) const
{

	if( ! this->IsUseful() ) return;

	_fileTextImage.AddString("class ");
	_fileTextImage.AddString( m_ClassDescription->m_CPlusPlusClassName );
	_fileTextImage.AddString(" {public: static const BaseObject::ClassDescription m_Description; };\n");

	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		pNextSon->ExportCPPClassDescription(_fileTextImage);
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}

}
#endif
#ifdef _ENGINE_EDITABLE_

void ClassNode::ExportCPPClassDescriptionLine( PackString &_fileTextImage ) const
{

	if( ! this->IsUseful() ) return;

	_fileTextImage.AddString("\t&");
	_fileTextImage.AddString( m_ClassDescription->m_CPlusPlusClassName );
	_fileTextImage.AddString("::m_Description,\n");

	ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		pNextSon->ExportCPPClassDescriptionLine(_fileTextImage);
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}

}
#endif
#ifdef _ENGINE_EDITABLE_

const BaseObject::ClassDescription *ClassNode::CheckClassListError() const
{
	return m_pLastClassDescWithFaultySerializer;
}
#endif
