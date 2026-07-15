// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_EquationSpline_XYZ_H
#define COM_M4NKIND_EquationSpline_XYZ_H

#include "VirtualEquationSpline.h"
#include "PackFloat.h"
#include "PackList_TimeTrack.h"

class EquationSpline_XYZ : public VirtualEquationSpline
{

public:

	EquationSpline_XYZ(void);

	BASEOBJECT_DEFINE_CLASS(EquationSpline_XYZ);

	static PackList_TimeTrack::TimeTrackElement *NewSplineElementXYZ(){ return new SplineElement(PackFloat::vd_XYZ); };

protected:

	PackList_TimeTrack		mSer_List;

	virtual	PackList_TimeTrack *GetList(){ return &mSer_List; };
};
#endif
