#ifndef	COM_M4NKIND_Object3DFontArial_H
#define COM_M4NKIND_Object3DFontArial_H

#include "Object3DFontVirtual.h"
#include "PackFloatByte_Limits.h"
#include "PackULong_Flags.h"
#include "PackList.h"
/*!
	\class	Object3DFontArial
	\ingroup	BaseObjectInherited_VedaLibDemo
	\brief	Define a list of characters with 3D objects.
			This is a fast done font with a static shape.
*/

class Object3DFontArial : public Object3DFontVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Object3DFontArial(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Object3DFontArial);
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
											float &_rCharWidth );

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! flags for all polygons
	//PackULong_Flags	mSer_Flags;   re-use and extend the flag member of VirtualObject.

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Closes everything.
	*/
	virtual void	CloseInternal(void);
#endif
	//! horrible static base of the font shape.
	static const signed char m_fontc[];

	//! \brief type for m_pCharTable .
	typedef struct {
		signed int	m_ShapeIndex;
		float		m_CharWidth;
	} FontBase ;
	//! precalculate a table with shape index.
	FontBase	*m_pCharTable;


};

#endif
