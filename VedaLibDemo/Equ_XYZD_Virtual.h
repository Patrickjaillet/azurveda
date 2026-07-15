#ifndef	COM_M4NKIND_Equ_XYZD_Virtual_H
#define COM_M4NKIND_Equ_XYZD_Virtual_H

#include "BaseObject.h"
#include "PackFloat.h"
#include "VirtualEquation.h"
class MarchinCubeSpace;

class Equ_XYZD_Virtual : public VirtualEquation
{

public:

	Equ_XYZD_Virtual(void);

	BASEOBJECT_DEFINE_VIRTUALCLASS(Equ_XYZD_Virtual);

	virtual bool SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] )=0;

	virtual bool	SetYZConstant( float _y,float _z)=0;

	virtual float ComputeByX( float _x)=0;

	float Compute( float _x,float _y,float _z );

	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

#ifdef _ENGINE_EDITABLE_

#endif

	static inline float QuickSqrtM(float _vv)
	{
		const float v1p0=1.0f;
		if(_vv>v1p0) return v1p0;
		_vv = v1p0-_vv;
		_vv = _vv*_vv;
		return v1p0-_vv ;
	};

protected:

	float	m_y;

	float	m_z;

	float	m_LastComputedDate;

#ifdef _ENGINE_EDITABLE_

#endif
};
#endif
