#ifndef	COM_M4NKIND_Object3DFontVirtual_H
#define COM_M4NKIND_Object3DFontVirtual_H

#include "Object3DMeshVirtual.h"
#include "PackFloatByte_Limits.h"
#include "PackObjectReference.h"
#include "PackList.h"
/*!
	\class	Object3DFontVirtual
	\ingroup	BaseObjectInherited_VedaLibDemo
	\brief	virtual base class for all collection of objects used as fonts
			for texts.
*/

class Object3DFontVirtual : public Object3DMeshVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Object3DFontVirtual(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(Object3DFontVirtual);
	/*!
		\brief	Return the 3D shape index of the object that fit one character.
				(then use index with RenderObject() )
				a character is given with a zero-ended string because we can support
				one byte character and more large char formats.
				If the char is not supportd, we return false.
				else,_rShapeIndex can return -1 if the char is space or tab.
		\param	_pString	zero ended char.
		\param	_rShapeIndex return index
		\param	_rCharWidth		the width of the char, to use them in the geometry.
	*/
	virtual bool GetShapeIndexForCharacter( const char *_pString,
											int &_rShapeIndex,
											float &_rCharWidth )=0;

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	class Shadow : public PackStruct
	{ public:
		// constructor:
		Shadow();
		//! texture to draw a back border:
		PackObjectReference		mSer_TextureReference;
		PackFloatByte_Limits	mSer_DecalX;
		PackFloatByte_Limits	mSer_DecalY;
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new  object.
	*/
	static BaseType *NewShadow();

	//!
	PackList	mSer_ShadowList;

};

#endif
