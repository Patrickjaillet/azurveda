/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
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
	// declare the name of this object stands for the class it manages:
/* note: in non-editable mode,(serialize_in only) classnone GetName() doesn't have the class ID.
  if( PackString::Compare( pNextSon->GetName() ,  className ) == 0 ) */

#ifdef _ENGINE_EDITABLE_
	mSerStr_ObjectName.Set( _classDescription.m_ClassName );
#endif
}
#ifdef _ENGINE_EDITABLE_
ClassNode::~ClassNode(void)
{
	// threadsafe lock: (don't change order of the 2 lines !!!)
	m_OwnerThreadIsCurrentlyManagingObjects = true;
  m_TESTDEBUG_CONTEXTLOCK_ID = 1;
	// wait unlock from all threads. 
	while(m_NumberOfThreadsCurrentlyLockingTheContext != 0){ }

	// note: for object-to-object dependance safety, we had to use DestroyManagedObject()
	DestroyAllManagedObjects();

	// recursive deletion of all subclass nodes.
	register ClassNode *pNextSon;
	register ClassNode *pSon = m_pFirstSonClassNode ;
	// IMPORTANT: PackReference's destruction at context destruction try to search dead ClassNodes.
	m_pFirstSonClassNode = 0L; // officially destroyed. 
	while( pSon )
	{	pNextSon = pSon->m_pNextBrotherClassNode ;
		delete pSon;
		pSon = pNextSon;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup	BaseContextDocGroup_ObjectEditionMethods
		\brief	Declare all objects as if each were used with BaseObject::Close().
				This can be used to force a re-creation of the whole context.
	*/
void	ClassNode::CloseAllObjects( )
{
	// for each objects:
	register BaseObject *pObject;
	register PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		pObject = (BaseObject *)pCell->GetManagedObject();
		pObject->Close(true);
		pCell = pCell->GetNextBrotherCell();
	}
	// for subclasses:
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{	
		pNextSon->CloseAllObjects();
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}
	
}
#endif
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
			It serializes first the class name, the number of object instancied, 
			then all object instancied, then
			all subclasses recursively.
*/
const unsigned char * ClassNode::Serialize_In( const unsigned char * _pDescriptionChunk )
{	
	NamedObject oObject;
	unsigned int chunksize;
	register const unsigned char *pNextChunk = _pDescriptionChunk ;

	//super: serialize our object members from ClassNode, it re-creates
	//  the class name:
	_pDescriptionChunk = NamedObject::Serialize_In(_pDescriptionChunk);
	// this pointer indicates the end of this chunk:
	chunksize =  GetChunkLength();
	pNextChunk += chunksize ;
#ifdef _ENGINE_EDITABLE_
	// make a check on the chunk name read. It must be our class code:
	if( PackString::Compare(mSerStr_ObjectName.Get(),m_ClassDescription->m_ClassName ) !=0)
	{
		m_SerializationState = eSerError_BadChunkHead;
		return pNextChunk;
	}
#endif
	// serialize the object list:
	//test: printf("ClassNode::Serialize_In:%s\n",GetName());
	_pDescriptionChunk = m_BaseObjectList.Serialize_In(_pDescriptionChunk);

	// if editable mode, count current number of objects:
#ifdef _ENGINE_EDITABLE_
	m_NumberOfObjectInstancied=0;
#endif
	// in both mode, set the context for the new objects.
	register PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while(pCell)
	{
		// set the BaseContext:
		register BaseObject *pOb = (BaseObject *)pCell->GetManagedObject();
		if(pOb) pOb->SetBaseContext(m_pBaseContext);
#ifdef _ENGINE_EDITABLE_
		m_NumberOfObjectInstancied++;
		// notify object creation:
		m_pBaseContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectNew,pOb,0L);
#endif
		pCell = pCell->GetNextBrotherCell();
	}

	// check all subclass Nodes serialized, and make them match with our database ClassNodes.
	// note: if the corresponding class were not registered, we can't load its objects !
	// but, we got to check for all subclass if we can, in order to load the more possible.
	while( _pDescriptionChunk < pNextChunk )
	{
		// next subclass test:
		oObject.Serialize_In(_pDescriptionChunk);
		// does this classNode exist ?
		register const char * className =  oObject.GetName() ;
	
		register ClassNode *pNextSon = m_pFirstSonClassNode ;
		while( pNextSon )
		{	
			// is it the right class in the context ?
			/* note: in non-editable mode,(serialize_in only) classnone GetName() doesn't have the class ID.
				old: if( PackString::Compare( pNextSon->GetName() ,  className ) == 0 ) */
			if( PackString::Compare( pNextSon->m_ClassDescription->m_ClassName ,  className ) == 0 )
			{
//test printf("ClassNode::->Sub\n");
				pNextSon->Serialize_In(_pDescriptionChunk);
//test printf("ClassNode::<-up\n");
				break;
			}
			pNextSon = pNextSon->m_pNextBrotherClassNode ;
		}
		// jump over the whole class chunk, read or not.
		_pDescriptionChunk += oObject.GetChunkLength() ;

	}
#ifdef _ENGINE_EDITABLE_
	m_SerializationState = eSerError_OK;
#endif
	// continue serialization:
	return(_pDescriptionChunk);
}

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.
*/
unsigned int ClassNode:: GetSerializedDescriptionSize(void)
{
	// get length for ClassNode members:
	register unsigned int length = NamedObject::GetSerializedDescriptionSize();
	// get the object list length:
	length += m_BaseObjectList.GetSerializedDescriptionSize();

	// serialize recursively subclassnodes if needed:
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{	
		if( pNextSon->IsUseful() ) 
			length += pNextSon->GetSerializedDescriptionSize();
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}
	// got to reset the chunk length member:
	//---------- we got to check that mSerUlong_ChunkLength don't grow with the right size !
	unsigned int previousLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() ;
	mSerUlong_ChunkLength.Set( length );
	previousLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() - previousLength ;
	if( previousLength > 0 )
	{
		length += previousLength;
		//---------- we got to check that twice:
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
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * ClassNode::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	//super: serialize ClassNode members:
	// including objects.
	_pDescriptionChunkToFill = NamedObject::Serialize_Out(_pDescriptionChunkToFill);
	// Serialize_Out the object list length:
	_pDescriptionChunkToFill = m_BaseObjectList.Serialize_Out(_pDescriptionChunkToFill);

	// serialise recursively subclassnodes:
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{	
		if( pNextSon->IsUseful() ) 
			_pDescriptionChunkToFill = pNextSon->Serialize_Out(_pDescriptionChunkToFill);
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}

	return(_pDescriptionChunkToFill);
}
#endif

/*!
	\brief	Find an Object instancied from that class or another subclass.
	\param _ObjectName the name of the object to get.
	\return the object or 0L if not found.
*/
BaseObject *ClassNode::GetBaseObject( const char *  _ObjectName)
{
	register BaseObject *pNextObject=0L;
	register PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while(pCell)
	{
		pNextObject = (BaseObject *)pCell->GetManagedObject();
		if( PackString::Compare( pNextObject->GetName(), _ObjectName ) == 0  ) return(pNextObject);
		pCell = pCell->GetNextBrotherCell();
	}
	// recurse in class tree:
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{	
		pNextObject = pNextSon->GetBaseObject(_ObjectName);
		if( pNextObject ) return(pNextObject);
		pNextSon	= pNextSon->m_pNextBrotherClassNode ;
	}
	//failed:
	return(0L);
}

/*!
	\brief	Find a Son ClassNode by its static description. It looks recursively in all branches of the context.
	\param _classDescription the descripter.
	\return the classnode or 0L if not found.
*/
ClassNode *ClassNode::GetSubClassNodeByDescription( const BaseObject::ClassDescription * _classDescription)
{
	// is it this classNode ?
	if( _classDescription ==  m_ClassDescription ) return(this);

	// recursive search of the class in all subnodes.
	register ClassNode *pSon = m_pFirstSonClassNode ;
	while( pSon )
	{	
		// search if sublevels are....
		register ClassNode *pclass = pSon->GetSubClassNodeByDescription( _classDescription );
		if( pclass ) return( pclass );
		//next brother...
		pSon = pSon->m_pNextBrotherClassNode ;
	}
	return(0L); //failed.
}
/*!
	\brief	Register a class derived from BaseObject to the context, down the class tree. 
		Used by a new context to register the classes, before any serialization or context edition. 
	\param _classDescription BaseObject ( and inherited class ) 's class descriptor.
*/
void	ClassNode::RegisterClassList( const BaseObject::ClassDescription * const * _pclassDescriptionList )
{	// search the superclass.
	register ClassNode *pPossibleDouble;
	register ClassNode *pSuperClass;

	while( *_pclassDescriptionList )
	{
		pPossibleDouble = GetSubClassNodeByDescription( *_pclassDescriptionList );
		pSuperClass		= GetSubClassNodeByDescription( (*_pclassDescriptionList)->m_SuperClassDescription );
		// if superclass doesnt exist, or class already registered, can't add class:
		if( (!pSuperClass) || (pPossibleDouble) ){ _pclassDescriptionList++; continue; }
#ifdef _ENGINE_EDITABLE_
		// If a new class try to be linked with an existing serialization identifier,but are
		// not both same class, warning !! We must check all brother class:
		register ClassNode *pSon = pSuperClass->m_pFirstSonClassNode ;
		while( pSon )
		{	
			if(PackString::Compare( pSon->m_ClassDescription->m_ClassName,
				(*_pclassDescriptionList)->m_ClassName)==0L)
			{
				// this descriptor can't be registered !
				m_pLastClassDescWithFaultySerializer = *_pclassDescriptionList ;
			}									
			//next brother...
			pSon = pSon->m_pNextBrotherClassNode ;
		}
#endif
			// if superclass exists, create a new node.
			register ClassNode *pNewClassNode = new ClassNode( **_pclassDescriptionList );
			// link the new leaf.
			pNewClassNode->m_pNextBrotherClassNode = pSuperClass->m_pFirstSonClassNode ;
			pSuperClass->m_pFirstSonClassNode = pNewClassNode ;
			// inform the ClassNode which BaseContext it's in:
			pNewClassNode->m_pBaseContext = m_pBaseContext;

		_pclassDescriptionList++;
	}
}

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Factory Method to create a new Object in the context. All object members
			should be inited to default, then use BaseObject's Set methods in editable mode.
			This method is used in editable mode, but in serialize_in only mode, it does
			not compute a new name.
	\param _ObjectName. the objectName then used to reference it. computed if NULL.
	\return The Object Created. 0L if class unknown or anywhat.
*/
BaseObject *ClassNode::NewObject( const char *  _ObjectName )
{
	PackStruct::Cell *pCell= m_BaseObjectList.AddElement();
	if( !pCell ) return(0L);
	BaseObject *pnewObj = (BaseObject *)pCell->GetManagedObject();
	if( !pnewObj ) return(0L);

//now all method is not compiled when noneditable: #ifdef _ENGINE_EDITABLE_
	// compute new name if needed:
	PackString	oPackString;
	const char *pClassName = m_ClassDescription->m_DefaultObjectName; //old: GetName() ; 
	unsigned int objectnumber = m_NumberOfObjectInstancied ;
	if( _ObjectName == 0L )
	{	// if no name, compute new object name
		oPackString.Set( pClassName );
		oPackString.AddInt( objectnumber );
	} else
	{
		oPackString.Set(_ObjectName);
		objectnumber=0;
	}
	// assure the object name is unique in the whole context.
	while( m_pBaseContext->GetBaseObject( oPackString.Get() ) )
	{
		objectnumber++;
		if( _ObjectName == 0L )
			// if no name, compute new object name
			oPackString.Set( pClassName );
		else
			oPackString.Set(_ObjectName);

		oPackString.AddInt( objectnumber );
	}
	// give the object name:
	pnewObj->SetName( oPackString.Get() );
	// set the BaseContext:
	pnewObj->SetBaseContext( m_pBaseContext );
	// count number of object managed 
	m_NumberOfObjectInstancied++;
	// notify object creation:
	m_pBaseContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectNew,pnewObj,0L);
	
//now all method ... #endif

	return( pnewObj );
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything for all BaseObject in the context. Still, the object exist and can be rebuild the same using CreateAllBaseObjects()
			note: is it so useful ? who need to do that ? an equivalent is
			automatic at destruction.
*/
/*
void ClassNode::CloseAllBaseObjects(void)
{
	// deletion of all the objects instancied from this class.
	register BaseObject *pNextObject = m_pFirstBaseObject;
	while( pNextObject )
	{
		pNextObject->Close(); // will occur an object create(), or not, if already done.
								// note that a create will create dependant objects recursively.
		pNextObject = pNextObject->GetNextBaseObjectInClassNodeList();
	}
	// recursive creation of all subclass's object nodes:
	register ClassNode *pSon = m_pFirstSonClassNode ;
	while( pSon )
	{
		pSon->CloseAllBaseObjects();
		pSon = pSon->m_pNextBrotherClassNode ;
	}

}
*/
#endif

#ifdef _ENGINE_EDITABLE_
/*!

*/
void	ClassNode::DestroyManagedObject( BaseObject * _objectToDelete )
{
	// search where the object is in the managed object list, then cut the element.
	//register BaseObject *pObjectBefore = 0L;
	register PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		if( pCell->GetManagedObject() == _objectToDelete )
		{
			// close all other object's dependance to it before the deletion.
			_objectToDelete->CloseReferences();
			// remove all references of the object from the context reference list:
			// OLD: NOW IT IS DONE IN PACKREFERENCEOBJECT DELETION
			// m_pBaseContext->UnLinkObjectReferences( _objectToDelete );
			// detach object from the context, so that member destruction does not throw useless events
			//NO! packreference link need a true object. _objectToDelete->SetBaseContext(0L);
			// notify object destruction, with a non sense pointer:
			m_pBaseContext->UpdateAllGUI( BaseContext::eVedaUpdate_ObjectDestroyed,_objectToDelete,0L);
			// delete from the classnode list: (this will then use operator delete):
			m_BaseObjectList.DeleteElement(_objectToDelete);
			// count down number of objects:
			m_NumberOfObjectInstancied--;

			// done
			return;
		}	
		pCell = pCell->GetNextBrotherCell();
	}

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	reset a new unique index for PackObjectReference serialization. It does recursively in all branches of the context.
		This reference index is only valid during serialization. 1 is the minimum
	\param _FirstnumberToSet the first number for the first oject managed by this ClassNode.
		\param  _increment the number that is added to get the next number.

*/
unsigned int ClassNode::SetObjectsUniqueReferenceIndex( unsigned int _FirstnumberToSet,int _increment )
{
	// set unique numbers for instancied objects:
	register PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		((BaseObject *)pCell->GetManagedObject())->SetUniqueReference( _FirstnumberToSet );
		pCell = pCell->GetNextBrotherCell();
		_FirstnumberToSet+=_increment;
	}
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{	
		_FirstnumberToSet = pNextSon->SetObjectsUniqueReferenceIndex( _FirstnumberToSet, _increment );
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}
	return(_FirstnumberToSet);
}
#endif
/*!
	\brief	Find an Object instancied from that class or another subclass by its serialized index. Should only be used by BaseContext::Serialize_In.
	\param _ObjectName the name of the object to get.
	\return the object or 0L if not found.
*/
BaseObject *ClassNode::GetBaseObjectBySerializedIndex( unsigned int _index)
{
	// search object in the list of object of that class:
	register BaseObject *pObject;
	register PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		pObject = (BaseObject *)pCell->GetManagedObject();
		if( pObject->GetUniqueReference() == _index ) return( pObject );
		pCell = pCell->GetNextBrotherCell();
	}

	// if not found, search in sub classes:
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{	
		pObject = pNextSon->GetBaseObjectBySerializedIndex( _index );
		if( pObject ) return( pObject );
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}
	// failed:
	return(0L);
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Destroy all managed object. Used to clean up the whole context.
*/
void ClassNode::DestroyAllManagedObjects()
{
	// deletion of all the objects instancied from this class.
	// note: for object-to-object dependance safety, we had to use DestroyManagedObject()
	while( m_BaseObjectList.GetFirstCell() )
	{
		// we can cast to "BaseObject *" as this list can only manage BaseObjects...
		DestroyManagedObject(  (BaseObject *) (m_BaseObjectList.GetFirstCell()->GetManagedObject() ) );
	}
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Destroy all managed object, and all object of all subclasses recursively.
*/
void ClassNode::ProtectedDestroyAllObjects()
{
	DestroyAllManagedObjects();
	// recursive deletion of all subclass objects.
	register ClassNode *pSon = m_pFirstSonClassNode ;
	while( pSon )
	{	
		pSon->ProtectedDestroyAllObjects();
		pSon = pSon->m_pNextBrotherClassNode ;
	}

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Remove all objects that are not refered directly or indirectly by the object passed.
	\param	_pObjectWhichAllOtherShouldDependOn Object Which All Other Should Depend On.
*/
void ClassNode::ProtectedDestroyAllIndependantObjects( BaseObject *_pObjectWhichAllOtherShouldDependOn )
{
	// destroy all objects that does not match.
	register BaseObject *pObject;
	register PackStruct::Cell *pNextCell,*pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		pNextCell = pCell->GetNextBrotherCell();
		pObject = (BaseObject *)pCell->GetManagedObject();
		if( !(pObject->isDependantOf( _pObjectWhichAllOtherShouldDependOn ) ) )
			m_pBaseContext->DestroyObject( pObject );

		pCell = pNextCell;
	}

	// check subclasses:
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{	
		pNextSon->ProtectedDestroyAllIndependantObjects( _pObjectWhichAllOtherShouldDependOn );
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Acts like Serialize_In() but keep all previous objects.
			So you can load and mix 2 or more context this way.
	\param	_pDescriptionChunk the binary image of a context.
	\param _pendName specify a name to add at the end of each new object.
*/
const unsigned char * ClassNode::MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_pendName)
{
	NamedObject oObject;
	unsigned int chunksize;
	register const unsigned char *pNextChunk = _pDescriptionChunk ;
	//super: serialize our object members from ClassNode, it re-creates
	//  the class name, and the object list
	_pDescriptionChunk = NamedObject::Serialize_In(_pDescriptionChunk);
	// keep the last object cell, or NULL if there was no previous objects for this class:
	register PackStruct::Cell *pLastCellBeforeMerge = m_BaseObjectList.GetLastCell();
	// serialize the object list WITHOUT DESTROYING PREVIOUS OBJECTS :
	_pDescriptionChunk = m_BaseObjectList.MergeSerialize_In(_pDescriptionChunk);
	// this pointer indicates the end of this chunk:
	chunksize =  GetChunkLength();
	pNextChunk += chunksize ;

	// if editable mode, count current number of objects:
	// m_NumberOfObjectInstancied=0; -> we add this number

	//set the context for the new objects.
	// note OPTIMISATIOn: we only set the context for the brand new objects in the list:
	register PackStruct::Cell *pCell;
	if( pLastCellBeforeMerge ) pCell = pLastCellBeforeMerge->GetNextBrotherCell();
	else  pCell = m_BaseObjectList.GetFirstCell();

	while(pCell)
	{
		// set the BaseContext:
		register BaseObject *pOb = (BaseObject *)pCell->GetManagedObject();
		if(pOb) pOb->SetBaseContext(m_pBaseContext);
		m_NumberOfObjectInstancied++;
		// notify object creation:
		m_pBaseContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectNew,pOb,0L);
		pCell = pCell->GetNextBrotherCell();
	}

	// check all subclass Nodes serialized, and make them match with our database ClassNodes.
	// note: if the corresponding class were not registered, we can't load its objects !
	// but, we got to check for all subclass if we can, in order to load the more possible.
	while( _pDescriptionChunk < pNextChunk )
	{
		// next subclass test:
		oObject.Serialize_In(_pDescriptionChunk);
		// does this classNode exist ?
		register const char * className =  oObject.GetName() ;
		register ClassNode *pNextSon = m_pFirstSonClassNode ;
		while( pNextSon )
		{	
			// is it the right class in the context ?
			if( PackString::Compare( pNextSon->GetName() ,  className ) == 0 )
			{
				pNextSon->MergeSerialize_In(_pDescriptionChunk,_pendName);
				break;
			}
			pNextSon = pNextSon->m_pNextBrotherClassNode ;
		}
		// jump over the whole class chunk, read or not.
		_pDescriptionChunk += oObject.GetChunkLength() ;

	}
	// continue serialization:
	return(_pDescriptionChunk);
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Set all object managed's name to "" (empty string) in order to same memory in the context. It is recursive to other sub classnodes.
*/
void ClassNode::FlushNames()
{
	// for each objects:
	register BaseObject *pObject;
	register PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	while( pCell )
	{
		pObject = (BaseObject *)pCell->GetManagedObject();
		pObject->SetName("");
		pCell = pCell->GetNextBrotherCell();
	}
	// for subclasses:
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{	
		pNextSon->FlushNames();
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	return true if this class has at least one object created, or got an inherited class that has
			at least an object created. false if it is not currently used.
			this method is used by serialize_out() to check if a class should be written.
	\return true or false
*/
bool ClassNode::IsUseful() const 
{
	PackStruct::Cell *pCell = m_BaseObjectList.GetFirstCell();
	if( pCell ) return true; // got at least one object.

	// for subclasses:
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{	
		if( pNextSon->IsUseful() ) return true;
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}

	// really no object managed directly or not:
	return false;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	register management of the same classes to another context.
			(and so create another ClassNodes to it.)
			Used by BaseContext::CloneClassesAndObjects().
*/
void ClassNode::ProtectedAddSameClasses( BaseContext *_pContextToAddManagement )
{
	//TODO
	const BaseObject::ClassDescription *pclassDescriptionList[2]=
	{0L,0L};
	pclassDescriptionList[0] = m_ClassDescription;

	_pContextToAddManagement->RegisterClassList( pclassDescriptionList );

	// for subclasses:
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{	
		pNextSon->ProtectedAddSameClasses(_pContextToAddManagement);
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	tool for BaseContext::ExportAsCPPTableFile() , recursive write of
			a .cpp definition of a class description.
*/
void ClassNode::ExportCPPClassDescription( PackString &_fileTextImage ) const
{
	// if this class doesn't manage objects or other usefull class, do nothing.
	if( ! this->IsUseful() ) return;
	// we are useful, we write the descripor:

	_fileTextImage.AddString("class ");
	_fileTextImage.AddString( m_ClassDescription->m_CPlusPlusClassName );
	_fileTextImage.AddString(" {public: static const BaseObject::ClassDescription m_Description; };\n");
// class EquationList {public: static const BaseObject::ClassDescription m_Description; };
// for subclasses:
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		pNextSon->ExportCPPClassDescription(_fileTextImage);
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	tool for BaseContext::ExportAsCPPTableFile() , recursive write of
			a .cpp definition of a class description.
	\param	_fileTextImage
*/
void ClassNode::ExportCPPClassDescriptionLine( PackString &_fileTextImage ) const
{
	// if this class doesn't manage objects or other usefull class, do nothing.
	if( ! this->IsUseful() ) return;
	// we are useful, we write the descripor:
	//	&classname::m_Description,
	_fileTextImage.AddString("\t&");
	_fileTextImage.AddString( m_ClassDescription->m_CPlusPlusClassName );
	_fileTextImage.AddString("::m_Description,\n");

// for subclasses:
	register ClassNode *pNextSon = m_pFirstSonClassNode ;
	while( pNextSon )
	{
		pNextSon->ExportCPPClassDescriptionLine(_fileTextImage);
		pNextSon = pNextSon->m_pNextBrotherClassNode ;
	}

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	If RegisterClassList() detected something wrong about
			the class list, it is reported here.
			From now on, the only possible error is: 2 class
			have the same serializer ID.
*/
const BaseObject::ClassDescription *ClassNode::CheckClassListError() const
{
	return m_pLastClassDescWithFaultySerializer;
}
#endif
