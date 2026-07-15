/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "BaseContext.h"

// libImage:
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
	// register all classes that the context can manage:
	const BaseObject::ClassDescription * const descTable[]=
	{	 
		// LibImage
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
