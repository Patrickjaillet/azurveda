// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_Object3DVirtual_H
#define COM_M4NKIND_Object3DVirtual_H

#include "BaseObject.h"
#include "VirtualMachine.h"
#include "PackULong_Flags.h"
#include "PackObjectReference.h"
#include "PackStruct.h"
#include "PackList.h"
#include "PackDynamicFloat.h"

class Object3DVirtual : public BaseObject
{

public:

	Object3DVirtual(void);

	BASEOBJECT_DEFINE_VIRTUALCLASS(Object3DVirtual);

	class ShapeModifierList : public PackList
	{
	public:

		ShapeModifierList(void);

		void	ModifyVertexList(VirtualMachine::InternalVertex *_pFirstVertex,const unsigned int _NumberOfVertex,float _date);
	};

	virtual void	RenderObject( double _frameDate,
									VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex=0,
									ShapeModifierList *_pModifierList=0L )=0;
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

	class ShapeModifier : public PackStruct
	{
	public:

		ShapeModifier(void);

		PackObjectReference	mSer_ModifierReference;

		PackDynamicFloat	mSer_Value;
	};

	static BaseType *NewShapeModifier();

protected:

#ifdef _ENGINE_EDITABLE_

	void ProcessPreview_CreateLine(VirtualMachine::InternalObject3DBuffer **_ppVertexAndPolygonBufferOut, unsigned int _nbPoint=2);
#endif
#ifdef _ENGINE_EDITABLE_

	void ProcessPreview_DrawGrid(VirtualMachine::InternalViewPort *_pPreviewViewPort );
#endif
};

#endif
