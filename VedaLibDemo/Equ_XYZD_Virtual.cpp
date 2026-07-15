#include "Equ_XYZD_Virtual.h"
#include "VirtualMachine.h"
BASEOBJECT_DECLARE_VIRTUALCLASS( "xyzt", Equ_XYZD_Virtual, VirtualEquation );

Equ_XYZD_Virtual::Equ_XYZD_Virtual() : VirtualEquation()
	,m_LastComputedDate(PackFloat::m_Max)
{

}

#ifdef _ENGINE_EDITABLE_

#endif

#ifdef _ENGINE_EDITABLE_

#endif

float Equ_XYZD_Virtual::Compute( float _x,float _y,float _z )
{
	SetYZConstant(_y,_z);
	return ComputeByX(_x);
}

void Equ_XYZD_Virtual::Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )
{
	if(m_LastComputedDate==_IncomingParameterTable[3])
	{
		float cubelimits[6];
		SetFrameDate(_IncomingParameterTable[3],0,cubelimits);
		m_LastComputedDate = _IncomingParameterTable[3];
	}
	SetYZConstant(_IncomingParameterTable[1],_IncomingParameterTable[2]);
	_OutgoingParameterTable[0] += ComputeByX(_IncomingParameterTable[0]);
}
