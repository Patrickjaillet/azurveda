// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_Object3DMultiplier_H
#define COM_M4NKIND_Object3DMultiplier_H

#include "PackObjectReference.h"
#include "Object3DVirtual.h"
#include "PackDynamicFloat.h"

class Object3DMultiplier : public Object3DVirtual
{

public:

	Object3DMultiplier(void);

	BASEOBJECT_DEFINE_CLASS(Object3DMultiplier);

	virtual void	RenderObject( double _frameDate,
									VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex=0,
									ShapeModifierList *_pModifierList=0L );

#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

protected:

	PackObjectReference	m_ShapeReference;

	PackObjectReference	m_ParticleSetReference;

};

#endif
