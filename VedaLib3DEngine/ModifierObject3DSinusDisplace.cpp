// SPDX-License-Identifier: LGPL-2.1-only

#include "ModifierObject3DSinusDisplace.h"
#include "PackFloat.h"
#include <math.h>
#ifndef sinf
#define	sinf sin
#endif
BASEOBJECT_DECLARE_CLASS( "sd", ModifierObject3DSinusDisplace, ModifierObject3DVirtual,"3DModifier Displace Sinus","DisSinus","Modify the vertex position of a dynamic 3D Objects, through 3DObjectScene Shape Modifier List.");

ModifierObject3DSinusDisplace::ModifierObject3DSinusDisplace() : ModifierObject3DVirtual()
	,mSer_Center(PackFloat::vd_XYZ)
	,mSer_Frequence(PackFloat::vd_XYZ)
	,mSer_Amplitude(PackFloat::vd_XYZ)
	,m_LastComputedDate(PackFloat::m_Max)
{
  REGISTER_MEMBER_PACKDYNAMICFLOAT_XYZ(mSer_Center,"Center",0.0f,0.0f,0.0f);
  REGISTER_MEMBER_PACKDYNAMICFLOAT_XYZ(mSer_Frequence,"Frequence",1.2f,1.2f,1.2f);
  REGISTER_MEMBER_PACKDYNAMICFLOAT_XYZ(mSer_Amplitude,"Amplitude",0.25f,0.25f,0.25f);

}

void ModifierObject3DSinusDisplace::Modify( VirtualMachine::InternalVertex *_pVertexList,
						const unsigned int _NbVertex,
						float _framedate)
{
	if(m_LastComputedDate != _framedate)
	{
		float	param[4];
		const float	v0=PackFloat::m_0p0;
		param[0]=param[1]=param[2]=v0;
		param[3]=_framedate;
		m_Center[0]=m_Center[1]=m_Center[2]=v0;
		m_Freq[0]=m_Freq[1]=m_Freq[2]=v0;
		m_Amp[0]=m_Amp[1]=m_Amp[2]=v0;
		mSer_Center.Compute(m_Center,param);
		mSer_Frequence.Compute(m_Freq,param);
		mSer_Amplitude.Compute(m_Amp,param);
		m_Freq[0] *= _framedate;
		m_Freq[1] *= _framedate;
		m_Freq[2] *= _framedate;
		m_LastComputedDate = _framedate;
	}
	unsigned int ii;
	for( ii=0 ; ii<_NbVertex ; ii++ )
	{
		float xx=(_pVertexList->m_x-m_Center[0])*m_Freq[0];
		float yy=(_pVertexList->m_y-m_Center[1])*m_Freq[1];
		float zz=(_pVertexList->m_z-m_Center[2])*m_Freq[2];

		_pVertexList->m_x += m_Amp[0]*(sinf(yy)*sinf(zz));
		_pVertexList->m_y += m_Amp[1]*(sinf(zz)*sinf(xx));
		_pVertexList->m_z += m_Amp[2]*(sinf(xx)*sinf(yy));
		_pVertexList++;
	}

}
