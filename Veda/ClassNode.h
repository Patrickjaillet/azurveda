/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_ClassNode_H
#define COM_M4NKIND_ClassNode_H

#include "NamedObject.h"
// serializable base type needed:
#include "PackString.h"
#include "BaseObject.h"
#include "PackList.h"

class BaseContext;
/*!
	\addtogroup	BaseContextDocGroup BaseContext Related Documentations
	\brief		Here are links to documentations related to class BaseContext. This class
				allows the management of a set of objects as an application context,
				and is provided with the base package.
				On this page, the methods are sorted by themes.
*/
/*! \addtogroup	BaseContextDocGroup_InitMethods	BaseContext Init Methods
	\ingroup	BaseContextDocGroup
	\brief		Init Methods for BaseContext
*/
/*! \addtogroup	BaseContextDocGroup_ObjectEditionMethods BaseContext Object Edition Methods
	\ingroup	BaseContextDocGroup
	\brief		The methods that allows to create Objects in a BaseContext.
				Then to edit objects, use the objects methods.
*/
/*! \addtogroup	BaseContextDocGroup_InputOutputMethods BaseContext Input & Output Methods
	\ingroup	BaseContextDocGroup
	\brief		Serialization methods. Also note BaseType::WriteFile() and
				BaseType::ReadFile() are valid  for %BaseContext.
*/
/*! \addtogroup	BaseContextDocGroup_EventManagement BaseContext Event Management Methods
	\ingroup	BaseContextDocGroup
	\brief		You can be notified of any change in the context with these Methods.
				This makes possible to build a complete user interface to edit the context.
*/
/*! \addtogroup	BaseContextDocGroup_Undoing BaseContext Undo & Redo  Methods
	\ingroup	BaseContextDocGroup
	\brief		%BaseContext provides an automatic management for all editable object members
				in the context. Active it with BaseContext::SetMaximumNumberOfUndoableEvents().
				Note: its is currently beta, because re-creation of destroyed objects
				or list elements by undo is not managed at the moment, So any deletion
				on a list will flush the undo stack.
*/

/*!
	\class	ClassNode

	\brief	 Class used privately by BaseContext to represent a class node in its
			database tree. This tree has the shape of the inheritance hierarchy.
			It manages a special hierarchic serialization which can re-create
			the object recursively. Basically, there is 1 classnode by class and context:
			It is initialized using a static BaseObject::ClassDescription, when using 
			BaseContext::RegisterClassList().			
			Again: It IS NOT a class that is instancied to make BaseObject objects, it just indicates
			the class in the class hierarchy of BaseContext, and BaseObjects are under it in a list.
			It is also used for the context database access, and management.
*/
class ClassNode : public NamedObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor.  Need a BaseObject::m_Descriptor static member.
		\parm _classDescription	The static Class description structure.
	*/
	ClassNode( BaseObject::ClassDescription const &_classDescription  );
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~ClassNode(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup	BaseContextDocGroup_ObjectEditionMethods
		\brief	Declare all objects as if each were used with BaseObject::Close().
				This can be used to force a re-creation of the whole context.
	*/
	void	CloseAllObjects( );
#endif

	/*!
		\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
				It serializes first the class name, the number of object instancied, then
				then all object instancied, then all subclasses recursively.
	*/
	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
		\return	byte size of the serialisation to do.
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

	/*!
		\brief	Find an Object instancied from that class or another subclass.
				It is needed in non-editable mode to get a given object in the context base.
		\param _ObjectName the name of the object to get.
		\return the object or 0L if not found.
	*/
	BaseObject *GetBaseObject(const char *  _ObjectName);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Factory Method to create a new Object in the context. All object members
				should be inited to default, then use BaseObject's Set methods in editable mode.
				This method is used in editable mode, but in serialize_in only mode, it does
				not compute a new name.
		\param _ObjectName. the objectName then used to reference it. computed if NULL.
		\return The Object Created. 0L if class unknown or anywhat.
	*/
	BaseObject *NewObject( const char *  _ObjectName=0L );
#endif
	/*!
		\ingroup BaseContextDocGroup_InitMethods
		\brief	Register a 0-ended list of static class descriptors ( BaseObject::ClassDescription )
			from objects inherited from BaseObject, to the context, down the class tree. 
			It is used by to register the classes in a context, before any serialization or context edition. 
			IMPORTANT: You must register the classes in an order so that the inherited classes are set
			<b>after their parents</b>. BaseContext registers BaseObject itself in its constructor.
			It can look like:
			\code
	const BaseObject::ClassDescription * const descTable[]=
	{	 
		// standard class from VedaLib:
		& VirtualMedia::m_Description,	
			& MediaTimePattern::m_Description, // class built over VirtualMedia.
		& MyClassInheritingBaseObject::m_Description,	
		0L
	};
	BaseContext	myContext;
	myContext.RegisterClassList(descTable);
			\endcode
			In editable mode, you can check all class has been serialized
			with CheckClassListError()
		\param _ppclassDescriptionList BaseObject ( and inherited class ) 's 0-ended class descriptor list.
	*/
	void	RegisterClassList( const BaseObject::ClassDescription * const * _ppclassDescriptionList );

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	In editable mode, it is possible to destroy an object from the context base:
				This should be only used privately by BaseContext.
				This version can only destroy objects managed by this ClassNode.
				This method is only compiled in editable mode.
		\param _objectToDelete The object to delete.
	*/
	void	DestroyManagedObject( BaseObject * _objectToDelete );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  return an explicit name for the class the object is instancied.
		\return a const character string.
	*/
	virtual const char *GetDisplayClassName() const { return ( m_ClassDescription->m_DisplayClassName ); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  return the default name that is set to new objects if not specified.
		\return a const character string.
	*/
	virtual const char *GetDefaultObjectName() const { return ( m_ClassDescription->m_DefaultObjectName ); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  return an explicit name for the class the object is instancied.
		\return a const character string.
	*/
	virtual const char *GetClassHelpDescription() const { return ( m_ClassDescription->m_ClassHelpDescription ); };
#endif

	/*!
		\brief	Find a Son ClassNode by its static description. It looks recursively in all branches of the context.
		\param _classDescription the descripter.
		\return the classnode or 0L if not found.
	*/
	ClassNode *GetSubClassNodeByDescription( const BaseObject::ClassDescription * _classDescription);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	get the first son classnode if not a leaf class, else NULL.Used by editors.
		\return the classnode or 0L if not found.
	*/
	 const  ClassNode *GetFirstSonClassNode() const { return m_pFirstSonClassNode; } ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	get the next brother classnode. If we are the last broteher, NULL.Used by editors.
		\return the classnode or 0L if not found.
	*/
	 const ClassNode *GetNextBrotherClassNode() const { return m_pNextBrotherClassNode; } ;
#endif
	/*!
		\brief	get the managed object list, in a read only way: you can get the object, edit the
			objects, but can't edit the list. Used by editors.
		\return the classnode or 0L if not found.
	*/
	 inline const  PackList *GetObjectList() const { return &m_BaseObjectList; } ;

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	false if class is able to create objects, true if virtual.
		\return true or false
	*/
	 bool IsAbstract() const { return ( 0L == m_ClassDescription->m_NewMethod); } ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	return true if this class has at least one object created, or got an inherited class that has
				at least an object created. false if it is not currently used.
				this method is used by serialize_out() to check if a class should be written.
		\return true or false
	*/
	 bool IsUseful() const ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Acts like Serialize_In() but keep all previous objects.
				So you can load and mix 2 or more context this way.
		\param	_pDescriptionChunk the binary image of a context.
		\param _pendName specify a name to add at the end of each new object.
	*/
	virtual  const unsigned char * MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_pendName=0L);
#endif

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	reset a new unique index for PackObjectReference serialization. It does recursively in all branches of the context.
			This reference index is only valid during serialization. 1 is the minimum
		\param _FirstnumberToSet the first number for the first oject managed by this ClassNode.
		\param  _increment the number that is added to get the next number.
		\return the next available index.
	*/
	unsigned int SetObjectsUniqueReferenceIndex( unsigned int _FirstnumberToSet=1,int _increment=1 );
#endif

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Set all object managed's name to "" (empty string) in order to same memory in the context. It is recursive to other sub classnodes.
	*/
	virtual	void FlushNames();
#endif

	/*!
		\brief	Find an Object instancied from that class or another subclass by its serialized index. Should only be used by BaseContext::Serialize_In.
		\param _index special number
		\return the object or 0L if not found.
	*/
	BaseObject *GetBaseObjectBySerializedIndex( unsigned int _index);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return ("ClassNode" ); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	tool for BaseContext::ExportAsCPPTableFile() , recursive write of
				a .cpp definition of a class description.
		\param	_fileTextImage
	*/
	void ExportCPPClassDescription( PackString &_fileTextImage ) const ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	tool for BaseContext::ExportAsCPPTableFile() , recursive write of
				a .cpp definition of a class description.
		\param	_fileTextImage	the source where a line is added.
	*/
	void ExportCPPClassDescriptionLine( PackString &_fileTextImage ) const;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	If RegisterClassList() detected something wrong about
				the class list, the faulty class is reported here.
				From now on, the only possible error is: 2 class
				have the same serializer ID, on the same hierarchy level.
		\return the faulty class, or 0 if OK.
	*/
	const BaseObject::ClassDescription *CheckClassListError() const;
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:

  //! reference to the static class descriptor. This classNode manages this class in this context.
	const BaseObject::ClassDescription * m_ClassDescription;
#ifdef _ENGINE_EDITABLE_
	//! Serializable member that stands for the number of objects instancied.
	unsigned int	m_NumberOfObjectInstancied;
#endif
#ifdef _ENGINE_EDITABLE_
	//!	\brief an enum used by serialization.
	typedef enum{
		eSerError_OK=0,
		eSerError_BadChunkHead
	} eSerializationError ;
	//! this explicit the state after the last Serialize_In() done.
	eSerializationError		m_SerializationState;
#endif
	//! object that handle the object list: it is serialized without registration
	PackList		m_BaseObjectList;

	//! pointer to Next class which inherit from the same upper class, or NULL (if it ends the list) .
	ClassNode		*m_pNextBrotherClassNode;

	//! pointer to the First classNode which class managed inherit from the class managed here, or NULL.
	ClassNode		*m_pFirstSonClassNode;
#ifdef _ENGINE_EDITABLE_
	// note: defined here for BaseContext.h, to handle lock in some methods
	//! Thread management: lock from the thread editing objects:
	volatile bool	m_OwnerThreadIsCurrentlyManagingObjects;
	//! test debug member, to check the lock state of a context if a crash occurs because of a bad lock.
	volatile int	m_TESTDEBUG_CONTEXTLOCK_ID;
	//! Thread management: number of threads currently using objects. This should come back to zero most of the times.
	volatile unsigned int	m_NumberOfThreadsCurrentlyLockingTheContext;
#endif
#ifdef _ENGINE_EDITABLE_
	//! if a faulty class descriptor was registered, report it: It's a developper debugger feature.
	const BaseObject::ClassDescription *m_pLastClassDescWithFaultySerializer;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destroy all managed object. Used to clean up the whole context.
	*/
	void DestroyAllManagedObjects();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destroy all managed object, and all object of all subclasses recursively.
	*/
	void ProtectedDestroyAllObjects();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Remove all objects that are not refered directly or indirectly by the object passed.
		\param	_pObjectWhichAllOtherShouldDependOn Object Which All Other Should Depend On.
	*/
	void ProtectedDestroyAllIndependantObjects( BaseObject *_pObjectWhichAllOtherShouldDependOn );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	register management of the same classes to another context.
				(and so create another ClassNodes to it.)
				Used by BaseContext::CloneClassesAndObjects().
	*/
	void ProtectedAddSameClasses( BaseContext *_pContextToAddManagement );
#endif

};

#endif
