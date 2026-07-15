/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_BaseType_H
#define COM_M4NKIND_BaseType_H
/*!
	\addtogroup	BaseSerializableClass	Base Type Serializable Classes
	\brief		Here are links to the low level types Classes provided by the base library.
				They each manage accessors, serialization, and events.
				They can be used alone, or as members of a BaseObject class.
				They all inherit from class BaseType.
*/
class BaseObject;
class BaseContext;
class PackString;
/*!
	\class	BaseType

	\brief	 Virtual class for all \mainprojectname serializable types.
			It is the root of all serializable classes, and actually all classes in \mainprojectname Inherit from it.

			All inherited objects will be able to read their values with Serialize_In(), and write
			it to a first call to GetSerializedDescriptionSize(), which return the size of the data to write,
			and then Serialize_Out() that writes it.
			In editable mode, each objects has direct access to ReadFile() and WriteFile() which uses the serialization.

			\mainprojectname Also provide a more complex Context management, through classes BaseContext and BaseObject :
			a BaseContext object manages a set of BaseObject. Simple BaseType are then used 
			as class members of BaseObjects. That's why this class is also able to tell if it is a baseObject member
			with GetObjectThatManagesThis() , and implements methods for BaseContext edition notification.

*/
class BaseType;
//! BaseTypeCreatorCallBackFunction stands for a pointer to a static method that make a new BaseType. It is used by PackStruct
typedef BaseType *(*BaseTypeCreatorCallBackFunction)(void);

class BaseType
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
	*/
	BaseType(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~BaseType(void);
#endif
	/*!
		\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
		\param	_pDescriptionChunk the objet description chunk. 
		\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
	*/
	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk)=0;

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
		\return	byte size of the serialisation to do.
	*/
	virtual unsigned int  GetSerializedDescriptionSize(void)=0;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	write the Current object definition to a Chunk using private packed types, recursively.
		\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
		\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
	*/
	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill)=0;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	write a serialized form, using GetSerializedDescriptionSize() and Serialize_Out().
		\param	_pFileName the file path. 
		\return 0 if OK.
	*/
	virtual unsigned int WriteFile(const char * _pFileName);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Try to read a serialized form, using Serialize_In().
		\param	_pFileName the file path.
		\return 0 if OK.
	*/
	virtual unsigned int ReadFile(const char * _pFileName);
#endif

	/*!
		\brief	The Object this one belong to as a member. Should only be used at init.
		\param _pManager BaseObject
	*/
	virtual	void	SetObjectThatManagesThis(BaseObject *_pManager);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Explicits the use of an object when used as a class member. It exists only in editable mode,
				and then can be used by any GUI. A member with an
				infoString not NULL, should be drawn in a GUI and receive updates.
				an object member with GetInfoString() should not be edited by an interface
				or undo-managed.
				Use REGISTER_MEMBER macros family: it registers the members and
				use SetInfoString() or not according to the compilation mode.
		\param	_InfoString a short string explaining what's the object for. 
	*/
	void	SetInfoString(const char * _InfoString);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Explicits the use of an object when used as a class member.
				It exists only in editable mode,
				and then can be used by any GUI. A member with an
				infoString not NULL, should be drawn in a GUI and receive updates.
				an object member with GetInfoString() to NULL should not be edited by an interface
				or undo-managed. set with SetInfoString().
		\return		a short string explaining what's the object for, or NULL if not set. 
	*/
	inline const char *	GetInfoString() const{ return m_pInfoString; };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const =0;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	convert the value of this object to an explicit string. The object manages the string privately,
			so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			 Note: this is not virtual, but each class can manage m_pValueString or not.
		\return	the value as a const string. 
	*/
	virtual const char	*ValueToString();
#endif

	/*!
		\brief	If there is any, get the object that manages this one.
		\return	the object that manages this one as a member, or NULL
	*/
	inline BaseObject	*GetObjectThatManagesThis(){ return m_pObjectThatManagesThis;  };

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Tool that makes an object take the values of another using serialization.
				<b>Use with care:</b>
				You got to clone a PackUlong with a PackULong, a PackString with a PackString:
				Clone Object of the same type or it will crash .	
				A test is done at the beginning to check that for both types are the sames,
				if not it does nothing. however it may still crash with 2 objects from unherited 
				classes. 
				Also, some type like PackObjectReference, which manages an implicit value that
				has external dependencies will not be cloned.
		\param _ModelObject the object took as a model.
	*/
	void	MakeCloneOf( BaseType &_ModelObject );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\typedef eVedaUpdate
		\brief	enum used by BaseContext::NotifyObjectChange() to 
				describe the notification meaning. Used by interfaces
				to monitor a context.
	*/
	typedef enum{
		//! 0 means No Update.
		eVedaUpdate_NoUpdate=0,
		//! Sent before a BaseObject member value change.( in order to lock the context to prevent object use from other thread.)
		eVedaUpdate_MemberIsGoingToChange,		
		//! a member of a BaseObject just changed.
		eVedaUpdate_MemberChanged,				// sent after the member value change, and is used by the context to manage undo stack and other things...
		//!  a member of a BaseObject just changed, but because of an undo.
		//! you may treat it the same as  eVedaUpdate_MemberChanged.
		eVedaUpdate_MemberChangedBecauseOfUndo, // so that the change does not re-affect the undo stack !!
		//! Member of object Destroyed ! Watch out, the ID of the member is a dead pointer.
		eVedaUpdate_MemberDestroyed,		// when there is a list/tree member and one cell is destroyed.
		//! new BaseObject created.
		eVedaUpdate_ObjectNew,
		//! BaseObject just destroyed.Watch out, the ID of the object is a dead pointer.
		eVedaUpdate_ObjectDestroyed,
		//! an object has just been Created; It's state has changed!
		eVedaUpdate_ObjectReCreated,
		//! an object has just been closed; It's state has changed!
		eVedaUpdate_ObjectClosed,
		//! TODO: a new class has been registered to the context.
		eVedaUpdate_NewRegisteredClass
	} eVedaUpdate;
	/*!
		\class ObjectManager
		\brief	Nested Class that manages events 
				(TODO,When final, this will give more independances between Base types
					and Context class)
	*/
	class ObjectManager 
	{ public:
		/*!
			\brief Used by BaseObjects "Set()" member methods to notify what happened when one of 
					the member was edited, to change its value. It is used to manage the Undo/Redo capability.
					You should not use this method unless you create a PackSerializable base type and code it's Set().
					TODO: not final.
			\param _UpDateType eVedaUpdate enum that identify the update info type.
			\param _ObjectMember	BaseType if _UpDateType is about a member.
			\param _PreviousShapeChunk	optionnal serialization of the previous state or 0L.
			\param _PreviousShapeChunkLength optionnal serialization of the previous state or 0L.
			\param _NewShapeChunk optionnal serialization of the new state or 0L.
			\param _NewShapeChunkLength optionnal serialization of the new state or 0L.
		*/
		virtual	void NotifyObjectChange( eVedaUpdate _UpDateType,BaseType *_ObjectMember,
									const unsigned char *_PreviousShapeChunk=0L,
									unsigned int		 _PreviousShapeChunkLength=0L,
									const unsigned char *_NewShapeChunk=0L,
									unsigned int		 _NewShapeChunkLength=0L
									)=0;
	};
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:

	//! an object can belong (or not) to a BaseObject,( and to a BaseContext through it).
	BaseObject				*m_pObjectThatManagesThis;

#ifdef _ENGINE_EDITABLE_
	//! a short string explaining what's the object for in editable mode. OK if NULL (no info).
	const char					*m_pInfoString;
#endif

#ifdef _ENGINE_EDITABLE_
	//! a string standing for an expression of the value of the object:
	PackString					*m_pValueString;
#endif

#ifdef _ENGINE_EDITABLE_
	//! used as a temp for object change notification in Begin/EndChangeNotification(): 
	unsigned char			*m_pPreviousValueShape;
	unsigned int			 m_PreviousValueShapeLength;
#endif

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	In edition mode, when a member of an object is changed (like in a Set()), 
		you got to use BeginChangeNotification(), make your changes, and then 
		EndChangeNotification(), so that the edition of this object and member
		is known by the context, and allows it to manage a GUI update and the undo stack.
	*/
	void	BeginChangeNotification();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	In edition mode, when a member of an object is changed (like in a Set()), 
		you got to use BeginChangeNotification(), make your changes, and then 
		EndChangeNotification(), so that the edition of this object and member
		is known by the context, and allows it to manage a GUI update and the undo stack.
	*/
	void	EndChangeNotification();
#endif

};

/*!
	\def	REGISTER_MEMBER

	\brief	This macro is used to register a serializable member in a class constructor.
			For editable mode, it uses _InfoString to explicit the use of the member, in order
			to display it in a GUI for example.
		
*/
#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER(_object,_InfoString) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString ); 
#else
#define		REGISTER_MEMBER(_object,_InfoString) \
RegisterSerializableMember(_object);
#endif

// end of file
#endif
