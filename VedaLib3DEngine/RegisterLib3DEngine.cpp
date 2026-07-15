/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "BaseContext.h"

// Other packages needed by 3DEngine:
#include "VirtualMedia.h"
#include "RegisterLibMath.h"
#include "RegisterLibImage.h"
// Lib3DEngine classes:
#include "Object3DVirtual.h"
#include "Object3DMeshVirtual.h"
#include "Object3DGrid.h"
#include "Object3DMesh3D.h"
#include "Object3DVirtualDynamic.h"
#include "Object3DLWOResource.h"
#include "Object3DMultiplier.h"
#include "ModifierObject3DVirtual.h"
#include "ModifierObject3DColor.h"
#include "ModifierObject3DSinusDisplace.h"
#include "Object3DScene.h"
#include "RenderTargetVirtual.h"
#include "RenderTargetTexture.h"
#include "RenderTargetRectangle.h"
#include "Media3DView.h"
#include "Texture3D.h"
#include "LightModel.h"
#include "ParticleSetVirtual.h"
#include "ParticleSetEquation.h"

void	RegisterLib3DEngine(BaseContext *_pContext )
{
	RegisterLibMath(_pContext);
	RegisterLibImage(_pContext);
	// register all classes that the context can manage:
	const BaseObject::ClassDescription * const descTable[]=
	{
		& VirtualMedia::m_Description,
			& Media3DView::m_Description,
		& Object3DVirtual::m_Description,
			& Object3DMeshVirtual::m_Description,
				& Object3DGrid::m_Description,
				& Object3DMesh3D::m_Description,
				& Object3DVirtualDynamic::m_Description, // used by LibDemo ATM.
				& Object3DLWOResource::m_Description,
				& Object3DMultiplier::m_Description,
			& Object3DScene::m_Description,

		& ModifierObject3DVirtual::m_Description,
			& ModifierObject3DColor::m_Description,
			& ModifierObject3DSinusDisplace::m_Description,

		& RenderTargetVirtual::m_Description,
			//re & RenderTargetTexture::m_Description,
			//re & RenderTargetRectangle::m_Description,

		& Texture3D::m_Description,
		& LightModel::m_Description,
		& ParticleSetVirtual::m_Description,
			& ParticleSetEquation::m_Description,
		0L
	};
	_pContext->RegisterClassList( descTable);
}
