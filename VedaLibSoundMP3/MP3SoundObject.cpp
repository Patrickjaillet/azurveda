/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "MP3SoundObject.h"
#include "mad.h"
#include <stdio.h>

BASEOBJECT_DECLARE_CLASS( "MP3", MP3SoundObject, VirtualMedia,"Sound Music MP3 File","mp3","MP3 File Sound support" );

MP3SoundObject::MP3SoundObject() : VirtualMedia()
	,m_pstream(0L)
	,m_pframe(0L)
	,m_psynth(0L)
	,m_nchannels(0L)
	,m_nsamples(0L)
	,m_samplerate(0L)
	,m_pleft_ch(0L)
	,m_pright_ch(0L)
	,m_NumberOfFrame(0L)
	,m_pFrameReferenceTable(0L)
	,m_nsamplesRunner(0L)
{
	// register serializable member, the file resource:
	REGISTER_MEMBER_RESOURCE( mSer_MP3File,"MP3File","mp3" );	

}
bool MP3SoundObject::head_check(unsigned long head)
{
   if ((head & 0xffe00000) != 0xffe00000)
   {
     return false;
   }
   if (!((head >> 17) & 3))
   {
     return false;
   }
   if (((head >> 12) & 0xf) == 0xf)
   {
     return false;
   }
   if (!((head >> 12) & 0xf))
   {
     return false;
   }
   if (((head >> 10) & 0x3) == 0x3)
   {
     return false;
   }
   if (((head >> 19) & 1) == 1 &&
       ((head >> 17) & 3) == 3 &&
       ((head >> 16) & 1) == 1)
   {
     return false;
   }
   if ((head & 0xffff0000) == 0xfffe0000)
   {
     return false;
   }
   return true;
 }
  // Song time determination stuff.
 // Stolen from xmms-mpg123 plugin.
 static const int bitrates_table[2][3][16] =
 {
   {
     {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448,},
     {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384,},
     {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320,}
   },
   {
     {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256,},
     {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160,},
     {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160,}
   }
 };
 
 static const int samplefreq_table[9] =
 {
   44100, 48000, 32000, 22050, 24000, 16000, 11025, 12000, 8000
 };
 
 int MP3SoundObject::decode_header(struct RawFrame *fr, unsigned long newhead)
 {
   if (newhead & (1 << 20))
   {
     fr->lsf = (newhead & (1 << 19)) ? 0x0 : 0x1;
     fr->mpeg25 = 0;
   }
   else
   {
     fr->lsf = 1;
     fr->mpeg25 = 1;
   }
   fr->lay = 4 - ((newhead >> 17) & 3);
   if (fr->mpeg25)
   {
     fr->sampling_frequency = 6 + ((newhead >> 10) & 0x3);
   }
   else
   {
     fr->sampling_frequency = ((newhead >> 10) & 0x3) + (fr->lsf * 3);
   }
 
   fr->bitrate_index = ((newhead >> 12) & 0xf);
   fr->padding = ((newhead >> 9) & 0x1);
 
   if (!fr->bitrate_index)
   {
     return (0);
   }
 
   switch (fr->lay)
   {
   case 1:
     fr->framesize = (long)bitrates_table[fr->lsf][0][fr->bitrate_index] *
                     12000;
     fr->framesize /= samplefreq_table[fr->sampling_frequency];
     fr->framesize = ((fr->framesize + fr->padding) << 2) - 4;
     break;
 
   case 2:
     fr->framesize = (long)bitrates_table[fr->lsf][1][fr->bitrate_index] *
                     144000;
     fr->framesize /= samplefreq_table[fr->sampling_frequency];
     fr->framesize += fr->padding - 4;
     break;
 
   case 3:
     fr->framesize = (long) bitrates_table[fr->lsf][2][fr->bitrate_index] *
                     144000;
     fr->framesize /= samplefreq_table[fr->sampling_frequency] << (fr->lsf);
     fr->framesize = fr->framesize + fr->padding - 4;
     break;
 
   default:
     return (0);
   }
 
   if(fr->framesize > 1792 /*MAXFRAMESIZE*/)
   {
     return 0;
   }
   else
   {
     return 1;
   }
 }


/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by CreateInternal().
*/
bool MP3SoundObject::CreateInternal(void)
{
	// check file resource:
	const unsigned char *pFileChunk;
	unsigned int	fileSize;
	mSer_MP3File.GetBinaryAtOffset(pFileChunk,fileSize);
	if(!pFileChunk)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_CANTOPENFILE();
		return false;
	}
 	m_pInputStart	= pFileChunk;
	m_InputLength	= fileSize;

	// ------------------------------------------------------
	//---- loop the whole MP3 a first time to get the number of frames:
	// raw method from a player plugin, not mad code.
	// first we just jump from a frame to the next to count them. should be fast:
	m_NumberOfFrame = 0;
	struct RawFrame	sRawFrame;
	unsigned long head=0;
	unsigned long index=0;
	while(index<m_InputLength)
	{
		// we cycle a 4 byte big endian read:
		head<<=8;
		head |= ((unsigned int) m_pInputStart[index]);
		index++;
		if( head_check(head))
		{
			// set the stream to the head
			int result = decode_header(&sRawFrame,head); // 1: OK
			if( result == 1)
			{
				index+=(sRawFrame.framesize);
				head = 0;
				m_NumberOfFrame++;
			}
		}
		// some ID3 tags can delay the frames:
		if(index>8192 && m_NumberOfFrame == 0)
		{
			BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR("Can't find frame tag within the 8192 first bytes");
			return false; // shit !! this is no MP3 File !!
		}
	}
	if(m_NumberOfFrame == 0 )
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR("No MP3 frame in file.");
		return false; // no frame in this MP3 ?
	}
	// alloc frame reference table to trace each frame position and time:
	// there are +1 elements: the last point the end and the total time.
	m_pFrameReferenceTable = new FrameReference[ m_NumberOfFrame +1] ;
	if(!m_pFrameReferenceTable)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
		return false; // oh!
	}
	// mad_decoder_init make a new on these struct, then _init() fill them !
	m_pstream = new struct mad_stream;
	m_pframe = new struct mad_frame;
	m_psynth = new struct mad_synth;
	mad_stream_init(m_pstream);
	mad_frame_init(m_pframe);
	mad_synth_init(m_psynth);
	mad_stream_options(m_pstream, 0);
	// ------------------------------------------------------
	//---- loop the whole MP3 frames again, and note the frames and time in the table:
	struct mad_stream precalcStream;
	mad_stream_init(&precalcStream);
	unsigned int nbFrame=0;
	mad_timer_t	totalTime;
	totalTime.fraction = totalTime.seconds = 0 ;
	head=0;
	index=0;
	while(index<m_InputLength)
	{
		// we cycle a 4 byte big endian read:
		head<<=8;
		head |= ((unsigned int) m_pInputStart[index]);
		index++;
		if( head_check(head))
		{
			// set the stream to the head
			int result = decode_header(&sRawFrame,head); // 1: OK
			if( result == 1)
			{
				mad_stream_buffer(&precalcStream, &(m_pInputStart[index-4]), sRawFrame.framesize+4);
				result = mad_header_decode(&m_pframe->header, &precalcStream);

				m_pFrameReferenceTable[nbFrame].m_pFrameStart = &(m_pInputStart[index-4]);
				m_pFrameReferenceTable[nbFrame].m_TimeStart = (totalTime.seconds<<8) |
										((totalTime.fraction/(MAD_TIMER_RESOLUTION>>8) )&255L); 
				mad_timer_add( &totalTime , m_pframe->header.duration );

				index+=(sRawFrame.framesize);
				head = 0;
				nbFrame++;
			}
		}	
	}
	// fill last frame ref table elements: the end:
	m_pFrameReferenceTable[nbFrame].m_pFrameStart = &(m_pInputStart[m_InputLength]);
	m_pFrameReferenceTable[nbFrame].m_TimeStart = totalTime.seconds * 1000 + 
									(totalTime.fraction)/352800UL ; 
	mad_stream_finish(&precalcStream);

		//m_pCurrentInputStart = m_pInputStart; // equ. SeekStream(0)
		//m_CurrentInputLength = m_InputLength;
	
	SeekStream(0); // rewind to start.
		
	m_MediaTimeLength = (double)( totalTime.seconds) + ((double)(totalTime.fraction) * (1.0f/352800000.0f)) ;

#ifdef _ENGINE_EDITABLE_
	m_ObjectInfoLine.Set("MP3 Length: ");
	m_ObjectInfoLine.AddInt(totalTime.seconds);
	m_ObjectInfoLine.AddString(" seconds.");
	//m_debugZeroedCase = 0;
#endif
	return(true);
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void MP3SoundObject::CloseInternal(void)
{
	// ALL VIRTUAL MEDIA MUST CALL UPPER CloseInternal()
	VirtualMedia::CloseInternal();
	// very important: flush residual decoded output:
	m_nsamplesRunner = 0;
	m_pleft_chRunner = m_pright_chRunner = 0L;

	if(m_pFrameReferenceTable)
	{
		delete [] m_pFrameReferenceTable;
		m_pFrameReferenceTable = 0L;
	}
	// free inits done by mad_stream_init() , etc...
	if(m_psynth)
	{
		mad_synth_finish(m_psynth);
		delete m_psynth;
		m_psynth = 0L;
	}
	if(m_pframe)
	{
		mad_frame_finish(m_pframe);
		delete m_pframe;
		m_pframe = 0L;
	}
	if(m_pstream)
	{
		mad_stream_finish(m_pstream);
		delete m_pstream;
		m_pstream = 0L;
	}
	m_ObjectInfoLine.Set("");
}
#endif


/*!
	\brief	 Process a media at a given date, 
			or do anything) using a machine. You must process the media on machine this->GetMachine().
			Note there are no play/pause or speed concept here, by default, a date is passed.

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pViewPort the viewport to render. Can't be 0L.
*/

void MP3SoundObject::ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort )
{

}

/*!
	\brief	 If the media had to generate real time sounds, 
			ProcessSoundInterupt will be called by a machine. There is a default 
			behaviour for this method, which does nothing. the float buffer 
			accumulate the whole machine sound, and in the better case, it will
			stand in cache.	_TimeLengthSec give a timelenth of the time played. 
	\param	_SoundBufferToAddYourSignal
*/
void MP3SoundObject::ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )
{
	unsigned int lengthToRender = _SoundBufferToAddYourSignal.m_LengthToRender;
	float	*pBufferToWrite = _SoundBufferToAddYourSignal.m_pSoundBuffer;
	// mad use MAD_F_FRACBITS = 28 fixed point. 268435456
	const float downscale = 1.0f / (1024.0f*1024.0f*256.0f) ;

	while(lengthToRender>0)
	{
		int decoderesult = -1;
		if(m_nsamplesRunner <= 0 ) // < : rare case.
		{
			//feed the input stream still mad_frame_decode can decode something.
			do 
			{
				decoderesult = mad_frame_decode( m_pframe, m_pstream );

				if(decoderesult == -1 )
				{
					if(!MAD_RECOVERABLE(m_pstream->error) ||
						m_pstream->error == MAD_ERROR_BUFLEN )
					{
							int startX256 = 
								((_SoundBufferToAddYourSignal.m_StartPlayTime_SecondUnit)<<8) |
								(_SoundBufferToAddYourSignal.m_StartPlayTime_SecondFrac>>24) ;
							int endX256 = 
								((_SoundBufferToAddYourSignal.m_EndPlayTime_SecondUnit)<<8) |
								(_SoundBufferToAddYourSignal.m_EndPlayTime_SecondFrac>>24) ;
							int nowX256 = startX256 - 
								(((startX256-endX256)*((int)lengthToRender))/((int)_SoundBufferToAddYourSignal.m_LengthToRender)) ;
							SeekStream(nowX256);

						if( !FeedStream() ) break; // false mean end of file, can't continue.
					} else
					{
						// do not re-feed after error:
						//m_debugZeroedCase++;
						// IMPORTANT !
						break; 
					}
					
				} // end if mad_frame_decode couldn't decode more.
				// note: decoderesult ==0 , no refeed.
			} while(decoderesult == -1);
			if(decoderesult == -1) break; // stream failed (end, or anything.).
		
			// frame OK
			// filter_func ?

			// generate sound:
			mad_synth_frame(m_psynth, m_pframe);
			// out:
			m_nchannels = m_psynth->pcm.channels;
			m_nsamples  = m_psynth->pcm.length;
			m_pleft_ch   = m_psynth->pcm.samples[0];
			m_pright_ch  = m_psynth->pcm.samples[1];
			m_samplerate = m_psynth->pcm.samplerate;
		
			//mad_decoder_run(m_pMadDecoder, MAD_DECODER_MODE_SYNC);
			if(m_nsamples == 0) break; // can't decode more.
			m_nsamplesRunner = m_nsamples;
			m_pleft_chRunner = m_pleft_ch;
			m_pright_chRunner = m_pright_ch;
			
				m_RunnerVector = (unsigned int)(( 65536.0f * ((float)m_psynth->pcm.samplerate)))
					/ _SoundBufferToAddYourSignal.m_PlayFrequency  ;
				m_RunnerIndex &= 0x0000ffff; // keep after point, reset to start of new buffer.
		}
		// first switch: if play frequency is the same as MP3 frequency, a copy would be OK.
		if(m_RunnerVector == 0x00010000)
		{	// for native MP3 speed, no time scale:
			// note m_RunnerIndex has no sense in such case, because it only keeps the low bits, after the point.
			register unsigned int ii = m_nsamplesRunner;
			if( lengthToRender>=ii )
			{	m_nsamplesRunner =0;
				lengthToRender -= ii;		
			} else
			{	m_nsamplesRunner -=lengthToRender;
				ii = lengthToRender;
				lengthToRender = 0;
			}
			if(m_psynth->pcm.channels ==1)
			{	// for mono case:
				while(ii)		
				{
					register float value = ((float)(*m_pleft_chRunner++)) * downscale ;
					(*pBufferToWrite++) += value;
					(*pBufferToWrite++) += value;			
					ii--;
				}
			} else
			{	// stereo case:
				while(ii)		
				{
					(*pBufferToWrite++) += ((float)(*m_pleft_chRunner++)) * downscale ;
					(*pBufferToWrite++) += ((float)(*m_pright_chRunner++)) * downscale ;
					ii--;
				}				
			} // end if/else  mono/stereo
		} // end test if MP3 freq == play freq.
		else if(m_RunnerVector < 0x00010000 && m_nsamplesRunner>1)
		{	// if MP3 freq is lower than play freq, scale and antialiazing:
			// if not enough sample were left for AA, we will not use AA.
			// find count to end according to the read buffer:
			unsigned int ii = ((m_nsamplesRunner-1)<<16); // -1 because we can't AA(interpolate) the last sample.
			ii -=m_RunnerIndex;
			unsigned int beforediv = ii;
			ii /= m_RunnerVector;
			// if there is some rest to the div, ++.
			if(ii*m_RunnerVector != beforediv ) ii++;
			if( lengthToRender<ii ) ii = lengthToRender;
			lengthToRender -= ii;
			signed int const *pleft = m_pleft_chRunner;
			register unsigned int sampleJustDone=(m_RunnerIndex>>16);
			// mono case:
			if(m_psynth->pcm.channels ==1)
			{	// mono cases:
				register float value;
				register float valuep1;
				const float aaif = 1.0f/65536.0f;
				while(ii)
				{					
					value = ((float)(pleft[sampleJustDone])) * downscale ;
					valuep1 = (((float)(pleft[sampleJustDone+1])) * downscale)-value ;
					value += valuep1 * ((float)(m_RunnerIndex & 0x0000ffff) * aaif ) ;
					(*pBufferToWrite++) += value;
					(*pBufferToWrite++) += value ;	
					m_RunnerIndex += m_RunnerVector;
					sampleJustDone = (m_RunnerIndex>>16) ;
					ii--;
				}
			} else
			{ // stereo case:
				signed int const *pright = m_pright_chRunner;
				register float value;
				register float valuep1;
				const float aaif = 1.0f/65536.0f;
				register float aarate;
				while(ii)
				{	
					aarate = ((float)(m_RunnerIndex & 0x0000ffff) * aaif ) ;
					value = ((float)(pleft[sampleJustDone])) * downscale ;
					valuep1 = (((float)(pleft[sampleJustDone+1])) * downscale)-value ;
					(*pBufferToWrite++) += value + valuep1 * aarate ;
					value = ((float)(pright[sampleJustDone])) * downscale ;
					valuep1 = (((float)(pright[sampleJustDone+1])) * downscale)-value ;
					(*pBufferToWrite++) += value + valuep1 * aarate ;
					m_RunnerIndex += m_RunnerVector;
					sampleJustDone = (m_RunnerIndex>>16) ;
					ii--;
				}
				m_pright_chRunner = pright+sampleJustDone;	
			} // end test stereo
			m_nsamplesRunner -= sampleJustDone; 
			m_pleft_chRunner = pleft+sampleJustDone;
			m_RunnerIndex &= 0x0000ffff; 	
		} // end test if MP3 freq is lower than play freq: antialias.
		else // (m_RunnerVector > 0x00010000 ||  m_nsamplesRunner<=1)
		{	// if MP3 freq if greater than play freq, scale over , no anti alias:
			// find count to end according to the read buffer:
			unsigned int ii = ((m_nsamplesRunner)<<16);
			ii -=m_RunnerIndex;
			unsigned int beforediv = ii;
			ii /= m_RunnerVector;
			// if there is some rest to the div, ++.
			if(ii*m_RunnerVector != beforediv ) ii++;
			if( lengthToRender<ii ) ii = lengthToRender;
			lengthToRender -= ii;
			signed int const *pleft = m_pleft_chRunner;
			register unsigned int sampleJustDone=(m_RunnerIndex>>16);
			// mono case:
			if(m_psynth->pcm.channels ==1)
			{	// mono cases:
				register float value;
				while(ii)
				{					
					value = ((float)(pleft[sampleJustDone])) * downscale ;
					(*pBufferToWrite++) += value;
					(*pBufferToWrite++) += value ;	
					m_RunnerIndex += m_RunnerVector;
					sampleJustDone = (m_RunnerIndex>>16) ;
					ii--;
				}
			} else
			{ // stereo case:
				signed int const *pright = m_pright_chRunner;
				while(ii)
				{					
					(*pBufferToWrite++) += ((float)(pleft[sampleJustDone])) * downscale ;
					(*pBufferToWrite++) += ((float)(pright[sampleJustDone])) * downscale ;
					m_RunnerIndex += m_RunnerVector;
					sampleJustDone = (m_RunnerIndex>>16) ;
					ii--;
				}
				m_pright_chRunner = pright+sampleJustDone;	
			} // end test stereo
			m_nsamplesRunner -= sampleJustDone; 
			m_pleft_chRunner = pleft+sampleJustDone;
			m_RunnerIndex &= 0x0000ffff; 
		}
	} // end while(lengthToRender>0)

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
void MP3SoundObject::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
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

// tool:
/*
static inline
signed int scale(mad_fixed_t sample)
{
  // round 
  sample += (1L << (MAD_F_FRACBITS - 16));

  // clip 
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

  // quantize 
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}*/
/*
	\brief	Continue stream feed if stream not finished.
*/
bool	MP3SoundObject::FeedStream()
{
	if(m_LastFrameIndex>=m_NumberOfFrame) return false; // the end, feed impossible.
	// note: we feed the minimum frame ! frames position has been precalculated.
	// the malicious MAD ask at least for a frame + 8 bytes to decode a frame from its stream.
	unsigned int frameLength = 8+(unsigned int)(m_pFrameReferenceTable[m_LastFrameIndex+1].m_pFrameStart)-
		(unsigned int)(m_pFrameReferenceTable[m_LastFrameIndex].m_pFrameStart);

	mad_stream_buffer(m_pstream, 
		m_pFrameReferenceTable[m_LastFrameIndex].m_pFrameStart,frameLength);
	// point next frame to play.
	m_LastFrameIndex++;
	return true;
}
/*
	\brief	If stream time is not synchro, reset the stream:
			it allows time-free random access.
	\param	_milliseconds
*/
void	MP3SoundObject::SeekStream(int _milliseconds)
{
	if(_milliseconds ==0)
	{
		m_LastFrameIndex = 0;
		m_RunnerIndex = 0; // the read index is reset, because of the jump.
		return;
	}
	// do nothing if near:
	if(m_LastFrameIndex<m_NumberOfFrame)
	{
		// we use -200 and +150 because such time error should be forgivable
		// in a realtime continuous play.
		// without pattern test: -150 -> 100 : OK
		int indexBefore=m_LastFrameIndex-2;
		int indexafter=m_LastFrameIndex+2;
		if(indexBefore<0 )indexBefore=0;
		if(indexafter>m_NumberOfFrame )indexafter=m_NumberOfFrame;
		int	lastFrameTime = (int) (m_pFrameReferenceTable[indexBefore].m_TimeStart)-300;
		int	nextFrameTime = (int) (m_pFrameReferenceTable[indexafter].m_TimeStart)+300;
		if(_milliseconds>=lastFrameTime && _milliseconds<nextFrameTime ) return; // OK.
	}
	// deal negative as end, and after length as end:
	if( _milliseconds<0 || _milliseconds>= m_pFrameReferenceTable[m_NumberOfFrame].m_TimeStart )
	{
		m_LastFrameIndex = m_NumberOfFrame;
		return;
	}
	// find new index by dicotomy, cause frametable can be very vast:	
	int	indexa=0;
	int	indexb=m_NumberOfFrame;
	while(indexa+1 != indexb )
	{
		int middle = (indexa+indexb)>>1;
		if(middle==indexa) middle++;
		//if(middle==indexb) middle++;
		//unsigned int t_a = m_pFrameReferenceTable[indexa].m_TimeStart ;
		unsigned int t_m = m_pFrameReferenceTable[middle].m_TimeStart ;
		//unsigned int t_b = m_pFrameReferenceTable[indexb].m_TimeStart ;
		if(_milliseconds<t_m)
			indexb=middle;
		 else
			indexa=middle;
	}
	//if((int)m_LastFrameIndex<indexa-2 || m_LastFrameIndex>indexa+2)
	//{
		m_LastFrameIndex = indexa;
		m_RunnerIndex = 0; // the read index is reset, because of the jump.
	//}

}
