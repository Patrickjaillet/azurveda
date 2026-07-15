// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_Object3DMeshVirtual_H
#define COM_M4NKIND_Object3DMeshVirtual_H

#include "Object3DVirtual.h"
#include "PackFloatByte_Limits.h"
#include "PackList.h"
#include "VirtualMachine.h"

class Object3DMeshVirtual : public Object3DVirtual
{

public:

	Object3DMeshVirtual(void);

	BASEOBJECT_DEFINE_VIRTUALCLASS(Object3DMeshVirtual);
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

	virtual void	RenderObject( double _frameDate,
									VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex=0,
									ShapeModifierList *_pModifierList=0L );

protected:

	virtual void CreateShape(float _newShapeTime, unsigned int _shapeIndex=0);

	float		m_LastShapeTime;

	ShapeModifierList	*m_pLastShapeModifierList;

	typedef enum {

		OB3DFlag_Static=1,

		OB3DFlag_LookCam=2,

		OB3DFlag_PreRender32=4,

		OB3DFlag_PreRender128=8,

		OB3DFlag_OneColor=16,

		OB3DFlag_Object3DVirtual_LastFlag=32
	} OB3DBitFlag ;

	PackULong_Flags	mSer_Flags;

	PackObjectReference	mSer_TextureRef;

	class Object3DBufferHandler {
		public:

		Object3DBufferHandler();

		VirtualMachine::InternalObject3DBuffer	*m_pObject3DBuffer;

		unsigned int	m_ActiveNumberOfVertex;

		VirtualMachine::InternalTexture			*m_pRenderTexture;
	};

	Object3DBufferHandler			*m_pObject3DBufferTable;

	unsigned int	m_CurrentNumberOf3DBuffer;

	class BoundingBox {
	public:
		float	m_x1,m_x2,m_y1,m_y2,m_z1,m_z2;

		void	FindBoundingBox( VirtualMachine::InternalObject3DBuffer *_pOb3d );
	}  ;

	bool CreateInternal_RenderObjectToTexture(void);

	void CreateInternal_FreezeStaticShape(void);

#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif
};

#endif
