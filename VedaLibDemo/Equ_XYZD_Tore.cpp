#include "Equ_XYZD_Tore.h"
#include <math.h>
#ifndef sqrtf
#define sqrtf sqrt
#endif
#ifndef sinf
#define sinf sin
#endif
#ifndef cosf
#define cosf cos
#endif
BASEOBJECT_DECLARE_CLASS( "tr", Equ_XYZD_Tore, Equ_XYZD_Virtual,"3DMetaShape Torus","EquTore","Equation that define a scalar value of a given space(x,y,z) and time(d), to make a torus. To be used with 3D Object MarchingCube Terms." );

Equ_XYZD_Tore::Equ_XYZD_Tore() : Equ_XYZD_Virtual()
	,mSer_Center(PackFloat::vd_XYZ)
	,mSer_AxisRotation(PackFloat::vd_X)
	,mSer_BigRayVariation(PackFloat::vd_X)
	,mSer_LittleRayVariation(PackFloat::vd_X)
{	
	REGISTER_MEMBER_PACKDYNAMICFLOAT( mSer_Center,"Center",0.0f);
	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS( mSer_BigRay,"BigRay",0.4f,0.0f,1.0f);
	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS( mSer_LittleRay,"LitRay",4.0f,0.5f,16.0f);
	REGISTER_MEMBER_PACKDYNAMICFLOAT(mSer_AxisRotation,"AxisRotation",0.0f);
	REGISTER_MEMBER_PACKDYNAMICFLOAT( mSer_BigRayVariation,"BigRayVar",0.0f);
	REGISTER_MEMBER_PACKDYNAMICFLOAT( mSer_LittleRayVariation,"LitRayVar",0.0f);

}

/*!
	\brief	 Set the framedate, and compute all terms with a constant date.
		pass a int* which must be equal to 0 at start.
	\param	_date			in seconds
	\param	_piTermIndex	pointer to an index of the equation term to process. 
	\param	_pbound			a 6 float table, xmin,xman,ymin,ymax,zmin,mzmax of the cube where values are !=1.0
	\return if true, there is an index left to process.
*/
bool Equ_XYZD_Tore::SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] )
{
	// executed once by frame:
	if(_iTermIndex>0) return false;
	float pin[4],pout[4];
	pin[3] = _date ;
	pout[0] = pin[0] = PackFloat::m_0p0 ;
	mSer_AxisRotation.Compute(pout,pin);
	m_sinMove = sinf( pout[0] );
	m_cosMove = cosf( pout[0] );
	m_center[0] = m_center[1] = m_center[2] = PackFloat::m_0p0 ;
	mSer_Center.Compute(m_center,pin);

	pout[0] = PackFloat::m_0p0 ;
	mSer_LittleRayVariation.Compute(pout,pin);
	m_littleRayConst = mSer_LittleRay.Get() + pout[0];
	// re-clamp
	if(m_littleRayConst<mSer_LittleRay.GetMin() )m_littleRayConst=mSer_LittleRay.GetMin();
	const float max=mSer_LittleRay.GetMax();
	if(m_littleRayConst>max )m_littleRayConst=max;

	pout[0] = PackFloat::m_0p0 ;
	mSer_BigRayVariation.Compute(pout,pin);
	m_BigRayConst = mSer_BigRay.Get() + pout[0];
	if(m_BigRayConst<mSer_BigRay.GetMin() )m_BigRayConst=mSer_BigRay.GetMin();
	const float bmax=mSer_BigRay.GetMax();
	if(m_BigRayConst>bmax )m_BigRayConst=bmax;

	const float	v1 = PackFloat::m_1p0 ;
	const float	vm1 = -1.0f;
	_pbound[0] =vm1 ;
	_pbound[1] =v1 ;
	_pbound[2] =vm1 ;
	_pbound[3] =v1 ;
	_pbound[4] =vm1 ;
	_pbound[5] =v1 ;

	return true;
}
/*!
	\brief	 
	\param	
*/
bool Equ_XYZD_Tore::SetYZConstant( float _y,float _z)
{
	register float	yd;
	_y -= m_center[1];
	_z -= m_center[2];
	yd = m_cosMove * _y - m_sinMove * _z;
	yd = yd*yd*m_littleRayConst;
	// for these (_y,_z) constant, everything will be >=1.0
	// this escape most cases.
	if(yd>=1.0f) return false; 
	m_ydyd = yd;
	// calculate Z rotated:
	yd = m_sinMove * _y + m_cosMove * _z;	
	yd = yd*yd;
/*shit...	if( (yd+m_BigRayConst-(1.0f*m_littleRayConst))
			>=1.0f)
	{		// will be always 1.0 whatever is X.
			return false;
	}*/
	m_zz = yd;
	
	return true;
}
/*!
	\brief	 
	\param	
*/
float Equ_XYZD_Tore::ComputeByX( float _x)
{
    register float   val;
	_x -= m_center[0];
	val=(_x*_x + m_zz) ;
	val= sqrtf(val); // need real sqrt.
	val-=m_BigRayConst;
	val = (val*val*m_littleRayConst) + m_ydyd ;
	return QuickSqrtM(val) ;
}
