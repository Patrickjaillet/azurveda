/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_VirtualEquationSpline_H
#define COM_M4NKIND_VirtualEquationSpline_H

#include "VirtualEquation.h"
#include "PackFloat_FixedPoint.h"
#include "PackULong_Flags.h"
#include "PackList_TimeTrack.h"

class PackResource;

/*!
	\class	VirtualEquationSpline
	\ingroup BaseObjectInherited_VedaLibMath BaseObjectInheritedDocGroup
	\brief	Spline function virtual model
*/
class VirtualEquationSpline : public VirtualEquation
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	VirtualEquationSpline();

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(VirtualEquationSpline);

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


	/*!
		\class	SplineElement
		\brief	 Nested class that describes a reference to an object3D, used as element in the list.
	*/
	class SplineElement : public PackList_TimeTrack::TimeTrackElement
	{
	public:
		//! the vector, it can be X,XY or XYZ according to the constructor init.
		PackFloat_FixedPoint 	mSer_Vector;
		//! \brief constructor . It registers the members
		SplineElement( PackFloat::VectorDimension _dim) : TimeTrackElement()
			,mSer_Vector(_dim,PackFloat::m_1Div64) // fix the serialized floating point precision.
		{
			REGISTER_MEMBER_PACKFLOAT(mSer_Vector ,"Vector", 0.0f );
		};
	};
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
	//! flags
#define		splf_Repeat	1
	PackULong_Flags	mSer_Flags;

	/*!
		\brief	 Return a list that manages SplineElement.
		\return the list
	*/
	virtual	PackList_TimeTrack *GetList()=0;

};
#endif
