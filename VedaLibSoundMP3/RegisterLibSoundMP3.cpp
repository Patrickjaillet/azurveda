// SPDX-License-Identifier: LGPL-2.1-only

#include "BaseContext.h"
#include "VirtualMedia.h"
#include "MP3SoundObject.h"

void	RegisterLibSoundMP3(BaseContext *_pContext )
{

	const BaseObject::ClassDescription * const descTable[]=
	{

		& VirtualMedia::m_Description,
			& MP3SoundObject::m_Description,
		0L
	};
	_pContext->RegisterClassList( descTable);
}
