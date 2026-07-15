/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackObjectReference_H
#define COM_M4NKIND_PackObjectReference_H

// serializable base type needed:
#include "PackULong.h"
#include "PackString.h"
#include "BaseObject.h"

/*!
	\class	PackObjectReference
	\ingroup	BaseSerializableClass
	\brief	 Base type managing a serializable reference to another object in the same context.
			a BaseObject can use a %PackObjectReference as a member, and when registering it,
			you specify a pointable base class. Then the %PackObjectReference will be able
			to point any object in the context that match this pointable class, or its inherited classes.\n
			You get the pointer with GetObjectPointer(). Getting NULL is valid, and simply means that nothing is pointed.
			You change the pointer with SetObjectPointer(), and you should check its enum_SetReferenceResult errorcode.\n
			 Please, don't stock a pointer from GetObjectPointer() elsewhere between
			 two functions, because %PackObjectReference are dynamic pointers, that can
			 change or become NULL during edition.
			 Registering a %PackObjectReference member in a BaseObject constructor 
			 can look like this:
			 \code
#include "myClass.h"
#include "VirtualImage.h"
myClass::myClass() : BaseObject()
{
	REGISTER_MEMBER_REFERENCE(	m_MyImagePointer,"the Reference",VirtualImage::m_Description );
}
			 \endcode
			 ...And then, inside your class code, you will get the image like this:
			 \code
void	myClass::myMethodThatUsesImages(void)
{
	// PackObjectReference ensures that the pointer can be casted to that class:
	VirtualImage *pImage = (VirtualImage *) m_MyImagePointer.GetObjectPointer();
	if(pImage != NULL)
	{
		// you can use pImage here.
	}
}
	\endcode
*/


class PackObjectReference : public PackULong
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. Each PackObjectReference refers to an object instancied
			from a given class, or from an inherited class. 
	*/
	PackObjectReference();
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackObjectReference(void);
#endif
	/*!
		\brief	Get the object refered. If it exists and has a right class type,
				Get will return the object. Else, Get would return NULL. You should get
				your objects one time by method, and never stock what you get, because
				it could be destroyed or closed between two method calls.
				\return the object, or NULL if reference set to NULL.
	*/
	inline BaseObject *GetObjectPointer() const { return m_pReferedObject;  };
#ifdef _ENGINE_EDITABLE_
	//! \typedef enum_SetReferenceResult
	//!	Result enum returned by SetObjectPointer()
	typedef enum {
		//! means the object is now pointed.
		eSetRef_SetOK=0,
		//! means the bug is in the application code, contact developers! (note; it really never hapended)
		eSetRef_BadReferenceInit_ShouldntHappen,
		//! you tried to link an object which is not from this context !
		eSetRef_ObjectFromOtherContext,
		//! each PackObjectReference can only point a given class or inheritage, and the object doesn't match that class.
		eSetRef_ObjectClassDoesntMatch,
		//! the link would create a circular dependance in the context. You can't do that !
		eSetRef_CantReferObjectThatReferThisOne
	} enum_SetReferenceResult;
	/*!
		\brief	Set the pointed object, from the same context. If it exists and has a right class type,
				It will be set as the refered object, and
				GetObjectPointer() will return the object. Else, it would return NULL.
				This Method is only compiled in editable mode. 
				In non editable mode, a shorter version is compiled.
			\param _Object the object to refer from now on.
			\return eSetRef_SetOK if OK, otherwise an error enum.
	*/
	virtual	enum_SetReferenceResult	SetObjectPointer( BaseObject * _Object );
#else
	/*!
		\brief	Set the object refered, non-editable version, done by BaseContext::Serialization_in()
		\param _Object the object to refer from now on.
	*/
	inline void	SetObjectPointer( BaseObject * _Object ){ m_pReferedObject = _Object ; };
#endif

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Set the kind of class of object that we will be able to refer. Is only possible in editable mode.
				In read-only mode, the referable type is implicit. It can only be done once.
	*/
	void	SetReferableBaseClass( const BaseObject::ClassDescription &_classDescription  );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Get the kind of class of object that we will be able to refer. Is only possible in editable mode.
				In read-only mode, the referable type is implicit.
	*/
	 const BaseObject::ClassDescription &GetReferableBaseClass();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
		\return	byte size of the serialisation to do.(1,2,3, or4)
	*/
	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	write the Current object definition to a Chunk using private packed types, recursively.
		\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
		\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
	*/
	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "PackObjectReference"; };
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
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Used to scan the list of all reference to an object (which start is in BaseObject):
		\return the next in the list or 0L if it ends.
	*/
	inline PackObjectReference * GetNextSelfReferenceNode(){ return m_pNextSelfReferenceNode; };
#endif

	/*!
		\brief	Set the kind of class of object that we will be able to refer. Is only possible in editable mode.
				In read-only mode, the referable type is implicit. It can only be done once.
		\return pointer on a pointer
	*/
	inline PackObjectReference	**GetPointerOnNextObjectReferenceInContextList(){ return( &m_pNextObjectReferenceInContextList ); };

	/*!
		\brief	The Object this one belong to as a member. Should only be used at init.
		\param _pManager	BaseObject
	*/
	virtual	void	SetObjectThatManagesThis(BaseObject *_pManager);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Update Reference To Unique Index. 
			It is used by context serialization
	*/
	inline void	UpdateSerializedID(void)
	{
		if( m_pReferedObject ) m_value = m_pReferedObject->GetUniqueReference() ;
		else m_value = 0 ;
	};
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	
	//! the objects refered. If destroyed in editablemode, become null.
	BaseObject		*m_pReferedObject;

#ifdef _ENGINE_EDITABLE_

	//! copy of the object name referenced. It is not serialized.
	//PackString		m_referencedObjectName;
	/*! The name of the class of an object this reference can refer. Should never be null.
		Note: it is took and pointed to an BaseObject static member, not copied in any way. It shoud be OK.
	*/
	const BaseObject::ClassDescription *m_ReferableclassDescription;

	//! in editable mode, chain list which start is in BaseObject, and list all the reference to the object.
	PackObjectReference	*m_pNextSelfReferenceNode;

#endif
	//! chain list which start is in BaseContext, and list all the references to the object for the whole context.
	PackObjectReference	*m_pNextObjectReferenceInContextList;
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	protected. Declare that the previous reference state is closed. equivalent to Set(0L);
	*/
	void	CloseReference();
#endif
};
#ifdef _ENGINE_EDITABLE_
/*!
	\def	REGISTER_MEMBER_REFERENCE
	\brief	This macro is used to register a serializable PackObjectReference member
			and specify the base class a reference can ask. It must be used in BaseObject's constructor,
			never elsewhere.
			It does compile less things in non-editable mode.
*/
#define		REGISTER_MEMBER_REFERENCE(_object,_InfoString,_classDescriptor) \
REGISTER_MEMBER(_object,_InfoString);\
_object.SetReferableBaseClass(_classDescriptor);
#else
#define		REGISTER_MEMBER_REFERENCE(_object,_InfoString,_classDescriptor) \
REGISTER_MEMBER(_object,_InfoString);
#endif
//end of file:
#endif
