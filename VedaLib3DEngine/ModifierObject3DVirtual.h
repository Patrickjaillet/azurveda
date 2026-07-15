/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_ModifierObject3DVirtual_H
#define COM_M4NKIND_ModifierObject3DVirtual_H
#include "BaseObject.h"
#include "VirtualMachine.h"

#include "PackFloatByte_Limits.h"
#include "PackDynamicType.h"
#include "PackLong_WithLimits.h"
#include "PackULong_Flags.h"
#include "PackTreeCell.h"
/*!
	\class	ModifierObject3DVirtual
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Virtual Model for Modifier objects that would
			 deform, or change in any way, a 3D Renderable 
			 Polygon object such as Object3DMeshVirtual.
*/

class ModifierObject3DVirtual : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	//ModifierObject3DVirtual(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(ModifierObject3DVirtual);

	/*
		\brief	Apply a modification on a vertex (shape or color or anything).
		\param	_pVertexList first vertex to modify
		\param	_NbVertex number of vertex in list.
		\param	_framedate	date in seconds.
	*/
	virtual	void Modify( VirtualMachine::InternalVertex *_pVertexList,
							const unsigned int _NbVertex,
							float _framedate)=0;

/*==================================================================
								PROTECTED
==================================================================*/
protected:

};

#endif
