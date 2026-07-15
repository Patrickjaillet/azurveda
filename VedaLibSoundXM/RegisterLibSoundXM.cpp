// SPDX-License-Identifier: LGPL-2.1-only

#include "BaseContext.h"

#include "VirtualMedia.h"
#include "XMSoundObject.h"

#include "VirtualEquation.h"
#include "EquationXMEvent.h"
void	RegisterLibSoundXM(BaseContext *_pContext )
{

	const BaseObject::ClassDescription * const descTable[]=
	{

		& VirtualMedia::m_Description,
			& XMSoundObject::m_Description,

		0L
	};
	_pContext->RegisterClassList( descTable);
}
