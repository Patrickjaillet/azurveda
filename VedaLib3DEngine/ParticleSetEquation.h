/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_ParticleSetEquation_H
#define COM_M4NKIND_ParticleSetEquation_H

#include "ParticleSetVirtual.h"
#include "PackLong_WithLimits.h"
#include "PackObjectReference.h"
/*!
	\class	ParticleSetEquation
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Class defining a Particle set which moves
			according to a VirtualEquation.
*/
class ParticleSetEquation : public ParticleSetVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	ParticleSetEquation(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(ParticleSetEquation);
	/*!
		\brief	get a particle in its current state.
		\param	_index	the particle index in the population.
	*/
	virtual void	SetFrameDate( float _frameDate );

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! reference to an equation that can use X,Y:
	PackObjectReference		mSerRef_Equation;

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	//virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	//virtual void	CloseInternal(void);
#endif

};

#endif
