// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_MediaTimePattern_H
#define COM_M4NKIND_MediaTimePattern_H

#include "VirtualMedia.h"
#include "VirtualMachine.h"

#include "PackFloat.h"
#include "PackList_TimeBlockTrack.h"
#include "PackLong.h"
#include "PackObjectReference.h"

class MediaTimePattern : public VirtualMedia
{

public:

	MediaTimePattern(void);

	BASEOBJECT_DEFINE_CLASS(MediaTimePattern);

	virtual void ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort );

	virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal );

	class PartToPlay : public PackList_TimeBlockTrack::TimeBlockTrackElement
	{
	public:

		PackObjectReference	mSer_ReferenceToPlay;

		PackFloat_FixedPoint mSer_TimeOffset;

		PackFloat_FixedPoint mSer_TimeSpeed;

		PartToPlay(void) : PackList_TimeBlockTrack::TimeBlockTrackElement()
			,mSer_TimeSpeed(PackFloat::m_0p0625)
		{
			REGISTER_MEMBER_REFERENCE(mSer_ReferenceToPlay ,"MediaToPlay", VirtualMedia::m_Description );
			REGISTER_MEMBER( mSer_TimeOffset,"Offset");
			REGISTER_MEMBER_PACKFLOAT( mSer_TimeSpeed,"Speed",1.0f);
		};
	};

	static PackList_TimeBlockTrack::TimeBlockTrackElement *NewPartToPlay(){ return new PartToPlay(); };

	static BaseType *NewTrack();

#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview_DrawTracks(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview_CreateSquare(VirtualMachine::InternalObject3DBuffer **_ppVertexAndPolygonBufferOut);
#endif

protected:

	PackList		mSer_TrackList;

	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_

	virtual void CloseInternal(void);
#endif

#ifdef _ENGINE_EDITABLE_

	VirtualMachine::InternalObject3DBuffer	*m_pPreviewSquareObject;
#endif

};
#endif
