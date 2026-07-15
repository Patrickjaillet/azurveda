#ifndef	COM_M4NKIND_Equ_XYZD_ParticleCubes_H
#define COM_M4NKIND_Equ_XYZD_ParticleCubes_H

#include "Equ_XYZD_Particle.h"
#include "PackFloat.h"
#include "PackFloat_FixedPoint.h"
#include "PackList.h"

class Equ_XYZD_ParticleCubes : public Equ_XYZD_Particle
{

public:

	Equ_XYZD_ParticleCubes(void);

	BASEOBJECT_DEFINE_CLASS(Equ_XYZD_ParticleCubes);

	virtual bool SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] );

	virtual bool SetYZConstant( float _y,float _z);

	virtual float ComputeByX( float _x);

protected:
	float	m_raySpaceHalf;
};
#endif
