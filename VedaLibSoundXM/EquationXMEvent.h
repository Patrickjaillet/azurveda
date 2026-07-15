/*! \file 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_EquationXMEvent_H
#define COM_M4NKIND_EquationXMEvent_H

#include "VirtualEquation.h"
#include "PackObjectReference.h"
#include "PackFloat.h"
#include "PackULong_Flags.h"
#include "PackList_TimeTrack.h"

class PackResource;

/*!
	\class	EquationXMEvent
	\brief	Describes a curve by using XM music events.
*/
class EquationXMEvent : public VirtualEquation
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	EquationXMEvent();

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(EquationXMEvent);

	/*!
		\brief	 Compute a scalar value according to a parameter list (X,Y,Z,D).
		\param	_OutgoingParameterTable result table. elements are writen or not according to the equation.
		\param _IncomingParameterTable. float table, the (X,Y,Z,D) parameters.
	*/
	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );


#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
				created object. This is done with this method. sub classes can implement it (or not) in
				any way, to explicit current shape of an object.<br>

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration
	*/
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the musique to listen
	PackObjectReference	mSer_XMMusicReference;
	//! the instrument to listen:
	PackULong	mSer_InstrumentNumber;
	//! the sample to listen:
	PackULong	mSer_SampleNumber;
	enum {
		bApplyFlag_VolToX=1,
		bApplyFlag_VolToY=2,
		bApplyFlag_VolToZ=4,
	};
	//! the flags
	PackULong_Flags	mSer_Flags;
	//! the dimension to return;
	PackFloat	mSer_VolumeScale;
	//! optimisation
	float	m_LastDateComputed;
	//!
	float	m_LastValueFound;
	//! 
	float	m_LastHighestLeft;
	//!
	float	m_LastHighestLeftDate;
	//!
	//float	m_LastHighestRight;
	//!
	//float	m_LastHighestRightDate;
};
#endif
