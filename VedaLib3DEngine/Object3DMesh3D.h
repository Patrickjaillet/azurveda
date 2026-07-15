// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_Object3DMesh3D_H
#define COM_M4NKIND_Object3DMesh3D_H
#include "Object3DMeshVirtual.h"
#include "PackFloatByte_Limits.h"
#include "PackList.h"

#include "PackULong_Flags.h"
#include "PackLong_WithLimits.h"

class Object3DMesh3D : public Object3DMeshVirtual
{

public:

	Object3DMesh3D(void);

	BASEOBJECT_DEFINE_CLASS(Object3DMesh3D);

	typedef enum {
		ePMode_Strips=1,
	} ePolygonModeFlags ;

	class PolygonList : public PackStruct
	{
	public:

		PolygonList();

		PackULong_Flags		mSer_Flags;

		PackLong_WithLimits	mSer_VertPerPoly;

		PackList		mSer_RefList;
	};

	static BaseType *NewPackULong();

	static BaseType *NewPolygonList();

#ifdef _ENGINE_EDITABLE_

	typedef enum {
		tm_Object3DMesh3D_SetAsCube=tm_BaseObject_FirstInheritageToolMethod,
		tm_Object3DMesh3D_LoadLWO,
		tm_Object3DMesh3D_FirstInheritageToolMethod,
	};

	virtual void ExecuteToolMethod( unsigned int _MethodIDToExecute ) ;
#endif
#ifdef _ENGINE_EDITABLE_

	void ToolMethod_SetAsCube() ;
#endif
#ifdef _ENGINE_EDITABLE_

	void ToolMethod_LoadLWO() ;
#endif

protected:

	static BaseType *NewPackFloatByte_Limits();

	PackFloatByte_Limits	mSer_NormalSplit;

	PackFloatByte_Limits	mSer_Pivot;

	PackFloat	mSer_Scale;

	PackLong_WithLimits	mSer_Subdivision;

	PackFloatByte_Limits	mSer_SplineFactor;

	PackList		mSer_StaticVertexList;

	PackList		mSer_PolygonList;

	virtual bool CreateInternal(void);

	virtual void CreateShape(float _newShapeTime, unsigned int _shapeIndex=0);
};

#endif
