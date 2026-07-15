/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackULong_Flags_H
#define COM_M4NKIND_PackULong_Flags_H

#include "PackULong.h"

/*!
	\class	PackULong_Flags
	\ingroup	BaseSerializableClass
	\brief	 PackULong_Flags specialize PackULong to define from 1 to 28 bit bitfield
			where each bit stands as a boolean value with a given meaning.
*/
// if we are in editable mode:
class PackULong_Flags : public PackULong
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
	*/
	PackULong_Flags(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackULong_Flags(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "PackULong::PackULong_Flags"; };
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
		\brief test if some flags are all switched on in in the value.
		\param _bitFlagsToTest bits to test.
	*/
	inline bool TestFlags( unsigned int _bitFlagsToTest ) const {return ((m_value&_bitFlagsToTest)!=0);};

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief add flags without changing the others.
		\param _FlagsToAdd flags to add.
	*/
	inline void AddFlags( unsigned int _FlagsToAdd ){ Set(Get()|_FlagsToAdd); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief remove flags without changing the others.
		\param _FlagsToRemove flags to remove.
	*/
	inline void RemoveFlags( unsigned int _FlagsToRemove ){Set( Get() &(~_FlagsToRemove) );};
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief set an infostring for the bitfield meaning.
		\param _meaning a string looking like: "bit0.bit1.bit2"
	*/
	virtual void SetBitFieldInfo( const char *_meaning );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief get an infostring for the bitfield meaning.
		\return a string looking like: "bit0.bit1.bit2"
	*/
	const char *GetBitFieldInfo( ){ return m_Bitfield_Meaning_InfoString; };
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:
#ifdef _ENGINE_EDITABLE_
	//! the BitFieldInfo string
	const char	*m_Bitfield_Meaning_InfoString;
#endif

};
#ifdef _ENGINE_EDITABLE_
/*!
	\def	REGISTER_MEMBER_FLAG

	\brief	This macro is used for member registration and to specify the meaning of each bit in editable mode:
*/
#define		REGISTER_MEMBER_FLAG(_object,_InfoString,_DefaultVal,_InfoBitFiled) \
REGISTER_MEMBER(_object,_InfoString)\
_object.Set(_DefaultVal);\
_object.SetBitFieldInfo( _InfoBitFiled ); 

#else
/*==================================================================
								WHEN NON EDITABLE:
==================================================================*/
/*!
	\def	REGISTER_MEMBER_FLAG

	\brief	This macro is used for member registration and to specify the meaning of each bit in editable mode:
*/

#define		REGISTER_MEMBER_FLAG(_object,_InfoString,_DefaultVal,_InfoBitFiled) \
REGISTER_MEMBER(_object,_InfoString)

// end of editable mode test :
#endif

// end of file:
#endif
