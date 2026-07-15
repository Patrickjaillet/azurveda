/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "ParticleSetEquation.h"
#include "VirtualEquation.h"
#include "PackFloat.h"
BASEOBJECT_DECLARE_CLASS( "q", ParticleSetEquation, ParticleSetVirtual,"3D Particle Set Equation","ParticleSet","Define a population set with an equation (InputX=index)" );

ParticleSetEquation::ParticleSetEquation() : ParticleSetVirtual()
{
	REGISTER_MEMBER_REFERENCE( mSerRef_Equation ,"Equation", VirtualEquation::m_Description );
	
	
}
/*!
	\brief	get a particle in its current state.
	\param	_index	the particle index in the population.
*/
void	ParticleSetEquation::SetFrameDate( float _frameDate )
{

	if(_frameDate == m_LastFrameDate) return ;
	m_LastFrameDate = _frameDate;
	VirtualEquation *pEquation = (VirtualEquation *) mSerRef_Equation.GetObjectPointer() ;
	if( pEquation )
	{
		float	in[4];
		float	findex= PackFloat::m_0p0;
		for(unsigned int ii=0 ; ii<m_NumberOfActiveParticle ; ii++ )
		{
			in[0] = findex;
			in[3] = _frameDate;
			 m_pCurrentParticleSet[ii].m_xyzd[0]=
			 m_pCurrentParticleSet[ii].m_xyzd[1]=
			 m_pCurrentParticleSet[ii].m_xyzd[2]=
			 m_pCurrentParticleSet[ii].m_xyzd[3]=PackFloat::m_0p0;
			 pEquation->Compute( m_pCurrentParticleSet[ii].m_xyzd , in );
			findex+=m_OneOverNumberOfActiveParticle;
		}
	}
} 
/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by CreateInternal().
*//*
bool ParticleSetEquation::CreateInternal(void)
{
	
	return ParticleSetVirtual::CreateInternal();
}*/
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*//*
void	ParticleSetEquation::CloseInternal(void)
{

}*/
#endif