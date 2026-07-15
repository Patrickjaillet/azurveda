/*! \file 
	\author victorien ferry & www.m4nkind.com 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "BaseContext.h"

// libImage:
#include "VirtualImage.h"
#include "ImageJPEG.h"

void	RegisterLibImageJPEG(BaseContext *_pContext )
{
	// register all classes that the context can manage:
	const BaseObject::ClassDescription * const descTable[]=
	{	 
		// LibImage
		& VirtualImage::m_Description,
			&ImageJPEG::m_Description,
		0L
	};
	_pContext->RegisterClassList( descTable);
}
