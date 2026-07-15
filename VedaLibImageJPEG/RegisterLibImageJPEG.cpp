// SPDX-License-Identifier: LGPL-2.1-only

#include "BaseContext.h"

#include "VirtualImage.h"
#include "ImageJPEG.h"

void	RegisterLibImageJPEG(BaseContext *_pContext )
{

	const BaseObject::ClassDescription * const descTable[]=
	{

		& VirtualImage::m_Description,
			&ImageJPEG::m_Description,
		0L
	};
	_pContext->RegisterClassList( descTable);
}
