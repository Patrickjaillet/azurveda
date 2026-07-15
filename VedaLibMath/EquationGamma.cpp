/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "EquationGamma.h"
#include <math.h>
#ifndef powf
#define powf pow
#endif
BASEOBJECT_DECLARE_CLASS( "Gm", EquationGamma, VirtualEquation,"Equation Gamma x=f(x)","GammaEqu.","Calibration equation. P1=unchanged center, P2=force" );

EquationGamma::EquationGamma() : VirtualEquation()
	,mSer_CurveStart(PackFloat::vd_XY)
	,mSer_CurveEnd(PackFloat::vd_XY)
{	
	REGISTER_MEMBER_PACKFLOAT_XY(mSer_CurveStart,"Start",0.0f,0.0f)
	REGISTER_MEMBER_PACKFLOAT_XY(mSer_CurveEnd,"End",1.0f,1.0f)
	REGISTER_MEMBER_PACKFLOATBYTE_0_1( mSer_Center,"Center",0.5f);
	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS( mSer_GammaForce,"Exp.Force",1.0f,0.0f,32.0f);

}

void EquationGamma::Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )
{
	float	fx =  _IncomingParameterTable[0];

	float	f_ax1 = mSer_CurveStart.Get(0);
	float	f_ay1 = mSer_CurveStart.Get(1);
	float	f_bx1 = mSer_CurveEnd.Get(0);
	float	f_by1 = mSer_CurveEnd.Get(1);

	float	center = mSer_Center.Get();
	float	force = mSer_GammaForce.Get();
	float	ct1 = PackFloat::m_1p0;

	if(fx<f_ax1)
	{
		_OutgoingParameterTable[0] = f_ay1 ;
		return;
	}
	if(fx>=f_bx1)
	{
		_OutgoingParameterTable[0] = f_by1 ;
		return;
	}
	fx = (fx-f_ax1) / (f_bx1-f_ax1); //[0,1]

	if( fx<center  )
	{
		fx/=center;
		fx = powf(fx,force);
		fx*= center;
	} else
	{	
		float domainLength = ct1-center;
		fx=ct1-fx;
		fx/=domainLength;
		fx = powf(fx,force);
		fx*= domainLength;
		fx=ct1-fx;
	}
	
	fx = f_ay1 + fx*(f_by1-f_ay1);

	_OutgoingParameterTable[0] = fx ;

}
