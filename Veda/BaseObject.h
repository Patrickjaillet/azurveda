// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_BaseObject_H
#define COM_M4NKIND_BaseObject_H

#include "NamedObject.h"

#include "PackString.h"
#include "VirtualMachine.h"
class BaseObject;
class BaseContext;
class PackObjectReference;

typedef BaseObject *(*CreatorCallBackFunction)(void);

class BaseObject : public NamedObject
{

public:

	BaseObject(void);

#ifdef _ENGINE_EDITABLE_

	virtual ~BaseObject(void);
#endif

	int Create(bool _immediate=true);
#ifdef _ENGINE_EDITABLE_

	void Close(bool _AndCloseAllDependantObjects=false);
#endif

	class ClassDescription
	{
		public:

		const CreatorCallBackFunction m_NewMethod;

		const char *  m_ClassName;

		const ClassDescription *  m_SuperClassDescription;
#ifdef _ENGINE_EDITABLE_

		const			char *	m_CPlusPlusClassName;

		const			char *	m_DefaultObjectName;

		const			char *	m_DisplayClassName;

		const			char *	m_ClassHelpDescription;
#endif
	};
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return ( GetClassDescription().m_ClassName ); };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetDisplayClassName() const { return ( GetClassDescription().m_DisplayClassName ); };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetDefaultObjectName() const { return ( GetClassDescription().m_DefaultObjectName ); };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassHelpDescription() const { return ( GetClassDescription().m_ClassHelpDescription ); };
#endif

	static const ClassDescription	m_Description;

#ifdef _ENGINE_EDITABLE_

	virtual const ClassDescription &GetClassDescription( void ) const;
#ifdef _ENGINE_EDITABLE_

	BaseObject *Clone(const char *_cloneName=0L);
#endif

#endif

	void	SetBaseContext(BaseContext *_pBaseContext);

	inline BaseContext *GetContext(void){ return(m_pBaseContext); };

	VirtualMachine			*GetMachine();
#ifdef _ENGINE_EDITABLE_

	inline PackObjectReference *GetFirstReferenceToThisObject(void ){ return(m_pFirstReferenceToThisObject);  };
#endif

#ifdef _ENGINE_EDITABLE_

	virtual void CloseReferences();
#endif

#ifdef _ENGINE_EDITABLE_

	inline void SetFirstReferenceToThisObject( PackObjectReference *_pReference ){ m_pFirstReferenceToThisObject = _pReference ;  };
#endif
#ifdef _ENGINE_EDITABLE_

	inline void SetUniqueReference( unsigned int _referenceIndex ){ mSer_UniqueReference.Set( _referenceIndex );  };
#endif

	inline unsigned int GetUniqueReference(){ return  mSer_UniqueReference.Get(); };
#ifdef _ENGINE_EDITABLE_

	bool	isDependantOf( BaseObject *_pObjectSuspectedOfOwnage );
#endif

	inline bool	isCreated() const { return m_bIsCreatedAccordingToAllUsedObject; };
#ifdef _ENGINE_EDITABLE_

	inline	float	GetCreationRate(){ return m_CreationRate ; };
#endif
#ifdef _ENGINE_EDITABLE_

	class	PreviewConfiguration
	{
	public:

		virtual	void	GetViewportScale(float &_x1,float &_y1,float &_x2,float &_y2) const =0 ;

		virtual	void	GetPreviewPosition(float &_PositionX,float &_PositionY,float &_PositionZ ) const =0;

		virtual	void	GetPreviewRotation(float &_Xaxis,float &_YAxis,float &_ZAxis) const =0;

		virtual	const BaseType	*GetFocusedObjectPointer() const =0;

		virtual	unsigned int	GetDrawPreferenceFlag() const =0;

		virtual double	GetPreviewStartTime() const =0;

		virtual double	GetPreviewEndTime() const =0;
	};
#endif
#ifdef _ENGINE_EDITABLE_

	virtual		void	StartPreview(){};
#endif
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual		void	EndPreview(){};
#endif
#ifdef _ENGINE_EDITABLE_

	inline	const char *GetObjectInfoLine() const { return m_ObjectInfoLine.Get(); };
#endif

#ifdef _ENGINE_EDITABLE_

	typedef enum {

		tm_BaseObject_FirstInheritageToolMethod=1
	} tm_BaseObject;

	virtual void ExecuteToolMethod( unsigned int _MethodIDToExecute ) ;
#endif
#ifdef _ENGINE_EDITABLE_

	unsigned int GetNextToolMethod( unsigned int _PreviousMethodID,unsigned int &_NextMethodflagInfo ,const char *&_pNextShortDisplayName,const char *&_pNextDisplayHelp );
#endif

#ifdef _ENGINE_EDITABLE_

	inline bool DidLastCreationFailed() const { return m_LastCreationFailed; };
#endif
#ifdef _ENGINE_EDITABLE_

	inline const char *GetLastCreationErrorString() const { return m_LastCreationErrorString.Get();  };
#endif

protected:

	PackULong			mSer_UniqueReference;

#ifdef _ENGINE_EDITABLE_

	PackString			m_ObjectInfoLine;
#endif
#ifdef _ENGINE_EDITABLE_

	PackString			m_LastCreationErrorString;
#endif
#ifdef _ENGINE_EDITABLE_

	float				m_CreationRate;
#endif
#ifdef _ENGINE_EDITABLE_

	PackObjectReference		*m_pFirstReferenceToThisObject;
#endif

	virtual bool CreateInternal(void){ return(true); };
#ifdef _ENGINE_EDITABLE_

	virtual void CloseInternal(void){};
#endif
#ifdef _ENGINE_EDITABLE_

	class	ToolMethodCell
	{	public:

		ToolMethodCell	*m_pNextCell;

		unsigned int	m_ToolMethodID;

		unsigned int	m_FlagInfo;

		PackString		m_ShortDisplayName;

		PackString		m_DisplayHelp;
	};
#endif
#ifdef _ENGINE_EDITABLE_

	void RegisterToolMethod( unsigned int _MethodID,unsigned int _MethodflagInfo ,const char *_pShortDisplayName,const char *_pDisplayHelp );
#endif
#ifdef _ENGINE_EDITABLE_

	ToolMethodCell	*m_pFirstToolMethodCell;
#endif
#ifdef _ENGINE_EDITABLE_

	static const char *	m_pErrorString_Memory;
	static const char *	m_pErrorString_CantOpenFile;
#endif

private:

	bool		m_bIsCreatedAccordingToAllUsedObject;
#ifdef _ENGINE_EDITABLE_

	bool				m_LastCreationFailed;
#endif

	BaseObject	*mCreationStackCell_pPrevCell;

	BaseObject	*mCreationStackCell_pNextCell;

};

#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_DEFINE_CLASS(_ClassName) \
static	const ClassDescription	m_Description; \
virtual const ClassDescription &GetClassDescription( void ) const;\
static	BaseObject *NewObject(void);\
virtual ~_ClassName(void);
#else
#define		BASEOBJECT_DEFINE_CLASS(_ClassName) \
static	const ClassDescription	m_Description; \
static	BaseObject *NewObject(void);
#endif

#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_DEFINE_VIRTUALCLASS(_ClassName) \
static	const ClassDescription	m_Description;\
virtual const ClassDescription &GetClassDescription( void ) const;\
virtual ~_ClassName(void);
#else
#define		BASEOBJECT_DEFINE_VIRTUALCLASS(_ClassName) \
static	const ClassDescription	m_Description;
#endif

#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_DECLARE_CLASS(_idname,_ClassName,_superClassName,_displayName,_defaultObjectName,_helpDescription) \
const BaseObject::ClassDescription _ClassName::m_Description= \
{ \
	_ClassName::NewObject,	\
	_idname , \
	&(_superClassName::m_Description), \
	#_ClassName,\
	_defaultObjectName,\
	_displayName, \
	_helpDescription \
}; \
BaseObject *_ClassName::NewObject(void){ return( new _ClassName() ); }\
_ClassName::~_ClassName(void) { Close(); } \
const BaseObject::ClassDescription &_ClassName::GetClassDescription( void ) const \
{	return( _ClassName::m_Description ); }
#else
#define		BASEOBJECT_DECLARE_CLASS(_idname,_ClassName,_superClassName,_displayName,_defaultObjectName,_helpDescription) \
const BaseObject::ClassDescription _ClassName::m_Description= \
{ \
	_ClassName::NewObject,	\
	_idname , \
	&(_superClassName::m_Description) \
}; \
BaseObject *_ClassName::NewObject(void){ return( new _ClassName() ); }
#endif

#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_DECLARE_VIRTUALCLASS(_idname,_ClassName,_superClassName) \
const BaseObject::ClassDescription _ClassName::m_Description= \
{ \
	0L,	\
	_idname , \
	&(_superClassName::m_Description), \
	#_ClassName,\
	0L,\
	0L,\
	0L\
};\
_ClassName::~_ClassName(void) { Close(); }\
const BaseObject::ClassDescription &_ClassName::GetClassDescription( void ) const\
{	return( _ClassName::m_Description ); }
#else
#define		BASEOBJECT_DECLARE_VIRTUALCLASS(_idname,_ClassName,_superClassName) \
const BaseObject::ClassDescription _ClassName::m_Description= \
{ \
	0L,	\
	_idname , \
	&(_superClassName::m_Description) \
};
#endif

#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_REGISTER_TOOLMETHOD( _NewMethodID, _MethodflagInfo , _pShortDisplayName, _pDisplayHelp )\
		RegisterToolMethod(_NewMethodID,_MethodflagInfo,_pShortDisplayName,_pDisplayHelp);
#else
#define		BASEOBJECT_REGISTER_TOOLMETHOD( _NewMethodID, _MethodflagInfo , _pShortDisplayName, _pDisplayHelp )	;
#endif

#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR( _string )\
		m_LastCreationErrorString.Set(_string);
#else
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR( _string )
#endif

#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY()\
		m_LastCreationErrorString.Set(BaseObject::m_pErrorString_Memory);
#else
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY()
#endif

#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_CANTOPENFILE()\
	m_LastCreationErrorString.Set(BaseObject::m_pErrorString_CantOpenFile);
#else
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_CANTOPENFILE()
#endif

#endif
