/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackFloatByte_Color_H
#define COM_M4NKIND_PackFloatByte_Color_H

#include "PackFloatByte_Limits.h"

#ifdef _ENGINE_EDITABLE_
// if we are in editable mode:
/*!
	\class	PackFloatByte_Color
	\ingroup	BaseSerializableClass
	\brief	It specify PackFloatByte_Limits to express a R,G,B color.
			 this specification is adressed with GetClassID() to interfaces.
			 in non-editable mode, it is PackFloatByte_Limits typedef.
*/
class PackFloatByte_Color : public PackFloatByte_Limits
{
/*==================================================================
								PUBLIC
==================================================================*/
public:

	/*!
		\brief	Constructor. 
	*/
	PackFloatByte_Color(void);

	/*!
		\brief	Constructor To initialize PackFloat as a multi dimentional vector. 
		\param _DimensionEnum on of the 3 VectorDimension enum.
	*/
	PackFloatByte_Color( VectorDimension _DimensionEnum );

	/*!
		\brief	Destructor. 
	*/
	virtual ~PackFloatByte_Color(void);

	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "PackFloat::PackFloatByte_Limits::PackFloatByte_Color"; };

/*==================================================================
								PROTECTED
==================================================================*/
protected:

};
#else
/*!
	\class	PackFloatByte_Color

	\brief	It specify PackFloatByte_Limits to express a R,G,B color.
			 this specification is adressed with GetClassID() to interfaces.
			 in non-editable mode, it is PackFloatByte_Limits typedef.
*/
typedef PackFloatByte_Limits PackFloatByte_Color;
#endif
// end of .h:
#endif
