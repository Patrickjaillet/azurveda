// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_XMSoundObject_H
#define COM_M4NKIND_XMSoundObject_H

#include "VirtualMedia.h"
#include "PackResource.h"
#include "PackULong.h"
#include "PackFloat.h"
#include "PackList.h"
#include "PackObjectReference.h"

typedef struct _sUniMiniXM sUniMiniXM;

class XMSoundObject : public VirtualMedia
{

public:

	XMSoundObject(void);

	BASEOBJECT_DEFINE_CLASS(XMSoundObject);

	virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal );

	int		GetCurrentPatternNumber();

	int		GetCurrentRowIndex();

	void	AddCurrentMixedVolume(unsigned int _instrument,
									unsigned int _sample,
									float &_volume);

	class SampleOverrideReference : public PackStruct
	{public:

		PackObjectReference		mSer_SampleReference;

		PackULong				mSer_InstrumentIndex;

		PackULong				mSer_SampleIndex;

		PackULong				mSer_Note;

		PackFloat				mSer_SecLength;

		SampleOverrideReference() ;
	};

	static  BaseType *NewSampleOverrideReference();

protected:

	PackResource	mSer_XMFile;

	PackFloat		mSer_TimeLength;

	PackList		mSer_SampleOverrideList;

	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

	sUniMiniXM	*m_pXM;

};

#endif
