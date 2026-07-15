/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackULong_Enums_H
#define COM_M4NKIND_PackULong_Enums_H

#include "PackULong.h"

#ifdef _ENGINE_EDITABLE_
// if we are in editable mode:
/*!
	\class	PackULong_Enums
	\ingroup	BaseSerializableClass
	\brief	 PackULong_Enums specialize PackULong to define from 1 to 28 bit bitfield
			where each bit stands as a boolean value with a given meaning.
*/
class PackULong_Enums : public PackULong
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
	*/
	PackULong_Enums(void);
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackULong_Enums(void);
	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "PackULong::PackULong_Enums"; };
	/*!
		\brief	convert the value of this object to an explicit string. The object manages the string privately,
			so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			 Note: this is not virtual, but each class can manage m_pValueString or not.
		\return	the value as a const string. 
	*/
	virtual const char	*ValueToString();
	/*!
		\brief set an infostring for the bitfield meaning.
		\param _meaning a string looking like: "enum0.enum1.enum2" for each known values
	*/
	virtual void SetEnumsInfo( const char *_meaning );
	/*!
		\brief get an infostring for the enums meaning.
		\return a string looking like:  "enum0.enum1.enum2" for each known values
	*/
	const char *GetEnumsInfo( ){ return m_Enums_Meaning_InfoString; };

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the BitFieldInfo string
	const char	*m_Enums_Meaning_InfoString;

};
/*!
	\def	REGISTER_MEMBER_ENUMS

	\brief	This macro is used for member registration and to specify the enum list
*/
#define		REGISTER_MEMBER_ENUMS(_object,_InfoString,_DefaultVal,_InfoEnumsList) \
REGISTER_MEMBER(_object,_InfoString)\
_object.Set(_DefaultVal);\
_object.SetEnumsInfo( _InfoEnumsList ); 

#else
/*==================================================================
								WHEN NON EDITABLE:
==================================================================*/
// if we are in non-editable mode:
/*!
	\class	PackULong_Enums

	\brief	 PackULong_Enums specialize PackULong to define from 1 to 28 bit bitfield
			where each bit stands as a boolean value with a given meaning.
*/
typedef PackULong PackULong_Enums;
/*!
	\def	REGISTER_MEMBER_ENUMS

	\brief	This macro is used for member registration and to specify the enum list
*/

#define		REGISTER_MEMBER_ENUMS(_object,_InfoString,_DefaultVal,_InfoEnumsList) \
REGISTER_MEMBER(_object,_InfoString)

// end of editable mode test :
#endif

// end of file:
#endif
