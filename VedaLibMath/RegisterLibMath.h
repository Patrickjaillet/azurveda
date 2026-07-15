/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_RegisterLibMath_H
#define COM_M4NKIND_RegisterLibMath_H

class BaseContext;
/*!
	\file	RegisterLibMath.h
	\brief	Register LibMath classes to a BaseContext.
*/
/*!
	\addtogroup	BaseObjectInherited_VedaLibMath BaseObject Inherited Classes For VedaLibMath
	\ingroup	BaseObjectInheritedDocByLib
	\brief		VedaLibMath defines VirtualEquation which method VirtualEquation::Compute()
				gives a model for any 4 dimensions equations.
				It unifies many other equations classes In this library.\n
				This Library Package and classes are under the\n
				<b>GNU LESSER GENERAL PUBLIC LICENSE Version 2.1</b>
*/
/*!
	\ingroup	BaseObjectInherited_VedaLibMath
	\brief	Register LibMath classes to a context, with BaseContext::RegisterClassList().
	\param	_pContext where to register class.
*/
void	RegisterLibMath(BaseContext *_pContext );

#endif