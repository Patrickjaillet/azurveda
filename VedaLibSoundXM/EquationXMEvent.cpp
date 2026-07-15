// SPDX-License-Identifier: LGPL-2.1-only

#include "EquationXMEvent.h"
#include "XMSoundObject.h"
#include "PackFloat.h"
#include <math.h>
BASEOBJECT_DECLARE_CLASS( "xme", EquationXMEvent, VirtualEquation,"Equation XM Event","XMEvent","Describes a curve by using XM music events.");

EquationXMEvent::EquationXMEvent() : VirtualEquation()
	,mSer_VolumeScale(PackFloat::vd_XY)
	,m_LastDateComputed(-1.0f)
	,m_LastValueFound(0.0f)
{
	REGISTER_MEMBER_REFERENCE( mSer_XMMusicReference,"XM Music",XMSoundObject::m_Description);
	REGISTER_MEMBER_PACKULONG( mSer_InstrumentNumber,"Instrument",1);
	REGISTER_MEMBER_PACKULONG( mSer_SampleNumber,"Sample",1);
	REGISTER_MEMBER_FLAG( mSer_Flags ,"ApplyVolume",bApplyFlag_VolToX,"VolToX.VolToY.VolToZ" );
	REGISTER_MEMBER_PACKFLOAT_XY( mSer_VolumeScale ,"VolumeScale",0.0f,1.0f );
}

void EquationXMEvent::Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )
{
	XMSoundObject *pXM = (XMSoundObject *)mSer_XMMusicReference.GetObjectPointer();
	if(!pXM) return;

	float vol,amp;
	if(_IncomingParameterTable[3] == m_LastDateComputed)
	{
		vol = m_LastValueFound;
	}else
	{
		float	realvolume=PackFloat::m_0p0;
		const float	v1p0 = PackFloat::m_1p0;
		pXM->AddCurrentMixedVolume(
			mSer_InstrumentNumber.Get(),
			mSer_SampleNumber.Get(),realvolume);
		float dateDelta = _IncomingParameterTable[3]-m_LastDateComputed ;
		dateDelta = fabsf(dateDelta);
		if(realvolume>m_LastValueFound)
			dateDelta*=12.0f;
		else
			dateDelta*=8.0f;

		if(dateDelta>v1p0)  dateDelta=v1p0;
		vol = realvolume*dateDelta + m_LastValueFound*(v1p0-dateDelta);
		m_LastValueFound = vol;
		m_LastDateComputed = _IncomingParameterTable[3] ;
	}
	amp = mSer_VolumeScale.Get(1)- mSer_VolumeScale.Get(0);

	vol = (vol*amp)+mSer_VolumeScale.Get(0);
	unsigned int flags = mSer_Flags.Get();

	if(flags & bApplyFlag_VolToX) _OutgoingParameterTable[0] += vol;
	if(flags & bApplyFlag_VolToY) _OutgoingParameterTable[1] += vol;
	if(flags & bApplyFlag_VolToZ) _OutgoingParameterTable[2] += vol;
}
#ifdef _ENGINE_EDITABLE_

void EquationXMEvent::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

	ProcessPreview_DrawEquation(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);

}
#endif
