// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_ParticleSetVirtual_H
#define COM_M4NKIND_ParticleSetVirtual_H

#include "BaseObject.h"
#include "PackLong_WithLimits.h"

class ParticleSetVirtual : public BaseObject
{

public:

	ParticleSetVirtual(void);

	BASEOBJECT_DEFINE_VIRTUALCLASS(ParticleSetVirtual);

	class Particle
	{	public:
		float	m_xyzd[4];
	};

	virtual void	SetFrameDate( float _frameDate )=0;

	inline unsigned int	GetNumberOfParticle(){ return m_NumberOfActiveParticle; };

	inline Particle	*Get( int _index ){ return &(m_pCurrentParticleSet[_index]); };

protected:

	PackLong_WithLimits		mSer_MaxNumberOfParticle;

	unsigned int			m_NumberOfActiveParticle;

	float					m_OneOverNumberOfActiveParticle;

	float					m_LastFrameDate;

	Particle				*m_pCurrentParticleSet;

	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif

};

#endif
