// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackObjectReference_H
#define COM_M4NKIND_PackObjectReference_H

#include "PackULong.h"
#include "PackString.h"
#include "BaseObject.h"

class PackObjectReference : public PackULong
{

public:

	PackObjectReference();
#ifdef _ENGINE_EDITABLE_

	virtual ~PackObjectReference(void);
#endif

	inline BaseObject *GetObjectPointer() const { return m_pReferedObject;  };
#ifdef _ENGINE_EDITABLE_

	typedef enum {

		eSetRef_SetOK=0,

		eSetRef_BadReferenceInit_ShouldntHappen,

		eSetRef_ObjectFromOtherContext,

		eSetRef_ObjectClassDoesntMatch,

		eSetRef_CantReferObjectThatReferThisOne
	} enum_SetReferenceResult;

	virtual	enum_SetReferenceResult	SetObjectPointer( BaseObject * _Object );
#else

	inline void	SetObjectPointer( BaseObject * _Object ){ m_pReferedObject = _Object ; };
#endif

#ifdef _ENGINE_EDITABLE_

	void	SetReferableBaseClass( const BaseObject::ClassDescription &_classDescription  );
#endif
#ifdef _ENGINE_EDITABLE_

	 const BaseObject::ClassDescription &GetReferableBaseClass();
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackObjectReference"; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char	*ValueToString();
#endif
#ifdef _ENGINE_EDITABLE_

	inline PackObjectReference * GetNextSelfReferenceNode(){ return m_pNextSelfReferenceNode; };
#endif

	inline PackObjectReference	**GetPointerOnNextObjectReferenceInContextList(){ return( &m_pNextObjectReferenceInContextList ); };

	virtual	void	SetObjectThatManagesThis(BaseObject *_pManager);
#ifdef _ENGINE_EDITABLE_

	inline void	UpdateSerializedID(void)
	{
		if( m_pReferedObject ) m_value = m_pReferedObject->GetUniqueReference() ;
		else m_value = 0 ;
	};
#endif

protected:

	BaseObject		*m_pReferedObject;

#ifdef _ENGINE_EDITABLE_

	const BaseObject::ClassDescription *m_ReferableclassDescription;

	PackObjectReference	*m_pNextSelfReferenceNode;

#endif

	PackObjectReference	*m_pNextObjectReferenceInContextList;
#ifdef _ENGINE_EDITABLE_

	void	CloseReference();
#endif
};
#ifdef _ENGINE_EDITABLE_

#define		REGISTER_MEMBER_REFERENCE(_object,_InfoString,_classDescriptor) \
REGISTER_MEMBER(_object,_InfoString);\
_object.SetReferableBaseClass(_classDescriptor);
#else
#define		REGISTER_MEMBER_REFERENCE(_object,_InfoString,_classDescriptor) \
REGISTER_MEMBER(_object,_InfoString);
#endif

#endif
