/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_EquationSoundSignal_H
#define COM_M4NKIND_EquationSoundSignal_H

#include "VirtualEquation.h"
#include "PackFloat.h"
#include "PackFloatByte_Limits.h"

/*!
	\class	EquationSoundSignal
	\ingroup BaseObjectInherited_VedaLibMath BaseObjectInheritedDocGroup
	\brief	Return the Machine's Sound signal output as an equation.
*/
class EquationSoundSignal : public VirtualEquation
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	EquationSoundSignal(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(EquationSoundSignal);

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
	PackFloat				mSer_LeftScale;
	//! end  of the affine curve
	PackFloat				mSer_RightScale;
	//! some optim:
	float				m_flastbufferlengthfloatversion;
	float				m_LeftAmplitude;
	float				m_RightAmplitude;
	int					m_ilastbufferlengthfloatversion;
};
#endif
