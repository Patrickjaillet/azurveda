// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_Object3DScene_H
#define COM_M4NKIND_Object3DScene_H

#include "PackDynamicFloat.h"
#include "PackDynamicFloat.h"
#include "Object3DVirtual.h"
#include "VirtualEquation.h"
#include "PackTreeCell.h"
#include "PackULong_Enums.h"
#include "PackList.h"
#include "PackLong_WithLimits.h"

class Object3DScene : public Object3DVirtual
{

public:

	Object3DScene(void);

	BASEOBJECT_DEFINE_CLASS(Object3DScene);

	virtual void	RenderObject( double _frameDate,
									VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex=0,
									ShapeModifierList *_pModifierList=0L );

#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

	typedef enum {

		eOb3DChannel_Position=0,

		eOb3DChannel_Rotation,

		eOb3DChannel_Scale,

		eOb3DChannel_Color,

		eOb3DChannel_Transparency
	} eOb3DChannel ;

	class ChannelModifier : public PackStruct
	{
	public:

		ChannelModifier(void);

		PackULong_Enums		mSer_ChannelID;

		PackDynamicFloat	mSer_Value;
	};

	static BaseType *NewChannelModifier();

	class Object3DReference : public PackStruct
	{
	public:

		Object3DReference(void);

		PackObjectReference	mSer_Object3DReference;

		PackULong	mSer_Object3DLayerIndex;

		PackLong_WithLimits	mSer_BeginAtNumber;

		PackLong_WithLimits	mSer_RecursionNumber;

		PackList	mSer_ChannelModifierList;

		ShapeModifierList mSer_ShapeModifierList;
	};

	static BaseType *NewObject3DReference();

protected:

	PackTreeCell		mSer_ObjectTree;

	void	ProcessObject( PackTreeCell* _pCell,VirtualMachine::InternalViewPort *_pVp
							,const float _rgba[4]
							,double _frameDate );
};

#endif
