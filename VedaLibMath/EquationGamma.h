/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_EquationGamma_H
#define COM_M4NKIND_EquationGamma_H

#include "VirtualEquation.h"
#include "PackFloat.h"
#include "PackFloatByte_Limits.h"

/*!
	\class	EquationGamma
	\ingroup BaseObjectInherited_VedaLibMath BaseObjectInheritedDocGroup
	\brief	color recalibration function using an exponent curve.
*/
class EquationGamma : public VirtualEquation
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	EquationGamma(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(EquationGamma);

	/*!
		\brief	 Compute a scalar value according to a parameter list (X,Y,Z,D).
		\param	_OutgoingParameterTable result table. elements are writen or not according to the equation.
		\param _IncomingParameterTable. float table, the (X,Y,Z,D) parameters.
	*/
	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! start of the affine curve
	PackFloat				mSer_CurveStart;
	//! end  of the affine curve
	PackFloat				mSer_CurveEnd;
	//! center of the gamma [0,1]
	PackFloatByte_Limits	mSer_Center;
	//! exponent force 0.0->32.0
	PackFloatByte_Limits	mSer_GammaForce;
};
#endif
