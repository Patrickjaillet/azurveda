// SPDX-License-Identifier: LGPL-2.1-only

#include "MediaTimePattern.h"
#include "BaseContext.h"

BASEOBJECT_DECLARE_CLASS( "TP", MediaTimePattern, VirtualMedia,"Media Time Pattern","Pattern","A pattern of many media to play at the same time." );

MediaTimePattern::MediaTimePattern() : VirtualMedia()
	,mSer_TrackList(NewTrack)
#ifdef _ENGINE_EDITABLE_
	,m_pPreviewSquareObject(0L)

#endif
{

	REGISTER_MEMBER( mSer_TrackList,"Track list" );

}

BaseType *MediaTimePattern::NewTrack()
{
	PackList_TimeBlockTrack *pNewTrack =  new PackList_TimeBlockTrack(NewPartToPlay);
#ifdef _ENGINE_EDITABLE_

	if(pNewTrack) pNewTrack->SetInfoString("Track");
#endif
	return pNewTrack;
}

bool MediaTimePattern::CreateInternal(void)
{

	double timeLength=0.0;
	PackList::Cell *pTrackListCell = mSer_TrackList.GetFirstCell();
	while(pTrackListCell)
	{
		PackList_TimeBlockTrack *pTrack = (PackList_TimeBlockTrack *)pTrackListCell->GetManagedObject();
		PackList::Cell *pTrackCell= pTrack->GetFirstCell();
		double tl = 0.0;
		while(pTrackCell)
		{
			tl += ((PackList_TimeBlockTrack::TimeBlockTrackElement*) (pTrackCell->GetManagedObject()))->GetTimeInSecond();
			pTrackCell = pTrackCell->GetNextBrotherCell();
		}
		if( tl>timeLength)	timeLength = tl;

		pTrackListCell = pTrackListCell->GetNextBrotherCell();
	}
	m_MediaTimeLength = timeLength;

#ifdef _ENGINE_EDITABLE_

	ProcessPreview_CreateSquare( &m_pPreviewSquareObject );

#endif
	return(true);
}
#ifdef _ENGINE_EDITABLE_

void MediaTimePattern::ProcessPreview_CreateSquare(VirtualMachine::InternalObject3DBuffer **_ppVertexAndPolygonBufferOut)
{
	GetMachine()->NewObject3DBuffer( _ppVertexAndPolygonBufferOut ,8,10,0);
	if( !(*_ppVertexAndPolygonBufferOut)) return;

	VirtualMachine::InternalVertex *pVert = (*_ppVertexAndPolygonBufferOut)->GetFirstVertex();
	int ii,jj;
	const float vert[]={
		-0.5f,-0.5f,
		0.5f,-0.5f,
		0.5f,0.5f,
		-0.5f,0.5f
	};
	jj=0;
	for(ii=0 ; ii<((sizeof(vert)/sizeof(float))>>1) ; ii++)
	{
		pVert->m_x = vert[jj]; jj++;
		pVert->m_y = vert[jj]; jj++;
		pVert->m_z = 0.0f;
		pVert++;
	}

	VirtualMachine::InternalTriangle *pTriangle = (*_ppVertexAndPolygonBufferOut)->GetFirstTriangle();
	pTriangle->m_p0 = 1;
	pTriangle->m_p1 = 2;
	pTriangle->m_p2 = 0;
	pTriangle++;
	pTriangle->m_p0 = 0;
	pTriangle->m_p1 = 2;
	pTriangle->m_p2 = 3;
	(*_ppVertexAndPolygonBufferOut)->SetNumberOfActiveTriangle(2);

}
#endif

#ifdef _ENGINE_EDITABLE_

void	MediaTimePattern::CloseInternal(void)
{

	GetMachine()->DeleteObject3DBuffer(&m_pPreviewSquareObject);
}
#endif

void MediaTimePattern::ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort )
{
	PackList::Cell *pTrackCell = mSer_TrackList.GetFirstCell();
	while(pTrackCell)
	{
		PackList_TimeBlockTrack *pTrack = (PackList_TimeBlockTrack *)pTrackCell->GetManagedObject();
		PackList::Cell *pPartCell = pTrack->GetFirstCell();
		PartToPlay	*pPart;
		double date=0.0;
		while(pPartCell)
		{
			double		start,end;
			pPart		= (PartToPlay *) pPartCell->GetManagedObject() ;

			start		= date;
			date += pPart->GetTimeInSecond() ;
			end = date;

			VirtualMedia *pPartMedia = (VirtualMedia *)pPart->mSer_ReferenceToPlay.GetObjectPointer();
			if(pPartMedia)
			{
				if( _frameDate >= start &&
					_frameDate < end
					)
				{
					pPartMedia->ProcessMedia( pPart->mSer_TimeOffset.Get() +
							(_frameDate- start  )
							* pPart->mSer_TimeSpeed.Get(),
							_pViewPort
							);
				}
			}
			pPartCell = pPartCell->GetNextBrotherCell();
		}

		pTrackCell = pTrackCell->GetNextBrotherCell();
	}

}

void MediaTimePattern::ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )
{
class FixedReal64 {
public:
	long long	value;
	FixedReal64(){};
	FixedReal64(int _hi,unsigned int _lo){ value=(((long long)_hi)<<32)|_lo; };
	FixedReal64(double _dl){value=(long long)(_dl*4294967296.0f); };
	inline	void	Set( FixedReal64 _ll){ value =_ll.value; };
	inline	void	Set32( int _hi,unsigned int _lo){ value =(((long long)_hi)<<32)|_lo; };
	inline	void	Set32High( int _hi){
						 value &= 0x00000000ffffffffLL;
						 value |= (((long long)_hi)<<32); };
	inline	void	Add( FixedReal64 _a){ value += _a.value; };
	inline	void	Sub( FixedReal64 _s){ value -= _s.value; };
	inline	void	Mul( FixedReal64 _m){ value >>=16; value *= ((_m.value)>>16); };
	inline	void	Div( FixedReal64 _d){ value /= (_d.value>>16); value<<=16; };
	inline	void	Neg(){ value= -value;};
	inline	void	ShitRight( const unsigned int _bitshift){ value>>=_bitshift; };
	inline	int		Get32High(){ return ((int)(value>>32)); };
	inline	unsigned int		Get32Low(){ return ((unsigned int)value); };
	inline	bool	Test(){ return (value != 0LL); };
	inline	double	ToDouble(){ return ((double)value) *(1.0/4294967296.0); };
};
	PackList::Cell *pTrackCell = mSer_TrackList.GetFirstCell();
	VirtualMachine::SoundBufferToAddYourSignal	subBuffer;
	subBuffer.m_PlayFrequency = _SoundBufferToAddYourSignal.m_PlayFrequency ;

	FixedReal64	startPlay(_SoundBufferToAddYourSignal.m_StartPlayTime_SecondUnit,
							_SoundBufferToAddYourSignal.m_StartPlayTime_SecondFrac);
	double bufferStartPlayTime = startPlay.ToDouble();
	FixedReal64	endPlay(_SoundBufferToAddYourSignal.m_EndPlayTime_SecondUnit,
							_SoundBufferToAddYourSignal.m_EndPlayTime_SecondFrac);
	double f_end =endPlay.ToDouble();
	double bufferTimeLength =f_end-bufferStartPlayTime;

	if(bufferTimeLength == 0.0) return;
	double invPatternTimeLength = (double)(_SoundBufferToAddYourSignal.m_LengthToRender)
										/ bufferTimeLength;
 int trackIndex=0;
 int debugNbMixPerCycle=0;
	while(pTrackCell)
	{
		PackList_TimeBlockTrack *pTrack = (PackList_TimeBlockTrack *)pTrackCell->GetManagedObject();
		PackList::Cell *pPartCell;
		PartToPlay		*pPartCur;
		double	TimeCur,TimeNext;
		int	bufposCur,bufposNext;

		pPartCell = pTrack->GetFirstCell();

		if(!pPartCell
			)
		{
			pTrackCell = pTrackCell->GetNextBrotherCell();
			continue;
		}

		TimeCur	= 0.0;
		bufposCur = (int)( (TimeCur - bufferStartPlayTime) *invPatternTimeLength );
 int debugpartIndex=0;
		while( pPartCell )
		{

			if(bufposCur>=(int)_SoundBufferToAddYourSignal.m_LengthToRender ) break;

			pPartCur= (PartToPlay *)pPartCell->GetManagedObject();

			TimeNext=TimeCur+ pPartCur->GetTimeInSecond();
			bufposNext = (int)( (TimeNext - bufferStartPlayTime) *invPatternTimeLength );

			if(bufposNext>0)
			{
				VirtualMedia *pPartMedia = (VirtualMedia *)pPartCur->mSer_ReferenceToPlay.GetObjectPointer();
				if(pPartMedia)
				{
					int subBufposCur = bufposCur;
					int	subBufposNext = bufposNext;
					double substart=0;
					double subend=TimeNext-TimeCur;

					if(subBufposCur<0 )
					{
						subBufposCur=0;
						substart = bufferStartPlayTime-TimeCur;
					}
					if(subBufposNext>=(int)(_SoundBufferToAddYourSignal.m_LengthToRender) )
					{
						subBufposNext = ((int)_SoundBufferToAddYourSignal.m_LengthToRender);
						subend =f_end - TimeCur;
					}

					subBuffer.m_pSoundBuffer = _SoundBufferToAddYourSignal.m_pSoundBuffer
						+ (subBufposCur<<1);
					subBuffer.m_LengthToRender = subBufposNext - subBufposCur ;
					FixedReal64 isubstart(substart);
					FixedReal64 isubend(subend);

					subBuffer.m_StartPlayTime_SecondUnit = isubstart.Get32High() ;
					subBuffer.m_StartPlayTime_SecondFrac = isubstart.Get32Low() ;
					subBuffer.m_EndPlayTime_SecondUnit = isubend.Get32High() ;
					subBuffer.m_EndPlayTime_SecondFrac = isubend.Get32Low() ;

					pPartMedia->ProcessSoundInterupt( subBuffer );
 debugNbMixPerCycle++;
				}

			}

			pPartCell = pPartCell->GetNextBrotherCell();

			TimeCur = TimeNext;
			bufposCur = bufposNext;

			debugpartIndex++;

		}

		pTrackCell = pTrackCell->GetNextBrotherCell();
		trackIndex++;
	}

	if(debugNbMixPerCycle>1)
	{
		trackIndex=0;
	}

}

#ifdef _ENGINE_EDITABLE_

void MediaTimePattern::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

	VirtualMachine::InternalViewPort	*pSubViewport;
	GetMachine()->NewViewPortChild(&pSubViewport,_pPreviewViewPort);
	if(pSubViewport)
	{

		pSubViewport->SetClippingCoordinates(0.0f,0.0f,0.2f,1.0f);
		pSubViewport->SetScaleCoordinates(0.0f,0.0f,0.2f,1.0f);
		ProcessPreview_DrawTracks(_frameDate,pSubViewport,_pPreviewConfiguration);

		pSubViewport->SetClippingCoordinates(0.2f,0.0f,1.0f,0.9f);
		pSubViewport->SetScaleCoordinates(0.2f,0.0f,1.0f,0.9f);
		ProcessMedia(_frameDate,pSubViewport);

		pSubViewport->SetClippingCoordinates(0.2f,0.9f,1.0f,1.0f);
		pSubViewport->SetScaleCoordinates(0.2f,0.9f,1.0f,1.0f);
		float	*pSoundBuffer=0L;
		unsigned int bufferlength=0;
		GetMachine()->GetCurrentSoundBufferImage( &pSoundBuffer , &bufferlength );
		ProcessPreview_DrawSoundBuffer( pSubViewport, pSoundBuffer, bufferlength );

		GetMachine()->DeleteViewPort(&pSubViewport);
	}

}
#endif
#ifdef _ENGINE_EDITABLE_

void MediaTimePattern::ProcessPreview_DrawTracks(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *)
{

	if( !m_pPreviewSquareObject  ) return;

	float	rgba_Track[4]={0.25f,0.25f,0.3125f,1.0f};
	float	rgba_Part[4]={0.75f,0.75f,0.75f,1.0f};
	float	rgba_EmptyPart[4]={0.375f,0.375f,0.375f,1.0f};
	const float rgba_TimeLine[4]={0.25f,0.75f,0.25f,0.5f};

	_pPreviewViewPort->Clear(0.1f,0.1f,0.1f);

	_pPreviewViewPort->Matrix_LoadID();
	_pPreviewViewPort->Matrix_Translate(0.0f,0.0f,-1.0f);
	_pPreviewViewPort->SetFOVLength(1.0f);

	unsigned int nbTrack = mSer_TrackList.GetNumberOfCell();
	if(nbTrack == 0) return;

	PackList::Cell *pCell = mSer_TrackList.GetFirstCell();
	float	TrackWidthRate = 1.0f / ((float)nbTrack);
	float	trackCenterX = (-1.0f+TrackWidthRate) * 0.5f;
	while(pCell)
	{

		_pPreviewViewPort->Matrix_Push();

			m_pPreviewSquareObject->SetColor(rgba_Track[0],rgba_Track[1],rgba_Track[2],rgba_Track[3]);
			_pPreviewViewPort->Matrix_Translate(trackCenterX,0.0f,0.0f);
			_pPreviewViewPort->Matrix_Scale( TrackWidthRate*0.96f,1.0f,1.0f);
			_pPreviewViewPort->RenderMesh(m_pPreviewSquareObject );

			m_pPreviewSquareObject->SetColor(rgba_Part[0],rgba_Part[1],rgba_Part[2],rgba_Part[3]);

			PackList_TimeBlockTrack *pTrack = (PackList_TimeBlockTrack *)pCell->GetManagedObject();
			PackList::Cell *pPartCell = pTrack->GetFirstCell();
			float date=0.0f;
			while(pPartCell )
			{
				PartToPlay *pPart = ((PartToPlay *)pPartCell->GetManagedObject());
				VirtualMedia *pSubMedia =  (VirtualMedia *)pPart->mSer_ReferenceToPlay.GetObjectPointer();
				float startDate = date;
				date += pPart->GetTimeInSecond();
				float endDate=date;

				float startY = (float)(-0.5f + (startDate/m_MediaTimeLength));
				float endY = (float)(-0.5f + (endDate/m_MediaTimeLength));
				if(startY!= endY)
				{
					float	trackCenterY = (startY+endY) * 0.5f;
					float	trackScaleY = (float)((endDate-startDate)/m_MediaTimeLength);
					if(pSubMedia)

						m_pPreviewSquareObject->SetColor(rgba_Part[0],rgba_Part[1],rgba_Part[2],rgba_Part[3]);
					else

						m_pPreviewSquareObject->SetColor(rgba_EmptyPart[0],rgba_EmptyPart[1],rgba_EmptyPart[2],rgba_EmptyPart[3]);

					_pPreviewViewPort->Matrix_Push();
						_pPreviewViewPort->Matrix_Translate(0.0f,trackCenterY,0.0f);
						_pPreviewViewPort->Matrix_Scale( 0.96f,trackScaleY*0.96f,1.0f);
						_pPreviewViewPort->RenderMesh(m_pPreviewSquareObject );
					_pPreviewViewPort->Matrix_Pop();
				}
				pPartCell = pPartCell->GetNextBrotherCell();
			}
		_pPreviewViewPort->Matrix_Pop();
		pCell = pCell->GetNextBrotherCell();
		trackCenterX += TrackWidthRate ;
	}

	m_pPreviewSquareObject->SetColor(rgba_TimeLine[0],rgba_TimeLine[1],rgba_TimeLine[2],rgba_TimeLine[3]);
	float timeline = (float)(-0.5f + (_frameDate/m_MediaTimeLength));
	_pPreviewViewPort->Matrix_Translate(0.0f,timeline,0.0f);
	_pPreviewViewPort->Matrix_Scale( 1.0f,1.0f/128.0f,1.0f);
	_pPreviewViewPort->RenderMesh(m_pPreviewSquareObject );

}
#endif
