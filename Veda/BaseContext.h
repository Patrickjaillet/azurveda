/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_BaseContext_H
#define COM_M4NKIND_BaseContext_H

#include "BaseObject.h"
#include "ClassNode.h"
#include "PackList.h"
/*!
	\class	BaseContext
	\ingroup BaseContextDocGroup
	\brief	\ref BaseContextDocGroup "You may also browse the BaseContext Related Documentations page sorted by theme."\n
			Class that manages a whole database as a set of Objects inheriting class BaseObject.\n 
			BaseContext::Serialize_Out() and BaseContext::Serialize_In() will provide a quick and full
			persistent serialization of all features of a whole azurveda Context. When reading the context,
			the objects references will be resolved.
			In editable compilation mode you can use:
				- Hook notifications for all events, useful for Graphical Interfaces: see AddGUIUpdateFunction()
				- Merge serialization: simply melt as many context objects you want with MergeSerialize_In()
				- Hability to destroy all objects that are useless to a given object with DestroyAllIndependantObjects().
					Together with the merge method, it gives a complete control for large and complex contexts.
				- automatic Undo/Redo stack functionnalities with Undo(), Redo(), SetMaximumNumberOfUndoableEvents().
					This undoes affect the modification of any editable object members in the context.
					(BETA: currently,undo stack is flushed when an element is destroyed in a list.)
				- Thread management withAddOneLockForThreadUse(),RemoveOneLockForThreadUse().\n
			\n
			To use a context object, you first init a context by registering possible extended classes
			or packages, derived from BaseObject, with RegisterClassList().
			In editable mode, you can create new BaseObjects using the BaseContext::NewObject(), and destroy them with
			BaseContext::DestroyObject(). Then you can initialize each objects using BaseObject::Create(), and close
			them with BaseObject::Close(). See BaseObject for more informations on Object Creation.\n
			After the context is created, you can provide a VirtualMachine with SetMachine(),
			it stands for the hardware-abstracted machine its objects will use.\n

			Low Level internal notes:
			- Internally, and transparently for the user, BaseContext manage a tree of the known classes with ClassNode.
			- BaseContext is derived from ClassNode, it Manages the virtual 'BaseObject', as a ClassNode.
			- It is possible to get the serialized size of the whole context at any moment with GetSerializedSize()
*/

class PackObjectReference;
class VirtualMachine;

class BaseContext : public ClassNode
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	friend class BaseObject;
	/*!
		\ingroup BaseContextDocGroup_InitMethods
		\brief	Constructor. 
	*/
	BaseContext(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_InitMethods
		\brief	Destructor. 
	*/
	virtual ~BaseContext(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup	BaseContextDocGroup_ObjectEditionMethods
		\brief	Faktory Method to create a new Object in the context. All object members
				should be inited to default, then use BaseObject's Set methods in editable mode to modify them.
				You must pass a valid Class descriptor and an _ObjectName, or nothing, in which case
				a name will be computed according to the class. If an object with the same name
				exists, then a new name is computed. get the final name with BaseObject::GetName();
				This method is only compiled in editable mode, in non-editable, Serialize_In() should construct everything.
		\param _classDescription the description struct of a class, like BaseObject::m_Description
		\param _ObjectName. the objectName then used to reference it can be NULL, then a new name is computed.
		\return The Object Created. 0L if class unknown or anywhat.
	*/
	BaseObject *NewObject(  const BaseObject::ClassDescription & _classDescription ,char *  _ObjectName=0L );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup	BaseContextDocGroup_ObjectEditionMethods
		\brief	In editable mode, it is possible to destroy an object from the context base:
				You got to use this method, not "delete".
		\param _objectToDelete The object to delete.
	*/
	void	DestroyObject( BaseObject * _objectToDelete );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup	BaseContextDocGroup_ObjectEditionMethods
		\brief	Really destroy all object from the context. It should result a clean
			context like it it was just constructed empty.
	*/
	void	DestroyAllObjects( );
#endif
	/*!
		\ingroup	BaseContextDocGroup_InputOutputMethods
		\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
				It serializes first the class name, the number of object instancied, then
				the number of sub-classnodes instancied, then all object instancied, then
				all subclasses recursively.
	*/
	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup	BaseContextDocGroup_InputOutputMethods
		\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
		\return	byte size of the serialisation to do.
	*/
	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup	BaseContextDocGroup_InputOutputMethods
		\brief	write the Current object definition to a Chunk using private packed types, recursively.
		\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
		\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
	*/
	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
	/*!
		\ingroup BaseContextDocGroup_InitMethods
		\brief	Link a VirtualMachine object to the Context, to provide
				a hardware/API abstraction that objects will use. 
				You must use this after the constructor and before serialization 
				or any edition. the context DOES NOT manage machine object destruction.
				so don't destroy it 'till context is destroyed.
		\param	_pMachine  the machine then used by the context
	*/
	virtual void SetMachine( VirtualMachine *_pMachine ){ m_pMachine =_pMachine;  };
	/*!
		\ingroup BaseContextDocGroup_InitMethods
		\brief	return the machine currently linked to the context.
		\param	_pMachine  the machine then used by the context
	*/
	inline VirtualMachine *GetMachine(){ return m_pMachine; };

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "BaseContext"; };
#endif
#ifdef _ENGINE_EDITABLE_

	/*!
		\ingroup BaseContextDocGroup_EventManagement
		\brief Used by BaseObjects "%Set()" member methods to notify what happened when one of 
				the member was edited, to change its value. It is used to manage the Undo/Redo capability.
				You should not use this method unless you create a PackSerializable base type and code it's Set() accessor.
		\param _UpDateType eVedaUpdate enum that identify the update info type.
		\param _pObjectEdited	BaseObject if _UpDateType is about an object or member.
		\param _ObjectMember	BaseType if _UpDateType is about a member.
		\param _PreviousShapeChunk	optionnal serialization of the previous state or 0L.
		\param _PreviousShapeChunkLength optionnal serialization of the previous state or 0L.
		\param _NewShapeChunk optionnal serialization of the new state or 0L.
		\param _NewShapeChunkLength optionnal serialization of the new state or 0L.
	*/
	void NotifyObjectChange( eVedaUpdate _UpDateType, BaseObject *_pObjectEdited, BaseType *_ObjectMember,
								const unsigned char *_PreviousShapeChunk=0L,
								unsigned int		 _PreviousShapeChunkLength=0L,
								const unsigned char *_NewShapeChunk=0L,
								unsigned int		 _NewShapeChunkLength=0L
								);
#endif
	/*!
		\brief used to scan the list of all object-to-object reference in this context.
				you can scan the list by then using PackObjectReference::GetPointerOnNextObjectReferenceInContextList()
		\return the first PackObjectReference.
	*/
	inline PackObjectReference		*GetFirstObjectReferenceInContextList(){ return m_pFirstObjectReferenceInContextList; };

#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_Undoing
		\brief	test if an Undo() is currently possible. Useful to enable or disable an
				Undo Function in an interface.
		\return true if it is.
	*/
	virtual bool CanUndo();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_Undoing
		\brief	Undo the last modification done on any object of the base, using a private "action history stack".
		\return true if the undo was "undone", false if no more undo available.
	*/
	virtual bool Undo();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_Undoing
		\brief	test if an Redo() is currently possible. Useful to enable or disable a
				Redo Function in an interface.
		\return true if it is.
	*/
	virtual bool CanRedo();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_Undoing
		\brief	Redo the last undo on any object of the base, using a private "action history stack".		
		\return true if the last undo was "redone", false if no more redo available.
	*/
	virtual bool Redo();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_Undoing
		\brief	Flush the Undo Stack: no undo or redo possible until next modification.
				Some edition events (object destruction) on the context need to completely 
		erase the Undo stack, because the event stack coherence is broken.
	*/
	void FlushUndoStack();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_Undoing
		\brief	The undo stack history can be less or more large. Default max is 0: no undo management.
		\param _maxnumber maximum number of undoable event.
	*/
	void SetMaximumNumberOfUndoableEvents( unsigned int _maxNumber );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_EventManagement
		\brief	You can specify one or more abstract GUI Views to update, by
		registering a function or a static method, that will be executed
		after each set() done on a member of a managed object, when editing, 
		and even for a undo/redoes. This function will receive a const description of what 
		was changed: the context concerned, the object concerned, and the object member concerned. 
		\param _func You must pass a function pointer on a static method pointer.
		\param _GUIObjectToUpdate a user object of your choice that will be returned for this function, or NULL.
	*/
	void AddGUIUpdateFunction(	void (*_func)(void *_GUIObjectToUpdateReturned,eVedaUpdate _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated) ,
								void *_GUIObjectToUpdate
								);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_EventManagement
		\brief	Remove an update function set by AddGUIUpdateFunction()
		\param _func the func pointer to remove.
		\return true if removed, false if unknown.
	*/
	bool RemoveGUIUpdateFunction(	void (*_func)(void *_GUIObjectToUpdateReturned,eVedaUpdate _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated)) ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_EventManagement
		\brief	send an update function to all registered GUIs.
		\param the update case.
		\param _ObjectUpdated the object updated in this context.
		\param _MemberUpdated the member of the object updated.
	*/
	void UpdateAllGUI( eVedaUpdate _updateType,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated);
#endif
	/*!
		\brief	link a new PackObjectReference to the context list. Done at object member inition.
		\param	_pObjectReferenceToLink the new reference to link. 
	*/
	void	LinkObjectReference( PackObjectReference *_pObjectReferenceToLink );
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	remove a PackObjectReference from the context list, at its deletion.
		\param	_pReferenceToUnlink ...
	*/
	void UnLinkObjectReference( PackObjectReference *_pReferenceToUnlink );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup	BaseContextDocGroup_ObjectEditionMethods
		\brief	Remove all objects that are not refered directly or indirectly by the object passed.
		\param	_pObjectWhichAllOtherShouldDependOn Object Which All Other Should Depend On.
	*/
	void DestroyAllIndependantObjects( BaseObject *_pObjectWhichAllOtherShouldDependOn );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup	BaseContextDocGroup_InputOutputMethods
		\brief	Acts like Serialize_In() but keep all previous objects.
				So you can load and mix 2 or more context this way.
		\param	_pDescriptionChunk the binary image of a context.
		\param _pendName specify a name to add at the end of each new object.
	*/
	virtual  const unsigned char * MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_pendName=0L);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_EventManagement
		\brief	Even in editable mode, only one thread owns a context and can
				edit, modify, use and destroy it. Other threads can just use 
				its objects and methods through a AddOneLockForThreadUse()
				RemoveOneLockForThreadUse() pair. If false is returned,
				it means the lock can't be done, and you should wait a bit and retry.
				This way, it avoids cases where an object is modified by the owner thread
				and used by another.(For Example, the sound management thread of the Machines use it.)
				Add/RemoveOneLockForThreadUse() must only be used by other threads.
				Don't use RemoveOneLockForThreadUse() if false is returned.
			
				Note: you can't use BaseObject::Create() on an object from another thread.
				But you have to test isCreated() before using object's method, like in the main thread:
				isCreated() ensures the object is coherently inited.
	*/
	virtual  bool AddOneLockForThreadUse();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup BaseContextDocGroup_EventManagement
		\brief	Even in editable mode, only one thread owns a context and can
				edit, modify, use and destroy it. Other threads can just use 
				its objects and methods through a AddOneLockForThreadUse()
				RemoveOneLockForThreadUse() pair. This is to avoid using an object
				which could be modified or destroyed by the owner thread.
				(it was made for sound management.)
	*/
	virtual  void RemoveOneLockForThreadUse();
#endif

#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup	BaseContextDocGroup_ObjectEditionMethods
		\brief	give to _ContextToModify the same shape as this.
				It means, the same classes are first registered to it, and then 
				the same objects are cloned into it.
		\param	_pContextToModify to object modified to shape this.
	*/
	void CloneClassesAndObjects(BaseContext *_pContextToModify);
#endif

#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup	BaseContextDocGroup_InputOutputMethods
		\brief	Export a .cpp file with 2 tables:
					- First, a table of all the class descriptors used by the context, and
					that can be used with RegisterClassList().
					- Then, the context serialization binary, standing as a unsigned char table.
				- This can be used for automatic executable generation by some implementations.
		\param	_filePath the path of the file to write. '.c' is added.
		\param	_pGobalTableName the name of the global variable in the .c file
		\param	_optimize if true, names are flushed and preferences destroyed, in a clone.
	*/
	void ExportAsCPPTableFile(const char *_pfileToExport,const char *_pGobalTableName,bool _optimize=true);
#endif

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! The context knows a list of all object-to-object references. This list is updated when the context is edited, and is used for reference resolution at input serialization.
	PackObjectReference		*m_pFirstObjectReferenceInContextList;
	//! The context knows or not a machine to use for hardware/API abstraction. This is not created or destroyed by the context.
	VirtualMachine			*m_pMachine;

#ifdef _ENGINE_EDITABLE_
	//! the nested class used by BaseContext::m_GUIUpdateFunctionList to manage a list of functions to send updates when the context is edited.
	class UpdateFunctionListCell : public PackStruct
	{ public:
		 void (*m_func)(void *_GUIObjectToUpdateReturned,eVedaUpdate _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated);
		 void *m_UserObject;
		 static BaseType *NewCell(){ return new UpdateFunctionListCell(); };
	};
	//! The list of GUI functions to update when editing this context:
	PackList			m_GUIUpdateFunctionList;
#endif

#ifdef _ENGINE_EDITABLE_
	//! the nested class used as cells of the undo stack:
	class UndoCell : public PackStruct
	{ 
	public:		
			UndoCell();
			void Init(unsigned int _editionType,BaseType *_pObjectMemberEdited,const unsigned char *_PreviousValueChunk,unsigned int _PreviousValueChunkLength,const unsigned char *_AfterValueChunk,unsigned int _AfterValueChunkLength);
			virtual ~UndoCell(void);
			inline BaseType	*GetMemberToChange(){ return m_pObjectMemberEdited;};
			inline unsigned char *GetPreviousValueChunk(){ return m_pPreviousValueChunk;};
			inline unsigned char *GetAfterValueChunk(){ return m_pAfterValueChunk;};
			static BaseType *NewCell(){ return new UndoCell(); };
	protected:
		unsigned int			m_EditionType; // only 0 managed for the moment: member change.
		BaseType	*m_pObjectMemberEdited;
		unsigned char			*m_pPreviousValueChunk;
		unsigned char			*m_pAfterValueChunk;
	};
	//! The stack of undoable/redoable events:
	PackList			m_UndoStack;
	//! the cell of m_UndoStack currently focused by  m_UndoStack;
	PackStruct::Cell	*m_pCurrentUndoCell;
	//! The current number of undo-able events:
	unsigned int			m_CurrentNumberOfUndoableEvents;
	//! The maximum number of undo-able events:
	unsigned int			m_MaximumNumberOfUndoableEvents;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Updates all reference members of the context so that the refered
		object 's serialization ID is took as serialized value.
	*/
	void UpdateAllReferencesSerializationID();
#endif
};

#endif
