#include "RegisterLibDemo.h"
#include "BaseContext.h"

#include "MediaAccess.h"
#include "VirtualMedia.h"
#include "MediaTimePattern.h"

#include "RegisterLibMath.h"
#include "RegisterLibImage.h"
#include "RegisterLib3DEngine.h"
#include "RegisterLibImageJPEG.h"

#include "RegisterLibSoundMP3.h"
#include "RegisterLibSoundXM.h"

#include "MarchinCubeSpace.h"
#include "TextLocalized.h"
#include "Object3DFontVirtual.h"
#include "Object3DFontArial.h"
#include "Object3DMarchCube.h"
#include "Object3DText.h"

#include "Equ_XYZD_Virtual.h"
#include "Equ_XYZD_Tore.h"
#include "Equ_XYZD_Particle.h"
#include "Equ_XYZD_ParticleCubes.h"
#include "Equ_XYZD_SinusWalls.h"

#include "Image2DEffectFire.h"

void	RegisterLibDemo(BaseContext *_pContext )
{

	RegisterLibMath(_pContext);
	RegisterLibImage(_pContext);
	RegisterLib3DEngine(_pContext);
	RegisterLibImageJPEG(_pContext);

	RegisterLibSoundMP3(_pContext);
	RegisterLibSoundXM(_pContext);
	const BaseObject::ClassDescription * const descTable[]=
	{

		& MediaAccess::m_Description,
		& VirtualMedia::m_Description,
			& MediaTimePattern::m_Description,

		& TextLocalized::m_Description,
		& MarchinCubeSpace::m_Description,
				& Object3DFontVirtual::m_Description,
					& Object3DFontArial::m_Description,
					& Object3DMarchCube::m_Description,
				& Object3DText::m_Description,
		& Equ_XYZD_Virtual::m_Description,
			& Equ_XYZD_Tore::m_Description,
			& Equ_XYZD_Particle::m_Description,
			& Equ_XYZD_ParticleCubes::m_Description,

		0L
	};
	_pContext->RegisterClassList( descTable );

}
