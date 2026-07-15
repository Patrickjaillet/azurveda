/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_RegisterLib3DEngine_H
#define COM_M4NKIND_RegisterLib3DEngine_H

class BaseContext;
/*!
	\file	RegisterLib3DEngine.h
	\brief	Register Lib3DEngine classes to a BaseContext.
*/
/*!
	\addtogroup	BaseObjectInherited_VedaLib3DEngine BaseObject Inherited Classes For VedaLib3DEngine
	\ingroup	BaseObjectInheritedDocByLib
	\brief		VedaLib3DEngine defines classes for a simple 3D Engine,
				 originaly designed for experimentations.
				This Library Package and classes are under the\n
				<b>GNU LESSER GENERAL PUBLIC LICENSE Version 2.1</b>
*/
/*!
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Register Lib3DEngine classes to a context, with BaseContext::RegisterClassList().
			It also registers: LibMath and LibImage
	\param	_pContext  where to register class.
*/
void	RegisterLib3DEngine(BaseContext *_pContext );

#endif