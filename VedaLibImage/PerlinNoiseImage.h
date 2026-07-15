/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PerlinNoiseImage_H
#define COM_M4NKIND_PerlinNoiseImage_H

#include "ProceduralImage.h"

/*!
	\class	PerlinNoiseImage
	\ingroup BaseObjectInherited_VedaLibImage BaseObjectInheritedDocGroup
	\brief	Class able to build a grey, 2n-pixelSized, perlin noise Image
*/

class PerlinNoiseImage : public ProceduralImage
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	PerlinNoiseImage(void);
	
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(PerlinNoiseImage);


/*==================================================================
								PROTECTED
==================================================================*/
protected:

/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
			It uses a faaast fractal-building perlin noise algorithm: all pixels
			are written Once, and 1 bitmap is used.
		\return false if creation failed.
*/
	virtual bool CreateInternal(void);
private:
	unsigned char GetPerlinMergeRandom( int vv, unsigned char uforce  );
};

#endif
