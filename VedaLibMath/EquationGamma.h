// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_EquationGamma_H
#define COM_M4NKIND_EquationGamma_H

#include "VirtualEquation.h"
#include "PackFloat.h"
#include "PackFloatByte_Limits.h"

class EquationGamma : public VirtualEquation
{

public:

	EquationGamma(void);

	BASEOBJECT_DEFINE_CLASS(EquationGamma);

	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

protected:

	PackFloat				mSer_CurveStart;

	PackFloat				mSer_CurveEnd;

	PackFloatByte_Limits	mSer_Center;

	PackFloatByte_Limits	mSer_GammaForce;
};
#endif
