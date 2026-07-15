// SPDX-License-Identifier: LGPL-2.1-only

#include "BaseContext.h"

#include "VirtualImage.h"
#include "RGBAImage.h"
#include "GreyImage.h"
#include "GreyImage2nSquareSized.h"
#include "ProceduralImage.h"
#include "PerlinNoiseImage.h"
#include "BubbleGreyImage.h"
#include "ImplicitEquationImage.h"
void	RegisterLibImage(BaseContext *_pContext )
{

	const BaseObject::ClassDescription * const descTable[]=
	{

		& VirtualImage::m_Description,
			&RGBAImage::m_Description,
			&GreyImage::m_Description,
				&GreyImage2nSquareSized::m_Description,
					&ProceduralImage::m_Description,
				&PerlinNoiseImage::m_Description,
				&BubbleGreyImage::m_Description,
				&ImplicitEquationImage::m_Description,
		0L
	};
	_pContext->RegisterClassList( descTable);
}
