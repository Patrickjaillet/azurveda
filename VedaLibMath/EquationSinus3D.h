/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_EquationSinus3D_H
#define COM_M4NKIND_EquationSinus3D_H

#include "VirtualEquation.h"
#include "PackFloat.h"
#include "PackFloatByte_Limits.h"
#include "PackDynamicFloat.h"
/*!
	\class	EquationSinus3D
	\ingroup BaseObjectInherited_VedaLibMath BaseObjectInheritedDocGroup
	\brief	return a X,Y,Z sinus according to a configuration and time.

*/
class EquationSinus3D : public VirtualEquation
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	EquationSinus3D(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(EquationSinus3D);

	/*!
		\brief	 Compute 1,2,3 or 4 value according to a parameter list (X,Y,Z,D).
				An equation can manage one or more dimensions on input and output.
				the result is <b>added</b> over the outgoing parameter table,
				and unused output parameters are left untouch, so you
				must init _OutgoingParameterTable before, with default values you will choose.
		\param	_OutgoingParameterTable result table. result is added over the table values.
		\param _IncomingParameterTable. float table, the (X,Y,Z,D) parameters.
	*/
	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! center of the equation.
	PackFloat				mSer_Center;
	//! frequence
	PackDynamicFloat		mSer_Frequence;
	//! amplitude
	PackDynamicFloat		mSer_Amplitude;
	//! particleIndex
	PackDynamicFloat		mSer_IndexMultiplier_Frequence;
	//! particleIndex
	PackDynamicFloat		mSer_IndexMultiplier_Amplitude;
	//! optimization 
	float	m_LastComputedDate;
	//! computed for date: m_LastComputedDate
	float	m_param_amplitude[4];
	//! computed for date: m_LastComputedDate
	float	m_param_frequence[4];
	// computed for m_LastComputedDate.
	float m_indexMultiplierFreq;
	// computed for m_LastComputedDate.
	float m_indexMultiplierAmp;
};
#endif
