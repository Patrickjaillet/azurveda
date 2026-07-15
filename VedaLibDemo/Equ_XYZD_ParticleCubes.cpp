#include "Equ_XYZD_ParticleCubes.h"
#include "ParticleSetVirtual.h"
#include <math.h>
BASEOBJECT_DECLARE_CLASS( "cb", Equ_XYZD_ParticleCubes, Equ_XYZD_Particle,"3DMetaShape Partic.Cubes","EquCube","Equation that define an implicit serie of cubes with a scalar value of a given space(x,y,z) and time(d) using a set of particles, with cube-like particles. To be used with 3D Object MarchingCube Terms." );

Equ_XYZD_ParticleCubes::Equ_XYZD_ParticleCubes() : Equ_XYZD_Particle()
{

}

bool Equ_XYZD_ParticleCubes::SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] )
{

	ParticleSetVirtual *pParticleSet = (ParticleSetVirtual *)mSer_ParticleSetRef.GetObjectPointer() ;
	if( !pParticleSet ) return false;

	pParticleSet->SetFrameDate( _date);

	if( _iTermIndex >= pParticleSet->GetNumberOfParticle() ) return false;
	if( _iTermIndex ==0 )
	{
		float	rr =  (mSer_Ray.Get())*8.0f;
		float	smt = mSer_Smooth.Get();

		m_raysquare = rr*rr;
		m_rayspace = (PackFloat::m_1p0/(rr))-smt;
		if(m_rayspace<PackFloat::m_0p0)m_rayspace=PackFloat::m_0p0;
		m_raySpaceHalf = m_rayspace * PackFloat::m_0p5;
	}
	ParticleSetVirtual::Particle *pParticle = pParticleSet->Get(_iTermIndex);
	m_xc = pParticle->m_xyzd[0];
	m_yc = pParticle->m_xyzd[1];
	m_zc = pParticle->m_xyzd[2];

	const float	ray =m_rayspace*1.125f;
	_pbound[0] = m_xc-ray ;
	_pbound[1] = m_xc+ray ;
	_pbound[2] = m_yc-ray ;
	_pbound[3] = m_yc+ray ;
	_pbound[4] = m_zc-ray ;
	_pbound[5] = m_zc+ray ;

	return true;
}

bool Equ_XYZD_ParticleCubes::SetYZConstant( float _y,float _z)
{

	_y -= m_yc;
	_z -= m_zc;
	_y = fabsf(_y);
	_z = fabsf(_z);
	const float	mray = m_raySpaceHalf;
	_y -= mray ;
	_z -= mray ;
	if(_y<0.0f) _y=0.0f;
	if(_z<0.0f) _z=0.0f;

	m_zz = (_z*_z + _y*_y)*16.0f;
	m_zz += (sinf(_y*8.0f)*cosf(_z*8.0f)*0.2f);
	if(m_zz>1.0f) return false;

	return true;
}

float Equ_XYZD_ParticleCubes::ComputeByX( float _x)
{
	_x -= m_xc ;
	_x = fabsf(_x);
	_x -=m_raySpaceHalf;
	if(_x<0.0f) _x=0.0f;
	_x = _x * _x;

	_x =  QuickSqrtM(m_zz+ _x*16.0f);

	return _x;

}
