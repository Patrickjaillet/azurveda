/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "EquationSinus3D.h"
#include <math.h>
#ifndef sinf
#define sinf sin
#endif
#ifndef cosf
#define cosf cos
#endif
BASEOBJECT_DECLARE_CLASS( "sin3", EquationSinus3D, VirtualEquation,"Equation Sinus 3D x,y,z=f(x,d)","Sinus","Sinus Equation in 3D, with features..." );

EquationSinus3D::EquationSinus3D() : VirtualEquation()
	,mSer_Center(PackFloat::vd_XYZ)
	,mSer_Frequence(PackFloat::vd_XYZ)
	,mSer_Amplitude(PackFloat::vd_XYZ)
	,mSer_IndexMultiplier_Frequence(PackFloat::vd_X)
	,mSer_IndexMultiplier_Amplitude(PackFloat::vd_X)
	,m_LastComputedDate(PackFloat::m_Max)
{	
	REGISTER_MEMBER_PACKFLOAT_XYZ(mSer_Center,"Center",0.0f,0.0f,0.0f);
	REGISTER_MEMBER_PACKDYNAMICFLOAT(mSer_Frequence,"Frequence",3.1415f*0.5f);
	REGISTER_MEMBER_PACKDYNAMICFLOAT(mSer_Amplitude,"Amplitude",0.5f);
	REGISTER_MEMBER_PACKFLOAT(mSer_IndexMultiplier_Frequence,"FreqMult",0.5f);
	REGISTER_MEMBER_PACKFLOAT(mSer_IndexMultiplier_Amplitude,"AmpMult",0.01f);
}

void EquationSinus3D::Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )
{

	const float date =_IncomingParameterTable[3] ; 
	if(m_LastComputedDate != date)
	{	// update to that frame:
		m_param_amplitude[0]=PackFloat::m_0p0;
		mSer_IndexMultiplier_Frequence.Compute(m_param_amplitude,_IncomingParameterTable);
		m_indexMultiplierFreq = m_param_amplitude[0];
		m_param_amplitude[0]=PackFloat::m_0p0;
		mSer_IndexMultiplier_Amplitude.Compute(m_param_amplitude,_IncomingParameterTable);
		m_indexMultiplierAmp = m_param_amplitude[0];
		m_param_amplitude[0]=m_param_amplitude[1]=m_param_amplitude[2]=m_param_amplitude[3]=
		m_param_frequence[0]=m_param_frequence[1]=m_param_frequence[2]=m_param_frequence[3]=PackFloat::m_0p0;
		mSer_Amplitude.Compute(m_param_amplitude,_IncomingParameterTable);
		mSer_Frequence.Compute(m_param_frequence,_IncomingParameterTable);
		m_LastComputedDate = date;
	}
/*
	float m_indexMultiplierFreq;
	// computed for m_LastComputedDate.
	float m_indexMultiplierAmp;
*/

	//_IncomingParameterTable[]
	_OutgoingParameterTable[0] += mSer_Center.Get(0)+cosf(
			date*(m_param_frequence[0]+m_indexMultiplierFreq*_IncomingParameterTable[0])
			)*(m_param_amplitude[0]+m_indexMultiplierAmp*_IncomingParameterTable[0]);
	_OutgoingParameterTable[1] += mSer_Center.Get(1)+sinf(
			date*(m_param_frequence[1]+m_indexMultiplierFreq*_IncomingParameterTable[0])
			)*(m_param_amplitude[1]+m_indexMultiplierAmp*_IncomingParameterTable[0]);
	_OutgoingParameterTable[2] += mSer_Center.Get(2)+sinf(
			date*(m_param_frequence[2]+m_indexMultiplierFreq*_IncomingParameterTable[0])
			)*(m_param_amplitude[2]+m_indexMultiplierAmp*_IncomingParameterTable[0]);

}
