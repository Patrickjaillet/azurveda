/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "ParticleSetVirtual.h"
#include "PackFloat.h"
BASEOBJECT_DECLARE_VIRTUALCLASS( "pctl", ParticleSetVirtual, BaseObject );

ParticleSetVirtual::ParticleSetVirtual() : BaseObject()
	,m_pCurrentParticleSet(0L)
	,m_LastFrameDate(-1.0f)
	,m_NumberOfActiveParticle(0L)
{
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_MaxNumberOfParticle,"MaxNumber",16,1,1024 );

}

/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by CreateInternal().
*/
bool ParticleSetVirtual::CreateInternal(void)
{
	m_pCurrentParticleSet = new Particle[mSer_MaxNumberOfParticle.Get() ];
	m_NumberOfActiveParticle = mSer_MaxNumberOfParticle.Get();
	m_OneOverNumberOfActiveParticle = PackFloat::m_1p0/((float)m_NumberOfActiveParticle);
	return true;
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void	ParticleSetVirtual::CloseInternal(void)
{
	if(m_pCurrentParticleSet )
	{
		delete [] m_pCurrentParticleSet;
		m_pCurrentParticleSet = 0L;
	}


}
#endif