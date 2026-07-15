/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_NamedObject_H
#define COM_M4NKIND_NamedObject_H

#include "PackStruct.h"
// Serializable base types needed:
#include "PackString.h"


/*!
	\class	NamedObject

	\brief	Private use. Base type for all Chunk Named class: It is derived from PackStruct.
			Basically, all chunk starting with a jump-length, then a name, will use this.
			It is then derivated into ClassNode and BaseObject, which are different in their use.

*/

class NamedObject : public PackStruct
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
	*/
	NamedObject(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~NamedObject(void);
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
		\brief	the chunk length once serialized. should be valid just after in or out serialisation, but not if the object was modified. use GetSerializedDescriptionSize() instead.
		\return the chunk length
	*/
	inline unsigned int GetChunkLength(){ return(mSerUlong_ChunkLength.Get() );  };

	/*!
		\brief	Return the name of the object as a character string.
		\return the name.
	*/
	inline const char *  GetName(void) const{ return( mSerStr_ObjectName.Get() ); };

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	set the name of the object as a character string. This is copied of course.
		\param _pname teh new name
	*/
	void  SetName( const char * _pname );
#endif

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! all named objects, ClassNode and BaseObjects, has the ability to jump over the chunk without reading it:
	PackULong		mSerUlong_ChunkLength;
	/*! Serializable member that stands for the name of the object, and can be used to reference it
		in the context database. A name should not (and can't) be used twice in the whole context.
	*/
	PackString		mSerStr_ObjectName;

	//! an object can belong (or not) to a BaseContext, as an object.
	BaseContext				*m_pBaseContext;
	
};

#endif
