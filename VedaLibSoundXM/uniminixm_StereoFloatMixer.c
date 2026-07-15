/*! \file 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/

#include "uniminixm_functions.h"
#ifndef NULL
#define	NULL	0L
#endif

// the MLongLong type handle int64 operations for sample play speed,
// that need to be very acurate.
// it is used as a fixed real number, with 32bits after the dot:
typedef signed long long MLongLong;
// multiply, and shift, because we are 32b.fixed real.
#define MLongLong_Mul( _ll, _llmultiplier) \
	_ll >>=16; \
	_ll *= (_llmultiplier>>16);
// divide, and shift, because we are 32b.fixed real.
#define MLongLong_Div( _ll,_lldivider ) \
	_ll /= (_lldivider>>16);  \
	_ll<<=16; 
// set the 32 high bits and let low bits
#define MLongLong_Set32High(_ll,_hi) \
	_ll &= (long long)(0x00000000ffffffffULL);\
	_ll |= (((long long)_hi)<<32); 
// copy a int64 on another:
#define MLongLong_Set( _ll,_ll2) _ll = _ll2;
// set the 32high/32low with 2 int32:
#define MLongLong_Set32( _ll,_hi,_lo)  _ll=((((long long)_hi)<<32)|((unsigned int)_lo));
#define	MLongLong_Add(_ll,_a) _ll += _a; 
#define	MLongLong_Sub(_ll,_s) _ll -= _s; 
#define	MLongLong_Neg(_ll) _ll = -_ll;
#define	MLongLong_Get32High(_ll) ((int)(_ll>>32))
#define	MLongLong_Get32Low(_ll) ((int)_ll)

/* this was a cpp inline class version {
public:
	signed long long	value;
	MLongLong(){};
	MLongLong(int _hi,unsigned int _lo){ value=(((long long)_hi)<<32)|_lo;};
	inline	void	Set( MLongLong _ll){ value =_ll.value; };
	inline	void	Set32( int _hi,unsigned int _lo){ value =(((long long)_hi)<<32)|_lo; };
	inline	void	Set32High( int _hi){
						 value &= (long long)(0x00000000ffffffffULL);
						 value |= (((long long)_hi)<<32); };
	inline	void	Add( MLongLong _a){ value += _a.value; };
	inline	void	Sub( MLongLong _s){ value -= _s.value; };
	inline	void	Mul( MLongLong _m){ value >>=16; value *= ((_m.value)>>16); };
	inline	void	Div( MLongLong _d){ value /= (_d.value>>16); value<<=16; };
	inline	void	Neg(){ value= -value;};
	inline	int		Get32High(){ return ((int)(value>>32)); };
	inline	int		Get32Low(){ return ((int)value); };
};*/
// local enum:
typedef enum{
	eMixEndCause_Buffer=0,
	eMixEndCause_Sample,
	eMixEndCause_MinimumStep,
} eMixEndCause;

/*
	\brief	Mix the sound signal after a call to UniMiniXM_RefreshTick().
			It will <b>add</b> the signal over a pre-inited float buffer,
			which is a stereo 2 float by sample table.(see struct sFloatStereoSample)
			This buffer should be set to zeroes, but you can mix other noises in the same signal.
			UniMiniXM_RefreshTick() returned the maximum number of sample you can mix
			before a next call to it, but you can perfectly subdivide the
			calls to this mixer function by asking less samples, and continue
			the mix after, many times.
			The remaining samples to mix before a refresh are returned by _numberOfSamplesLeftToMix.
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\param	_pSoundBufferToAddSignalOn pointer to the beginning of the buffer to write.
	\param	_soundBufferMaxLength sample length of  _pSoundBufferToAddSignalOn
*/
void	UniMiniXM_AddSoundOnStereoFloatBuffer(sUniMiniXM *_pUniMiniXM,
						sFloatStereoSample *_pSoundBufferToAddSignalOn,
						unsigned int _soundBufferMaxLength)
{
	static const float mix_div255 = 1.0f/255.0f;
	static const float mix_VolRamp = (256.0f*255.0f) ;
	static const float mix_divVolRamp = 1.0f/(256.0f*255.0f) ;
	float *pFloatEndBuffer;
	int ii;
	if (_soundBufferMaxLength ==0) return;

	// we are going to mix this amount of samples:
	if(_pUniMiniXM->m_nsamplesStillToBeMixed>_soundBufferMaxLength)
		_pUniMiniXM->m_nsamplesStillToBeMixed -= _soundBufferMaxLength;
	else _pUniMiniXM->m_nsamplesStillToBeMixed = 0;

	pFloatEndBuffer = (float *)_pSoundBufferToAddSignalOn ;
	pFloatEndBuffer += (_soundBufferMaxLength << 1); // * 8 bytes * length.
	// loop through channels. there are 2 channels by track.
	for (ii = 0; ii < _pUniMiniXM->m_NumberOfChannels<<1 ; ii++)
	{
		float *pFloatBuffer ;
		MLongLong lmixpos;
		sUniMiniXM_Sample *pSample;
		int iLeftToMix,iLeftToMix_old,iRampCount;		
		// each track manages 2 channels, for fine sample swapping:
		sUniMiniXM_Channel *pChannel = &(_pUniMiniXM->m_pMusicChannelTable[ii>>1].m_Channel[ii&1]);		
		pFloatBuffer     = (float *)_pSoundBufferToAddSignalOn; // restart table that will be added a new Left+Right signal.
		pSample = pChannel->m_pSample;
		if(!pSample) continue; // no sample, skip this channel!
					// -> eax
       // find the more little length to mix.
		iRampCount = 0;
		//Calculate Loop Count:
		MLongLong_Set32(lmixpos,pChannel->m_SamplePositionHigh32,pChannel->m_SamplePositionLow32);
		while(pFloatBuffer < pFloatEndBuffer) // should be equal at the end
		{
			MLongLong sampleLeftToMix;	// hhhhllll
			MLongLong sampleSpeed;
			// if the volume has changed, end condition equals a volume ramp
			float	mix_rampspeedleft=0.0f;
			float	mix_rampspeedright=0.0f;
			eMixEndCause mix_endflag = eMixEndCause_Buffer;
			char silent=0;	// false by def
			int speedInt; // antialiasing has only sense when speed is [0,1]
			char antialias; // check if AA is needed.
			// ticks to the ends... calculated with pointers differences :-)
			// because many pass can happen because of bidirectionnal loops.
			iLeftToMix = ( ((int)pFloatEndBuffer)- ((int)pFloatBuffer) )>>3;
			// don't init sampleSpeed before loop, because value is affcted by Bidirectionnal loop for reverse !
			MLongLong_Set32(sampleSpeed,pChannel->m_SampleVectorHigh32,pChannel->m_SampleVectorLow32);
			if( pChannel->m_CurrentVectorDirection == eSampleLoopForward )
			{
				// work out how many samples left from mixpos to loop end
				MLongLong_Set32(sampleLeftToMix,pSample->m_LoopStartIndex + pSample->m_LoopLength,0);
				MLongLong_Sub(sampleLeftToMix,lmixpos);
			} else
			{	
				// work out how many samples left from mixpos to loop start
				MLongLong	m_LoopStartIndex;
				MLongLong_Set32(m_LoopStartIndex, pSample->m_LoopStartIndex,0);
				MLongLong_Set(sampleLeftToMix,lmixpos);
				MLongLong_Sub(sampleLeftToMix,m_LoopStartIndex);
			}
			// edx:eax now contains number of samples left to mix
			if(MLongLong_Get32High(sampleLeftToMix) <1000000) // if >=1000000 , sample writing has no end ? (i guess)
			{
				int sampleleft;
				MLongLong_Div(sampleLeftToMix,sampleSpeed);
				sampleleft = MLongLong_Get32High(sampleLeftToMix);
				if(MLongLong_Get32Low(sampleLeftToMix) !=0) sampleleft++;
				if(sampleleft<iLeftToMix)
				{
					iLeftToMix = sampleleft;
					mix_endflag = eMixEndCause_Sample ;
				}
			}
			iLeftToMix_old = iLeftToMix ;
			// if it tries to continue an old ramp, but the target has changed,
			// set up a new ramp
			if(	(pChannel->m_Slide_Count !=0) &&
				( pChannel->m_LeftVolume == pChannel->m_Slide_LeftTarget) &&
				( pChannel->m_RightVolume == pChannel->m_Slide_RightTarget)
				)
			{
				// restore old ramp
				iRampCount = pChannel->m_Slide_Count;
				mix_rampspeedleft = pChannel->m_Slide_LeftSpeed;
				mix_rampspeedright = pChannel->m_Slide_RightSpeed;
			}else
			{
				int rightv; //volume slide start
				int leftv = pChannel->m_LeftVolume - (pChannel->m_Slide_LeftVolume>>8);
				pChannel->m_Slide_LeftTarget = pChannel->m_LeftVolume ;
				if( leftv != 0)
				{
					mix_rampspeedleft = ((float)leftv) * mix_div255 * _pUniMiniXM->m_fInvSampleStep;
					pChannel->m_Slide_LeftSpeed = mix_rampspeedleft;
					iRampCount = _pUniMiniXM->m_MinimumSampleStep;
				}
				// right:
				rightv = pChannel->m_RightVolume - (pChannel->m_Slide_RightVolume>>8);
				pChannel->m_Slide_RightTarget = pChannel->m_RightVolume ;
				if( rightv != 0)
				{
					mix_rampspeedright = ((float)rightv) * mix_div255 * _pUniMiniXM->m_fInvSampleStep;
					pChannel->m_Slide_RightSpeed = mix_rampspeedright;
					iRampCount = _pUniMiniXM->m_MinimumSampleStep;
				}
			}

			// ramp end:
			if(iRampCount>0)
			{	// we have sound volume changing:
				pChannel->m_Slide_Count = iRampCount;
				if(iRampCount<iLeftToMix) iLeftToMix = iRampCount;	
			} else
			{
				//optim: if no volume change and both volume are zero, no mix !!
				// note: use logic or to check both 0 in one test.
				silent=(!(pChannel->m_Slide_RightVolume|pChannel->m_Slide_LeftVolume));
			}

			// get volume multiplier:
			// first convert from 24:8 to 0-255, then make 0-1.0f
		
			// Set up inner loop:
			// lmixpos and sampleSpeed already inited here.

			if(pChannel->m_CurrentVectorDirection == eSampleLoopBackward )
			{
				MLongLong_Neg(sampleSpeed); // ping pong -> change speed:
			}
			// antialiasing has only sense when speed is [0,1]
			speedInt = MLongLong_Get32High(sampleSpeed);
			// note: we have 2 octave were AA has a sense,[-1,1],
			// whatever the sample base frequency is.
			antialias= (char)(speedInt>=-1 && speedInt<=1);	//AA]-2,2[						

			if(silent)
			{
				MLongLong	multiplier;
				// both volume to zero and no volume change means no mix !
				// this simulate a silent mix.
				pFloatBuffer += (iLeftToMix<<1); // jump over write table.
				// sample run got to be done as if we mix !
				MLongLong_Set32(multiplier,iLeftToMix,0);
				MLongLong_Mul(multiplier,sampleSpeed);
				MLongLong_Add(lmixpos,multiplier);
			} else
			{
				float	fMixRampRightVol = ((float) pChannel->m_Slide_RightVolume) * mix_divVolRamp;
				float	fMixRampLeftVol = ((float) pChannel->m_Slide_LeftVolume) * mix_divVolRamp;

				float *pFloatSampleToMix = pSample->m_pBuffer; // get pointer to sample buffer
				if(antialias) // more common case
				{
					int jj=iLeftToMix;
					float *pAntiAliasTable=_pUniMiniXM->m_pAntiAliasTable;
					// inner loop with volume ramp and antialiasing !
					while(jj>0)
					{
						register float	samp,samp1p1,ffrac;
						int sampleIndex = MLongLong_Get32High(lmixpos);
						//old AA unsigned int fractionIndex = ((unsigned int) lmixpos.Get32Low())>>1; // 31 because int->float is unsigned.
						//old AA ffrac = ((float)fractionIndex) * mix_1over2gig; // [0,1]				
						samp = pFloatSampleToMix[ sampleIndex ] ;	
						samp1p1 = pFloatSampleToMix[ sampleIndex+1 ] ;
						ffrac = pAntiAliasTable[ ((unsigned int)MLongLong_Get32Low(lmixpos))>>24 ];
						MLongLong_Add(lmixpos,sampleSpeed); // add speed 
						samp1p1 -=samp;
						samp += (samp1p1 * ffrac);
						*(pFloatBuffer++) +=  (samp*fMixRampLeftVol) ;
						*(pFloatBuffer++) +=  (samp*fMixRampRightVol);
						// ramp volume:
						fMixRampLeftVol += mix_rampspeedleft ;
						fMixRampRightVol += mix_rampspeedright ;
						jj--;
					}		
				} else 
				{	// no AA:
					int jj=iLeftToMix;
					// inner loop with volume ramp and NO antialiasing !
					while(jj>0)
					{
						register float	samp = pFloatSampleToMix[ MLongLong_Get32High(lmixpos) ] ;	
						MLongLong_Add(lmixpos,sampleSpeed); // add speed 
						*(pFloatBuffer++) +=  (samp*fMixRampLeftVol) ;
						*(pFloatBuffer++) +=  (samp*fMixRampRightVol);
						// ramp volume:
						fMixRampLeftVol += mix_rampspeedleft ;
						fMixRampRightVol += mix_rampspeedright ;
						jj--;
					}		
				} // end else test antialias.
				// volume ramp state restore:
				if( iRampCount > 0 )
				{
					pChannel->m_Slide_RightVolume =(int)(fMixRampRightVol * mix_VolRamp );
					pChannel->m_Slide_LeftVolume =(int)(fMixRampLeftVol * mix_VolRamp );
					iRampCount -= iLeftToMix; //there can be ramp left.
					mix_rampspeedleft = 0.0f;
					mix_rampspeedright = 0.0f;
					pChannel->m_Slide_Count = iRampCount;
					// if rampcount now = 0, a vol. ramp has FINISHED, so finish the rest of the mix
					if(iRampCount ==0)
					{
						// clear out the ramp speeds
						pChannel->m_Slide_LeftSpeed = 0.0f;
						pChannel->m_Slide_RightSpeed = 0.0f;
						// clamp the 2 volumes together in case the speed wasnt accurate enough!
						pChannel->m_Slide_LeftVolume = pChannel->m_LeftVolume<<8;
						pChannel->m_Slide_RightVolume = pChannel->m_RightVolume<<8;
						// is it 0 because ramp ended only? or both ended together??
						// if sample ended together with ramp.. problems .. loop isnt handled
						if( iLeftToMix_old !=  iLeftToMix)
						{
							// start again and continue rest of mix
							continue;
						}
					}
				} // end volume ramp state restore.
			} // end test if not silent.

			if( mix_endflag != eMixEndCause_Buffer )
			{
				// sUniMiniXM_SampleBUFF_END , but it can be because of a loop or the real sample end.
				// switch on loop, and continue.
				int	loopmode = pSample->m_LoopMode;
				if( loopmode == eLoopType_Circular)
				{
					// we really got to do that to manage the right frequence, in short loop cases with high freq :
					int ppos = MLongLong_Get32High(lmixpos) ;
					int end = pSample->m_LoopStartIndex + pSample->m_LoopLength ;
					do{
						ppos -= pSample->m_LoopLength;
					}while(ppos>=end);
					if(ppos < (int) (pSample->m_LoopStartIndex))
					{
						ppos =pSample->m_LoopStartIndex; // happens when m_LoopStartIndex=0 and low bits left.
					}

					MLongLong_Set32High(lmixpos, ppos );
					// continue -> while( pFloatBuffer < pFloatEndBuffer )
					continue;
				} 
				if(loopmode == eLoopType_PingPong)
				{
					if( pChannel->m_CurrentVectorDirection == eSampleLoopBackward )
					{
						// boing at the start:
						int unit = MLongLong_Get32High(lmixpos);
						int frac =MLongLong_Get32Low( lmixpos);
						MLongLong_Set32(lmixpos, pSample->m_LoopStartIndex+(pSample->m_LoopStartIndex-unit), /*0xffffffff*/-frac);
					
						pChannel->m_CurrentVectorDirection = eSampleLoopForward ;
					}else
					{
						// boing at the end
						int unit = MLongLong_Get32High(lmixpos);
						int frac = MLongLong_Get32Low(lmixpos);
						int uend = pSample->m_LoopStartIndex+pSample->m_LoopLength;
						MLongLong_Set32(lmixpos, uend-(unit-uend) ,/*0xffffffff*/-frac);
						pChannel->m_CurrentVectorDirection = eSampleLoopBackward ;
					}
					// continue -> while( pFloatBuffer < pFloatEndBuffer )
					continue;
				}
				// if here, simple end of the sample, next channel.
				break;
			}  // end of test if end because of sample loop. 
			/*else
			{	useless:
				// mix_endflag == eMixEndCause_Buffer 
			}*/
			//if(!continueMix) break;
		} // end while( pFloatBuffer < pFloatEndBuffer )
		// FinishUpChannel
		pChannel->m_SamplePositionHigh32 = MLongLong_Get32High(lmixpos);
		pChannel->m_SamplePositionLow32 = MLongLong_Get32Low(lmixpos);
	} // end loop by channel.
 
	// --- dans clip copy, function d'aprés:
	// if there wasnt any channels mixed (all their sptr's were null)
	// then just clear, dont clip
}
