// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_VirtualEquationSpline_H
#define COM_M4NKIND_VirtualEquationSpline_H

#include "VirtualEquation.h"
#include "PackFloat_FixedPoint.h"
#include "PackULong_Flags.h"
#include "PackList_TimeTrack.h"

class PackResource;

class VirtualEquationSpline : public VirtualEquation
{

public:

	VirtualEquationSpline();

	BASEOBJECT_DEFINE_VIRTUALCLASS(VirtualEquationSpline);

	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

	class SplineElement : public PackList_TimeTrack::TimeTrackElement
	{
	public:

		PackFloat_FixedPoint 	mSer_Vector;

		SplineElement( PackFloat::VectorDimension _dim) : TimeTrackElement()
			,mSer_Vector(_dim,PackFloat::m_1Div64)
		{
			REGISTER_MEMBER_PACKFLOAT(mSer_Vector ,"Vector", 0.0f );
		};
	};
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

protected:

#define		splf_Repeat	1
	PackULong_Flags	mSer_Flags;

	virtual	PackList_TimeTrack *GetList()=0;

};
#endif
