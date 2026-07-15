/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "EquationSpline_XYZ.h"
#include <math.h>
BASEOBJECT_DECLARE_CLASS( "XYZ", EquationSpline_XYZ, VirtualEquationSpline,"Equation Spline x,y,z=f(t)","SplineXYZ.","3 Dimension Spline curve defined by a list of keys. X,Y,Z=spline(t)" );

EquationSpline_XYZ::EquationSpline_XYZ() : VirtualEquationSpline()
	,mSer_List(NewSplineElementXYZ)
{	
	REGISTER_MEMBER( mSer_List,"Key List");
}

