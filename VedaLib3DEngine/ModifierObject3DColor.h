/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_ModifierObject3DColor_H
#define COM_M4NKIND_ModifierObject3DColor_H
#include "ModifierObject3DVirtual.h"

//#include "PackFLoatByte_Color.h"
#include "PackDynamicFloat.h"
/*!
	\class	ModifierObject3DColor
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Modifier that would shade the color of an object
			in realtime.
*/

class ModifierObject3DColor : public ModifierObject3DVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	ModifierObject3DColor(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(ModifierObject3DColor);

	/*
		\brief	Apply a modification on a vertex (shape or color or anything).
		\param	_pVertexList first vertex to modify
		\param	_NbVertex number of vertex in list.
		\param	_framedate	date in seconds.
	*/
	virtual	void Modify( VirtualMachine::InternalVertex *_pVertexList,
							const unsigned int _NbVertex,
							float _framedate);

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! dynamics for color:
	PackDynamicFloat	mSer_BaseColor_RGB;
	//!  dynamics for transparency:
	PackDynamicFloat	mSer_BaseColor_Alpha;
	//! optimisation trick.
	float	m_LastComputedDate;
	//! current date values.
	float	m_dateRGBA[4];

};

#endif
