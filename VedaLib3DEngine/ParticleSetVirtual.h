/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_ParticleSetVirtual_H
#define COM_M4NKIND_ParticleSetVirtual_H

#include "BaseObject.h"
#include "PackLong_WithLimits.h"
/*!
	\class	ParticleSetVirtual
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	virtual Class defining a generic Particle set.
*/
class ParticleSetVirtual : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	ParticleSetVirtual(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(ParticleSetVirtual);
	/*!
		\class	Particle
		\brief	
	*/
	class Particle
	{	public:
		float	m_xyzd[4];
	};

	/*!
		\brief	get a particle in its current state.
		\param	_index	the particle index in the population.
	*/
	virtual void	SetFrameDate( float _frameDate )=0;

	/*!
		\brief	get current number of particle you can get with Get()
		\return current max.
	*/
	inline unsigned int	GetNumberOfParticle(){ return m_NumberOfActiveParticle; };

	/*!
		\brief	get a particle in its current state.
		\param	_index	the particle index in the population.
	*/
	inline Particle	*Get( int _index ){ return &(m_pCurrentParticleSet[_index]); };

/*==================================================================
								PROTECTED
==================================================================*/
protected:

	//! current number of Particle.
	PackLong_WithLimits		mSer_MaxNumberOfParticle;

	//! number of active part.
	unsigned int			m_NumberOfActiveParticle;
	//! optimization....
	float					m_OneOverNumberOfActiveParticle;

	//! last frame date computed.
	float					m_LastFrameDate;

	//! a particle set that can be accessed.
	Particle				*m_pCurrentParticleSet;

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	virtual void	CloseInternal(void);
#endif

};

#endif
