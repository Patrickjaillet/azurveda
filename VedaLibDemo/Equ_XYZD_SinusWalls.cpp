#include "Equ_XYZD_SinusWalls.h"
#include <math.h>
BASEOBJECT_DECLARE_CLASS( "sw", Equ_XYZD_SinusWalls, Equ_XYZD_Virtual,"3DMetaShape SinusWalls","EquWalls","Equation that define a scalar value of a given space(x,y,z) and time(d) To be used with 3D Object MarchingCube Terms." );

Equ_XYZD_SinusWalls::Equ_XYZD_SinusWalls() : Equ_XYZD_Virtual()
	,mSer_SinAmp(PackFloat::vd_X)
	,mSer_FloorAlt(PackFloat::vd_X)
	,mSer_CeilingAlt(PackFloat::vd_X)
{
	REGISTER_MEMBER_FLAG( mSer_WallFlags,"Walls",swe_Floor,"Floor.Ceiling.West.East.North.South");

	REGISTER_MEMBER_PACKDYNAMICFLOAT( mSer_SinAmp,"SinAmp",0.0f);
	REGISTER_MEMBER_PACKDYNAMICFLOAT( mSer_FloorAlt,"FloorAlt",-0.75f);
	REGISTER_MEMBER_PACKDYNAMICFLOAT( mSer_CeilingAlt,"FloorAlt",0.75f);
	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS( mSer_Thickness,"Thickness",0.2f,0.0f,1.0f);
}

bool Equ_XYZD_SinusWalls::SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] )
{
	unsigned int flags = mSer_WallFlags.Get() ;
	unsigned int FlagMask=1;

	unsigned int ii=0;
	while( FlagMask != (1<<6) )
	{
		if(FlagMask & flags)
		{
			if( ii == _iTermIndex) break;
			ii++;
		}
		FlagMask<<=1;
	}
	if(FlagMask == (1<<6) ) return false;
	switch(FlagMask)
	{
		case swe_Floor:
		{
			_pbound[0] = -1.0f;
			_pbound[1] = 1.0f;
			_pbound[4] = -1.0f;
			_pbound[5] =1.0f;
			_pbound[2] = -1.0f;
			_pbound[3] = 0.35f;
			_pbound[4] = -1.0f;
			_pbound[5] =1.0f;
		} break;
		case swe_Ceiling:
		{

		} break;
		case swe_West:
		{

		} break;
		case swe_East:
		{

		} break;
		case swe_North:
		{

		} break;
		case swe_South:
		{

		} break;
		default: break;
	}

	_pbound[0] = -1.0f;
	_pbound[1] = 1.0f;
	_pbound[2] = -1.0f;
	_pbound[3] = 0.35f;
	_pbound[4] = -1.0f;
	_pbound[5] =1.0f;

	m_zc = _date*2.0f;
	return true;
}

bool Equ_XYZD_SinusWalls::SetYZConstant( float _y,float _z)
{

	_y *=8.0f;
	_z *=8.0f;
	_z += m_zc;
	float base_y = _y;
	const float famplitude = 0.4f;
	if(base_y>1.0f) base_y = 1.0f;

	m_y = base_y;
	m_zz = _z+ sin(_y)*famplitude;
	_y += sin(_z+ sin(_y)+m_zc)*famplitude;
	m_yc = sin(_y)*cos(_z)*famplitude;

	if((m_y-famplitude)>=1.0f) return false;
	return true;
}

float Equ_XYZD_SinusWalls::ComputeByX( float _x)
{

	float	val;
	_x *=8.0f;
	val =  m_y + sin(_x+m_zz)*m_yc;

		if(val>1.0f) val=1.0f;
	return val;

}
