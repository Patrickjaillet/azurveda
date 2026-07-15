// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_EquationSpline_XY_H
#define COM_M4NKIND_EquationSpline_XY_H

#include "VirtualEquationSpline.h"
#include "PackFloat.h"
#include "PackList_TimeTrack.h"

class EquationSpline_XY : public VirtualEquationSpline
{

public:

	EquationSpline_XY(void);

	BASEOBJECT_DEFINE_CLASS(EquationSpline_XY);

	static PackList_TimeTrack::TimeTrackElement *NewSplineElementXY(){ return new SplineElement(PackFloat::vd_XY); };

protected:

	PackList_TimeTrack		mSer_List;

	virtual	PackList_TimeTrack *GetList(){ return &mSer_List; };
};
#endif
