// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_Object3DGrid_H
#define COM_M4NKIND_Object3DGrid_H
#include "Object3DMeshVirtual.h"

#include "PackFloatByte_Limits.h"
#include "PackDynamicType.h"
#include "PackLong_WithLimits.h"
#include "PackULong_Flags.h"
#include "PackTreeCell.h"

class Object3DGrid : public Object3DMeshVirtual
{

public:

	Object3DGrid(void);

	BASEOBJECT_DEFINE_CLASS(Object3DGrid);

protected:

	PackLong_WithLimits	mSer_SubdivisionX;

	PackLong_WithLimits	mSer_SubdivisionY;

	PackFloat	mSer_DimensionXY;

	PackFloat	mSer_MappingUV1;

	PackFloat	mSer_MappingUV2;

	virtual bool CreateInternal(void);

	virtual void CreateShape(float _newShapeTime, unsigned int _shapeIndex=0);
};

#endif
