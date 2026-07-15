/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "Object3DMultiplier.h"
#include "VirtualEquation.h"
#include "ParticleSetVirtual.h"

BASEOBJECT_DECLARE_CLASS( "mltp", Object3DMultiplier, Object3DVirtual,"3D Object Particle Multiplier","ob3dmult","3D Object that Renders a shape many times according to a particle set." );

Object3DMultiplier::Object3DMultiplier() : Object3DVirtual()
{
	REGISTER_MEMBER_REFERENCE(	m_ShapeReference,"Shape",Object3DVirtual::m_Description );
	REGISTER_MEMBER_REFERENCE(	m_ParticleSetReference,"Particle Set",ParticleSetVirtual::m_Description );

}
/*!
	\brief	Process a real time computing and drawing. Can be extended.
	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pViewPort	where to render.
	\param	_shapeIndex	optional index for multiple shape objects. def to zero.
*/
void	Object3DMultiplier::RenderObject( double _frameDate, 
										 VirtualMachine::InternalViewPort *_pViewPort,
										const float _rgba[4],										 
										 unsigned int _shapeIndex,
										 ShapeModifierList *_pModifierList)
{
	// note: using RenderObject(), the matrix has a current state in _pViewPort,
	// including camera position and anything.
	//ProcessObject( &mSer_ObjectTree,_pViewPort,_frameDate );

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
/*!
	\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. This is done with this method. sub classes can implement it (or not) in
			any way, to explicit current shape of an object.<br>

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
	\param	_pPreviewConfiguration
*/
void Object3DMultiplier::ProcessPreview(
				double _frameDate,
				VirtualMachine::InternalViewPort *_pPreviewViewPort,
				const PreviewConfiguration *_pPreviewConfiguration
				)
{
	// draw grid:
	Object3DVirtual::ProcessPreview(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);
	// draw ob:
		// then draw our objects:
//	if(m_CurrentNumberOf3DBuffer==0) return;
//	unsigned int shapeIndex = ((unsigned int)_frameDate) % m_CurrentNumberOf3DBuffer;
	RenderObject(_frameDate,_pPreviewViewPort,0);

}
#endif