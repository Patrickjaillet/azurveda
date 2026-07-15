/*! \file 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "BaseContext.h"

#include "VirtualMedia.h"
#include "XMSoundObject.h"

#include "VirtualEquation.h"
#include "EquationXMEvent.h"
void	RegisterLibSoundXM(BaseContext *_pContext )
{
	// register all classes that the context can manage:
	const BaseObject::ClassDescription * const descTable[]=
	{	 
		// LibBAOOM music
		& VirtualMedia::m_Description,
			& XMSoundObject::m_Description,
//		& VirtualEquation::m_Description,
//			&EquationXMEvent::m_Description,
		0L
	};
	_pContext->RegisterClassList( descTable);
}
