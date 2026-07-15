/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_RegisterLibImage_H
#define COM_M4NKIND_RegisterLibImage_H

class BaseContext;
/*!
	\addtogroup	BaseObjectInherited_VedaLibImage BaseObject Inherited Classes For VedaLibImage
	\ingroup	BaseObjectInheritedDocByLib
	\brief		VedaLibImage provides an abstract definition for 2D Images.
				Through class VirtualImage, and many classes that create procedural 
				images.
				This Library Package and classes are under the\n
				<b>GNU LESSER GENERAL PUBLIC LICENSE Version 2.1</b>
*/
/*!
	\ingroup BaseObjectInherited_VedaLibImage
	\brief	Register LibImage classes to a context, with BaseContext::RegisterClassList().
			It also Registers LibMath.
	\param	_pContext where to register class.
*/
void	RegisterLibImage(BaseContext *_pContext );

#endif