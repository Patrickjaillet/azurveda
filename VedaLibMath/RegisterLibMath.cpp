// SPDX-License-Identifier: LGPL-2.1-only

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

	const BaseObject::ClassDescription * const descTable[]=
	{
		&VirtualEquation::m_Description,

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
