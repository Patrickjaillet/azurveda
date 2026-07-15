#include "RegisterLibDemo.h"
#include "BaseContext.h"
// AzurVeda Base class needed by the context:
#include "MediaAccess.h"
#include "VirtualMedia.h"
#include "MediaTimePattern.h"
// Other packages needed by the context:
#include "RegisterLibMath.h"
#include "RegisterLibImage.h"
#include "RegisterLib3DEngine.h"
#include "RegisterLibImageJPEG.h"
//re #include "RegisterLibBAOOM.h"
#include "RegisterLibSoundMP3.h"
#include "RegisterLibSoundXM.h"
// libdemo:
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
	// register all classes that the context can manage:
	RegisterLibMath(_pContext);
	RegisterLibImage(_pContext);
	RegisterLib3DEngine(_pContext);
	RegisterLibImageJPEG(_pContext);
//re	RegisterLibBAOOM(_pContext);
	RegisterLibSoundMP3(_pContext);
	RegisterLibSoundXM(_pContext);
	const BaseObject::ClassDescription * const descTable[]=
	{	 
		// standard class from VedaLib:
		& MediaAccess::m_Description,
		& VirtualMedia::m_Description,
			& MediaTimePattern::m_Description,
		// LibDemo
		& TextLocalized::m_Description,
		& MarchinCubeSpace::m_Description,		
				& Object3DFontVirtual::m_Description,
					& Object3DFontArial::m_Description,
					& Object3DMarchCube::m_Description,
				& Object3DText::m_Description,
		& Equ_XYZD_Virtual::m_Description,
			& Equ_XYZD_Tore::m_Description, // extented from math/VirtualEquation
			& Equ_XYZD_Particle::m_Description,
			& Equ_XYZD_ParticleCubes::m_Description,
//re			& Equ_XYZD_SinusWalls::m_Description,
		0L
	};
	_pContext->RegisterClassList( descTable );


}

