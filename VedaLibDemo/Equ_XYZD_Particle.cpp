#include "Equ_XYZD_Particle.h"
#include "ParticleSetVirtual.h"
#include <math.h>
BASEOBJECT_DECLARE_CLASS( "ptcl", Equ_XYZD_Particle, Equ_XYZD_Virtual,"3DMetaShape Particle","ParticleEqu","Equation that define a scalar value of a given space(x,y,z) and time(d) using a set of particles. To be used with 3D Object MarchingCube Terms." );

Equ_XYZD_Particle::Equ_XYZD_Particle() : Equ_XYZD_Virtual()
{	
	REGISTER_MEMBER_REFERENCE( mSer_ParticleSetRef ,"Particles", ParticleSetVirtual::m_Description );

//	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_NumberOfParticle,"Number",127,1,512);

	REGISTER_MEMBER_PACKFLOATBYTE_0_1( mSer_Ray,"Ray",0.5f);
	REGISTER_MEMBER_PACKFLOATBYTE_0_1( mSer_Smooth,"Smooth",0.0f);

}

/*!
	\brief	 Set the framedate, and compute all terms with a constant date.
		pass a int* which must be equal to 0 at start.
	\param	_date			in seconds
	\param	_piTermIndex	pointer to an index of the equation term to process. 
	\param	_pbound			a 6 float table, xmin,xman,ymin,ymax,zmin,mzmax of the cube where values are !=1.0
	\return if true, there is an index left to process.
*/
bool Equ_XYZD_Particle::SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] )
{
	ParticleSetVirtual *pParticleSet = (ParticleSetVirtual *)mSer_ParticleSetRef.GetObjectPointer() ;
	if( !pParticleSet ) return false; // no particle at all.

	// compute particle set:
	pParticleSet->SetFrameDate( _date);

	if( _iTermIndex >= pParticleSet->GetNumberOfParticle() ) return false;

	//int	nbParticle = 20; 19
	//if(_iTermIndex>=mSer_NumberOfParticle.Get()) return false;
	if( _iTermIndex ==0 )
	{
		float	rr =  (mSer_Ray.Get())*8.0f;
		float	smt = mSer_Smooth.Get();
		//smt = smt*smt;
		m_raysquare = rr*rr;
		m_rayspace = (1.0f/(rr))-smt;
		if(m_rayspace<0.0f)m_rayspace=0.0f;
	}
	ParticleSetVirtual::Particle *pParticle = pParticleSet->Get(_iTermIndex);
	m_xc = pParticle->m_xyzd[0];
	m_yc = pParticle->m_xyzd[1];
	m_zc = pParticle->m_xyzd[2];
	/*old	// center
	float	pcn =(float)_iTermIndex;
	_date *=0.5f ;
	_date += pcn*0.15f ;
	
	m_xc = sin(_date)*(0.8f*sin(pcn*0.2f));
	m_yc = sin(_date*1.5f)* 0.8f*sin(_date+pcn*0.4f);
	m_zc = cos(_date*2.0f)* 0.8f*cos(_date+pcn*0.4f);
*/
	float	ray =m_rayspace;
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
bool Equ_XYZD_Particle::SetYZConstant( float _y,float _z)
{
	_y -= m_yc;
	_z -= m_zc;
	m_zz = (_z*_z + _y*_y)*m_raysquare + mSer_Smooth.Get();
	if(m_zz>1.0f) return false;
	return true;
}
/*!
	\brief	 
	\param	
*/
float Equ_XYZD_Particle::ComputeByX( float _x)
{
	_x -= m_xc ;
	_x = _x*_x;
	return QuickSqrtM(m_zz+ _x*m_raysquare) ;

}
