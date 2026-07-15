// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_EquationSinus3D_H
#define COM_M4NKIND_EquationSinus3D_H

#include "VirtualEquation.h"
#include "PackFloat.h"
#include "PackFloatByte_Limits.h"
#include "PackDynamicFloat.h"

class EquationSinus3D : public VirtualEquation
{

public:

	EquationSinus3D(void);

	BASEOBJECT_DEFINE_CLASS(EquationSinus3D);

	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

protected:

	PackFloat				mSer_Center;

	PackDynamicFloat		mSer_Frequence;

	PackDynamicFloat		mSer_Amplitude;

	PackDynamicFloat		mSer_IndexMultiplier_Frequence;

	PackDynamicFloat		mSer_IndexMultiplier_Amplitude;

	float	m_LastComputedDate;

	float	m_param_amplitude[4];

	float	m_param_frequence[4];

	float m_indexMultiplierFreq;

	float m_indexMultiplierAmp;
};
#endif
