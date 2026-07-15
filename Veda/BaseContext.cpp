/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "BaseContext.h"
#include "ClassNode.h"
#include "BaseObject.h"
#include "PackObjectReference.h"
#include "PackResource.h"
#include "PackString.h"

BaseContext::BaseContext()
	// as a BaseContext is also the root ClassNode, it stands for the BaseObject class:
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
	// as we are a  root ClassNode cell, the Root cell of the database is: ourself.
	m_pBaseContext = this;
}
#ifdef _ENGINE_EDITABLE_
BaseContext::~BaseContext(void)
{
	/* note: the lock is now in ~ClassNode()
	// threadsafe lock: (don't change order of the 2 lines !!!)
	m_OwnerThreadIsCurrentlyManagingObjects = true;
	// wait unlock from all threads. 
	while(m_NumberOfThreadsCurrentlyLockingTheContext != 0){if( m_pMachine) m_pMachine->Sleep(20);}
	*/
	// destroy all cells in undo stack if needed:
	FlushUndoStack();
	// destroy update function list
	while( m_GUIUpdateFunctionList.GetFirstCell()  )	
	{
		RemoveGUIUpdateFunction(
			((UpdateFunctionListCell *)(m_GUIUpdateFunctionList.GetFirstCell()->GetManagedObject()))
			->m_func
			);
	}
	// of course, the lock is forever.
	//////// m_OwnerThreadIsCurrentlyManagingObjects = true;

	// note: deletion does: 1. Process destructor 2.Delete Class Member 3.Process upper destructors.
	// that is why deletion of all objects in the context must be done by ProtectedDestroyAllObjects()
	// and not by virtual destructors, that would try to use destroed context members.
	ProtectedDestroyAllObjects();
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Factory Method to create a new Object in the context. All object members
			should be inited to default, then use BaseObject's Set methods in editable mode to modify them.
			You must pass a valid Class descriptor and an _ObjectName, or nothing, in which case
			a name will be computed according to the class. If an object with the same name
			exists, then a new name is computed. get the final name with BaseObject::GetName();
			This method is only compiled in editable mode, in non-editable, Serialize_In() should construct everything.
	\param _classDescription the description struct of a class, like BaseObject::m_Description
	\param _ObjectName. the objectName then used to reference it can be NULL, then a new name is computed.
	\return The Object Created. 0L if class unknown or anywhat.
*/
BaseObject *BaseContext::NewObject( const BaseObject::ClassDescription & _classDescription ,char *  _ObjectName )
{
	// find class 
	ClassNode *pClassNode = GetSubClassNodeByDescription( &_classDescription);
	if( !pClassNode ) return(0L);

	// creation of the object.
	return( pClassNode->NewObject(_ObjectName) ); // can be 0L.

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	In editable mode, all BaseObject should be destroyed by this function.
			This method is only compiled in editable mode.
	\param _objectToDelete The object to delete.
*/
void	BaseContext::DestroyObject( BaseObject * _objectToDelete )
{
	// find the classnode which manage this object in the context:
	ClassNode *pClassNode = GetSubClassNodeByDescription(  &_objectToDelete->GetClassDescription() );
	if( !pClassNode ) return; // deletion impossible

	// threadsafe lock: (don't change order of the 2 lines !!!)
	m_OwnerThreadIsCurrentlyManagingObjects =true;
	m_TESTDEBUG_CONTEXTLOCK_ID = 5;
	// wait unlock from all thread. 
	while(m_NumberOfThreadsCurrentlyLockingTheContext != 0){
		if( m_pMachine) m_pMachine->Sleep(20);
		}

		// WE PULL THE PLUG ON YOU !
		pClassNode->DestroyManagedObject( _objectToDelete );
	
	//! unlock for other threads:
	m_OwnerThreadIsCurrentlyManagingObjects =false;

	
	// destroying an object occurs a undo cache flush, ATM:
	FlushUndoStack();


}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.
*/
unsigned int BaseContext:: GetSerializedDescriptionSize(void)
{
	// before the context serialization, reset unique reference number ID for each objects. 
	SetObjectsUniqueReferenceIndex();
	// ... then set all this index as serialized value for references:
	UpdateAllReferencesSerializationID();

	return( ClassNode::GetSerializedDescriptionSize() );
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * BaseContext::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	// note: should have been done in GetSerializedDescriptionSize() ...
	// redone for security:
	// before the context serialization, reset unique reference number ID for each objects. Then reference will use them:
	SetObjectsUniqueReferenceIndex();
	// ... then set all this index as serialized value for references:
	UpdateAllReferencesSerializationID();
	// classic ClassNode way:
	return( ClassNode::Serialize_Out( _pDescriptionChunkToFill ) );
}
#endif
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
			It serializes first the class name, the number of object instancied, then
			the number of sub-classnodes instancied, then all object instancied, then
			all subclasses recursively.
*/
const unsigned char * BaseContext::Serialize_In( const unsigned char * _pDescriptionChunk)
{
#ifdef _ENGINE_EDITABLE_
	// make a check on the beginning of the file. we should have a 
	// PackULong + PackString, with a PackString contining "Ob".
	// else, it is not a azurveda Context file.
	const unsigned char *pdummyTest = _pDescriptionChunk;
	PackULong	dummyUL; // stand for NamedObject::mSerUlong_ChunkLength
	PackString	dummyString;// stand for NamedObject::mSerStr_ObjectName
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
	// wait unlock from all threads. 
	while(m_NumberOfThreadsCurrentlyLockingTheContext != 0){if( m_pMachine) m_pMachine->Sleep(20);}
#endif

#ifdef _ENGINE_EDITABLE_
	// In editable mode, got to clear the whole former context, before creating a new one.
	FlushUndoStack();
	ProtectedDestroyAllObjects();
#endif
	// serialize all subclassNodes, objects, (and their reference IDs).
	_pDescriptionChunk = ClassNode::Serialize_In( _pDescriptionChunk ); 
	// note: ClassNode,NamedObject and PackStruct serIn were called.

	// re-translate all reference IDs to object pointers:
	PackObjectReference *pRef = m_pFirstObjectReferenceInContextList; 
	while( pRef )
	{	
		BaseObject *pObjRefered = GetBaseObjectBySerializedIndex( pRef->Get() );
		//old pRef->SetObjectPointer( pObjRefered );
		/* InterfacePrefObject::SpecialReference uses SetObjectPointer()
		to check useless cells and autodestroy them, even in this load loop,
		in rare cases cells are saved
		this loop should not create/delete anything. if(pObjRefered) is added for security.
		*/
		if(pObjRefered) pRef->SetObjectPointer( pObjRefered );
		// next:
		pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
	}
#ifdef _ENGINE_EDITABLE_
	m_OwnerThreadIsCurrentlyManagingObjects = false;
#endif
	return(_pDescriptionChunk);
}
/*!
	\brief	link a new PackObjectReference to the context list. Done at member inition.
	\param	_pObjectReferenceToLink the new reference to link. 
*/
void	BaseContext::LinkObjectReference( PackObjectReference *_pObjectReferenceToLink )
{
	//printf("BaseContext::LinkObjectReference\n");
	*(_pObjectReferenceToLink->GetPointerOnNextObjectReferenceInContextList()) =
		m_pFirstObjectReferenceInContextList;
	m_pFirstObjectReferenceInContextList = _pObjectReferenceToLink;

}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	remove a PackObjectReference from the context list, at its deletion.
	\param	_pReferenceToUnlink ...
*/
void BaseContext::UnLinkObjectReference( PackObjectReference *_pReferenceToUnlink )
{
	// scan all references, unlink those which belong to the object.
	PackObjectReference **ppref = &m_pFirstObjectReferenceInContextList;
	while( *ppref )
	{
		if( (*ppref) == _pReferenceToUnlink )
		{
			// unlink:
			*ppref =  *((*ppref)->GetPointerOnNextObjectReferenceInContextList()) ;
			// the unlink act as a "next" too.
		}  
		else
		{
			// next:
			ppref = (*ppref)->GetPointerOnNextObjectReferenceInContextList();
		}
	}
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	test if an Undo() is possible.
	\return true if it is.
*/
bool BaseContext::CanUndo()
{
	return(m_pCurrentUndoCell != 0L);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Undo the last modification done on any object of the base, using a private "action history stack".
	\return true if the undo was "undone", false if no more undo available.

*/
bool BaseContext::Undo()
{
	// if undo not enabled:
	if(!m_pCurrentUndoCell) return false; 

	// reserialize the undo on the previous state:
	UndoCell *pUndoCell = (UndoCell *)m_pCurrentUndoCell->GetManagedObject();

	// notify "before-change":
	NotifyObjectChange( BaseContext::eVedaUpdate_MemberIsGoingToChange, 
							pUndoCell->GetMemberToChange()->GetObjectThatManagesThis(),
							pUndoCell->GetMemberToChange()  );

		pUndoCell->GetMemberToChange()->Serialize_In(
			pUndoCell->GetPreviousValueChunk()
			);
		//------ then push the current position down the stack:
		m_pCurrentUndoCell = m_pCurrentUndoCell->GetPreviousBrotherCell(); 

	// notify "after-change":
	NotifyObjectChange( BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo, 
							pUndoCell->GetMemberToChange()->GetObjectThatManagesThis(),
							pUndoCell->GetMemberToChange()  );
	
	return true;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	test if an Redo() is possible.
	\return true if it is.
*/
bool BaseContext::CanRedo()
{	
	if(!m_pCurrentUndoCell) return( m_UndoStack.GetFirstCell() != 0L );
	else return(   m_pCurrentUndoCell->GetNextBrotherCell() != 0L  );
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Redo the last undo on any object of the base, using a private "action history stack".		
	\return true if the last undo was "redone", false if no more redo available.

*/
bool BaseContext::Redo()
{
	//---- first get the current position up:
	if(!m_pCurrentUndoCell) 
	{	// case where the current undo is at the beginning:
		m_pCurrentUndoCell = m_UndoStack.GetFirstCell();
		if( !m_pCurrentUndoCell ) return(false);
	}
	else
	{	// case if we are last redo possible:
		if(  m_pCurrentUndoCell->GetNextBrotherCell() == 0L ) return(false);
		m_pCurrentUndoCell = m_pCurrentUndoCell->GetNextBrotherCell() ;
	}
	
	// reserialize the redo on the state:	
	UndoCell *pRedoCell =(UndoCell *)m_pCurrentUndoCell->GetManagedObject();
	
	// notify "before-change":
	NotifyObjectChange( BaseContext::eVedaUpdate_MemberIsGoingToChange, 
							pRedoCell->GetMemberToChange()->GetObjectThatManagesThis(),
							pRedoCell->GetMemberToChange()  );
	
	
		pRedoCell->GetMemberToChange()->Serialize_In(
			pRedoCell->GetAfterValueChunk()
			);

	// notify "after-change":
	NotifyObjectChange( BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo, 
							pRedoCell->GetMemberToChange()->GetObjectThatManagesThis(),
							pRedoCell->GetMemberToChange()  );

	return true;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Really destroy all object from the context. It should result a clean
		context like it it was just constructed empty.
*/
void	BaseContext::DestroyAllObjects( )
{	
	//we just bring this ClassNode protected method to a BaseContext public one:
	ProtectedDestroyAllObjects();
}
#endif


#ifdef _ENGINE_EDITABLE_
/*!
	\brief Used by BaseObjects "Set()" member methods to notify what happened when one of 
			the member was edited, to change its value. It is used to manage the Undo/Redo capability.
			You should not use this method unless you create a PackSerializable base type and code it's Set().
	\param 
	\param
	\param
	\param
	\param
	\param
*/
void BaseContext::NotifyObjectChange( eVedaUpdate _UpDateType,BaseObject *_pObjectEdited, BaseType *_ObjectMember,
							const unsigned char *_PreviousShapeChunk,
							unsigned int		 _PreviousShapeChunkLength,
							const unsigned char *_NewShapeChunk,
							unsigned int		 _NewShapeChunkLength
							)
{
	
	//if it is a member change with informations, update undo stack:
	if( m_MaximumNumberOfUndoableEvents >0 &&
		_UpDateType == eVedaUpdate_MemberChanged &&
		_ObjectMember->GetClassID() && 
			(PackString::Compare(_ObjectMember->GetClassID(),"PackList") != 0) && // no undo management for these class.
			(PackString::Compare(	_ObjectMember->GetClassID(),"PackTreeCell") != 0) &&
		_PreviousShapeChunk &&
		_NewShapeChunk
		)
	{
		// flush all cells on the "redo" side of the current position:
		if( m_pCurrentUndoCell ) // case where current cell is not first cell
		{
			while( m_pCurrentUndoCell->GetNextBrotherCell() )
			{
				m_UndoStack.DeleteElement( m_pCurrentUndoCell->GetNextBrotherCell()->GetManagedObject() );
				m_CurrentNumberOfUndoableEvents--;
			}
		} else
		{ //m_pCurrentUndoCell == 0L means first cell is selected, but there may be cells in the list:
			FlushUndoStack();
		}
		// update m_pCurrentUndoCell for the next func !!!
		// add new undo/redo cell on top:
		PackStruct::Cell *plistcell = m_UndoStack.AddElement();
		m_CurrentNumberOfUndoableEvents++;

		UndoCell *pUndoCell = (UndoCell *)plistcell->GetManagedObject();
		pUndoCell->Init(
				eVedaUpdate_MemberChanged, _ObjectMember,
				_PreviousShapeChunk,
				_PreviousShapeChunkLength,
				_NewShapeChunk,
				_NewShapeChunkLength );
		// the new current cell is the new one:
		m_pCurrentUndoCell = plistcell ;
		//	void Init(unsigned int _editionType,BaseType *_pObjectMemberEdited,unsigned char *_PreviousValueChunk,unsigned int _PreviousValueChunkLength,unsigned char *_AfterValueChunk,unsigned int _AfterValueChunkLength);

		// if maximum number of cell reached, kill cell on bottom:
		if( m_CurrentNumberOfUndoableEvents > m_MaximumNumberOfUndoableEvents )
		{
			if( m_UndoStack.GetFirstCell() )
				m_UndoStack.DeleteElement(
					m_UndoStack.GetFirstCell()->GetManagedObject() );
		
			m_CurrentNumberOfUndoableEvents--;
		}
	}
	// if a member of an object is going to change, context will be incoherent a short time
	// if get by another thread (like: member changed, but still isCreated() true.)
	// so we lock the context, the time we change values and set correct create flags to everyones.
	if( _UpDateType == eVedaUpdate_MemberIsGoingToChange )
	{
		// threadsafe lock: (don't change order of the 2 lines !!!)
		m_OwnerThreadIsCurrentlyManagingObjects = true;
		m_TESTDEBUG_CONTEXTLOCK_ID = 4;
		// wait unlock from all threads. 
		while(m_NumberOfThreadsCurrentlyLockingTheContext != 0){if( m_pMachine) m_pMachine->Sleep(20);}
		
	}
	// if member changed, notify dependant object that they should update:
	if( _UpDateType == eVedaUpdate_MemberChanged ||
		_UpDateType == eVedaUpdate_MemberChangedBecauseOfUndo)
	{
		// this will set correct "isCreated()" flags to every interested objects:
		_pObjectEdited->Close(true); // true means: also close all dependant objects.
		//! then unlock for other threads:
		m_OwnerThreadIsCurrentlyManagingObjects =false;
	}
	// if member list destroyed, flush undo stack:
	if( _UpDateType == eVedaUpdate_MemberDestroyed ) FlushUndoStack();

	// update interfaces: (can be member change, new objects,...)
	UpdateAllGUI( _UpDateType, _pObjectEdited , _ObjectMember );

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Some editions event (object destruction) on the context need to completely 
	erase the Undo stack, because the event stack coherence is broken.
*/
void BaseContext::FlushUndoStack()
{
	while( m_UndoStack.GetFirstCell() )
		m_UndoStack.DeleteElement((unsigned int)0);
	m_CurrentNumberOfUndoableEvents = 0;
	m_pCurrentUndoCell = 0L;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	The undo stack history can be less or more large. Default max is 0: no undo management.
	\param _maxnumber
*/
void BaseContext::SetMaximumNumberOfUndoableEvents( unsigned int _maxNumber )
{
	//only at init.
	if( m_MaximumNumberOfUndoableEvents != 0 ) return;
	m_MaximumNumberOfUndoableEvents = _maxNumber;

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	You can specify one or more abstract GUI Views to update, by
	registering a function or a static method, that will be executed
	after each set() done on a member of a managed object, when editing, 
	and even for a undo/redoes. This function will receive a const description of what 
	was changed: the context concerned, the object concerned, and the object member concerned. 
	\param _func You must pass a function pointer on a static method pointer.
	\param _GUIObjectToUpdate a user object of your choice that will be returned for this function, or NULL.
*/
void BaseContext::AddGUIUpdateFunction(	void (*_func)(void *_GUIObjectToUpdateReturned,eVedaUpdate _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated) ,
								void *_GUIObjectToUpdate
								)
{
	// try to add a cell to the update list:
	if( !_func ) return;
	PackStruct::Cell *pNewCEll = m_GUIUpdateFunctionList.AddElement();
	if(!pNewCEll) return;
	UpdateFunctionListCell *pUpdateCell = (UpdateFunctionListCell *)pNewCEll->GetManagedObject();

	pUpdateCell->m_func			= _func;
	pUpdateCell->m_UserObject	=  _GUIObjectToUpdate ; // can be NULL.

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Remove an update function set by AddGUIUpdateFunction()
	\param _func the func pointer to remove.
	\return true if removed, false if unknown.
*/
bool BaseContext::RemoveGUIUpdateFunction(	void (*_func)(void *_GUIObjectToUpdateReturned,eVedaUpdate _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated) )
{
	// search the cell which manage this function in the update list:
	PackStruct::Cell *pCell = m_GUIUpdateFunctionList.GetFirstCell();
	while( pCell )
	{
		if( 
			((UpdateFunctionListCell *)pCell->GetManagedObject())->m_func
			== 	_func	
			)
		{
			// destroy the update func.
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
	// copy chunks:
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
	// copy chunks:
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
/*!
	\brief	send an update function to all registered GUI.
	\param the update case.
	\param _ObjectUpdated the object updated in this context.
	\param _MemberUpdated the member of the object updated.
*/
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
/*!
	\brief	Remove all objects that are not refered directly or indirectly by the object passed.
	\param	_pObjectWhichAllOtherShouldDependOn Object Which All Other Should Depend On.
*/
void BaseContext::DestroyAllIndependantObjects( BaseObject *_pObjectWhichAllOtherShouldDependOn )
{
	// quit if object is not from this context:
	if( _pObjectWhichAllOtherShouldDependOn->GetContext() != this ) return;
	// scan all objects
	ProtectedDestroyAllIndependantObjects(_pObjectWhichAllOtherShouldDependOn);
	//TODO
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Acts like Serialize_In() but keep all previous objects.
			So you can load and mix 2 or more context this way.
	\param	_pDescriptionChunk the binary image of a context.
	\param  _endName specify a name to add at the end of each new object.
*/
const unsigned char * BaseContext::MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_endName)
{
	// in Serialize_In() wer do stuff like:
	//FlushUndoStack();
	//ProtectedDestroyAllObjects();
	
	// set 0 to all reference number of the current context objects:
	SetObjectsUniqueReferenceIndex(0,0);
	// and 0 also to the PackObjectReference's corresponding magic num.
	// their pointers stay correct.
	register PackObjectReference *pRef = m_pFirstObjectReferenceInContextList; 
	while( pRef )
	{	 
		pRef->Set(0UL); // ref num and don't change pointer.
		pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
	}

	// serialize all subclassNodes, objects, (and their reference IDs).
	_pDescriptionChunk = ClassNode::MergeSerialize_In( _pDescriptionChunk , _endName ); 
	// re-translate all reference IDs to object pointers:
	pRef = m_pFirstObjectReferenceInContextList; 
	while( pRef )
	{	 
		unsigned int refnum = pRef->Get();
		// all 0's are kept untouch: old context objects or NULL references.
		if(refnum != 0)
		{
			BaseObject *pObjRefered = GetBaseObjectBySerializedIndex(refnum );
			if(pObjRefered) pRef->SetObjectPointer( pObjRefered );
		} 
		// next:
		pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
	}


	return(_pDescriptionChunk);

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Even in editable mode, only one thread owns a context and can
			edit, modify, use and destroy it. Other threads can just use 
			its objects and methods through a AddOneLockForThreadUse()
			RemoveOneLockForThreadUse() pair. This is to avoid using an object
			which could be modified or destroyed by the owner thread.
			(it was made for sound management.)
			AddOneLockForThreadUse() Should only be used by other threads, 
			and will wait if an object management is currently done.
*/
bool BaseContext::AddOneLockForThreadUse()
{
	// do not exange a single line !!! interupt could break between !
	m_NumberOfThreadsCurrentlyLockingTheContext++;
	// this way we are 100% safe:
	if(m_OwnerThreadIsCurrentlyManagingObjects)	
	{
		m_NumberOfThreadsCurrentlyLockingTheContext--;
		return false;
	}
	return true;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Even in editable mode, only one thread owns a context and can
			edit, modify, use and destroy it. Other threads can just use 
			its objects and methods through a AddOneLockForThreadUse()
			RemoveOneLockForThreadUse() pair. This is to avoid using an object
			which could be modified or destroyed by the owner thread.
			(it was made for sound management.)
*/
void BaseContext::RemoveOneLockForThreadUse()
{
	m_NumberOfThreadsCurrentlyLockingTheContext--;

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	give to _ContextToModify the same shape as this.
			It means, the same class are first registered, and THEN 
			the objects polulation are cloned into _ContextToModify.
	\param	_pContextToModify to object modified to shape this.
*/
void BaseContext::CloneClassesAndObjects(BaseContext *_pContextToModify)
{
	if(!_pContextToModify) return;
	// if something was there, destroy:
	_pContextToModify->DestroyAllObjects();
	// assume that this context classes are also registered:
	ProtectedAddSameClasses(_pContextToModify);
	// serialize the very same shape.
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
/*!
	\brief	Export a C file with the context serialization standing as a unsigned char table.
			This could be used for automatic executable generation by some implementations.
	\param	_filePath the path of the file to write. '.c' is added.
	\param	_pGobalTableName the name of the global variable in the .c file
	\param	_optimize if true, names are flushed and preferences destroyed.
*/
void BaseContext::ExportAsCPPTableFile(const char *_pfileToExport,const char *_pGobalTableName,bool _optimize)
{
	BaseContext	*ObjToSerialize=this; // default: unoptimized object.
	BaseContext	dummyClone;
	// first, we got to optimize.
	if(_optimize)
	{
		CloneClassesAndObjects( &dummyClone );
		dummyClone.FlushNames();
		ObjToSerialize = &dummyClone;
	}
	// write .cpp file

	// write table of list 

	unsigned int serializedSize = ObjToSerialize->GetSerializedDescriptionSize();
	if(serializedSize == 0) return; // shouldnt happen.
	unsigned char	*pchunk = new unsigned char[serializedSize];
	if( pchunk )
	{
		unsigned char	*pChunkVector=pchunk;
		// write the chunk form.
		ObjToSerialize->Serialize_Out(pchunk);

		// write CPP file
	/*
class BaseObject { public: class ClassDescription; };

class VirtualEquation { public: static const BaseObject::ClassDescription m_Description; };

class EquationList {public: static const BaseObject::ClassDescription m_Description; };
class EquationGamma {public:  static const BaseObject::ClassDescription m_Description; };
class VirtualEquationSpline {public:  static const BaseObject::ClassDescription m_Description; };
class EquationSpline_X {public:  static const BaseObject::ClassDescription m_Description; };
class EquationSpline_XY {public:  static const BaseObject::ClassDescription m_Description; };
class EquationSpline_XYZ {public:  static const BaseObject::ClassDescription m_Description; };	
	*/

		//PackResource	cfileRessource;
		PackString	fileText;
		PackResource	fileres;
		fileres.SetFile(_pfileToExport);
		fileres.SetLinkMode( false); // it's a file to write.

		//PackString	lineText;
		// write extern definition of needed classes.
		fileText.Set("class BaseObject { public: class ClassDescription; };\n");
		// cycle sons classnodes because BaseObject itself, managed by BaseContext, must not be listed.
		register const ClassNode *pNextSon = m_pFirstSonClassNode ;
		while( pNextSon )
		{
			pNextSon->ExportCPPClassDescription(fileText);
			pNextSon = pNextSon->GetNextBrotherClassNode() ;
		}
		// then write the table that will link the classes
		fileText.AddString("const BaseObject::ClassDescription * ContextClassTable[]=\n{\n");
		pNextSon = m_pFirstSonClassNode ;
		while( pNextSon )
		{
			pNextSon->ExportCPPClassDescriptionLine(fileText);
			pNextSon = pNextSon->GetNextBrotherClassNode() ;
		}
		fileText.AddString("0L\n};\n");
	
		// write context shape data. 
		fileText.AddString("\nunsigned char ");
		if(_pGobalTableName == 0L) _pGobalTableName="ContextDescription";
		fileText.AddString(_pGobalTableName);
		fileText.AddString("[");
		fileText.AddInt(serializedSize);
		fileText.AddString("]=\n{\n");
	
		fileres.SetBinary( (const char *) fileText.Get(), fileText.length() );
		fileText.Set(""); // flush.

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
				//fileText.AddString( lineText.Get() );
				//lineText.
			}
		}
		//fileText.AddString(lineText.Get());
		fileText.AddString("\n};\n");
		fileres.SetBinary( (const char *) fileText.Get(), fileText.length(),true );

		// actually, the binary serialization of PackString acts like a text file.

	//	fileres.SetBinary( (const char *) fileText.Get(), fileText.length() );
		//fileText.WriteFile(_pfileToExport);
		//cfileRessource.

		// delete serialized buffer:
		delete [] pchunk;
	}
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Updates all reference members of the context so that the refered
	object 's serialization ID is took as serialized value.
*/
void BaseContext::UpdateAllReferencesSerializationID()
{
	// re-translate all reference IDs to object pointers: -for the whole context-
	PackObjectReference *pRef = GetFirstObjectReferenceInContextList(); 
	while( pRef )
	{	
		pRef->UpdateSerializedID();
		// next:
		pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
	}	
}
#endif