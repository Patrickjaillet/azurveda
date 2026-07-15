/*! \file 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
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
	// FIND CURRENT VALUE;
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
		dateDelta = fabs(dateDelta);
		if(realvolume>m_LastValueFound)
			dateDelta*=12.0f;	// go faster when going up.
		else
			dateDelta*=8.0f;	// slower when down.

		if(dateDelta>v1p0)  dateDelta=v1p0;
		vol = realvolume*dateDelta + m_LastValueFound*(v1p0-dateDelta);
		m_LastValueFound = vol;
		m_LastDateComputed = _IncomingParameterTable[3] ;
	}
	amp = mSer_VolumeScale.Get(1)- mSer_VolumeScale.Get(0);
	// rescale values:
	vol = (vol*amp)+mSer_VolumeScale.Get(0);
	unsigned int flags = mSer_Flags.Get();

	if(flags & bApplyFlag_VolToX) _OutgoingParameterTable[0] += vol;
	if(flags & bApplyFlag_VolToY) _OutgoingParameterTable[1] += vol;
	if(flags & bApplyFlag_VolToZ) _OutgoingParameterTable[2] += vol;
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. This is done with this method. sub classes can implement it (or not) in
			any way, to explicit current shape of an object.<br>

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
	\param	_pPreviewConfiguration
*/
void EquationXMEvent::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	// classic drawing.
	//VirtualEquation::ProcessPreview(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);
	ProcessPreview_DrawEquation(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);

}
#endif