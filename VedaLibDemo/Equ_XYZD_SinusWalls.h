#ifndef	COM_M4NKIND_Equ_XYZD_SinusWalls_H
#define COM_M4NKIND_Equ_XYZD_SinusWalls_H

#include "Equ_XYZD_Virtual.h"
#include "PackFloat.h"
#include "PackULong_Flags.h"
#include "PackFloatByte_Limits.h"
#include "PackDynamicFloat.h"

class Equ_XYZD_SinusWalls : public Equ_XYZD_Virtual
{

public:

	Equ_XYZD_SinusWalls(void);

	BASEOBJECT_DEFINE_CLASS(Equ_XYZD_SinusWalls);

	virtual bool SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] );

	virtual bool SetYZConstant( float _y,float _z);

	virtual float ComputeByX( float _x);

protected:
	typedef enum
	{
		swe_Floor=1,
		swe_Ceiling=2,
		swe_West=4,
		swe_East=8,
		swe_North=16,
		swe_South=32
	} SinusWallBits ;

	PackULong_Flags		mSer_WallFlags;

	PackDynamicFloat	mSer_SinAmp;

	PackDynamicFloat	mSer_FloorAlt;

	PackDynamicFloat	mSer_CeilingAlt;

	PackFloatByte_Limits	mSer_Thickness;

	float	m_y;
	float	m_yc;
	float	m_zc;

	float	m_zz;

};
#endif
