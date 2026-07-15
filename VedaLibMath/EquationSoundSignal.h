// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_EquationSoundSignal_H
#define COM_M4NKIND_EquationSoundSignal_H

#include "VirtualEquation.h"
#include "PackFloat.h"
#include "PackFloatByte_Limits.h"

class EquationSoundSignal : public VirtualEquation
{

public:

	EquationSoundSignal(void);

	BASEOBJECT_DEFINE_CLASS(EquationSoundSignal);

	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

protected:

	PackFloat				mSer_LeftScale;

	PackFloat				mSer_RightScale;

	float				m_flastbufferlengthfloatversion;
	float				m_LeftAmplitude;
	float				m_RightAmplitude;
	int					m_ilastbufferlengthfloatversion;
};
#endif
