// SPDX-License-Identifier: LGPL-2.1-only

#include "Object3DMultiplier.h"
#include "VirtualEquation.h"
#include "ParticleSetVirtual.h"

BASEOBJECT_DECLARE_CLASS( "mltp", Object3DMultiplier, Object3DVirtual,"3D Object Particle Multiplier","ob3dmult","3D Object that Renders a shape many times according to a particle set." );

Object3DMultiplier::Object3DMultiplier() : Object3DVirtual()
{
	REGISTER_MEMBER_REFERENCE(	m_ShapeReference,"Shape",Object3DVirtual::m_Description );
	REGISTER_MEMBER_REFERENCE(	m_ParticleSetReference,"Particle Set",ParticleSetVirtual::m_Description );

}

void	Object3DMultiplier::RenderObject( double _frameDate,
										 VirtualMachine::InternalViewPort *_pViewPort,
										const float _rgba[4],
										 unsigned int _shapeIndex,
										 ShapeModifierList *_pModifierList)
{

	Object3DVirtual *pShape = (Object3DVirtual *)m_ShapeReference.GetObjectPointer();
	if(!pShape) return;
	ParticleSetVirtual *pParticles = (ParticleSetVirtual *)m_ParticleSetReference.GetObjectPointer();
	if( !pParticles )
	{
		pShape->RenderObject(_frameDate,_pViewPort,_rgba,_shapeIndex);
		return;
	}

	pParticles->SetFrameDate(_frameDate);

	unsigned int ii;
	const unsigned int nbparticle = pParticles->GetNumberOfParticle();

	for( ii=0 ; ii<nbparticle ; ii++ )
	{
		ParticleSetVirtual::Particle *pParticle = pParticles->Get(ii);
		_pViewPort->Matrix_Push();
		_pViewPort->Matrix_Translate(	pParticle->m_xyzd[0],
										pParticle->m_xyzd[1],
										pParticle->m_xyzd[2] );

		pShape->RenderObject(_frameDate,_pViewPort,_rgba,_shapeIndex);
		_pViewPort->Matrix_Pop();
	}

}
#ifdef _ENGINE_EDITABLE_

void Object3DMultiplier::ProcessPreview(
				double _frameDate,
				VirtualMachine::InternalViewPort *_pPreviewViewPort,
				const PreviewConfiguration *_pPreviewConfiguration
				)
{

	Object3DVirtual::ProcessPreview(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);

	RenderObject(_frameDate,_pPreviewViewPort,0);

}
#endif
