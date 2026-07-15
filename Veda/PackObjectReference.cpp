/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackObjectReference.h"
#include "ClassNode.h"
#include "BaseContext.h"

PackObjectReference::PackObjectReference(  ) : PackULong()
	,m_pReferedObject(0L)
	,m_pNextObjectReferenceInContextList(0L)
#ifdef _ENGINE_EDITABLE_
	,m_pNextSelfReferenceNode(0L)
	,m_ReferableclassDescription(0L)
#endif
{
}
#ifdef _ENGINE_EDITABLE_
PackObjectReference::~PackObjectReference(void)
{
	// unlink from BaseContext reference list.
	//note: it is done by classnode::destroyManagedObject().
	// renote: no, classnode::destroyManagedObject() only close references TO the managed object.
	// this destructor MUST also close this reference TOWARD another possible exitsing objects.
	CloseReference();
	// unlink from context list:
	if( m_pObjectThatManagesThis && 
		m_pObjectThatManagesThis->GetContext() )
		m_pObjectThatManagesThis->GetContext()->UnLinkObjectReference(this);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Set the name of the object refered. If it exists and has a right class type,
			Get will return the object. Else, Get would return 0L.
			This Method is only compiled in editable mode.
*/
PackObjectReference::enum_SetReferenceResult	
		PackObjectReference::SetObjectPointer( BaseObject * _Object )
{
	// do nothing if same object:
	if( _Object == m_pReferedObject ) return( eSetRef_SetOK );

	// get the father class node of the class we can refer:
	BaseContext *pBaseContext = m_pObjectThatManagesThis->GetContext();
	
	// important note: if Set(NULL), the class match doesn't have to be checked.
	// at context destruction, object are destroyed with their ClassNode.
	// in that case, Set(NULL) are sent to some PackObjectReference, which
	// target ClassNode are already destroyed. So Set(NULL) MUSTNOT check ClassNode.
	if(_Object)
	{
		ClassNode *pFatherClassNode = pBaseContext->GetSubClassNodeByDescription( m_ReferableclassDescription );
		// at context destruction,
		if( !pFatherClassNode ) return( eSetRef_BadReferenceInit_ShouldntHappen ); // let closed.

		// test if the object is from the same context:
		if( _Object->GetContext() != pBaseContext ) return( eSetRef_ObjectFromOtherContext );

		// ...and of a correct type.It has to:
		if( ! (pFatherClassNode->GetSubClassNodeByDescription( &(_Object->GetClassDescription())  ))
		) return( eSetRef_ObjectClassDoesntMatch ); // unvalid object.

		// if 'this' is already dependant (refered by) (directly or indirectly) of the object to refer,
		// 'this' can't refer it. This is a recursive test, preventing the context from infinite loops:
		if( m_pObjectThatManagesThis->isDependantOf(_Object) )
			return( eSetRef_CantReferObjectThatReferThisOne ); 
	} /*else
	{
		note the object passed can be NULL: it closes the reference.
	}*/
	// tests passed !
	BeginChangeNotification();
		// close the last reference if there was one:
		CloseReference();

		// the object can officially be refered !
		m_pReferedObject = _Object ;

		// link this reference to this object's reference back-list
		if(_Object)
		{
			m_pNextSelfReferenceNode = _Object->GetFirstReferenceToThisObject();
			_Object->SetFirstReferenceToThisObject( this );
		}
	// occur update (and undo?)
	EndChangeNotification();
	// OK:
	return( eSetRef_SetOK );
}
#endif

/*!
	\brief	Get the object refered. If it exists and has a right class type,
			Get will return the object. Else, Get would return 0L. You should get
			your objects one time by method, and never stock what you get, because
			it could be destroyed or closed between two method calls.
			If the object refered is destroyed, it will become NULL automatically.
*/
/* old: set to inline.
BaseObject *PackObjectReference::GetObjectPointer() const
{	
	// if not done, force the recursive creation of the object:
	//no more with new BaseObject::Create() method    ... if( m_pReferedObject ) m_pReferedObject->Create();
	// can be null if none linked:
	return(m_pReferedObject);
}
*/
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Set the kind of class of object that we will be able to refer. Is only possible in editable mode.
			In read-only mode, the referable type is implicit.
*/
void	PackObjectReference::SetReferableBaseClass( const BaseObject::ClassDescription &_classDescription )
{
	m_ReferableclassDescription = &_classDescription;
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	protected. if a reference was there, set it to NULL.
*/
void	PackObjectReference::CloseReference()
{
	// do nothing if no reference:
	if( !m_pReferedObject ) return;
	
	// kill cell in the reference list:
	// search ourself in the list: (we must be there.)
	PackObjectReference *pReferenceCell = m_pReferedObject->GetFirstReferenceToThisObject();
	if( pReferenceCell == this )
	{
		// case where this reference was first:
		// unlink this:
		m_pReferedObject->SetFirstReferenceToThisObject( m_pNextSelfReferenceNode );
		
	} else
		while( pReferenceCell )
		{
			if( pReferenceCell->m_pNextSelfReferenceNode == this )
			{
				// unlink this:
				 pReferenceCell->m_pNextSelfReferenceNode =
					m_pNextSelfReferenceNode ;

				break;
			}
			pReferenceCell = pReferenceCell->m_pNextSelfReferenceNode;
		}

	// the reference is empty:
	m_pReferedObject = 0L;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.(1,2,3, or4)
*/
unsigned int PackObjectReference:: GetSerializedDescriptionSize(void)
{
	/* OLD AND DANGEROUS: unique ref is now updated in BaseContext::UpdateReferencesToUniqueIndex()
	if( m_pReferedObject ) m_value = m_pReferedObject->GetUniqueReference() ;
	else m_value = 0 ;	*/
	return( PackULong::GetSerializedDescriptionSize() );
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * PackObjectReference::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	/* OLD AND DANGEROUS: unique ref is now updated in BaseContext::UpdateReferencesToUniqueIndex()
	if( m_pReferedObject ) m_value = m_pReferedObject->GetUniqueReference() ;
	else m_value = 0 ;	*/
	return( PackULong::Serialize_Out( _pDescriptionChunkToFill ) );
}
#endif

/*!
	\brief	The Object this one belong to as a member. Should only be used at init.
	\param _pBaseContext
*/
void	PackObjectReference::SetObjectThatManagesThis(BaseObject *_pManager)
{
	PackULong::SetObjectThatManagesThis(_pManager);
	//link the reference in the context reference list
	if( _pManager )	_pManager->GetContext()->LinkObjectReference( this );
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Get the kind of class of object that we will be able to refer. Is only possible in editable mode.
			In read-only mode, the referable type is implicit.
*/
const BaseObject::ClassDescription &PackObjectReference::GetReferableBaseClass()
{
	return *m_ReferableclassDescription;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	convert the value of this object to an explicit string. The object manages the string privately,
		so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			Note: this is not virtual, but each class can manage m_pValueString or not.
	\return	the value as a const string. 
*/
const char	*PackObjectReference::ValueToString()
{
	if( m_pReferedObject ) 
		return m_pReferedObject->GetName() ;
	else return("-");
}
#endif
