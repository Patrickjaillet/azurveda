/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackString_MultipleLines_H
#define COM_M4NKIND_PackString_MultipleLines_H

#include "PackString.h"
#ifdef	_ENGINE_EDITABLE_
/*!
	\class	PackString_MultipleLines
	\ingroup	BaseSerializableClass
	\brief	 Extension to PackString to manages long texts with
				multiple lines.
				When used in non editable compilation mode, 
				it is cast as a PackString.

*/
class PackString_MultipleLines : public PackString
{
/*==================================================================
								PUBLIC
==================================================================*/
public:

	/*!
		\brief	Constructor. 
	*/
	PackString_MultipleLines(void);

	/*!
		\brief	Destructor. 
	*/
	virtual ~PackString_MultipleLines(void);

	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "PackString::PackString_MultipleLines"; };

/*==================================================================
								PROTECTED
==================================================================*/
protected:

};
#else
// non editable mode:
typedef PackString PackString_MultipleLines;
#endif
//end of file:
#endif
