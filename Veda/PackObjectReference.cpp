// SPDX-License-Identifier: LGPL-2.1-only

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

	CloseReference();

	if( m_pObjectThatManagesThis &&
		m_pObjectThatManagesThis->GetContext() )
		m_pObjectThatManagesThis->GetContext()->UnLinkObjectReference(this);
}
#endif
#ifdef _ENGINE_EDITABLE_

PackObjectReference::enum_SetReferenceResult
		PackObjectReference::SetObjectPointer( BaseObject * _Object )
{

	if( _Object == m_pReferedObject ) return( eSetRef_SetOK );

	BaseContext *pBaseContext = m_pObjectThatManagesThis->GetContext();

	if(_Object)
	{
		ClassNode *pFatherClassNode = pBaseContext->GetSubClassNodeByDescription( m_ReferableclassDescription );

		if( !pFatherClassNode ) return( eSetRef_BadReferenceInit_ShouldntHappen );

		if( _Object->GetContext() != pBaseContext ) return( eSetRef_ObjectFromOtherContext );

		if( ! (pFatherClassNode->GetSubClassNodeByDescription( &(_Object->GetClassDescription())  ))
		) return( eSetRef_ObjectClassDoesntMatch );

		if( m_pObjectThatManagesThis->isDependantOf(_Object) )
			return( eSetRef_CantReferObjectThatReferThisOne );
	}

	BeginChangeNotification();

		CloseReference();

		m_pReferedObject = _Object ;

		if(_Object)
		{
			m_pNextSelfReferenceNode = _Object->GetFirstReferenceToThisObject();
			_Object->SetFirstReferenceToThisObject( this );
		}

	EndChangeNotification();

	return( eSetRef_SetOK );
}
#endif

#ifdef _ENGINE_EDITABLE_

void	PackObjectReference::SetReferableBaseClass( const BaseObject::ClassDescription &_classDescription )
{
	m_ReferableclassDescription = &_classDescription;
}
#endif

#ifdef _ENGINE_EDITABLE_

void	PackObjectReference::CloseReference()
{

	if( !m_pReferedObject ) return;

	PackObjectReference *pReferenceCell = m_pReferedObject->GetFirstReferenceToThisObject();
	if( pReferenceCell == this )
	{

		m_pReferedObject->SetFirstReferenceToThisObject( m_pNextSelfReferenceNode );

	} else
		while( pReferenceCell )
		{
			if( pReferenceCell->m_pNextSelfReferenceNode == this )
			{

				 pReferenceCell->m_pNextSelfReferenceNode =
					m_pNextSelfReferenceNode ;

				break;
			}
			pReferenceCell = pReferenceCell->m_pNextSelfReferenceNode;
		}

	m_pReferedObject = 0L;
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned int PackObjectReference:: GetSerializedDescriptionSize(void)
{

	return( PackULong::GetSerializedDescriptionSize() );
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * PackObjectReference::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{

	return( PackULong::Serialize_Out( _pDescriptionChunkToFill ) );
}
#endif

void	PackObjectReference::SetObjectThatManagesThis(BaseObject *_pManager)
{
	PackULong::SetObjectThatManagesThis(_pManager);

	if( _pManager )	_pManager->GetContext()->LinkObjectReference( this );
}
#ifdef _ENGINE_EDITABLE_

const BaseObject::ClassDescription &PackObjectReference::GetReferableBaseClass()
{
	return *m_ReferableclassDescription;
}
#endif
#ifdef _ENGINE_EDITABLE_

const char	*PackObjectReference::ValueToString()
{
	if( m_pReferedObject )
		return m_pReferedObject->GetName() ;
	else return("-");
}
#endif
