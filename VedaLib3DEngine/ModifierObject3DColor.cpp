// SPDX-License-Identifier: LGPL-2.1-only

#include "ModifierObject3DColor.h"
#include "PackFloat.h"
BASEOBJECT_DECLARE_CLASS( "cl", ModifierObject3DColor, ModifierObject3DVirtual,"3DModifier Color","mdfColor","Modify the colors and transparency of a dynamic 3D Objects, through 3DObjectScene Shape Modifier List.");

ModifierObject3DColor::ModifierObject3DColor() : ModifierObject3DVirtual()
	,m_LastComputedDate(PackFloat::m_Max)
	,mSer_BaseColor_RGB(PackFloat::vd_XYZ)
	,mSer_BaseColor_Alpha(PackFloat::vd_X)
{
	REGISTER_MEMBER_PACKDYNAMICFLOAT_XYZ(mSer_BaseColor_RGB,"Color",1.0f,1.0f,1.0f);
	REGISTER_MEMBER_PACKDYNAMICFLOAT(mSer_BaseColor_Alpha,"Transparency",1.0f);

}

void ModifierObject3DColor::Modify( VirtualMachine::InternalVertex *_pVertexList,
						const unsigned int _NbVertex,
						float _framedate)
{
	float	param_in[4],param_out[4];
	const float	v0 = PackFloat::m_0p0;
	param_in[3]=_framedate;
	unsigned int ii;
	for( ii=0 ; ii<_NbVertex ; ii++ )
	{
		param_in[0]=_pVertexList->m_x;
		param_in[1]=_pVertexList->m_y;
		param_in[2]=_pVertexList->m_z;
		if(mSer_BaseColor_RGB.GetSelectedIndex()!=0)
		{
		param_out[0]=param_out[1]=param_out[2]=v0;
		mSer_BaseColor_RGB.Compute(param_out,param_in);
		_pVertexList->m_ColorRGBA[0]*= param_out[0];
		_pVertexList->m_ColorRGBA[1]*= param_out[1];
		_pVertexList->m_ColorRGBA[2]*= param_out[2];
		}

		if(mSer_BaseColor_Alpha.GetSelectedIndex()!=0)
		{
			param_out[0]=v0;
			mSer_BaseColor_Alpha.Compute(param_out,param_in);
			_pVertexList->m_ColorRGBA[3]*= param_out[0];
		}
		_pVertexList++;
	}
}
