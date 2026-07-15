/*! \file 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/

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
/*
	\brief	You got to provide this clean memory allocator C function:
*/
void uniminixm_FreeAlloc(void *_pAlloc)
{
	delete [] _pAlloc;
}



#ifndef NULL
#define	NULL	0L
#endif
/*
	Note: code based on shitty  XM engine.
*/
BASEOBJECT_DECLARE_CLASS( "XM", XMSoundObject, VirtualMedia,"Sound Music XM File","XM","XM File Sound support" );

XMSoundObject::XMSoundObject() : VirtualMedia()
	,mSer_SampleOverrideList( &NewSampleOverrideReference )
	,m_pXM(0L)
{
	// register serializable member, the file resource:
	REGISTER_MEMBER_RESOURCE( mSer_XMFile,"XMFile","xm" );	
	//! the time length of the media is precomputed in erditable mode, and just read in non-editable.
	REGISTER_MEMBER(mSer_TimeLength,0L);
	REGISTER_MEMBER(mSer_SampleOverrideList,/*"Sample Override"*/0L );
}

//! \brief subclass constructor . It registers the members
XMSoundObject::SampleOverrideReference::SampleOverrideReference() : PackStruct()
//	,mSer_TimeMultiplier(PackFloat::vd_X,PackFloat::m_1Div64) // fix the serialized floating point precision.
{
	REGISTER_MEMBER_REFERENCE( mSer_SampleReference,"SampleSubXM",VirtualMedia::m_Description);
	REGISTER_MEMBER_PACKULONG( mSer_InstrumentIndex,"Instrument",0);
	REGISTER_MEMBER_PACKULONG( mSer_SampleIndex,"Sample",0);
	REGISTER_MEMBER_PACKULONG( mSer_Note,"ToneDelta",0);
	REGISTER_MEMBER_PACKFLOAT( mSer_SecLength,"LengthSec",2.0f);

}
/*!
	\brief	static method passed to the list to create new elements.
	\return a new element.
*/
BaseType *XMSoundObject::NewSampleOverrideReference()
{ return new SampleOverrideReference(); }	

/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by CreateInternal().
*/
bool XMSoundObject::CreateInternal(void)
{
	// check file resource:
	const unsigned char *pFileChunk;
	unsigned int	fileSize;
	mSer_XMFile.GetBinaryAtOffset(pFileChunk,fileSize);
	if(!pFileChunk)
	{	
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_CANTOPENFILE();
		return false;
	}
	// init/load:
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
	// override sample 
	PackList::Cell *pCell = mSer_SampleOverrideList.GetFirstCell();
	while(pCell)
	{
		SampleOverrideReference *pSampleOverideRef =
				(SampleOverrideReference *)pCell->GetManagedObject();
		VirtualMedia *pSampleMedia = (VirtualMedia *) pSampleOverideRef->mSer_SampleReference.GetObjectPointer();

		// render the sample with the media object:
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
			//new float[bufferlength<<1]; //<<1 because write is stereo.
		if(!pBuffer)
		{	BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
			return false;		
		}
		soundBufferStruct.m_pSoundBuffer = pBuffer ;
		// throw sound buffer creation:
		pSampleMedia->ProcessSoundInterupt( soundBufferStruct );
		// mix stereo signal to mono
		for(int ii=0;ii<bufferlength ; ii++)
		{
			pBuffer[ii]=(pBuffer[ii<<1]+pBuffer[(ii<<1)+1])*0.5f;
		}
		// replace XM sample:
		UniMiniXM_ReplaceSample(m_pXM,
							pSampleOverideRef->mSer_InstrumentIndex.Get(),
							pSampleOverideRef->mSer_SampleIndex.Get(),
							pBuffer,
							bufferlength,
							pSampleOverideRef->mSer_Note.Get());
		//finish with temp sound buffer:
		delete [] pBuffer;
		// next in list:
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
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void XMSoundObject::CloseInternal(void)
{
	// ALL VIRTUAL MEDIA MUST CALL UPPER CloseInternal()
	VirtualMedia::CloseInternal();
	if(m_pXM)
	{	
		UniMiniXM_Close( m_pXM );	
		delete m_pXM;
		m_pXM=0L;
	}
		
		
}
#endif


/*!
	\brief	 Process a media at a given date, 
			or do anything) using a machine. You must process the media on machine this->GetMachine().
			Note there are no play/pause or speed concept here, by default, a date is passed.

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pViewPort the viewport to render. Can't be 0L.
*/
/*
void XMSoundObject::ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort )
{

}*/
/*
	\brief when playing, return the current pattern number which is played.
	\return pattern index.
*/
int		XMSoundObject::GetCurrentPatternNumber()
{
	return m_pXM->m_Position ;
}
/*
	\brief when playing, return the current row index in the pattern.
		Watch out, the maximum row by pattern vary.
	\return the row.
*/
int		XMSoundObject::GetCurrentRowIndex()
{
	return m_pXM->m_LineIndex ;
}
/*!
	\brief	 If the media had to generate real time sounds, 
			ProcessSoundInterupt will be called by a machine. There is a default 
			behaviour for this method, which does nothing. the float buffer 
			accumulate the whole machine sound, and in the better case, it will
			stand in cache.	_TimeLengthSec give a timelenth of the time played. 
	\param	_SoundBufferToAddYourSignal
*/
void XMSoundObject::ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )
{
	unsigned int lengthToRender = _SoundBufferToAddYourSignal.m_LengthToRender;
	float	*pBufferToWrite = _SoundBufferToAddYourSignal.m_pSoundBuffer;

	while(lengthToRender>0)
	{
		//re-feed generation if needed:
		if(UniMiniXM_GetSamplesLeftToMix(m_pXM) <= 0 )
		{

#ifdef _UNIMINIXM_SEEKTIME_
			// re-synchronize if needed:
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
			// refresh effect automat, so that we can mix more sounds:
			UniMiniXM_RefreshTick(m_pXM);
		}
		if(UniMiniXM_GetSamplesLeftToMix(m_pXM)<=lengthToRender)
		{
			unsigned int stillToBeMixed = UniMiniXM_GetSamplesLeftToMix(m_pXM);
			UniMiniXM_AddSoundOnStereoFloatBuffer(m_pXM,
							(sFloatStereoSample *)pBufferToWrite,
							stillToBeMixed); // after this call, no sample left to mix!
			pBufferToWrite+=(stillToBeMixed<<1);
			lengthToRender -= stillToBeMixed ;
		} else
		{	// m_nsamplesStillToBeMixed > lengthToRender
			// mix remaining samples:
			UniMiniXM_AddSoundOnStereoFloatBuffer(m_pXM,
							(sFloatStereoSample *)pBufferToWrite,
							lengthToRender);
			pBufferToWrite+=(lengthToRender<<1);	
			//lengthToRender = 0 ;	useless but senseful: break do the same:
			break;
		}
	}

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
void XMSoundObject::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

	float	*pSoundBuffer=0L;
	unsigned int bufferlength=0;
	GetMachine()->GetCurrentSoundBufferImage( &pSoundBuffer , &bufferlength );
	ProcessPreview_DrawSoundBuffer( _pPreviewViewPort, pSoundBuffer, bufferlength );

	// create a sub-viewport to render in a part of the initial viewport:
	/*VirtualMachine::InternalViewPort	*pSubViewport;
	GetMachine()->NewViewPortChild(&pSubViewport,_pPreviewViewPort);
	if(pSubViewport)
	{
		pSubViewport->SetClippingCoordinates(0.05f,0.4f,0.95f,0.6f);
		// draw sound buffer there:
		ProcessPreview_DrawSoundBuffer( pSubViewport, pSoundBuffer, bufferlength );
		GetMachine()->DeleteViewPort(&pSubViewport);
	}*/

}
#endif

/*
	\brief return the current volume mixed for a given sample.
	\param	_instrument index.
	\param	_sample index.
	\param	_volume current volume is added there. 
*/
void	XMSoundObject::AddCurrentMixedVolume(unsigned int _instrument,
								unsigned int _sample, float &_volume)
{
	UniMiniXM_AddCurrentMixedVolume(m_pXM,_instrument,_sample,&_volume);

}
