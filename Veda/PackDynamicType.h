/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackDynamicType_H
#define COM_M4NKIND_PackDynamicType_H

#include "PackStruct.h"
#include "PackULong_Enums.h"
class BaseObject;
/*!
	\class	PackDynamicType
	\ingroup	BaseSerializableClass
	\brief	 PackDynamicType serializes and manages an enumeration number (like PackULong_Enums) that
				define the shape of what is serialized with. the different shapes have to be registered
				through RegisterShapeConstructors() at construction, so it has to be extended to be active. 
				See exemple in VedaLibImage / PackDynamicFloat.
*/
class PackDynamicType : public PackStruct
{
/*==================================================================
								PUBLIC
==================================================================*/
public:

	/*!
		\brief	Constructor. 
	*/
	PackDynamicType(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackDynamicType(void);
#endif
	/*!
		\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
		\param	_pDescriptionChunk the objet description chunk. 
		\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
	*/
	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

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
				Note: it is a PackStruct, but is not declared as a packstruct, as it has only one of the members valid.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "PackDynamicType"; };
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
		\brief	Change the shape of this dynamic Type.
		\param	_shapeEnum an enum value, its meaning vary with registered types.
	*/
	void Set(unsigned int _shapeEnum );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	get the current effective type.
		\return	a BaseType. type is one of the registered shapes, or 0L.
	*/
	inline BaseType *Get(){ return m_pSelectedType; };
#endif

	/*!
		\brief	get the current effective type.
		\return	a BaseType. type is one of the registered shapes, or 0L.
	*/
	inline unsigned int GetSelectedIndex(){ return mSer_TypeEnum.Get(); };

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the enum that choose the active shape: 0 always means: "nothing".
	PackULong_Enums		mSer_TypeEnum;

	//! the object that currently really is the object. can be 0L:
	BaseType	*m_pSelectedType;

};

// end of file:
#endif
