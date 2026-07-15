/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackULong_H
#define COM_M4NKIND_PackULong_H

#include "BaseType.h"

/*!
	\class	PackULong
	\ingroup	BaseSerializableClass
	\brief	 Base type managing "28 bit unsigned long", which serialize a number
			between [0,512Mb-1], which can be written on 1, 2,3 or 4 bytes when needed.
			It is used for the automated serialization managed by class PackStruct.
			It outputs an "unsigned int" you access with Get()
*/

class PackULong : public BaseType
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Constructor. 
	*/
	PackULong(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackULong(void);
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
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return("PackULong"); };
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
		\brief	change the value:
		\param	_value 28b unsigned int value.
	*/
	virtual void	Set(unsigned int _value); 
#endif
	/*!
		\brief	Get the value
		\return	the 28b unsigned int value.
	*/
	inline unsigned int	Get() const { return m_value ; };
	/*!
		\brief	change the value:
		\param	_value 28b unsigned int value.
	*/
	inline void	operator +=(unsigned int _value){ m_value +=  _value; };

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	/*!		the value of this class: 
			 Note that it will be trunked to 256Mb-1 maximum at serialization!
	*/
	unsigned int	m_value;
};

/*!
	\def	REGISTER_MEMBER_PACKULONG

	\brief	This macro is used to register a serializable member in a class constructor.
			For editable mode, it uses _InfoString to explicit the use of the member, in order
			to display it in a GUI for example.
		
*/
#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKULONG(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.Set(_DefaultVal);
#else
#define		REGISTER_MEMBER_PACKULONG(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);
#endif

// end of file:
#endif
