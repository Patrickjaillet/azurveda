/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "BaseContext.h"
#include "VirtualMedia.h"
#include "MP3SoundObject.h"

void	RegisterLibSoundMP3(BaseContext *_pContext )
{
	// register all classes that the context can manage:
	const BaseObject::ClassDescription * const descTable[]=
	{	 
		// LibBAOOM music
		& VirtualMedia::m_Description,
			& MP3SoundObject::m_Description,
		0L
	};
	_pContext->RegisterClassList( descTable);
}
