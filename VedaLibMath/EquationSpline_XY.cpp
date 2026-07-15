// SPDX-License-Identifier: LGPL-2.1-only

#include "EquationSpline_XY.h"
#include <math.h>
BASEOBJECT_DECLARE_CLASS( "XY", EquationSpline_XY, VirtualEquationSpline,"Equation Spline x,y=f(t)","SplineXY.","2 Dimension Spline curve defined by a list of keys. X,Y=spline(t)" );

EquationSpline_XY::EquationSpline_XY() : VirtualEquationSpline()
	,mSer_List(NewSplineElementXY)
{
	REGISTER_MEMBER( mSer_List,"Key List");

}
