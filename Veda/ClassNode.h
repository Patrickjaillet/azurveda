// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_ClassNode_H
#define COM_M4NKIND_ClassNode_H

#include "NamedObject.h"

#include "PackString.h"
#include "BaseObject.h"
#include "PackList.h"

class BaseContext;

class ClassNode : public NamedObject
{

public:

	ClassNode( BaseObject::ClassDescription const &_classDescription  );
#ifdef _ENGINE_EDITABLE_

	virtual ~ClassNode(void);
#endif
#ifdef _ENGINE_EDITABLE_

	void	CloseAllObjects( );
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif

	BaseObject *GetBaseObject(const char *  _ObjectName);

#ifdef _ENGINE_EDITABLE_

	BaseObject *NewObject( const char *  _ObjectName=0L );
#endif

	void	RegisterClassList( const BaseObject::ClassDescription * const * _ppclassDescriptionList );

#ifdef _ENGINE_EDITABLE_

	void	DestroyManagedObject( BaseObject * _objectToDelete );
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetDisplayClassName() const { return ( m_ClassDescription->m_DisplayClassName ); };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetDefaultObjectName() const { return ( m_ClassDescription->m_DefaultObjectName ); };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassHelpDescription() const { return ( m_ClassDescription->m_ClassHelpDescription ); };
#endif

	ClassNode *GetSubClassNodeByDescription( const BaseObject::ClassDescription * _classDescription);
#ifdef _ENGINE_EDITABLE_

	 const  ClassNode *GetFirstSonClassNode() const { return m_pFirstSonClassNode; } ;
#endif
#ifdef _ENGINE_EDITABLE_

	 const ClassNode *GetNextBrotherClassNode() const { return m_pNextBrotherClassNode; } ;
#endif

	 inline const  PackList *GetObjectList() const { return &m_BaseObjectList; } ;

#ifdef _ENGINE_EDITABLE_

	 bool IsAbstract() const { return ( 0L == m_ClassDescription->m_NewMethod); } ;
#endif
#ifdef _ENGINE_EDITABLE_

	 bool IsUseful() const ;
#endif
#ifdef _ENGINE_EDITABLE_

	virtual  const unsigned char * MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_pendName=0L);
#endif

#ifdef _ENGINE_EDITABLE_

	unsigned int SetObjectsUniqueReferenceIndex( unsigned int _FirstnumberToSet=1,int _increment=1 );
#endif

#ifdef _ENGINE_EDITABLE_

	virtual	void FlushNames();
#endif

	BaseObject *GetBaseObjectBySerializedIndex( unsigned int _index);
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return ("ClassNode" ); };
#endif
#ifdef _ENGINE_EDITABLE_

	void ExportCPPClassDescription( PackString &_fileTextImage ) const ;
#endif
#ifdef _ENGINE_EDITABLE_

	void ExportCPPClassDescriptionLine( PackString &_fileTextImage ) const;
#endif
#ifdef _ENGINE_EDITABLE_

	const BaseObject::ClassDescription *CheckClassListError() const;
#endif

protected:

	const BaseObject::ClassDescription * m_ClassDescription;
#ifdef _ENGINE_EDITABLE_

	unsigned int	m_NumberOfObjectInstancied;
#endif
#ifdef _ENGINE_EDITABLE_

	typedef enum{
		eSerError_OK=0,
		eSerError_BadChunkHead
	} eSerializationError ;

	eSerializationError		m_SerializationState;
#endif

	PackList		m_BaseObjectList;

	ClassNode		*m_pNextBrotherClassNode;

	ClassNode		*m_pFirstSonClassNode;
#ifdef _ENGINE_EDITABLE_

	volatile bool	m_OwnerThreadIsCurrentlyManagingObjects;

	volatile int	m_TESTDEBUG_CONTEXTLOCK_ID;

	volatile unsigned int	m_NumberOfThreadsCurrentlyLockingTheContext;
#endif
#ifdef _ENGINE_EDITABLE_

	const BaseObject::ClassDescription *m_pLastClassDescWithFaultySerializer;
#endif
#ifdef _ENGINE_EDITABLE_

	void DestroyAllManagedObjects();
#endif
#ifdef _ENGINE_EDITABLE_

	void ProtectedDestroyAllObjects();
#endif
#ifdef _ENGINE_EDITABLE_

	void ProtectedDestroyAllIndependantObjects( BaseObject *_pObjectWhichAllOtherShouldDependOn );
#endif
#ifdef _ENGINE_EDITABLE_

	void ProtectedAddSameClasses( BaseContext *_pContextToAddManagement );
#endif

};

#endif
