/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackString_H
#define COM_M4NKIND_PackString_H

#include "BaseType.h"

/*!
	\class	PackString
	\ingroup	BaseSerializableClass
	\brief	 Base type managing a zero-ended ascii string.
			It is used for the automated serialization.

*/

class PackString : public BaseType
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
#ifdef	_ENGINE_EDITABLE_
	/*!
		\brief	Constructor. 
	*/
	PackString(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackString(void);
#endif
	/*!
		\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
		\param	_pDescriptionChunk the objet description chunk. 
		\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
	*/
	virtual const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

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
	virtual const char *GetClassID() const { return "PackString"; };
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
		\brief	add a int value at the end of the current chain.
		\param	_value the integer value.
	*/
	void AddInt( int _value);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	apend a character string..
		\param	_pstr 
	*/
	void AddString( const char * _pstr);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief set the string value. It reallocates memory.
		  as it is used by serialisation, it returns a pointer on the read memory AFTER the zero for convenience.
			This "set" is exceptionnaly compiled in non-editable mode, because
			it is used by serialization.
		\param _pCharacterString the string to copy.
		\return a pointer after read memory.
	*/
	virtual	void Set( const char *  _pCharacterString );
#endif
	/*!
		\brief set the string value.
	*/
	inline const char *  Get() const {return( (char * ) m_StringBuffer );};

	/*!
		\brief compare a string. looks like strcmp().
		\param _pStringToCompare the string to compare to the value
		\param _pOtherStringToCompare the other string
		\return 0 if the same. case sensitive test.
	*/
	static int		Compare( const char *  _pStringToCompare, const char *  _pOtherStringToCompare );
	/*!
		\brief  like strlen()
		\return length, not including the last zero.
	*/
	inline unsigned int length(){ return length(Get())-1; };

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the string buffer.
	char	*m_StringBuffer;

#ifdef _ENGINE_EDITABLE_
	//! the re-allocation constant.
	static const unsigned int	m_GrowByteLength=64;
	//! the current m_StringBuffer used buffer. as it is zero ended, m_StringBuffer[m_UsedByteLength-1]==0
	unsigned int	m_UsedByteLength;
	//! current m_StringBuffer total allocation. is multiple of m_GrowByteLength.
	unsigned int	m_TotalByteLength;
#endif

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief delete if was allocated.
	*/
	virtual void Free();
#endif

	/*!
		\brief private tool. like strlen(), but with +1 for the zero end.
		\param _pString string.
		\return length+1 for the zero at the end.
	*/
	static unsigned int length( const char *_pString);
	/*!
		\brief private tool. like strcpy(), but with +1 for the zero end.
		as it is used by serialisation, it returns a pointer on the read memory AFTER the zero for convenience.
		\param _pStringToFill destination.
		\param _pSourceString source.
		\return a pointer after read memory
	*/
	static  const char * strcopy( char *_pStringToFill, const char *_pSourceString);
};

/*!
	\def	REGISTER_MEMBER_PACKSTRING

	\brief	This macro is used to register a serializable member in a class constructor.
			For editable mode, it uses _InfoString to explicit the use of the member, in order
			to display it in a GUI for example.
		
*/
#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKSTRING(_object,_InfoString) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );
#else
#define		REGISTER_MEMBER_PACKSTRING(_object,_InfoString) \
RegisterSerializableMember(_object);
#endif

//end of file:
#endif
