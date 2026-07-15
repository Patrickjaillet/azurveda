#ifndef	COM_M4NKIND_Equ_XYZD_Tore_H
#define COM_M4NKIND_Equ_XYZD_Tore_H

#include "Equ_XYZD_Virtual.h"
#include "PackFloat.h"
#include "PackFloatByte_Limits.h"
#include "PackDynamicFloat.h"

class Equ_XYZD_Tore : public Equ_XYZD_Virtual
{

public:

	Equ_XYZD_Tore(void);

	BASEOBJECT_DEFINE_CLASS(Equ_XYZD_Tore);

	virtual bool SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] );

	virtual bool SetYZConstant( float _y,float _z);

	virtual float ComputeByX( float _x);

protected:

	PackDynamicFloat	mSer_Center;

	PackFloatByte_Limits	mSer_BigRay;

	PackFloatByte_Limits	mSer_LittleRay;

	PackDynamicFloat	mSer_AxisRotation;

	PackDynamicFloat	mSer_BigRayVariation;

	PackDynamicFloat	mSer_LittleRayVariation;

	float	m_sinMove;

	float	m_cosMove;

	float	m_littleRayConst;

	float	m_BigRayConst;

	float	m_ydyd;

	float	m_zz;
	float	m_center[4];
	float m_centerX,m_centerY;
};
#endif
