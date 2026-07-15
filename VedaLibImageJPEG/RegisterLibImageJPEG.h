/*! \file 
	\author victorien ferry & www.m4nkind.com 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_RegisterLibImageJPEG_H
#define COM_M4NKIND_RegisterLibImageJPEG_H

class BaseContext;

/*!
	\addtogroup	BaseObjectInherited_VedaLibImageJPEG BaseObject Inherited Classes For VedaLibImageJPEG
	\ingroup	BaseObjectInheritedDocByLib
	\brief		VedaLibImageJPEG provides a VirtualImage extended class
				that loads resource JPG Files. It is based on jpeg-6b from
				the independant jpeg group. Read Readme files for more informations.
				This Library Package and classes are under the\n
				<b>GNU LESSER GENERAL PUBLIC LICENSE Version 2.1</b>
*/
/*!
	\ingroup BaseObjectInherited_VedaLibImageJPEG
	\brief	Register classes for a LibImageJPEG context
	\param	_pContext context where to init classes.
*/
void	RegisterLibImageJPEG(BaseContext *_pContext );

#endif