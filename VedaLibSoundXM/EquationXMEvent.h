// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_EquationXMEvent_H
#define COM_M4NKIND_EquationXMEvent_H

#include "VirtualEquation.h"
#include "PackObjectReference.h"
#include "PackFloat.h"
#include "PackULong_Flags.h"
#include "PackList_TimeTrack.h"

class PackResource;

class EquationXMEvent : public VirtualEquation
{

public:

	EquationXMEvent();

	BASEOBJECT_DEFINE_CLASS(EquationXMEvent);

	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

protected:

	PackObjectReference	mSer_XMMusicReference;

	PackULong	mSer_InstrumentNumber;

	PackULong	mSer_SampleNumber;
	enum {
		bApplyFlag_VolToX=1,
		bApplyFlag_VolToY=2,
		bApplyFlag_VolToZ=4,
	};

	PackULong_Flags	mSer_Flags;

	PackFloat	mSer_VolumeScale;

	float	m_LastDateComputed;

	float	m_LastValueFound;

	float	m_LastHighestLeft;

	float	m_LastHighestLeftDate;

};
#endif
