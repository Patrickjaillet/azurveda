/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_RegisterLibMP3_H
#define COM_M4NKIND_RegisterLibMP3_H

/*!
	\addtogroup	BaseObjectInherited_VedaLibSoundMP3 BaseObject Inherited Classes For VedaLibSoundMP3
	\ingroup	BaseObjectInheritedDocByLib
	\brief		VedaLibSoundMP3 provides a BaseObject extension that
				plays a sound signal from a .mp3 file resource, using
				libmad. The source used for playing may vary in the future.
				Read Readme files for more informations.
				This Library Package and classes are under the\n
				<b>GNU GENERAL PUBLIC LICENSE Version 2, June 1991</b>
*/
class BaseContext;
/*!
	\ingroup	BaseObjectInherited_VedaLibSoundMP3
	\brief	Register classes for a VedaLibSoundMP3 context
*/
void	RegisterLibSoundMP3(BaseContext *_pContext );

#endif
