/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_BaseObject_H
#define COM_M4NKIND_BaseObject_H
/*!
	\addtogroup	BaseObjectInheritedDocGroup BaseObject Inherited Classes: All of them.
	\brief		Here are links to classes inherited from BaseObject.
				It means that all these classes can be registered to a BaseContext
				with BaseContext::RegisterClassList(), and then object instancied from it 
				with BaseContext::NewObject(), if not virtual. they can then be edited, referenced, in this context.
				 These classes may be virtual or not.
				In this list, each class can come from one library package or another.
*/
/*!
	\addtogroup	BaseObjectInheritedDocByLib BaseObject Inherited Classes by Library Packages.
	\brief		Here are links to classes inherited from BaseObject.
				It means that all these classes can be registered to a BaseContext
				with BaseContext::RegisterClassList(), and then object instancied from it 
				with BaseContext::NewObject(), if not virtual. they can then be edited, referenced, in this context.
				 These classes may be virtual or not.
*/
/*!
	\addtogroup	BaseObjectInherited_Veda BaseObject Inherited Classes For VedaLib
	\ingroup	BaseObjectInheritedDocByLib
	\brief		Here are links to classes inherited from BaseObject In VedaLib, the base package.
				You should have a look at the classes here, in this packages, because
				it provides not only BaseObject, the root class, but also
				some other inherited virtual class that can be used to unify some
				behaviour, like VirtualMedia which can be a base for any animation
				or sound, or InterfacePrefObject, which keep informations on
				the way a \mainprojectname interface could save its preferences.
				These classes can be registered to a BaseContext.
				with BaseContext::RegisterClassList().
				 These classes may be virtual or not.\n
				This Library Package and classes are under the\n
				<b>GNU LESSER GENERAL PUBLIC LICENSE Version 2.1</b>
*/

#include "NamedObject.h"
// serializable base type needed:
#include "PackString.h"
#include "VirtualMachine.h"
class BaseObject;
class BaseContext;
class PackObjectReference;			

//! CreatorCallBackFunction stands for a pointer to a static method that make a new BaseObject.
typedef BaseObject *(*CreatorCallBackFunction)(void);

/*!
	\class	BaseObject
	\ingroup BaseObjectInheritedDocGroup BaseObjectInherited_Veda
	\brief	 Virtual Base class for all objects managed in a BaseContext.\n
			- To be used in such context, Each classes (virtual or not) inheriting BaseObject should
			be first registered to this context using BaseContext::RegisterClassList(), by passing
			the static descriptor m_description to it.
			You must <b>not</b> construct these objects with operator new and delete, but with
			public methods BaseContext::NewObject() and BaseContext::DestroyObject().
			All Object Instances (and their serializable members) will then be serialized with the context.\n
			- There is 2 levels of construction for objects managed by a context:\n
			You register your serializable member in the C++ constructor, but you
			write your construction code  by ovverriding the protected virtual method CreateInternal(),
			and your destruction code must stand in an override of the protected virtual method CloseInternal().
			It means, all allocation done in CreateInternal() must be close in CloseInternal().
			- To throw the creation of your object from outside the context, you must use
			the public Create(), and check its return code. To close it, use Close(). Destroying
			a BaseContext with the delete operator will delete all its BaseObjects.
			You can check the creation state of an object at any time with IsCreated().
			The methods of your objects should only be used when the object is created with no error.
			- Your object may have registered PackObjectReference members: they are
			 dynamic pointers to other objects in the context. Your object will be dependant of
			 the objects it points at. In fact, Create() assumes that if your object
			 is successfully created, <b>all its dependant objects are also created successfully</b>.
			 It means that, inside your CreateInternal() code, the pointer you get from your 
			 %PackObjectReference members points objects that are already created succesfully.
			 Look Create() for more.
			- About GetName(): All BaseObject are named for convenience when edited, but
				this name isn't usually used to refer the objects.
			- Extending a registerable %BaseObject must be done by using a define macro
			  in its header (.h) and a declaration macro in its (.cpp). These macros
			  will automatise the code of the destructor, but the constructor must be coded.
				you must call the super constructor from it, and then register your serializable members in it.
				from class SimpleTest, you could have this SimpleTest.h:
				\code
#ifndef	SIMPLETEST_H
#define SIMPLETEST_H
#include "BaseObject.h"
#include "PackULong.h"
class SimpleTest : public BaseObject
{ public:
	SimpleTest();
	BASEOBJECT_DEFINE_CLASS(SimpleTest);
	void	MyMethod(void);
 protected:
	PackULong	mSerializableMember;
};
#endif
				\endcode
			...and a possible SimpleTest.cpp could be:
				\code
#include "SimpleTest.h"
BASEOBJECT_DECLARE_CLASS( "serId", SimpleTest, BaseObject,"Simple Test Class","ObjectDefaultName","Well, It was just a simple class to show how.");
SimpleTest::SimpleTest() : BaseObject()
{
	REGISTER_MEMBER_PACKULONG( mSerializableMember,"The Member",1337);
}
void	SimpleTest::MyMethod(void)
{	// this should get value 1337 after creation:
	int	serializedvalue = mSerializableMember.Get();
}
				\endcode
			use macro BASEOBJECT_DEFINE_VIRTUALCLASS(_ClassName) for a virtual class
			use macro BASEOBJECT_DEFINE_CLASS(_ClassName) for a class able to create objects.
			Note this macro define the destructor, but not the constructor.
			and in the .cpp class declaration, use :
			BASEOBJECT_DECLARE_VIRTUALCLASS(_serId,_ClassName,_superClassName) for a virtual class,
			BASEOBJECT_DECLARE_CLASS(_serId,_ClassName,_superClassName,_displayName,_defaultObjectName,_helpDescription)
			..for a class able to create objects, where _serId is a short character string
			to identify the class in the binary serialization, _ClassName is the C++ class name,
			_superClassName is the C++ classname of the upper class to inherit, _displayName
			is an explicit name for GUIs, and _helpDescription is a long sentence explaining what is the class for.
			these macro will define a public static class descriptor, m_Description, that will
			be used to register the class in a context ( see BaseContext::RegisterClassList() )\n
			- To add serializable members, use \ref BaseSerializableClass "any type" inherited from virtual class BaseTypes
			(PackULong, PackFloat, PackList,...) as agregates members, and then,
			in the class constructor, register them as serializable members of the object
			with a macro of the family:\n
			REGISTER_MEMBER(_memberobject,_InfoString)
			where  _memberobject is the member, and _InfoString an explicit name for interface to display.
			note: if _InfoString is NULL (0L), the member will be serialized, but not editable and shown in GUIs.
			Other REGISTER_MEMBER_XXXX family macros also ask for a default value,
			then some parameters according to the member type: see .h interface files.
			Once registered, you can use Get()/Set() on it, it will be managed by the context
			undo stack, and a lot of BaseContext features.
			Each class in VedaLibImage, VedaLibMath and other extended libraries 
			are examples of how to do it.
*/
class BaseObject : public NamedObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	BaseObject(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~BaseObject(void);
#endif
	/*!
		\brief	Public Method that ensures the object is construct,
				and ensures that all needed objects in the context are build before.
				Close() should close everything opened by Create() for this object.
				if _immediate is true, it will build everything immediately and return 0.
				if false, it will begin to build needed objects, and then return an integer
				which stand for the rest of objects to be built before this object is completly created.
				if Create(false) is thrown again and again, this weight will get down to zero, and zero
				mean the object is created (like with Create() ). It allows to follow the progression
				of the creation in a loop, and so progressbars.
				Note it runs the protected virtual CreateInternal() method to do it. Extend CreateInternal()
				to make the construction, do not extend create.
				\param _immediate if true, it will build everything and return 0. If false, make just a part of the creation and return a construction weight.
				\return 0 when object is completly built, otherwise a weight integer with positive value when constrution is still to be performed. a negative value means creation impossible.
	*/
	int Create(bool _immediate=true);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Public method that will close everythinh opened with Create(),
				and declare the object Closed. The object is not destroyed,
				and can be rebuild eback using Create().
				Extend CreateInternal() and CloseInternal() to manage your objects.
				This method also sends a "Object Close" message in edition mode.
		\param	_AndCloseAllDependantObjects	if true, all dependant objects are close the same way.
	*/
	void Close(bool _AndCloseAllDependantObjects=false);
#endif

	/*!
		\class  ClassDescription
		\brief	BaseObject Nested class that describes each BaseObject and inherited class statically.
				It is defined automaticaly by macros BASEOBJECT_DECLARE_CLASS in class definition.
	*/
	class ClassDescription
	{
		public:
		//! static NewObject() method. this is then used by the BaseContext factory to create objects.
		const CreatorCallBackFunction m_NewMethod;
		//! serialized class name. Each extended class have to take a different name. This is used by the context serialization to identify a class. 
		const char *  m_ClassName;
		//! static super class name. used to identify the class in the implementation hierarchy.
		const ClassDescription *  m_SuperClassDescription;
#ifdef _ENGINE_EDITABLE_
		//! when editable, knows their C++ exact class name:
		const			char *	m_CPlusPlusClassName;
		//! when editable, object has a default base name:
		const			char *	m_DefaultObjectName;
		//! when editable, a class got a display name for GUIs.
		const			char *	m_DisplayClassName;
		//! when editable, a class got a help descriptor chain:
		const			char *	m_ClassHelpDescription;
#endif
	};
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.

				For all "BaseObject", this name is the same that identify the class into the context:
				No need to extend this method further after BaseObject.

		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return ( GetClassDescription().m_ClassName ); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  return an explicit name for the class the object is instancied.
		\return a const character string.
	*/
	virtual const char *GetDisplayClassName() const { return ( GetClassDescription().m_DisplayClassName ); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  return the default name that is set to new objects if not specified:
		\return a const character string.
	*/
	virtual const char *GetDefaultObjectName() const { return ( GetClassDescription().m_DefaultObjectName ); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  return an explicit name for the class the object is instancied.
		\return a const character string.
	*/
	virtual const char *GetClassHelpDescription() const { return ( GetClassDescription().m_ClassHelpDescription ); };
#endif
	//! the static class description object, to be reimplemented in each class to describe it, with BASEOBJECT_DECLARE_VIRTUALCLASS / BASEOBJECT_DECLARE_CLASS macros
	static const ClassDescription	m_Description;

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	
		\return 
	*/
	virtual const ClassDescription &GetClassDescription( void ) const;
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief Create a new object with the exact same member values in the same context.
				references to other objects are kept the same, so that the clone uses
				the same objects as the original.
		\param _cloneName the name of the new clone.
		\return a new object of the same shape, or 0L if failed.
	*/
	BaseObject *Clone(const char *_cloneName=0L);
#endif

#endif
	/*!
		\brief	The BaseContext the Object belong to. Should only be used by BaseContext at object creation.
				As it is set after construction, the chunk initialisation continue there the
				initialisation of registered members.
		\param _pBaseContext
	*/
	void	SetBaseContext(BaseContext *_pBaseContext);
	/*!
		\brief	get the context this object belong to.
		\return	the context.
	*/
	inline BaseContext *GetContext(void){ return(m_pBaseContext); };
	/*!
		\brief	The Machine of which the context is implemented. It is a gateway for hardware abstraction.
		\return a machine used by the object and context.
	*/
	VirtualMachine			*GetMachine();
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	return the First Reference To This Object, and then scan the list of all PackObjectReference
			that refers the objects.
		\return FirstReferenceToThisObject
	*/
	inline PackObjectReference *GetFirstReferenceToThisObject(void ){ return(m_pFirstReferenceToThisObject);  };
#endif

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	all reference to this object will be closed. used by deletion done by ClassNode.
	*/
	virtual void CloseReferences();
#endif

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	set FirstReferenceToThisObject, should be used privately.
		\param  _pReference FirstReferenceToThisObject
	*/
	inline void SetFirstReferenceToThisObject( PackObjectReference *_pReference ){ m_pFirstReferenceToThisObject = _pReference ;  };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Used by the context to set the reference index for serialization, to manage PackObjectReference serialisation:
		\param  _referenceIndex the unique index of the object  in the whole database.
	*/
	inline void SetUniqueReference( unsigned int _referenceIndex ){ mSer_UniqueReference.Set( _referenceIndex );  };
#endif
	/*!
		\brief	Used by PackObjectReference for input serialization. See ClassNode::SetObjectsUniqueReferenceIndex()
		\return  the unique index of the object  in the whole database.
	*/
	inline unsigned int GetUniqueReference(){ return  mSer_UniqueReference.Get(); };
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	if this object is refered by another one (which means: needed to be created.)
				directly, or indirectly through other objects references, it will return true,
				otherwise false if no dependance are found at all. This is a recursive test.
		\param  _pObjectSuspectedOfOwnage the object supsected of ownage.
		\return true if dependant.
	*/
	bool	isDependantOf( BaseObject *_pObjectSuspectedOfOwnage );
#endif
	/*!
		\brief	return false if allocated but not created, true if also created.
		\return true if currently created.
	*/
	inline bool	isCreated() const { return m_bIsCreatedAccordingToAllUsedObject; };
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	the rate of creation, 0.0f if created, 1.0f if not created at all.
		\return the 0,1 rate
	*/
	inline	float	GetCreationRate(){ return m_CreationRate ; };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\class	PreviewConfiguration
		\brief	Through PreviewConfiguration, ProcessPreview() can read information from an editor, on how
				the object preview should look like. The data can be used (or not) in a way or another,
				and the way the editor change them may vary. It is a public nested class.
	*/
	class	PreviewConfiguration
	{
	public:
		//! \brief	return a viewport scale in 0.0,0.0,1.0,1.0 units. useful to zoom 2D previews..
		virtual	void	GetViewportScale(float &_x1,float &_y1,float &_x2,float &_y2) const =0 ;
		//! \brief	return an abstract preview position.
		virtual	void	GetPreviewPosition(float &_PositionX,float &_PositionY,float &_PositionZ ) const =0;
		//! \brief	return an abstract preview Rotation.
		virtual	void	GetPreviewRotation(float &_Xaxis,float &_YAxis,float &_ZAxis) const =0;
		//! \brief	return 0L or a pointer on a focused member, or sub-member, or member of another object that may be linked to this one.
		virtual	const BaseType	*GetFocusedObjectPointer() const =0;
		//!	\brief	return a draw preference flag.(draw grid, etc...)
		virtual	unsigned int	GetDrawPreferenceFlag() const =0;
		//! \brief preview time lapse start
		virtual double	GetPreviewStartTime() const =0;
		//! \brief preview time end start
		virtual double	GetPreviewEndTime() const =0;
	};
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	any suit of call to ProcessPreview() should be done between
				StartPreview() and EndPreview() .
				EndPreview(). This is needed to stop sound mixing from the object
				when the preview change.
	*/
	virtual		void	StartPreview(){};
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Extend this method to draw your preview !
				a GUI could need to play, draw, print, or output from any way, a preview of a 
				created object. Inherited classes can implement it in
				any way, to explicit the current shape of an object.

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration Preview Configuration object. Never 0L. Do not keep a pointer to it.
	*/
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	any suit of call to ProcessPreview() should be done between
				StartPreview() and EndPreview() .
				EndPreview(). This is needed to stop sound mixing from the object
				when the preview change.
	*/
	virtual		void	EndPreview(){};
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	In editable mode, it returns an information output to display,
				usually given in ProcessPreview(), using m_ObjectInfoLine.
		\return a character string 
	*/
	inline	const char *GetObjectInfoLine() const { return m_ObjectInfoLine.Get(); };
#endif

#ifdef _ENGINE_EDITABLE_
	/*!
		\typedef tm_BaseObject
		\brief	Begin the enumeration for ExecuteToolMethod()
	*/
	typedef enum {
		//! first enum to extend tool method index.
		tm_BaseObject_FirstInheritageToolMethod=1
	} tm_BaseObject;
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
	virtual void ExecuteToolMethod( unsigned int _MethodIDToExecute ) ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	get next tool method ID, or 0 if no more. Send 0 as _PreviousMethodID to get the first.
	*/
	unsigned int GetNextToolMethod( unsigned int _PreviousMethodID,unsigned int &_NextMethodflagInfo ,const char *&_pNextShortDisplayName,const char *&_pNextDisplayHelp );
#endif

	/*
		\ class	ReferenceMember
		\ brief	Note: THIS WAS EXPERIMENTAL !!!
				Inner base class that stands for special data members to a BaseObject,
				which need to exist by reference to this objects.
				It means, if a tree or a list refers this same object
				many times. the constant serialized members will be the same,
				but	some data could have to change according to a given reference.
				If you don't need this feature, don't extend ReferenceMembers and
				CreateNewReferenceMembers() .
	*/
	/*class	ReferenceMembers
	{ public:
	};*/
	/*
		\ brief	THIS WAS EXPERIMENTAL !!! get a new ReferenceMembers , or NULL (0L) if such feature
			has no sense with this object. Pay attention, BaseObject and inherited
			can know about a ReferenceMembers object, but ReferenceMembers objects
			shouldn't care about their BaseObject, because CreateNewReferenceMembers()
			can be thrown when their objects itself is not created.
		\ return new members by reference, or empty adress.
	*/
	//virtual	ReferenceMembers	*CreateNewReferenceMembers();
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	If the last call to Create() failed, this return true,
				and GetLastCreationErrorString() could explain why.
	*/
	inline bool DidLastCreationFailed() const { return m_LastCreationFailed; };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	
	*/
	inline const char *GetLastCreationErrorString() const { return m_LastCreationErrorString.Get();  };
#endif	
//	BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR("No ");
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! Unique reference used only for serilalization, and PackObjectReference resolution. 
	PackULong			mSer_UniqueReference;

#ifdef _ENGINE_EDITABLE_
	//!	In editable mode, an information output to display, in any form.
	//! usually given by ProcessPreview().
	PackString			m_ObjectInfoLine;
#endif
#ifdef _ENGINE_EDITABLE_
	//!	In editable mode, if Create() fail, this string may be inited.
	PackString			m_LastCreationErrorString;
#endif
#ifdef _ENGINE_EDITABLE_
	//! the rate of creation, 0.0f if created, 1.0f if not created at all.
	float				m_CreationRate;
#endif
#ifdef _ENGINE_EDITABLE_
	//! In editable mode, trace the list of all reference to this object:
	PackObjectReference		*m_pFirstReferenceToThisObject;
#endif

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by Create().
				If the object is already created, it return false, and all Create methods should exit.
				Note it runs the protected virtual CreateInternal() method to do it.
				\return true if object has to be built.
	*/
	virtual bool CreateInternal(void){ return(true); };
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	method that closes everything. Still, the object exist and can be rebuild the same using Create().
				The real code should be extended in the protected virtual method CloseInternal()
	*/
	virtual void CloseInternal(void){};
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Register a tool method. It is a private Nested class that manage the known ToolMethod in edition mode.
	*/
	class	ToolMethodCell
	{	public:
		//! next method or 0L.
		ToolMethodCell	*m_pNextCell;
		//! ID registered. should be unique for a given class. should be managed by an enum.
		unsigned int	m_ToolMethodID;	
		//! information bits about the methods (load a format, save a format.)
		unsigned int	m_FlagInfo;
		//! ShortDisplayName
		PackString		m_ShortDisplayName;
		//! description of what does the method:
		PackString		m_DisplayHelp;
	};
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Register a tool method. Do not use it directly,
			use macro version  BASEOBJECT_REGISTER_TOOLMETHOD() It should be set in the constructors.
	*/
	void RegisterToolMethod( unsigned int _MethodID,unsigned int _MethodflagInfo ,const char *_pShortDisplayName,const char *_pDisplayHelp );
#endif
#ifdef _ENGINE_EDITABLE_
	//! next method or 0L.
	ToolMethodCell	*m_pFirstToolMethodCell;
#endif
#ifdef _ENGINE_EDITABLE_
	//! some optimsation stuff to not make the executable string table grow too much...
	static const char *	m_pErrorString_Memory;
	static const char *	m_pErrorString_CantOpenFile;
#endif
/*==================================================================
								private
==================================================================*/
private:
	//! true if this and all the Used Object are created according to their members.
	//! this was set private because only Close() should be able to set it false.
	bool		m_bIsCreatedAccordingToAllUsedObject;
#ifdef _ENGINE_EDITABLE_
	//!	if last Create() failed, this is true.
	//! note, this is different from m_bIsCreatedAccordingToAllUsedObject,
	//! because an object could be buildable & closed, or unbuildable & closed:
	bool				m_LastCreationFailed;
#endif
	// context private use. Each BaseObject is also a cell of a list that trace the creation process.
	BaseObject	*mCreationStackCell_pPrevCell;
	// context private use. Each BaseObject is also a cell of a list that trace the creation process.
	BaseObject	*mCreationStackCell_pNextCell;

};

/*!
	\def	BASEOBJECT_DEFINE_CLASS

	\brief	This macro is used by all class inherited from BaseObject to create their class descriptor.
			Then this description is used for Class registration to a context.
			Use BASEOBJECT_DEFINE_CLASS in the .h and
			use BASEOBJECT_DECLARE_CLASS in the cpp.
*/
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
/*!
	\def	BASEOBJECT_DEFINE_VIRTUALCLASS

	\brief	This macro is used by all class inherited from BaseObject to create their class descriptor.
			Then this description is used for Class registration to a context.
			Use BASEOBJECT_DEFINE_VIRTUALCLASS in the .h and
			use BASEOBJECT_DECLARE_VIRTUALCLASS in the cpp.
		\param	_ClassName	the C++ class name.
*/
#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_DEFINE_VIRTUALCLASS(_ClassName) \
static	const ClassDescription	m_Description;\
virtual const ClassDescription &GetClassDescription( void ) const;\
virtual ~_ClassName(void);
#else
#define		BASEOBJECT_DEFINE_VIRTUALCLASS(_ClassName) \
static	const ClassDescription	m_Description;
#endif
/*!
	\def	BASEOBJECT_DECLARE_CLASS

	\brief	This macro is used by all class inherited from BaseObject to create their class descriptor.
			Then this description is used for Class registration to a context.
			You specify a char string static class name independant of your real C++ class name, and the C++
			class name of your class, and the class you inherit from.
	\param	_idname			a string tag that is used in serialization chunk to mark the class. 
	\param	_ClassName		the C++ class name.
	\param	_superClassName the C++ superclass name.
	\param	_displayName	string: a displayable class name. note class can be sorted by names.
	\param	_defaultObjectName string: the default name for objects.
	\param	_helpDescription	string: a long text description help to be displayed for the class.
*/
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
/*!
	\def	BASEOBJECT_DECLARE_VIRTUALCLASS

	\brief	Same as BASEOBJECT_DECLARE_CLASS , but for virtual, uninstanciable, classes.
*/
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

/*!
	\def	BASEOBJECT_REGISTER_TOOLMETHOD

	\brief	Macro that make accessible a tool method for this class. It must be used in constructors.
			See RegisterToolMethod().
	\param	_NewMethodID	an enum ID for this method.
	\param	_MethodflagInfo what type of method is it
	\param	_pShortDisplayName display name for the method.
	\param	_pDisplayHelp help on what does the method.
*/
#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_REGISTER_TOOLMETHOD( _NewMethodID, _MethodflagInfo , _pShortDisplayName, _pDisplayHelp )\
		RegisterToolMethod(_NewMethodID,_MethodflagInfo,_pShortDisplayName,_pDisplayHelp);
#else
#define		BASEOBJECT_REGISTER_TOOLMETHOD( _NewMethodID, _MethodflagInfo , _pShortDisplayName, _pDisplayHelp )	;
#endif

/*!
	\def	BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR

	\brief	Macro to be used in CreateInternal() construction function
			before a call to "return false" when the construction fails,
			to explicit why the object couldn't be built. For memory
			allocation error, use BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY()

	\param	_string a character string.
*/
#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR( _string )\
		m_LastCreationErrorString.Set(_string);
#else
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR( _string ) 
#endif
/*!
	\def	BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY

	\brief	Macro to be used in CreateInternal() construction function
			before a call to "return false" when the construction fails,
			in the case of a memory allocation.
	\param	_string a character string.
*/
#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY()\
		m_LastCreationErrorString.Set(BaseObject::m_pErrorString_Memory);
#else
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY() 
#endif
/*!
	\def	BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY

	\brief	Macro to be used in CreateInternal() construction function
			before a call to "return false" when the construction fails,
			in the case of a memory allocation.
	\param	_string a character string.
*/
#ifdef _ENGINE_EDITABLE_
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_CANTOPENFILE()\
	m_LastCreationErrorString.Set(BaseObject::m_pErrorString_CantOpenFile);
#else
#define		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_CANTOPENFILE() 
#endif
// end of .h
#endif
