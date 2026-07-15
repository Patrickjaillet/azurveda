/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "BaseContext.h"

#include "VirtualEquation.h"
#include "EquationList.h"
#include "EquationGamma.h"
#include "VirtualEquationSpline.h"
#include "EquationSpline_X.h"
#include "EquationSpline_XY.h"
#include "EquationSpline_XYZ.h"
#include "EquationSoundSignal.h"
#include "EquationSinus3D.h"

void	RegisterLibMath(BaseContext *_pContext )
{
	// register all classes that the context can manage:
	const BaseObject::ClassDescription * const descTable[]=
	{	 
		&VirtualEquation::m_Description,
			//&EquationList::m_Description,
			&EquationGamma::m_Description,
			&VirtualEquationSpline::m_Description,
			&EquationSpline_X::m_Description,
				&EquationSpline_XY::m_Description,
					&EquationSpline_XYZ::m_Description,
			&EquationSoundSignal::m_Description,
			&EquationSinus3D::m_Description,
		0L
	};
	_pContext->RegisterClassList( descTable);
}
