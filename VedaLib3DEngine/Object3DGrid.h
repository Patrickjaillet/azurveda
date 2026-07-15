/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_Object3DGrid_H
#define COM_M4NKIND_Object3DGrid_H
#include "Object3DMeshVirtual.h"

#include "PackFloatByte_Limits.h"
#include "PackDynamicType.h"
#include "PackLong_WithLimits.h"
#include "PackULong_Flags.h"
#include "PackTreeCell.h"
/*!
	\class	Object3DGrid
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Define an object with a list of vertex and polygons.
*/

class Object3DGrid : public Object3DMeshVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Object3DGrid(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Object3DGrid);

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! subdivision.
	PackLong_WithLimits	mSer_SubdivisionX;
	//! subdivision.
	PackLong_WithLimits	mSer_SubdivisionY;
	//! space dimension.
	PackFloat	mSer_DimensionXY;
	//! coord yx mapping
	PackFloat	mSer_MappingUV1;
	//! coord yx mapping
	PackFloat	mSer_MappingUV2;

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	virtual bool CreateInternal(void);
	/*!
		\brief	update the shape of the object for this date, during
				the lifetime the object, when it is created.
				by defdault, does nothing, so static object can be created
				with CreateInternal() only.
		\param	_newShapeTime the new date.
		\param	_shapeIndex the index of the shape to modify.
	*/
	virtual void CreateShape(float _newShapeTime, unsigned int _shapeIndex=0);
};

#endif
