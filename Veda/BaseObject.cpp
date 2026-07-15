/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "BaseObject.h"
#include "PackObjectReference.h"
#include "BaseContext.h"

//! the static class description object, to be reimplemented in each class to describe it.
// it is done differently from other object declaration, because it's the root one.
const BaseObject::ClassDescription BaseObject::m_Description=
{
	0L	// new static method callback for factory.
	,"Ob"				// class ID name.
	,0L					// super class IDname
#ifdef _ENGINE_EDITABLE_
	,"BaseObject"
	,0L
	,0L
	,0L
#endif
};
#ifdef _ENGINE_EDITABLE_
//! some optimsation stuff to not make the executable string table grow too much...
const char *BaseObject::m_pErrorString_Memory="Memory Allocation Error";
const char *BaseObject::m_pErrorString_CantOpenFile="Can't Open File";
#endif
/*!
	\brief	 static function to register to the context at the context inition.
			All inherited class HAVE TO extend it. Don't use it directly and don't use the "new" operator, use
			BaseContext::NewObject()
	\return	a new object, but again: don't use it.
*/

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
	// de-create
	Close();
	// destroy registered tool method:
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
/*!
	\brief	Public Method that assures the object is constructed,
			and assures that all needed objects in the context are build before.
			Close() should close everything opened by Create() for this object.
			if _immediate is true, it will build everything immediately and return 0.
			if false, it will begin to build needed objects, and then return an integer
			which stand for the rest of objects to be built before this object is completly created.
			if Create(false) is thrown again and again, this weight will get down to zero, and zero
			mean the object is created (like with Create() ). It allows to make progressbars.
			Note it runs the protected virtual CreateInternal() method to do it. Extend CreateInternal()
			to make the construction, not create.
			\param if true, it will build everything and return 0. If false, make just a part of the creation and return a construction weight.
			\return 0 when object is completly built, otherwise a weight integer, <0 means creation impossible.
*/
int BaseObject::Create(bool _immediate)
{
	// if it is ok:
	if( m_bIsCreatedAccordingToAllUsedObject ) return 0;
	BaseContext *pContext = GetContext();
	if(!pContext) return -1; // would mean object was created without context: impossible.
	// ---------------- PASS 1 : --------------
	// link objects in a dependance list that trace the dependance hierarchy:
	// it is the algorythm that allows to de-recurse a tree by using a stack.
	// we also use this pass to find the "workweight" still to do.
	mCreationStackCell_pPrevCell = 0L;
	mCreationStackCell_pNextCell = 0L;	// no object to create after this in pass 2.
	BaseObject	*pLastInList = this ;
	BaseObject	*pObjectToBeCreated = this;	// 
	unsigned int buildingTotalWeight = 1;
	while( pObjectToBeCreated )
	{
		// link all objects needed:
		PackObjectReference	*pRef =	pContext->GetFirstObjectReferenceInContextList();
		while( pRef )
		{
			if( pRef->GetObjectThatManagesThis() == pObjectToBeCreated )
			{	// this reference is one of pObjectToBeCreated members !
				BaseObject *pUsedObject = pRef->GetObjectPointer();
				if( pUsedObject && (pUsedObject != pLastInList) ) // don't link twice if>1 reference. 
				{	
					// this object is needed and must be created before !!!	
			//nomore if( !(pUsedObject->m_bIsCreatedAccordingToAllUsedObject) )
			//nomore {	// if not created according to its definition, got to do it:
						// warning: this object could possibly have been already linked up the stack.
						// in such case, we got to unlink it there, before relink it at the "last end" level.
						BaseObject	*pVerifyUp = pLastInList;
						while( pVerifyUp )
						{
							if( pVerifyUp == pUsedObject )
							{
								//unlink:
								pVerifyUp->mCreationStackCell_pPrevCell->mCreationStackCell_pNextCell = 
									pVerifyUp->mCreationStackCell_pNextCell;
								pVerifyUp->mCreationStackCell_pNextCell->mCreationStackCell_pPrevCell =
									pVerifyUp->mCreationStackCell_pPrevCell;
								buildingTotalWeight--; // number of objects still to be created.
								break; // exit while().
							}
							pVerifyUp = pVerifyUp->mCreationStackCell_pPrevCell;
						}
						// link at the end:
						pLastInList->mCreationStackCell_pNextCell = pUsedObject;
						pUsedObject->mCreationStackCell_pPrevCell = pLastInList;
						pUsedObject->mCreationStackCell_pNextCell = 0L; // needed to exit.
						pLastInList = pUsedObject ;
						buildingTotalWeight++; // total number of objects to be created.
			//nomore		}
				}
			}
			// next reference in context ref list:
			pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());	
		}
		// ....
		pObjectToBeCreated = pObjectToBeCreated->mCreationStackCell_pNextCell ;
	}
	// ---------------- PASS 2 : --------------
	// creates objects iteratively, from the stack, the other way round so that Used object are already created.
	// if _immediate==false, only build one object and return the weight left to create the object.
	unsigned int buildingDoneWeight = 0;

	while( pLastInList  )
	{
		buildingDoneWeight++;
		m_CreationRate = 1.0f- ((float)buildingDoneWeight)/((float)buildingTotalWeight);

		// pLastInList is not created according to the definition:
		if( !(pLastInList->m_bIsCreatedAccordingToAllUsedObject) )
		{	
			
			// re-create the object if needed:
			bool lasterrorstate = pLastInList->DidLastCreationFailed();
			if( !(pLastInList->CreateInternal()) ) 
			{
				// test:
				// printf("creation failed at:%s err:%s\n",pLastInList->GetClassDescription().m_CPlusPlusClassName , pLastInList->GetLastCreationErrorString()  );

				//no pLastInList->Close(); // reclose what could have been already open in CreateInternal().
				// in this case, don't use Close() but CloseInternal()
				// cause the bit IsCreated is off already.
				pLastInList->CloseInternal();
				pLastInList->m_LastCreationFailed = true;
				if(!lasterrorstate)
				{
					pContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectClosed,pLastInList,0L); 
				}
				// all other objects * BETWEEN * the main object and the first failing one
				// on the dependency chain are also broken, but because of that first failure.
				// we can continue a while() loop here, because we use return just after.
				BaseObject	*pFirstObjectToFailOnDependencyChain = pLastInList;
				pLastInList = pFirstObjectToFailOnDependencyChain->mCreationStackCell_pPrevCell ;
				while(pLastInList)
				{
					//don't forget the dependencies are like a tree; a failure only break one branch.
					// so we need this test to  sort the "broken line" in the tree.
					if(pFirstObjectToFailOnDependencyChain->isDependantOf(pLastInList))
					{
						// declare this object as failed. (being here IMPLIES pLastInList is already closed.)
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
				return(-2); // -2: one object creation failed.
			}
			m_LastCreationErrorString.Set(""); // no error string, by default.
			pLastInList->m_LastCreationFailed = false;
			// declare object as created:
			// declare updatedaccording to edited definition:
			pLastInList->m_bIsCreatedAccordingToAllUsedObject = true;
			pContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectReCreated ,pLastInList,0L); 
			// quit if non immediate mode:
			if( !_immediate ) return buildingTotalWeight-buildingDoneWeight;
		}	


		pLastInList = pLastInList->mCreationStackCell_pPrevCell ;
	}
	m_CreationRate = 0.0f;
	return 0; //ok, created.
}
#endif
#ifndef _ENGINE_EDITABLE_
// non-editable version:
/*!
	\brief	Public Method that assures the object is constructed,
			and assures that all needed objects in the context are build before.
			Close() should close everything opened by Create() for this object.
			if _immediate is true, it will build everything immediately and return 0.
			if false, it will begin to build needed objects, and then return an integer
			which stand for the rest of objects to be built before this object is completly created.
			if Create(false) is thrown again and again, this weight will get down to zero, and zero
			mean the object is created (like with Create() ). It allows to make progressbars.
			Note it runs the protected virtual CreateInternal() method to do it. Extend CreateInternal()
			to make the construction, not create.
			\param if true, it will build everything and return 0. If false, make just a part of the creation and return a construction weight.
			\return 0 when object is completly built, otherwise a weight integer, <0 means creation impossible.
*/
int BaseObject::Create(bool _immediate)
{
	// if it is ok:
	if( m_bIsCreatedAccordingToAllUsedObject ) return 0;
	BaseContext *pContext = GetContext();
	if(!pContext) return -1; // would mean object was created without context: impossible.
	// ---------------- PASS 1 : --------------
	// link objects in a dependance list that trace the dependance hierarchy:
	// it is the algorythm that allows to de-recurse a tree by using a stack.
	// we also use this pass to find the "workweight" still to do.
	mCreationStackCell_pPrevCell = 0L;
	mCreationStackCell_pNextCell = 0L;	// no object to create after this in pass 2.
	BaseObject	*pLastInList = this ;
	BaseObject	*pObjectToBeCreated = this;	// 
	unsigned int buildingWeight = 1;
	while( pObjectToBeCreated )
	{
		// link all objects needed:
		PackObjectReference	*pRef =	pContext->GetFirstObjectReferenceInContextList();
		while( pRef )
		{
			if( pRef->GetObjectThatManagesThis() == pObjectToBeCreated )
			{	// this reference is one of pObjectToBeCreated members !
				BaseObject *pUsedObject = pRef->GetObjectPointer();
				if( pUsedObject && (pUsedObject != pLastInList) ) // don't link twice if>1 reference. 
				{	// this object is needed and must be created before !!!	
					if( !(pUsedObject->m_bIsCreatedAccordingToAllUsedObject) )
					{	// if not created according to its definition, got to do it:
						// warning: this object could possibly have been already linked up the stack.
						// in such case, we got to unlink it there, before relink it at the "last end" level.
						BaseObject	*pVerifyUp = pLastInList;
						while( pVerifyUp )
						{
							if( pVerifyUp == pUsedObject )
							{
								//unlink:
								pVerifyUp->mCreationStackCell_pPrevCell->mCreationStackCell_pNextCell = 
									pVerifyUp->mCreationStackCell_pNextCell;
								pVerifyUp->mCreationStackCell_pNextCell->mCreationStackCell_pPrevCell =
									pVerifyUp->mCreationStackCell_pPrevCell;
								buildingWeight--; // number of objects still to be created.
								break; // exit while().
							}
							pVerifyUp = pVerifyUp->mCreationStackCell_pPrevCell;
						}
						// link at the end:
						pLastInList->mCreationStackCell_pNextCell = pUsedObject;
						pUsedObject->mCreationStackCell_pPrevCell = pLastInList;
						pUsedObject->mCreationStackCell_pNextCell = 0L; // needed to exit.
						pLastInList = pUsedObject ;
						buildingWeight++; // number of objects still to be created.
					}
				}
			}
			// next reference in context ref list:
			pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());	
		}
		// ....
		pObjectToBeCreated = pObjectToBeCreated->mCreationStackCell_pNextCell ;
	}
	// ---------------- PASS 2 : --------------
	// creates objects iteratively, from the stack, the other way round so that Used object are already created.
	// if _immediate==false, only build one object and return the weight left to create the object.
	while( pLastInList  )
	{
		// create the object:
		if( !(pLastInList->CreateInternal()) ) return(-2); // -2: one object creation failed.
		// declare object as created:
		pLastInList->m_bIsCreatedAccordingToAllUsedObject = true;
		buildingWeight--;		
		if( !_immediate ) return buildingWeight;

		pLastInList = pLastInList->mCreationStackCell_pPrevCell ;
	}
	return 0; //ok, created.
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void BaseObject::Close(bool _AndCloseAllDependantObjects)
{
	if( !m_bIsCreatedAccordingToAllUsedObject ) return; // already done.
	// throw decreation:
	CloseInternal();
	// declare closed:
	m_bIsCreatedAccordingToAllUsedObject = false;
	m_LastCreationFailed = false;
	m_CreationRate =1.0f ;

	// scan the list of the references 
	if(_AndCloseAllDependantObjects)
	{
		PackObjectReference *pref = m_pFirstReferenceToThisObject;
		while( pref )
		{	
			BaseObject *pObject = pref->GetObjectThatManagesThis() ;
			// done like that, because "this" is updated or not,
			// but, dependant object are not aware that is sure:
			pObject->Close(true);
			pref = pref->GetNextSelfReferenceNode() ;
		}
	}
	// update GUIs to tell that we had closed the object:
	// note: close message has to be sent AFTER recursive close,
	// because close message can generate reading while the objects
	// creation flags are not coherent.
	if( m_pBaseContext ) m_pBaseContext->NotifyObjectChange( BaseContext::eVedaUpdate_ObjectClosed, this,0L); 
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	
	\return 
*/
const BaseObject::ClassDescription &BaseObject::GetClassDescription( void ) const
{
	return( BaseObject::m_Description );
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	all reference to this object will be closed. used by deletion done by ClassNode.
*/
void BaseObject::CloseReferences()
{
	// closing a reference unlink the cell in the list, so this loop scan and close all references to this object:
	while( m_pFirstReferenceToThisObject )
	{	
		m_pFirstReferenceToThisObject->SetObjectPointer(0L);
	}

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	if this object is refered by another one (which means: needed to be created.)
			directly, or indirectly through other objects references, it will return true,
			otherwise false if no dependance are found at all. This is a recursive test.
	\param  _pObjectSuspectedOfOwnage the object supsected of ownage.
	\return true if dependant.
*/
bool	BaseObject::isDependantOf( BaseObject *_pObjectSuspectedOfOwnage )
{
	// we are dependant of ourself:
	if( _pObjectSuspectedOfOwnage == this ) return(true);
	// scan the list of the references 
	PackObjectReference *pref = m_pFirstReferenceToThisObject;
	while( pref )
	{	
		BaseObject *pObject = pref->GetObjectThatManagesThis() ;
		if( pObject == _pObjectSuspectedOfOwnage ) return( true );
		// note: this should never goes to infinite recursion,
		// because PackObjectReference::Set() prevents it in edition mode.
		if( pObject->isDependantOf(_pObjectSuspectedOfOwnage) ) return( true);
		pref = pref->GetNextSelfReferenceNode() ;
	}
	// we are independant of _pObjectSuspectedOfOwnage: 
	return(false);
}
#endif
/*!
	\brief	The BaseContext the Object belong to. Should only be used by BaseContext.
			It is extended to apply recurively on all members.
	\param _pBaseContext
*/
void	BaseObject::SetBaseContext(BaseContext *_pBaseContext)
{
	m_pBaseContext = _pBaseContext ; 
	//old: ()now use GetMachine()     m_pMachine = _pBaseContext->GetMachine();
	// tell all regsitered members that we manage them:
	register Cell *pCell = GetFirstCell();
	while( pCell )
	{
		// inform this object is managed by this:
		pCell->GetManagedObject()->SetObjectThatManagesThis(this);
		pCell = pCell->GetNextBrotherCell();
	}
}

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. This is done with this method. sub classes can implement it (or not) in
			any way, to explicit current shape of an object.

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
	\param	_pPreviewConfiguration Preview Configuration object. Never 0L. Do not keep a pointer to it.
*/
void BaseObject::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief Create a new object with the exact same member values in the same context.
			references to other objects are kept the same, so that the clone uses
			the same objects as the original.
	\return a new object of the same shape, or 0L if failed.
*/
BaseObject *BaseObject::Clone(const char *_cloneName)
{
	// we must be a part of a context. 
	if( !GetContext() ) return(0L);
	// create object of the same type:
	BaseObject *pnewOb = GetContext()->NewObject( GetClassDescription() );
	if(!pnewOb)	return(0L);

	// Reset unique reference number ID for each objects.
	GetContext()->SetObjectsUniqueReferenceIndex();
	// ... then set all this index as serialized value for references:
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

	// re-translate all reference IDs to object pointers: -for the whole context-
	PackObjectReference *pRef = GetContext()->GetFirstObjectReferenceInContextList(); 
	while( pRef )
	{	
		unsigned int sernum = pRef->Get();
		BaseObject *pObjRefered = GetContext()->GetBaseObjectBySerializedIndex( sernum );
		pRef->SetObjectPointer( pObjRefered );
		// next:
		pRef = *(pRef->GetPointerOnNextObjectReferenceInContextList());
	}

	return(pnewOb);
}
#endif
/*!
	\brief	The Machine of which the context is implemented. It is a gateway for hardware abstraction.
	\return a machine used by the object and context.
*/
VirtualMachine	*BaseObject::GetMachine()
{
	return m_pBaseContext->GetMachine() ;
}

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Tool Method main entry. In editable mode, the object can register a set of methods identified 
			with an ID number by using RegisterToolMethod(), and throw the methods through a switch in ToolMethod(). 
			Then an interface can ask about the available tool methods for an object, and provide action on it.
			This was made to allow special external processing to edit an object, by exemple by importing a file,
			or to allow export of the object in a file format. By default, it does nothing for BaseObject.
			Important: 0 can't be used as an ID. You can only register an ID once, for a given class.
			See RegisterToolMethod()
	\param	_MethodIDToExecute	the tool method ID, greater than 0.
*/
void BaseObject::ExecuteToolMethod( unsigned int _MethodIDToExecute ) 
{
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get next tool method ID, or 0 if no more. Send 0 as _PreviousMethodID to get the first.
*/
unsigned int BaseObject::GetNextToolMethod( unsigned int _PreviousMethodID,unsigned int &_NextMethodflagInfo ,const char *&_pNextShortDisplayName,const char *&_pNextDisplayHelp )
{
	ToolMethodCell *pCell = m_pFirstToolMethodCell;

	while(pCell )
	{
		if(_PreviousMethodID == 0) break; // get first if ID ==0
		int previousid = pCell->m_ToolMethodID ;
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
/*!
	\brief	Register a tool method. Do not use it directly,
		use macro version  BASEOBJECT_REGISTER_TOOLMETHOD() It should be set in the constructors.
*/
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
/*!
	\brief	THIS WAS EXPERIMENTAL !!! get a new ReferenceMembers , or NULL (0L) if such feature
		has no sense with this object. Pay attention, BaseObject and inherited
		can know about a ReferenceMembers object, but ReferenceMembers objects
		shouldn't care about their BaseObject, because CreateNewReferenceMembers()
		can be thrown when their objects itself is not created.
	\return new members by reference, or empty adress.
*/
/*BaseObject::ReferenceMembers	*BaseObject::CreateNewReferenceMembers()
{
	return 0L;
}*/
