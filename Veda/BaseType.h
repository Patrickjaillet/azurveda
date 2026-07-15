// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_BaseType_H
#define COM_M4NKIND_BaseType_H

class BaseObject;
class BaseContext;
class PackString;

class BaseType;

typedef BaseType *(*BaseTypeCreatorCallBackFunction)(void);

class BaseType
{

public:

	BaseType(void);

#ifdef _ENGINE_EDITABLE_

	virtual ~BaseType(void);
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk)=0;

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void)=0;
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill)=0;
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned int WriteFile(const char * _pFileName);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned int ReadFile(const char * _pFileName);
#endif

	virtual	void	SetObjectThatManagesThis(BaseObject *_pManager);

#ifdef _ENGINE_EDITABLE_

	void	SetInfoString(const char * _InfoString);
#endif
#ifdef _ENGINE_EDITABLE_

	inline const char *	GetInfoString() const{ return m_pInfoString; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const =0;
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char	*ValueToString();
#endif

	inline BaseObject	*GetObjectThatManagesThis(){ return m_pObjectThatManagesThis;  };

#ifdef _ENGINE_EDITABLE_

	void	MakeCloneOf( BaseType &_ModelObject );
#endif
#ifdef _ENGINE_EDITABLE_

	typedef enum{

		eVedaUpdate_NoUpdate=0,

		eVedaUpdate_MemberIsGoingToChange,

		eVedaUpdate_MemberChanged,

		eVedaUpdate_MemberChangedBecauseOfUndo,

		eVedaUpdate_MemberDestroyed,

		eVedaUpdate_ObjectNew,

		eVedaUpdate_ObjectDestroyed,

		eVedaUpdate_ObjectReCreated,

		eVedaUpdate_ObjectClosed,

		eVedaUpdate_NewRegisteredClass
	} eVedaUpdate;

	class ObjectManager
	{ public:

		virtual	void NotifyObjectChange( eVedaUpdate _UpDateType,BaseType *_ObjectMember,
									const unsigned char *_PreviousShapeChunk=0L,
									unsigned int		 _PreviousShapeChunkLength=0L,
									const unsigned char *_NewShapeChunk=0L,
									unsigned int		 _NewShapeChunkLength=0L
									)=0;
	};
#endif

protected:

	BaseObject				*m_pObjectThatManagesThis;

#ifdef _ENGINE_EDITABLE_

	const char					*m_pInfoString;
#endif

#ifdef _ENGINE_EDITABLE_

	PackString					*m_pValueString;
#endif

#ifdef _ENGINE_EDITABLE_

	unsigned char			*m_pPreviousValueShape;
	unsigned int			 m_PreviousValueShapeLength;
#endif

#ifdef _ENGINE_EDITABLE_

	void	BeginChangeNotification();
#endif
#ifdef _ENGINE_EDITABLE_

	void	EndChangeNotification();
#endif

};

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER(_object,_InfoString) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );
#else
#define		REGISTER_MEMBER(_object,_InfoString) \
RegisterSerializableMember(_object);
#endif

#endif
