#include "Equ_XYZD_ParticleCubes.h"
#include "ParticleSetVirtual.h"
#include <math.h>
BASEOBJECT_DECLARE_CLASS( "cb", Equ_XYZD_ParticleCubes, Equ_XYZD_Particle,"3DMetaShape Partic.Cubes","EquCube","Equation that define an implicit serie of cubes with a scalar value of a given space(x,y,z) and time(d) using a set of particles, with cube-like particles. To be used with 3D Object MarchingCube Terms." );

Equ_XYZD_ParticleCubes::Equ_XYZD_ParticleCubes() : Equ_XYZD_Particle()
{	

}

/*!
	\brief	 Set the framedate, and compute all terms with a constant date.
		pass a int* which must be equal to 0 at start.
	\param	_date			in seconds
	\param	_piTermIndex	pointer to an index of the equation term to process. 
	\param	_pbound			a 6 float table, xmin,xman,ymin,ymax,zmin,mzmax of the cube where values are !=1.0
	\return if true, there is an index left to process.
*/
bool Equ_XYZD_ParticleCubes::SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] )
{
	//int	nbParticle = 20; 19
/*	if(_iTermIndex>7) return false;

	// center
	float	pcn =(float)_iTermIndex;
	_date *=0.5f ;
	_date += pcn*0.4f ;
	
	m_xc = sin(_date)*(0.8f*sin(pcn*0.2f));
	m_yc = sin(_date*1.5f)* 0.8f*sin(_date+pcn*0.4f);
	m_zc = cos(_date*2.0f)* 0.8f*cos(_date+pcn*0.4f);

	float	ray = 0.35f;

	_pbound[0] = m_xc-ray ;
	_pbound[1] = m_xc+ray ;
	_pbound[2] = m_yc-ray ;
	_pbound[3] = m_yc+ray ;
	_pbound[4] = m_zc-ray ;
	_pbound[5] = m_zc+ray ;
*/
	ParticleSetVirtual *pParticleSet = (ParticleSetVirtual *)mSer_ParticleSetRef.GetObjectPointer() ;
	if( !pParticleSet ) return false; // no particle at all.

	// compute particle set:
	pParticleSet->SetFrameDate( _date);

	if( _iTermIndex >= pParticleSet->GetNumberOfParticle() ) return false;
	if( _iTermIndex ==0 )
	{
		float	rr =  (mSer_Ray.Get())*8.0f;
		float	smt = mSer_Smooth.Get();
		//smt = smt*smt;
		m_raysquare = rr*rr;
		m_rayspace = (PackFloat::m_1p0/(rr))-smt;
		if(m_rayspace<PackFloat::m_0p0)m_rayspace=PackFloat::m_0p0;
		m_raySpaceHalf = m_rayspace * PackFloat::m_0p5;
	}
	ParticleSetVirtual::Particle *pParticle = pParticleSet->Get(_iTermIndex);
	m_xc = pParticle->m_xyzd[0];
	m_yc = pParticle->m_xyzd[1];
	m_zc = pParticle->m_xyzd[2];

	const float	ray =m_rayspace*1.125;
	_pbound[0] = m_xc-ray ;
	_pbound[1] = m_xc+ray ;
	_pbound[2] = m_yc-ray ;
	_pbound[3] = m_yc+ray ;
	_pbound[4] = m_zc-ray ;
	_pbound[5] = m_zc+ray ;

	return true;
}
/*!
	\brief	 
	\param	
*/
bool Equ_XYZD_ParticleCubes::SetYZConstant( float _y,float _z)
{
	//_y*=2.5f;
	//_z*=2.5f;
	_y -= m_yc;
	_z -= m_zc;
	_y = fabs(_y);
	_z = fabs(_z);
	const float	mray = m_raySpaceHalf; // 0.15f
	_y -= mray ;
	_z -= mray ;
	if(_y<0.0f) _y=0.0f;
	if(_z<0.0f) _z=0.0f;


	// 2.5 *2.5 = 6.25
	m_zz = (_z*_z + _y*_y)*16.0f;
	m_zz += (sin(_y*8.0f)*cos(_z*8.0f)*0.2f);
	if(m_zz>1.0f) return false;

	return true;
}
/*!
	\brief	 
	\param	
*/
float Equ_XYZD_ParticleCubes::ComputeByX( float _x)
{
	_x -= m_xc ;
	_x = fabs(_x);
	_x -=m_raySpaceHalf;
	if(_x<0.0f) _x=0.0f;
	_x = _x * _x;
	// 2.5 *2.5 = 6.25
	_x =  QuickSqrtM(m_zz+ _x*16.0f);
//	_x += s
	return _x;

}
