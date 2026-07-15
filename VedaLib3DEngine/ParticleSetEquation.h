// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_ParticleSetEquation_H
#define COM_M4NKIND_ParticleSetEquation_H

#include "ParticleSetVirtual.h"
#include "PackLong_WithLimits.h"
#include "PackObjectReference.h"

class ParticleSetEquation : public ParticleSetVirtual
{

public:

	ParticleSetEquation(void);

	BASEOBJECT_DEFINE_CLASS(ParticleSetEquation);

	virtual void	SetFrameDate( float _frameDate );

protected:

	PackObjectReference		mSerRef_Equation;

#ifdef _ENGINE_EDITABLE_

#endif

};

#endif
