#ifndef	COM_M4NKIND_Equ_XYZD_Particle_H
#define COM_M4NKIND_Equ_XYZD_Particle_H

#include "Equ_XYZD_Virtual.h"
#include "PackFloat.h"
#include "PackFloatByte_Limits.h"
#include "PackLong_WithLimits.h"
#include "PackObjectReference.h"

class Equ_XYZD_Particle : public Equ_XYZD_Virtual
{

public:

	Equ_XYZD_Particle(void);

	BASEOBJECT_DEFINE_CLASS(Equ_XYZD_Particle);

	virtual bool SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] );

	virtual bool SetYZConstant( float _y,float _z);

	virtual float ComputeByX( float _x);

protected:

	PackObjectReference		mSer_ParticleSetRef;

	PackFloatByte_Limits	mSer_Ray;

	PackFloatByte_Limits	mSer_Smooth;

	float	m_xc;
	float	m_yc;
	float	m_zc;

	float	m_raysquare;
	float	m_rayspace;
	float	m_zz;

};
#endif
