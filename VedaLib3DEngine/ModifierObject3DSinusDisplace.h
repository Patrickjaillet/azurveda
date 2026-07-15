/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_ModifierObject3DSinusDisplace_H
#define COM_M4NKIND_ModifierObject3DSinusDisplace_H
#include "ModifierObject3DVirtual.h"
#include "PackDynamicFloat.h"

/*!
	\class	ModifierObject3DSinusDisplace
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Modifier that would deform an object
			in realtime.
*/

class ModifierObject3DSinusDisplace : public ModifierObject3DVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	ModifierObject3DSinusDisplace(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(ModifierObject3DSinusDisplace);

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
	//! a center to scroll.(X,Y,Z)
	PackDynamicFloat	mSer_Center;
	//! the distance frequence for modifying.(X,Y,Z)
	PackDynamicFloat	mSer_Frequence;
	//!  the distance frequence for modifying.(X,Y,Z)
	PackDynamicFloat	mSer_Amplitude;
	//! optimization
	float	m_LastComputedDate;
	//! computed result
	float	m_Center[4];
	//! computed result
	float	m_Freq[4];
	//! computed result
	float	m_Amp[4];

};

#endif
