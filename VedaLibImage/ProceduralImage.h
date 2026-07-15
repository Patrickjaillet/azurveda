/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_ProceduralImage_H
#define COM_M4NKIND_ProceduralImage_H

#include "GreyImage2nSquareSized.h"
#include "PackULong_RandomSeed.h"
#include "PackLong_WithLimits.h"

/*!
	\class	ProceduralImage
	\ingroup BaseObjectInherited_VedaLibImage BaseObjectInheritedDocGroup
	\brief	Virtual Base class for procedural Images. It manages a random number,
			and serialize its editable random seed number, which defines the whole shape.
*/

class ProceduralImage : public GreyImage2nSquareSized
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	ProceduralImage(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(ProceduralImage);

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the random Seed that decide the form of the whole procedural generation:
	PackULong_RandomSeed	mSer_RandomSeed;

	//! a value defining the general recursion force of the algorythm used:
	PackLong_WithLimits		mSer_Recursion;
	/*!
		\brief	Method that really build the object using the serializable parameters.
				For the virtual ProceduralImage class, it reset a random seed number which defines a shape.
				the superclass VirtualImage allocates the image memory.
				Close() should close everything opened by Create().
	*/
	virtual bool CreateInternal(void);

};

#endif
