// SPDX-License-Identifier: LGPL-2.1-only

#include "XMSoundObject.h"
#include "uniminixm_functions.h"

void *uniminixm_CleanAlloc(unsigned int _bytesize)
{
	unsigned char *pnew = new unsigned char[_bytesize];
	if(!pnew) return 0L;
	unsigned int ii;
	for(ii=0;ii<_bytesize;ii++) pnew[ii]=0;
	return (void* )pnew;
}

void uniminixm_FreeAlloc(void *_pAlloc)
{
	delete [] _pAlloc;
}

#ifndef NULL
#define	NULL	0L
#endif

BASEOBJECT_DECLARE_CLASS( "XM", XMSoundObject, VirtualMedia,"Sound Music XM File","XM","XM File Sound support" );

XMSoundObject::XMSoundObject() : VirtualMedia()
	,mSer_SampleOverrideList( &NewSampleOverrideReference )
	,m_pXM(0L)
{

	REGISTER_MEMBER_RESOURCE( mSer_XMFile,"XMFile","xm" );

	REGISTER_MEMBER(mSer_TimeLength,0L);
	REGISTER_MEMBER(mSer_SampleOverrideList,0L );
}

XMSoundObject::SampleOverrideReference::SampleOverrideReference() : PackStruct()

{
	REGISTER_MEMBER_REFERENCE( mSer_SampleReference,"SampleSubXM",VirtualMedia::m_Description);
	REGISTER_MEMBER_PACKULONG( mSer_InstrumentIndex,"Instrument",0);
	REGISTER_MEMBER_PACKULONG( mSer_SampleIndex,"Sample",0);
	REGISTER_MEMBER_PACKULONG( mSer_Note,"ToneDelta",0);
	REGISTER_MEMBER_PACKFLOAT( mSer_SecLength,"LengthSec",2.0f);

}

BaseType *XMSoundObject::NewSampleOverrideReference()
{ return new SampleOverrideReference(); }

bool XMSoundObject::CreateInternal(void)
{

	const unsigned char *pFileChunk;
	unsigned int	fileSize;
	mSer_XMFile.GetBinaryAtOffset(pFileChunk,fileSize);
	if(!pFileChunk)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_CANTOPENFILE();
		return false;
	}

	m_pXM = new sUniMiniXM;
	if(!m_pXM)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
		return false;
	}
	eXMMusicInitResult res = UniMiniXM_Init(
								m_pXM,	pFileChunk,	fileSize,
								(int)(GetMachine()->GetPlayFrequency()));
	if(res == eResult_MemoryError )
	{	BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
		return false;
	}
	if(res == eResult_FileFormatError )
	{	BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR("Unvalid XM File");
		return false;
	}

	PackList::Cell *pCell = mSer_SampleOverrideList.GetFirstCell();
	while(pCell)
	{
		SampleOverrideReference *pSampleOverideRef =
				(SampleOverrideReference *)pCell->GetManagedObject();
		VirtualMedia *pSampleMedia = (VirtualMedia *) pSampleOverideRef->mSer_SampleReference.GetObjectPointer();

		VirtualMachine::SoundBufferToAddYourSignal soundBufferStruct;
		float basefreq = 22050.0f ;
		int bufferlength =  (int)(basefreq * pSampleOverideRef->mSer_SecLength.Get());
		if(bufferlength<=0 || pSampleMedia==0L)
		{
			pCell = pCell->GetNextBrotherCell();
			continue;
		}
		soundBufferStruct.m_StartPlayTime_SecondUnit=
		soundBufferStruct.m_StartPlayTime_SecondFrac=0;
		unsigned int endsec =  (unsigned int)(pSampleOverideRef->mSer_SecLength.Get()*65536.0f);
		soundBufferStruct.m_EndPlayTime_SecondUnit= endsec>>16;
		soundBufferStruct.m_EndPlayTime_SecondFrac= endsec & 0x0000ffff;
		soundBufferStruct.m_LengthToRender = bufferlength;
		soundBufferStruct.m_PlayFrequency = basefreq;
		float *pBuffer = (float*) uniminixm_CleanAlloc(sizeof(float)*(bufferlength<<1));

		if(!pBuffer)
		{	BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
			return false;
		}
		soundBufferStruct.m_pSoundBuffer = pBuffer ;

		pSampleMedia->ProcessSoundInterupt( soundBufferStruct );

		for(int ii=0;ii<bufferlength ; ii++)
		{
			pBuffer[ii]=(pBuffer[ii<<1]+pBuffer[(ii<<1)+1])*0.5f;
		}

		UniMiniXM_ReplaceSample(m_pXM,
							pSampleOverideRef->mSer_InstrumentIndex.Get(),
							pSampleOverideRef->mSer_SampleIndex.Get(),
							pBuffer,
							bufferlength,
							pSampleOverideRef->mSer_Note.Get());

		delete [] pBuffer;

		pCell = pCell->GetNextBrotherCell();
	}
#ifdef _UNIMINIXM_SEEKTIME_
	mSer_TimeLength.Set(m_pXM->m_TimeLength);
#endif
	m_MediaTimeLength = mSer_TimeLength.Get();

#ifdef _ENGINE_EDITABLE_
	m_ObjectInfoLine.Set("XM NbChan:");
	m_ObjectInfoLine.AddInt(m_pXM->m_NumberOfChannels);
	m_ObjectInfoLine.AddString(" NbSec:");
	m_ObjectInfoLine.AddInt((int)m_MediaTimeLength);
	m_ObjectInfoLine.AddString(" PatLong:");
	m_ObjectInfoLine.AddInt(m_pXM->m_PatternLength);
	m_ObjectInfoLine.AddString(" NbInst:");
	m_ObjectInfoLine.AddInt(m_pXM->m_NumberOfInstruments);
#endif
	return(true);
}
#ifdef _ENGINE_EDITABLE_

void XMSoundObject::CloseInternal(void)
{

	VirtualMedia::CloseInternal();
	if(m_pXM)
	{
		UniMiniXM_Close( m_pXM );
		delete m_pXM;
		m_pXM=0L;
	}

}
#endif

int		XMSoundObject::GetCurrentPatternNumber()
{
	return m_pXM->m_Position ;
}

int		XMSoundObject::GetCurrentRowIndex()
{
	return m_pXM->m_LineIndex ;
}

void XMSoundObject::ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )
{
	unsigned int lengthToRender = _SoundBufferToAddYourSignal.m_LengthToRender;
	float	*pBufferToWrite = _SoundBufferToAddYourSignal.m_pSoundBuffer;

	while(lengthToRender>0)
	{

		if(UniMiniXM_GetSamplesLeftToMix(m_pXM) <= 0 )
		{

#ifdef _UNIMINIXM_SEEKTIME_

			int startX256 =
				((_SoundBufferToAddYourSignal.m_StartPlayTime_SecondUnit)<<8) |
				(_SoundBufferToAddYourSignal.m_StartPlayTime_SecondFrac>>24) ;
			int endX256 =
				((_SoundBufferToAddYourSignal.m_EndPlayTime_SecondUnit)<<8) |
				(_SoundBufferToAddYourSignal.m_EndPlayTime_SecondFrac>>24) ;
			int nowX256 = startX256 -
						(((startX256-endX256)*((int)lengthToRender))/((int)_SoundBufferToAddYourSignal.m_LengthToRender)) ;
			UniMiniXM_SeekTime(m_pXM,nowX256);
#endif

			UniMiniXM_RefreshTick(m_pXM);
		}
		if(UniMiniXM_GetSamplesLeftToMix(m_pXM)<=lengthToRender)
		{
			unsigned int stillToBeMixed = UniMiniXM_GetSamplesLeftToMix(m_pXM);
			UniMiniXM_AddSoundOnStereoFloatBuffer(m_pXM,
							(sFloatStereoSample *)pBufferToWrite,
							stillToBeMixed);
			pBufferToWrite+=(stillToBeMixed<<1);
			lengthToRender -= stillToBeMixed ;
		} else
		{

			UniMiniXM_AddSoundOnStereoFloatBuffer(m_pXM,
							(sFloatStereoSample *)pBufferToWrite,
							lengthToRender);
			pBufferToWrite+=(lengthToRender<<1);

			break;
		}
	}

}
#ifdef _ENGINE_EDITABLE_

void XMSoundObject::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

	float	*pSoundBuffer=0L;
	unsigned int bufferlength=0;
	GetMachine()->GetCurrentSoundBufferImage( &pSoundBuffer , &bufferlength );
	ProcessPreview_DrawSoundBuffer( _pPreviewViewPort, pSoundBuffer, bufferlength );

}
#endif

void	XMSoundObject::AddCurrentMixedVolume(unsigned int _instrument,
								unsigned int _sample, float &_volume)
{
	UniMiniXM_AddCurrentMixedVolume(m_pXM,_instrument,_sample,&_volume);

}
