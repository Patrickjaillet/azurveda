/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "EquationSoundSignal.h"
#include "VirtualMachine.h"

BASEOBJECT_DECLARE_CLASS( "snd", EquationSoundSignal, VirtualEquation,"Equation Sound Signal","equSound","return the machine's current stereo sound curve. in:[0,1]=time, out: x,y=stereo" );

EquationSoundSignal::EquationSoundSignal() : VirtualEquation()
	,mSer_LeftScale(PackFloat::vd_XY)
	,mSer_RightScale(PackFloat::vd_XY)
	,m_ilastbufferlengthfloatversion(0L)
{	
	REGISTER_MEMBER_PACKFLOAT_XY(mSer_LeftScale,"LeftScale",0.0f,1.0f)
	REGISTER_MEMBER_PACKFLOAT_XY(mSer_RightScale,"RightScale",0.0f,1.0f)

}

void EquationSoundSignal::Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )
{
	// we have high interest to optimize this, because it could becalled once for a pixel
	// or things like this.
	float fx = _IncomingParameterTable[0];
	_OutgoingParameterTable[0] =mSer_LeftScale.Get(0); // default.
	_OutgoingParameterTable[1] =mSer_RightScale.Get(0);
	float	v0p5= PackFloat::m_0p5;
	if(fx<0.0f || fx>=1.0f)
	{
		_OutgoingParameterTable[0] += (v0p5*m_LeftAmplitude);
		_OutgoingParameterTable[1] += (v0p5*m_RightAmplitude) ;
		return;
	}
	float	*pSoundBuffer=0L;
	unsigned int bufferlength=0;
	GetMachine()->GetCurrentSoundBufferImage( &pSoundBuffer , &bufferlength );
	// watch out, for a given machine,bufferlength  can vary or not.
	// for under 50millisec, the buffer should be the same however.
	if(bufferlength !=m_ilastbufferlengthfloatversion)
	{
		m_flastbufferlengthfloatversion = (float)(bufferlength); 
		m_LeftAmplitude		= mSer_LeftScale.Get(1) - mSer_LeftScale.Get(0);
		m_RightAmplitude	= mSer_RightScale.Get(1) - mSer_RightScale.Get(0);
		m_ilastbufferlengthfloatversion = bufferlength;
	}
	pSoundBuffer += ((int)(fx*m_flastbufferlengthfloatversion))<<1;
	_OutgoingParameterTable[0] += (v0p5+pSoundBuffer[0]*v0p5)*m_LeftAmplitude ;
	_OutgoingParameterTable[1] += (v0p5+pSoundBuffer[1]*v0p5)*m_RightAmplitude ;

}
