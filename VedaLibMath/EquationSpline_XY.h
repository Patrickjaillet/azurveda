/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_EquationSpline_XY_H
#define COM_M4NKIND_EquationSpline_XY_H

#include "VirtualEquationSpline.h"
#include "PackFloat.h"
#include "PackList_TimeTrack.h"
/*!
	\class	EquationSpline_XY
	\ingroup BaseObjectInherited_VedaLibMath BaseObjectInheritedDocGroup
	\brief	Spline function for XY
*/
class EquationSpline_XY : public VirtualEquationSpline
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	EquationSpline_XY(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(EquationSpline_XY);

	/*!
		\brief	static method passed to the list to create new elements.
		\return a new element.
	*/
	static PackList_TimeTrack::TimeTrackElement *NewSplineElementXY(){ return new SplineElement(PackFloat::vd_XY); };
	
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! list of objects in the scenes. According to the type of class, it can be serialized (Scene3D) or created ( Scene3DText )
	PackList_TimeTrack		mSer_List;
	/*!
		\brief	 Return a list that manages SplineElement.
		\return the list
	*/
	virtual	PackList_TimeTrack *GetList(){ return &mSer_List; };
};
#endif
