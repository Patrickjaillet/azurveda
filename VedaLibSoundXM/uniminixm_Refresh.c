/*! \file 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "uniminixm_functions.h"

#define	UNIMINIXM_COMPILE_MODPERIOD
#define eXMFx_INSTRUMENTVIBRATO_COMPILE
#define eXMFx_VOLUMEENVELOPE_COMPILE
#define eXMFx_PANENVELOPE_COMPILE
#define eXMFx_VOLUME_COMPILE
#define eXMFx_TREMOLO_COMPILE
// 0 xm found using it, 20 lines of codes:
//#define eXMFx_TREMOR_COMPILE
#define eXMFx_0XY_Arpeggio_COMPILE
#define eXMFx_3XX_PortamentoTo_COMPILE
#define eXMFx_1XX_PortaUp_COMPILE
#define eXMFx_2XX_PortaDown_COMPILE
#define eXMFx_5XX_PortamentoToAndVolumeSlide_COMPILE
#define eXMFx_4XY_Vibrato_COMPILE
#define eXMFx_6XX_VibratoAndVolumeSlide_COMPILE
#define eXMFx_8XX_SetPanPosition_COMPILE
#define eXMFx_9XX_SampleOffset_COMPILE
#define eXMFx_AXY_VolumeSlide_COMPILE
#define eXMFx_BXX_PatternJump_COMPILE
#define eXMFx_CXX_SetVolume_COMPILE
#define eXMFx_DXX_PatternBreak_COMPILE
#define eXMFx_E1X_FinePortamentoUp_COMPILE
#define eXMFx_E2X_FinePortamentoDown_COMPILE
#define eXMFx_E4X_VibratoToWave_COMPILE
#define eXMFx_E5X_SetFineTune_COMPILE
#define eXMFx_E6X_PatternLoop_COMPILE
#define eXMFx_E7X_SetTremoloWave_COMPILE
#define eXMFx_E8X_SetPanPosition16_COMPILE
// 0 xm using it,...5lines. 
//#define eXMFx_E9X_Retrig_COMPILE
#define eXMFx_EAX_FineVolumeSlideUp_COMPILE
#define eXMFx_EBX_FineVolumeSlideDown_COMPILE
// 0 xm using it,  4 lines...
//#define eXMFx_ECX_NoteCut_COMPILE
#define eXMFx_EDX_NoteDelay_COMPILE
#define eXMFx_EEX_m_PatternDelayCounter_COMPILE
#define eXMFx_FXY_SetSpeed_COMPILE
#define eXMFx_GXX_SetGlobalVolume_COMPILE
#define eXMFx_HXX_GlobalVolumeSlide_COMPILE
#define eXMFx_KXX_KeyOff_COMPILE
// 0 xm found using it, take 20 lines:
//#define eXMFx_LXX_SetEnvelopePosition_COMPILE
// 0 xm found using it, take 15 lines:
//#define eXMFx_PXX_PanSlide_COMPILE
// find 2 chiptune using it,20 lines code:
#define eXMFx_RXX_MultiRetrig_COMPILE
#define eXMFx_XXX_ExtraFinePortamento_COMPILE

// use of optimized memset:
// for memcpy() memset(),  string.h is more platform independant than memory.h.
#include <string.h>
#include <math.h>
// abs() is more likely in stdlib than in math:
#include <stdlib.h>

#ifndef sinf
#define sinf sin
#endif
#ifndef powf
#define powf pow
#endif
#ifndef fabsf
#define fabsf fabs
#endif
#ifndef NULL
#define	NULL	0L
#endif
#ifndef FALSE
#define	FALSE	0L
#endif
#ifndef TRUE
#define	TRUE	-1L
#endif
#ifdef	UNIMINIXM_TRACE_VOLUME_BY_SAMPLE
void	UniMiniXM_OpenMixMonitor(sUniMiniXM_Sample *pSample,sUniMiniXM_Channel *pNewMixChannnel )
{
	int ii;
	for(ii=0; ii<MAXNUMBEROFMIXINGMONITORABLE ; ii++ )
		if(pSample->m_Monitor.m_pChannelMixed[ii]==0L)
		{	pSample->m_Monitor.m_pChannelMixed[ii] = pNewMixChannnel;
			return;
		}
}
#endif
#ifdef	UNIMINIXM_TRACE_VOLUME_BY_SAMPLE
void	UniMiniXM_CloseMixMonitor(sUniMiniXM_Sample *pSample,sUniMiniXM_Channel *pOldMixChannnel )
{
	int ii;
	for(ii=0; ii<MAXNUMBEROFMIXINGMONITORABLE ; ii++ )
		if(pSample->m_Monitor.m_pChannelMixed[ii]==pOldMixChannnel)
		{	pSample->m_Monitor.m_pChannelMixed[ii] = 0L;
			return;
		}
}
#endif
/*
	\brief	Refresh Track Line
	\return  enum for end checking.
*/
static unsigned int  RefreshTrackLine( sUniMiniXM *_pUniMiniXM );
/*
	\brief	Refresh Effects. use m_mod.
*/
static void RefreshEffects( sUniMiniXM *_pUniMiniXM );
/*
	\brief	Refresh Flags . use m_mod.
*/
static void RefreshFlags( sUniMiniXM *_pUniMiniXM, sUniMiniXM_TrackAutomat * pTrack, sUniMiniXM_Sample * pSampleToLaunch );

/*
	\brief	Reset struct
*/
static void ResetTrack(sUniMiniXM_TrackAutomat * pTrack, sUniMiniXM_Sample * pSample);

static const float	m_FrequencyFXConstant2PiDiv64= (2.0f * 3.141592f / 64.0f);

void SetBPM(sUniMiniXM *_pUniMiniXM, int bpm);
/*
	\brief	Refresh the music effects for a tick time. It also returns
			the amount of sample you will be able to mix with 
			UniMiniXM_AddSoundOnStereoFloatBuffer() in parameter _numberOfSamplesToMix.			
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\return a bRefreshTickReportBits flag bit set, that indicates if the music
			ended, or entered a never ending loop.
*/
unsigned int UniMiniXM_RefreshTick( sUniMiniXM *_pUniMiniXM )
{
	// we check if the song continue or not:
	unsigned int songcontinue = 1 ;

	if (_pUniMiniXM->m_TickCount)
		RefreshEffects(_pUniMiniXM);	// no tick start: simple refresh.
	else
	{
		//  next pattern & line
		if (_pUniMiniXM->m_NextPosition >= 0) _pUniMiniXM->m_Position = _pUniMiniXM->m_NextPosition;
		if (_pUniMiniXM->m_NextLine >= 0) _pUniMiniXM->m_LineIndex = _pUniMiniXM->m_NextLine;

		if(!RefreshTrackLine(_pUniMiniXM))	// Refresh automat with new effects (+ check for infinite loop)
			songcontinue = 0;

		// if no effects, continue automat.
		if (_pUniMiniXM->m_NextLine == -1)
		{
			_pUniMiniXM->m_NextLine = _pUniMiniXM->m_LineIndex + 1;
			if (_pUniMiniXM->m_NextLine >= _pUniMiniXM->m_pPattern[_pUniMiniXM->m_pPatternIndexList[_pUniMiniXM->m_Position]].m_NumberOfLines)	// if end of pattern 
			{
				_pUniMiniXM->m_NextPosition = _pUniMiniXM->m_Position + 1;	
				if (_pUniMiniXM->m_NextPosition >= (int) _pUniMiniXM->m_PatternLength)
				{
					_pUniMiniXM->m_NextPosition = (int) _pUniMiniXM->m_RestartPosition;
					songcontinue = 0;
				}
				_pUniMiniXM->m_NextLine = 0;	// start at top of pattn
			}
		}
  }

	_pUniMiniXM->m_TickCount++;
	if (_pUniMiniXM->m_TickCount >= _pUniMiniXM->m_F0XSpeed + _pUniMiniXM->m_PatternDelayCounter)
	{
		_pUniMiniXM->m_PatternDelayCounter = 0;
		_pUniMiniXM->m_TickCount = 0;
	}
	// we can now mix this amount of samples:
	_pUniMiniXM->m_nsamplesStillToBeMixed = _pUniMiniXM->m_BPMSamplesPerTick;
	_pUniMiniXM->m_TotalTickMixed += _pUniMiniXM->m_BPMSamplesPerTick;
	return songcontinue;
}

int GetModPeriod(int note, int finetune)
{
  int period;
  const float v2p0 = 2.0f;
  const float vDiv12p0 = 1.0f/12.0f;
  const float v13p375 = 13.375f;
  period = (int) (powf(v2p0, ((float) (132 - note))*vDiv12p0) * v13p375);

  // interpolate for finer tuning
  if (finetune < 0 && note)
  {
    int diff = period - (int) (powf(v2p0, (float) (132 - (note - 1)) *vDiv12p0) * v13p375);

    diff *= abs(finetune);
    diff /= 128;
    period -= diff;
  }
  else
  {
    int diff = (int) (powf(v2p0, (float) (132 - (note + 1))*vDiv12p0) * v13p375) - period;

    diff *= finetune;
    diff /= 128;
    period += diff;
  }

  return period;
}

void GetInstrumentVibrato(sUniMiniXM_TrackAutomat * pTrack, sUniMiniXM_Instrument *pInstr)
{
  register int freqdelta = 0;
  switch (pInstr->m_VibratoType)
  {
    case 0:
      freqdelta = (int) (sinf((float) (pTrack->m_InstrumentVibratoPosition) * (2.0f * 3.141592f / 256.0f)) * 64.0f);
      break;
    case 1:
		if (pTrack->m_InstrumentVibratoPosition < 128)
			freqdelta = 64;		// square
		else
			freqdelta = -64;
      break;
    case 2:
      freqdelta = (128 - ((pTrack->m_InstrumentVibratoPosition + 128) % 256)) >> 1;
      break;
    case 3:
      freqdelta = (128 - (((256 - pTrack->m_InstrumentVibratoPosition) + 128) % 256)) >> 1;
      break;
  };

  freqdelta *= pInstr->m_VibratoDepth;
  if (pInstr->m_VibratoSweep)
    freqdelta = freqdelta * pTrack->m_InstrumentVibratoSweepPosition / pInstr->m_VibratoSweep;
  freqdelta >>= 6;

  pTrack->m_FrequencyDelta += freqdelta;

  pTrack->m_InstrumentVibratoSweepPosition++;
  if (pTrack->m_InstrumentVibratoSweepPosition > pInstr->m_VibratoSweep)
    pTrack->m_InstrumentVibratoSweepPosition = pInstr->m_VibratoSweep;

  pTrack->m_InstrumentVibratoPosition += pInstr->m_VibratoFreq;
  if (pTrack->m_InstrumentVibratoPosition > 255)
    pTrack->m_InstrumentVibratoPosition -= 256;

  pTrack->m_RefreshFlag |= eBitRefresh_Frequency;
}


void GetPortamento(sUniMiniXM_TrackAutomat * ptrack)
{
  if (ptrack->m_Frequency < ptrack->m_PortamentoTarget)
  {
    ptrack->m_Frequency += (int) ptrack->m_PortamentoVector << 2;
    if (ptrack->m_Frequency > ptrack->m_PortamentoTarget)
      ptrack->m_Frequency = ptrack->m_PortamentoTarget;
  }

  else if (ptrack->m_Frequency > ptrack->m_PortamentoTarget)
  {
    ptrack->m_Frequency -= (int) ptrack->m_PortamentoVector << 2;
    if (ptrack->m_Frequency < ptrack->m_PortamentoTarget)
      ptrack->m_Frequency = ptrack->m_PortamentoTarget;
  }

  ptrack->m_RefreshFlag |= eBitRefresh_Frequency;	
}

void GetVibrato(sUniMiniXM_TrackAutomat * pTrack)
{
	int freqdelta = 0;
	unsigned char cc;

	cc = (pTrack->m_VibratoPosition & 31);

	switch (pTrack->m_VibratoType & 3)
	{
		case 0:
			freqdelta = (int) fabsf( ((sinf( ((float) (pTrack->m_VibratoPosition)) * m_FrequencyFXConstant2PiDiv64 )) * 
											256.0f) );
			break;
		case 1:
			cc <<= 3;
			if (pTrack->m_VibratoPosition < 0)
			cc = 255 - cc;
			freqdelta = cc;
		break;
		case 2:
		case 3:
			freqdelta = 255;
		break;
	};
	pTrack->m_RefreshFlag |= eBitRefresh_Frequency;	
	freqdelta *= pTrack->m_VibratoDepth;
	freqdelta = (freqdelta>>5) & 0xfffffffc;

	if (pTrack->m_VibratoPosition >= 0)
		pTrack->m_FrequencyDelta = -freqdelta;
	else
		pTrack->m_FrequencyDelta = freqdelta;
}

void GetEnvelope(sUniMiniXM_TrackAutomat * pTrack, 
					 int *ppos, 
					 int *ptick, 
					 unsigned char type, 
					 int numpoints, 
					 unsigned short *ppoints, 
					 unsigned char loopend, 
					 unsigned char _LoopStartIndex, 
					 unsigned char sustain, 
					 int *pValueHigh32, 
					 int *pValueLow32, 
					 signed char *envstopped, 
					 int *envdelta, 
					 unsigned char control)
{
	if (*ppos < numpoints)
	{
		if (*ptick == ppoints[(*ppos) << 1])
		{
			int currentpos, nextpos,currentTick, nexttick,
					curr, next, tickdiff;

			// loop
			if ((type & eEnvelopeBit_Loop) && *ppos == loopend)
			{
				*ppos = _LoopStartIndex;
				*ptick = ppoints[(*ppos) << 1];
			}

			currentpos = *ppos;
			nextpos = (*ppos) + 1;
			currentTick = ppoints[currentpos << 1];	// tick at this point
			nexttick = ppoints[nextpos << 1];	// tick at next point
			curr = ppoints[(currentpos << 1) + 1] << 16;
			next = ppoints[(nextpos << 1) + 1] << 16;
			// if last, abort envlp and continue last value.
			if (*ppos == numpoints - 1)
			{
				*pValueHigh32 = ppoints[(currentpos << 1) + 1];
				*envstopped = TRUE;
				pTrack->m_RefreshFlag |= control;
				return;
			}
			//  sustain
			if ((type & eEnvelopeBit_Sustain) && currentpos == sustain && !pTrack->m_KeyOffFlag)
			{
				*pValueHigh32 = ppoints[(currentpos << 1) + 1];
				pTrack->m_RefreshFlag |= control;
				return;
			}
			// interpolate 2 points to find delta step
			tickdiff = nexttick - currentTick;
			if (tickdiff)
				*envdelta = (next - curr) / tickdiff;
			else
				*envdelta = 0;
			*pValueLow32 = curr;
			(*ppos)++;
		}
		else
			*pValueLow32 += *envdelta;	
	}

	*pValueHigh32 = *pValueLow32 >> 16;
	(*ptick)++;

	pTrack->m_RefreshFlag |= control;
}
void UpdateVolume(sUniMiniXM_TrackAutomat * pTrack, unsigned char volume)
{
	// KRB optimisation for size:
  register const unsigned char volumeEnvelopeLoopType = volume>>4;
  register const unsigned char volvalue = volume & 0x0F;
	// we assume volumeEnvelopeLoopType 0 can't happen.
  if (volumeEnvelopeLoopType >= 0x01 && volumeEnvelopeLoopType <= 0x09) 
  {	
	  if(volumeEnvelopeLoopType<=0x05)  
		  pTrack->m_Volume = volume - 0x10; // case 1,2,3,4,5
	  else // 6,7,8,9
	  {
		if((volumeEnvelopeLoopType &1)) // case  7,9
		{
			pTrack->m_Volume += volvalue;
			if (pTrack->m_Volume > 0x40)  pTrack->m_Volume = 0x40;	
		}else // case 6,8
		{
			pTrack->m_Volume -= volvalue;
			if (pTrack->m_Volume < 0) pTrack->m_Volume = 0;				
		}
	  } 
      pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	  return;
  } 
  // else: case a,b,f
  if(volumeEnvelopeLoopType==0x0a)
  {
	  pTrack->m_VibratoVector = volvalue;
		return;
  }
  if(volumeEnvelopeLoopType==0x0b)
  {
	  pTrack->m_VibratoDepth = volvalue;
		return;
  }
  if(volumeEnvelopeLoopType==0x0f)
  {
	 if (volvalue)
	  pTrack->m_PortamentoVector = volvalue << 4;
	pTrack->m_PortamentoTarget = pTrack->m_Period;
	pTrack->m_RefreshFlag &= ~eBitRefresh_TriggerSample;
	return;
  }
   // else: case c,d,e
	if(volumeEnvelopeLoopType==0x0c)  pTrack->m_Pan = volvalue << 4;
	else if(volumeEnvelopeLoopType==0x0d)  pTrack->m_Pan -= volvalue ;
	else /* LAST CASE IS IMPLICIT, NO TEST: if(volumeEnvelopeLoopType==0x0e) */  
		pTrack->m_Pan += volvalue ;
	// for c,d,e:
	pTrack->m_RefreshFlag |= eBitRefresh_Pan;
	// func end.

}

/*
	\brief	Refresh Track Line
	\return  enum for end checking.
*/
unsigned int  RefreshTrackLine( sUniMiniXM *_pUniMiniXM )
{
  int ii;
  signed char breakflag = FALSE;
  signed char jumpflag = FALSE;
  int notinfiniteloop = 1;
  sUniMiniXM_Note *pNote;

  _pUniMiniXM->m_NextPosition = -1;
  _pUniMiniXM->m_NextLine = -1;

  // Point our note pointer to the correct pattern buffer, and to the
  // correct offset in this buffer indicated by line and number of channels
  pNote = _pUniMiniXM->m_pPattern[_pUniMiniXM->m_pPatternIndexList[_pUniMiniXM->m_Position]].m_pNoteTable 
	  + (_pUniMiniXM->m_LineIndex * _pUniMiniXM->m_NumberOfChannels);

  if (!pNote)
    return notinfiniteloop;

  // Loop through each channel in the line until we have finished
  for (ii = 0; ii < _pUniMiniXM->m_NumberOfChannels; ii++, pNote++)
  {
	sUniMiniXM_TrackAutomat *pTrack = NULL;
	sUniMiniXM_Instrument *pInstr = NULL;
	sUniMiniXM_Sample *pSample = NULL;
	unsigned char param_x, param_y;
	int oldvolume, oldfreq, oldpan;
	signed char porta = FALSE;

	param_x = pNote->m_EffectParameter >> 4;	// get effect param x
	param_y = pNote->m_EffectParameter & 0xF;	// get effect param y

	pTrack = &(_pUniMiniXM->m_pMusicChannelTable[ii]);

	porta = (pNote->m_Effect == eXMFx_3XX_PortamentoTo || pNote->m_Effect == eXMFx_5XX_PortamentoToAndVolumeSlide);

    // first store note and instrument number if there was one
    if ( pNote->m_InstrumentIndex && !porta )
      pTrack->m_InstrumentIndex = pNote->m_InstrumentIndex - 1; // keep it

    if (pNote->m_PlayNote && pNote->m_PlayNote != eBitRefresh_KeyOffMask && !porta)	
      pTrack->m_Note = pNote->m_PlayNote - 1;	// remember the note

	if (pTrack->m_InstrumentIndex >= _pUniMiniXM->m_NumberOfInstruments)
	{
		continue; // unvalid sample -> next track.
	}
	// set up some instrument and sample pointers
	pInstr = &_pUniMiniXM->m_pInstrument[pTrack->m_InstrumentIndex];
	if (pInstr->m_KeyTable[pTrack->m_Note] >= 16) continue; // no sample->no track.
	//else:
	pSample = pInstr->m_SampleTable[pInstr->m_KeyTable[pTrack->m_Note]];
	if(!pSample) continue;

    oldvolume = pTrack->m_Volume;
    oldfreq = pTrack->m_Frequency;
    oldpan = pTrack->m_Pan;

    // if there is no more tremolo, set volume to volume + last tremolo delta
    if (pTrack->m_PreviousEffect == eXMFx_7XX_Tremolo && pNote->m_Effect != eXMFx_7XX_Tremolo)
      pTrack->m_Volume += pTrack->m_VolumeDelta;
    pTrack->m_PreviousEffect = pNote->m_Effect;

    pTrack->m_VolumeDelta = 0;
    pTrack->m_RefreshFlag = 0;

	// note change
	if (pNote->m_PlayNote && 
		pNote->m_PlayNote != eBitRefresh_KeyOffMask)
    {
      // get note according to relative note
      pTrack->m_CurrentBaseNote = pNote->m_PlayNote + pSample->m_RelativeNote - 1;

      // get period according to realnote and finetune
      if (_pUniMiniXM->m_Flags & eBitFlagXM_LinearFreq)
      {
	pTrack->m_Period = (10 * 12 * 16 * 4) - (pTrack->m_CurrentBaseNote * 16 * 4) - (pSample->m_FineTune / 2);
      }
      else
      {
#ifdef UNIMINIXM_COMPILE_MODPERIOD
	pTrack->m_Period = GetModPeriod(pTrack->m_CurrentBaseNote, pSample->m_FineTune);
#endif
      }
      // frequency only changes if there are no portamento effects
      if (!porta) pTrack->m_Frequency = pTrack->m_Period;

      pTrack->m_RefreshFlag = eBitRefresh_TriggerSample;
    }
    pTrack->m_FrequencyDelta = 0;
    pTrack->m_RefreshFlag |= eBitRefresh_Frequency;
    pTrack->m_RefreshFlag |= eBitRefresh_Volume;

    if (pNote->m_InstrumentIndex)
		ResetTrack(pTrack, pSample);

#ifdef eXMFx_VOLUME_COMPILE
    if (pNote->m_Volume)    UpdateVolume(pTrack, pNote->m_Volume);
#endif
	// key off
     if (pNote->m_PlayNote == eBitRefresh_KeyOffMask || pNote->m_Effect == eXMFx_KXX_KeyOff)
      pTrack->m_KeyOffFlag = TRUE;
		// envlp
#ifdef eXMFx_VOLUMEENVELOPE_COMPILE
    if (pInstr->m_VolumeEnvelopeLoopType & eEnvelopeBit_On)
    {
		if (!pTrack->m_EnvelopeVolumeStopFlag)
			GetEnvelope(pTrack, 
					&pTrack->m_EnvelopeTickPosition, 
					&pTrack->m_EnvelopeTickCount, 
					pInstr->m_VolumeEnvelopeLoopType, 
					pInstr->m_NumberOfVolumePoints, 
					&pInstr->m_VolumePoints[0], 
					pInstr->m_VolumeEnvelopeLoopEnd, 
					pInstr->m_VolumeEnvelopeLoopStartIndex, 
					pInstr->m_NumberOfSustainPoints, 
					&pTrack->m_EnvelopeVolumeHigh32,
					&pTrack->m_EnvelopeVolumeLow32, 
					&pTrack->m_EnvelopeVolumeStopFlag, 
					&pTrack->m_EnvelopeVolumeDelta, 
					eBitRefresh_Volume);
    }
    else
#endif
    if (pTrack->m_KeyOffFlag)
      pTrack->m_EnvelopeVolumeHigh32 = 0;

#ifdef eXMFx_PANENVELOPE_COMPILE
    if (pInstr->m_PanEnvelopeLoopType & eEnvelopeBit_On && !pTrack->m_EnvelopePanStopFlag)
      GetEnvelope(pTrack, 
			&pTrack->m_EnvelopeTickPanPosition, 
			&pTrack->m_EnvelopeTickPanCount, 
			pInstr->m_PanEnvelopeLoopType, 
			pInstr->m_NumberOfPanPoints, 
			&pInstr->m_PanPoints[0], 
			pInstr->m_PanLoopEnd, 
			pInstr->m_PanLoopStartIndex, 
			pInstr->m_NumberOfPanSustainPoints, 
			&pTrack->m_EnvelopePanHigh32, 
			&pTrack->m_EnvelopePanLow32, 
			&pTrack->m_EnvelopePanStopFlag, 
			&pTrack->m_EnvelopePanDelta, 
			eBitRefresh_Pan);
#endif
    // do volume fade out:
    if (pTrack->m_KeyOffFlag)
    { 
      pTrack->m_FadeVolume -= pInstr->m_VolumeFadeOut;
      if (pTrack->m_FadeVolume < 0)
	pTrack->m_FadeVolume = 0;
      pTrack->m_RefreshFlag |= eBitRefresh_Volume;
    }
	switch (pNote->m_Effect)
    {
		// KRB: default to break, and some comment set for empty cases.
	default: break;
	// no 0XX effect processed on this pass.
#ifdef eXMFx_1XX_PortaUp_COMPILE
      case eXMFx_1XX_PortaUp: //1
	  {
	TRACE_EFFECT_USE(eXMFx_1XX_PortaUp);
	if (pNote->m_EffectParameter)
	  pTrack->m_PortamentoUp = pNote->m_EffectParameter;
	break;
      }
#endif
#ifdef eXMFx_2XX_PortaDown_COMPILE
      case eXMFx_2XX_PortaDown: //2
      {
		  	TRACE_EFFECT_USE(eXMFx_2XX_PortaDown);
	if (pNote->m_EffectParameter)
	  pTrack->m_PortamentoDown = pNote->m_EffectParameter;
	break;
      }
#endif
#ifdef eXMFx_3XX_PortamentoTo_COMPILE
      case eXMFx_3XX_PortamentoTo: //3
      {
		  	TRACE_EFFECT_USE(eXMFx_3XX_PortamentoTo);
	if (pNote->m_EffectParameter)
	  pTrack->m_PortamentoVector = pNote->m_EffectParameter;
	pTrack->m_PortamentoTarget = pTrack->m_Period;
	pTrack->m_RefreshFlag &= ~eBitRefresh_TriggerSample;
	pTrack->m_RefreshFlag &= ~eBitRefresh_Frequency;
	break;
      }
#endif
#ifdef eXMFx_5XX_PortamentoToAndVolumeSlide_COMPILE
      case eXMFx_5XX_PortamentoToAndVolumeSlide: //5
      {
		  	TRACE_EFFECT_USE(eXMFx_5XX_PortamentoToAndVolumeSlide);
	pTrack->m_PortamentoTarget = pTrack->m_Period;
	if (pNote->m_EffectParameter)
	  pTrack->m_VolumeSlide = pNote->m_EffectParameter;
	pTrack->m_RefreshFlag &= ~eBitRefresh_TriggerSample;
	pTrack->m_RefreshFlag &= ~eBitRefresh_Frequency;
	break;
      }
#endif
#ifdef eXMFx_4XY_Vibrato_COMPILE
      case eXMFx_4XY_Vibrato: // 4
      {
		  	TRACE_EFFECT_USE(eXMFx_4XY_Vibrato);
	if (param_x)
	  pTrack->m_VibratoVector = param_x;
	if (param_y)
	  pTrack->m_VibratoDepth = param_y;
	GetVibrato(pTrack);
	break;
      }
#endif
#ifdef eXMFx_6XX_VibratoAndVolumeSlide_COMPILE
	  case eXMFx_6XX_VibratoAndVolumeSlide:
      {
		  	TRACE_EFFECT_USE(eXMFx_6XX_VibratoAndVolumeSlide);
		if (pNote->m_EffectParameter)
			pTrack->m_VolumeSlide = pNote->m_EffectParameter;
		GetVibrato(pTrack);
	break;			// not processed on tick 0
      }
#endif
#ifdef eXMFx_TREMOLO_COMPILE
      case eXMFx_7XX_Tremolo:
      {
		  	TRACE_EFFECT_USE(eXMFx_7XX_Tremolo);
	if (param_x)
	  pTrack->m_TremoloVector = param_x;
	if (param_y)
	  pTrack->m_TremoloDepth = param_y;
	break;
      }
#endif
#ifdef eXMFx_8XX_SetPanPosition_COMPILE
      case eXMFx_8XX_SetPanPosition:
      {
		  	TRACE_EFFECT_USE(eXMFx_8XX_SetPanPosition);
	pTrack->m_Pan = pNote->m_EffectParameter;
	pTrack->m_RefreshFlag |= eBitRefresh_Pan;
	break;
      }
#endif
#ifdef eXMFx_9XX_SampleOffset_COMPILE
      case eXMFx_9XX_SampleOffset:
      {		  	
			unsigned int offset=0;
TRACE_EFFECT_USE(eXMFx_9XX_SampleOffset);
	if (pNote->m_EffectParameter) offset = (int)(pNote->m_EffectParameter);
	offset <<=8; 

	if (offset >= pSample->m_LoopStartIndex + pSample->m_LoopLength)
	{
	  pTrack->m_RefreshFlag &= ~eBitRefresh_TriggerSample;
	  pTrack->m_RefreshFlag |= eBitRefresh_Stop;
	}
	else
		pTrack->m_Channel[pTrack->m_ChannelIndex].m_SampleOffset = offset;
	break;
      }
#endif
#ifdef eXMFx_AXY_VolumeSlide_COMPILE
      case eXMFx_AXY_VolumeSlide:
      {
		  	TRACE_EFFECT_USE(eXMFx_AXY_VolumeSlide);
		if (pNote->m_EffectParameter != 0)
			pTrack->m_VolumeSlide = pNote->m_EffectParameter;
	break;
      }
#endif
#ifdef eXMFx_BXX_PatternJump_COMPILE
      case eXMFx_BXX_PatternJump:	// --- 00 B00 : --- 00 D63 , should put us at ord=0, line=63
      {
		  	TRACE_EFFECT_USE(eXMFx_BXX_PatternJump);
	_pUniMiniXM->m_NextPosition = pNote->m_EffectParameter;
	_pUniMiniXM->m_NextLine = 0;
	if (_pUniMiniXM->m_NextPosition >= (int) _pUniMiniXM->m_PatternLength)
	  _pUniMiniXM->m_NextPosition = 0;
	jumpflag = 1;
	if(_pUniMiniXM->m_NextPosition<_pUniMiniXM->m_Position)
		notinfiniteloop =0;
	else
	if(_pUniMiniXM->m_NextPosition==_pUniMiniXM->m_Position && _pUniMiniXM->m_NextLine< _pUniMiniXM->m_LineIndex)
		notinfiniteloop =0;

	break;
      }
#endif
#ifdef eXMFx_CXX_SetVolume_COMPILE
      case eXMFx_CXX_SetVolume:
      {
		  	TRACE_EFFECT_USE(eXMFx_CXX_SetVolume);
	pTrack->m_Volume = pNote->m_EffectParameter;
	pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	break;
      }
#endif
#ifdef eXMFx_DXX_PatternBreak_COMPILE
      case eXMFx_DXX_PatternBreak:
      {
		  	TRACE_EFFECT_USE(eXMFx_DXX_PatternBreak);
	_pUniMiniXM->m_NextLine = (param_x * 10) + param_y;
	if (_pUniMiniXM->m_NextLine > 63)
	  _pUniMiniXM->m_NextLine = 0;
	if (!breakflag && !jumpflag)
	  _pUniMiniXM->m_NextPosition = _pUniMiniXM->m_Position + 1;
	if (_pUniMiniXM->m_NextPosition >= (int) _pUniMiniXM->m_PatternLength)
	  _pUniMiniXM->m_NextPosition = 0;
	break;
      }
#endif
      case eXMFx_EXX_SpecialEffect:
      {
		  	TRACE_EFFECT_USE(eXMFx_EXX_SpecialEffect);
    //BEGINING OF XM SPECIAL 'E' EFFECT SWITCH !
	switch (param_x)
	{
	    // not processed on tick 0 / unsupported
/* KRB: for size, we'd better set them commented:
	  case eXMFx_E9X_Retrig:
	  case eXMFx_ECX_NoteCut:
	  case eXMFx_E0X_SetFilter:
	  case eXMFx_EFX_FunkRepeat:
	  case eXMFx_E3X_SetGlissando:*/
	  default: break;
#ifdef eXMFx_E1X_FinePortamentoUp_COMPILE
	  case eXMFx_E1X_FinePortamentoUp:
	  {
		  TRACE_EFFECT_USE_SPECIAL(eXMFx_E1X_FinePortamentoUp);
	    if (param_y)
	      pTrack->m_FinePortamentoUp = param_y;
	    pTrack->m_Frequency -= (pTrack->m_FinePortamentoUp << 2);
	    break;
	  }
#endif
#ifdef eXMFx_E2X_FinePortamentoDown_COMPILE
	  case eXMFx_E2X_FinePortamentoDown:
	  {
		  TRACE_EFFECT_USE_SPECIAL(eXMFx_E2X_FinePortamentoDown);
	    if (param_y)
	      pTrack->m_FinePortamentoDown = param_y;
	    pTrack->m_Frequency += (pTrack->m_FinePortamentoDown << 2);
	    break;
	  }
#endif
#ifdef eXMFx_E4X_VibratoToWave_COMPILE
	  case eXMFx_E4X_VibratoToWave:
	  {
		  TRACE_EFFECT_USE_SPECIAL(eXMFx_E4X_VibratoToWave);
	    pTrack->m_VibratoType &= 0xF0;
	    pTrack->m_VibratoType |= param_y;
	    break;
	  }
#endif
#ifdef eXMFx_E5X_SetFineTune_COMPILE
	  case eXMFx_E5X_SetFineTune:
	  {
		  TRACE_EFFECT_USE_SPECIAL(eXMFx_E5X_SetFineTune);
	    pSample->m_FineTune = param_y;
	    break;
	  }
#endif
#ifdef eXMFx_E6X_PatternLoop_COMPILE
	  case eXMFx_E6X_PatternLoop:
	  {
		  TRACE_EFFECT_USE_SPECIAL(eXMFx_E6X_PatternLoop);
	    if (param_y == 0)
	      pTrack->m_PatternLoopStartLine = _pUniMiniXM->m_LineIndex;
	    else
	    {
	      if (!pTrack->m_PatternLoopCount)
		pTrack->m_PatternLoopCount = param_y;
	      else
		pTrack->m_PatternLoopCount--;
	      if (pTrack->m_PatternLoopCount)
		_pUniMiniXM->m_LineIndex = pTrack->m_PatternLoopStartLine - 1;
	    }
	    break;
	  }
#endif
#ifdef eXMFx_E7X_SetTremoloWave_COMPILE
	  case eXMFx_E7X_SetTremoloWave:
	  {
		  TRACE_EFFECT_USE_SPECIAL(eXMFx_E7X_SetTremoloWave);
	    pTrack->m_VibratoType &= 0xF;
	    pTrack->m_VibratoType |= (param_y << 4);
	    break;
	  }
#endif
#ifdef eXMFx_E8X_SetPanPosition16_COMPILE
	  case eXMFx_E8X_SetPanPosition16:
	  {
		  TRACE_EFFECT_USE_SPECIAL(eXMFx_E8X_SetPanPosition16);
	    pTrack->m_Pan = param_y << 4;
	    pTrack->m_RefreshFlag |= eBitRefresh_Pan;
	    break;
	  }
#endif
#ifdef eXMFx_EAX_FineVolumeSlideUp_COMPILE
	  case eXMFx_EAX_FineVolumeSlideUp:
	  {
		  TRACE_EFFECT_USE_SPECIAL(eXMFx_EAX_FineVolumeSlideUp);
	    if (param_y)
	      pTrack->m_FineVolumeSlide = param_y;

	    pTrack->m_Volume += pTrack->m_FineVolumeSlide;

	    pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	    break;
	  }
#endif
#ifdef eXMFx_EBX_FineVolumeSlideDown_COMPILE
	  case eXMFx_EBX_FineVolumeSlideDown:
	  {
		  TRACE_EFFECT_USE_SPECIAL(eXMFx_EBX_FineVolumeSlideDown);
	    if (param_y)
	      pTrack->m_FineVolumeSlide = param_y;

	    pTrack->m_Volume -= pTrack->m_FineVolumeSlide;

	    pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	    break;
	  }
#endif
#ifdef eXMFx_EDX_NoteDelay_COMPILE
	  case eXMFx_EDX_NoteDelay:
	  {
		  TRACE_EFFECT_USE_SPECIAL(eXMFx_EDX_NoteDelay);
	    pTrack->m_Volume = oldvolume;
	    pTrack->m_Frequency = oldfreq;
	    pTrack->m_Pan = oldpan;
	    pTrack->m_RefreshFlag &= ~eBitRefresh_Frequency;
	    pTrack->m_RefreshFlag &= ~eBitRefresh_Volume;
	    pTrack->m_RefreshFlag &= ~eBitRefresh_Pan;
	    pTrack->m_RefreshFlag &= ~eBitRefresh_TriggerSample;
	    break;
	  }
#endif
#ifdef eXMFx_EEX_m_PatternDelayCounter_COMPILE
	  case eXMFx_EEX_m_PatternDelayCounter:
	  {
		  TRACE_EFFECT_USE_SPECIAL(eXMFx_EEX_m_PatternDelayCounter);
	    _pUniMiniXM->m_PatternDelayCounter = param_y;
	    _pUniMiniXM->m_PatternDelayCounter *= _pUniMiniXM->m_F0XSpeed;
	    break;
	  }
#endif
	};
	break;
      } // END OF XM SPECIAL 'E' EFFECT SWITCH !
#ifdef eXMFx_FXY_SetSpeed_COMPILE
      case eXMFx_FXY_SetSpeed:
      {
		  	TRACE_EFFECT_USE(eXMFx_FXY_SetSpeed);
	if (pNote->m_EffectParameter < 0x20)
	  _pUniMiniXM->m_F0XSpeed = pNote->m_EffectParameter;
	else
	  SetBPM(_pUniMiniXM,pNote->m_EffectParameter);
	break;
      }
#endif
#ifdef eXMFx_GXX_SetGlobalVolume_COMPILE
      case eXMFx_GXX_SetGlobalVolume:
      {
		  	TRACE_EFFECT_USE(eXMFx_GXX_SetGlobalVolume);
	_pUniMiniXM->m_GlobalVolume = pNote->m_EffectParameter;
	if (_pUniMiniXM->m_GlobalVolume > 64)
	  _pUniMiniXM->m_GlobalVolume = 64;
	pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	break;
      }
#endif
#ifdef eXMFx_HXX_GlobalVolumeSlide_COMPILE
      case eXMFx_HXX_GlobalVolumeSlide:
      {
		  	TRACE_EFFECT_USE(eXMFx_HXX_GlobalVolumeSlide);
	if (pNote->m_EffectParameter)
	  _pUniMiniXM->m_GlobalVolumeSlide = pNote->m_EffectParameter;
	break;
      }
#endif
#ifdef eXMFx_LXX_SetEnvelopePosition_COMPILE
      case eXMFx_LXX_SetEnvelopePosition:
      {
		  	TRACE_EFFECT_USE(eXMFx_LXX_SetEnvelopePosition);
			int currentpos, nextpos, currenttick, nexttick,
				currentVolume, nextvol, tickdiff;

	if (!(pInstr->m_VolumeEnvelopeLoopType & eEnvelopeBit_On))
	  break;

	currentpos = 0;
	// get and interpolate new envlp position
	while (pNote->m_EffectParameter > pInstr->m_VolumePoints[(currentpos + 1) << 1] && currentpos < pInstr->m_NumberOfVolumePoints)
	  currentpos++;

	pTrack->m_EnvelopeTickPosition = currentpos;

	// if the last position, abort the envelope and continue last volume
	if (pTrack->m_EnvelopeTickPosition >= pInstr->m_NumberOfVolumePoints - 1)
	{
	  pTrack->envvol = pInstr->m_VolumePoints[((pInstr->m_NumberOfVolumePoints - 1) << 1) + 1];
	  pTrack->m_EnvelopeVolumeStopFlag = TRUE;
	  break;
	}

	pTrack->m_EnvelopeVolumeStopFlag = FALSE;
	pTrack->m_EnvelopeTickCount = pNote->m_EffectParameter;

	nextpos = pTrack->m_EnvelopeTickPosition + 1;

	currenttick = pInstr->m_VolumePoints[currentpos << 1];	// get tick at this point
	nexttick = pInstr->m_VolumePoints[nextpos << 1];	// get tick at next point

	currentVolume = pInstr->m_VolumePoints[(currentpos << 1) + 1] << 16;	// get VOL at this point << 16
	nextvol = pInstr->m_VolumePoints[(nextpos << 1) + 1] << 16;	// get VOL at next point << 16

	// interpolate 2 points to find delta step
	tickdiff = nexttick - currenttick;
	if (tickdiff)
	  pTrack->envm_VolumeDelta = (nextvol - currentVolume) / tickdiff;
	else
	  pTrack->envm_VolumeDelta = 0;

	tickdiff = pTrack->m_EnvelopeTickCount - currenttick;

	pTrack->envvolfrac = currentVolume + (pTrack->envm_VolumeDelta * tickdiff);
	pTrack->envvol = pTrack->envvolfrac >> 16;
	pTrack->m_EnvelopeTickPosition++;
	break;
      }
#endif
#ifdef eXMFx_PXX_PanSlide_COMPILE
      case eXMFx_PXX_PanSlide:
      {
		  	TRACE_EFFECT_USE(eXMFx_PXX_PanSlide);
	if (pNote->m_EffectParameter)
	{
	  pTrack->panslide = pNote->m_EffectParameter;
	  pTrack->m_RefreshFlag |= eBitRefresh_Pan;
	}
	break;
      }
#endif
#ifdef eXMFx_RXX_MultiRetrig_COMPILE
      case eXMFx_RXX_MultiRetrig:
      {
		  	TRACE_EFFECT_USE(eXMFx_RXX_MultiRetrig);
	if (pNote->m_EffectParameter)
	{
			pTrack->m_RetrigXParam = param_x;
			pTrack->m_RetrigYParam = param_y;
	}
	break;
      }
#endif
#ifdef eXMFx_TREMOR_COMPILE
      case eXMFx_TXX_Tremor:
      {
		  	TRACE_EFFECT_USE(eXMFx_TXX_Tremor);
	if (pNote->m_EffectParameter)
	{
	  pTrack->m_TremorKeepX = (param_x + 1);
	  pTrack->m_TremorKeepY = (param_y + 1);
	}
	// tremor
	if (pTrack->m_TremorPosition >= pTrack->m_TremorKeepX)
		pTrack->m_VolumeDelta = -pTrack->m_Volume;
	pTrack->m_TremorPosition++;
	if (pTrack->m_TremorPosition >= (pTrack->m_TremorKeepX + pTrack->m_TremorKeepY))
		pTrack->m_TremorPosition = 0;
	pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	
	break;
      }
#endif
#ifdef eXMFx_XXX_ExtraFinePortamento_COMPILE
      case eXMFx_XXX_ExtraFinePortamento:
      {
		  	TRACE_EFFECT_USE(eXMFx_XXX_ExtraFinePortamento);
	if (param_x == 1)
	{
	  if (param_y)
	    pTrack->m_ExtraPortamentoUp = param_y;
	  pTrack->m_Frequency -= pTrack->m_ExtraPortamentoUp;
	}
	else if (param_x == 2)
	{
	  if (param_y)
	    pTrack->m_ExtraPortamentoDown = param_y;
	  pTrack->m_Frequency += pTrack->m_ExtraPortamentoDown;
	}
	break;
      }
#endif
    };

#ifdef eXMFx_INSTRUMENTVIBRATO_COMPILE
		GetInstrumentVibrato(pTrack, pInstr);
#endif
    RefreshFlags(_pUniMiniXM,pTrack, pSample );
	// bound volume here, it is better than testing it everywhere:
	if( pTrack->m_Volume<0 ) pTrack->m_Volume = 0;
	else if(pTrack->m_Volume>64) pTrack->m_Volume=64;
  } // end of loop by track
  return notinfiniteloop; // means not entered an infinite loop.
}

/*
	\brief	Refresh Effects
*/
void RefreshEffects( sUniMiniXM *_pUniMiniXM )
{
  sUniMiniXM_Note *pNote;
  int ii;

  pNote = _pUniMiniXM->m_pPattern[_pUniMiniXM->m_pPatternIndexList[_pUniMiniXM->m_Position]].m_pNoteTable + 
				(_pUniMiniXM->m_LineIndex * _pUniMiniXM->m_NumberOfChannels);

  if (!pNote) return;

  for (ii = 0; ii < _pUniMiniXM->m_NumberOfChannels; ii++, pNote++)
  {
    sUniMiniXM_TrackAutomat *pTrack = NULL;
    sUniMiniXM_Instrument *pInstr = NULL;
    sUniMiniXM_Sample *pSample = NULL;
    unsigned char effect, param_x, param_y;

	pTrack = &_pUniMiniXM->m_pMusicChannelTable[ii];

	// unvalid insrument -> no valid track ATM, go next.
	if (pTrack->m_InstrumentIndex >= (int) _pUniMiniXM->m_NumberOfInstruments) continue;
	pInstr = &_pUniMiniXM->m_pInstrument[pTrack->m_InstrumentIndex];
	// another validity test:
	if (pInstr->m_KeyTable[pTrack->m_Note] >= 16) continue;

	pSample = pInstr->m_SampleTable[pInstr->m_KeyTable[pTrack->m_Note]];
	if(!pSample) continue;

	// get effect and parameters
	effect = pNote->m_Effect;	
	param_x = pNote->m_EffectParameter >> 4;	
	param_y = pNote->m_EffectParameter & 0xF;	

	pTrack->m_VolumeDelta = 0;		// init for tremolo / tremor etc
	pTrack->m_FrequencyDelta = 0;	// init for vibrato / arpeggio etc
	pTrack->m_RefreshFlag = 0;

	// envelope change
#ifdef eXMFx_VOLUMEENVELOPE_COMPILE
    if (pInstr->m_VolumeEnvelopeLoopType & eEnvelopeBit_On && !pTrack->m_EnvelopeVolumeStopFlag)
      GetEnvelope(pTrack, 
					&pTrack->m_EnvelopeTickPosition, 
					&pTrack->m_EnvelopeTickCount, 
					pInstr->m_VolumeEnvelopeLoopType, 
					pInstr->m_NumberOfVolumePoints, 
					&pInstr->m_VolumePoints[0], 
					pInstr->m_VolumeEnvelopeLoopEnd, 
					pInstr->m_VolumeEnvelopeLoopStartIndex, 
					pInstr->m_NumberOfSustainPoints, 
					&pTrack->m_EnvelopeVolumeHigh32, 
					&pTrack->m_EnvelopeVolumeLow32, 
					&pTrack->m_EnvelopeVolumeStopFlag, 
					&pTrack->m_EnvelopeVolumeDelta, 
					eBitRefresh_Volume);
#endif
#ifdef eXMFx_PANENVELOPE_COMPILE
    if (pInstr->m_PanEnvelopeLoopType & eEnvelopeBit_On && !pTrack->m_EnvelopePanStopFlag)
      GetEnvelope(pTrack, 
					&pTrack->m_EnvelopeTickPanPosition, 
					&pTrack->m_EnvelopeTickPanCount, 
					pInstr->m_PanEnvelopeLoopType, 
					pInstr->m_NumberOfPanPoints, 
					&pInstr->m_PanPoints[0], 
					pInstr->m_PanLoopEnd, 
					pInstr->m_PanLoopStartIndex, 
					pInstr->m_NumberOfPanSustainPoints, 
					&pTrack->m_EnvelopePanHigh32, 
					&pTrack->m_EnvelopePanLow32, 
					&pTrack->m_EnvelopePanStopFlag, 
					&pTrack->m_EnvelopePanDelta, 
					eBitRefresh_Pan);
#endif

    // Volume fade
	if (pTrack->m_KeyOffFlag)
    {
      pTrack->m_FadeVolume -= pInstr->m_VolumeFadeOut;
      if (pTrack->m_FadeVolume < 0)
	pTrack->m_FadeVolume = 0;
      pTrack->m_RefreshFlag |= eBitRefresh_Volume;
    }

#ifdef eXMFx_VOLUME_COMPILE
    switch (pNote->m_Volume >> 4)
    {
      case 0x6:
      {
	pTrack->m_Volume -= (pNote->m_Volume & 0xF);
	pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	break;
      }
      case 0x7:
      {
	pTrack->m_Volume += (pNote->m_Volume & 0xF);
	pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	break;
      }
#ifdef eXMFx_4XY_Vibrato_COMPILE
      case 0xb:
      {
	pTrack->m_VibratoDepth = (pNote->m_Volume & 0xF);

	GetVibrato(pTrack);

	pTrack->m_VibratoPosition += pTrack->m_VibratoVector;
	if (pTrack->m_VibratoPosition > 31)
	  pTrack->m_VibratoPosition -= 64;
	break;
      }
#endif
      case 0xd:
      {
	pTrack->m_Pan -= (pNote->m_Volume & 0xF);
	pTrack->m_RefreshFlag |= eBitRefresh_Pan;
	break;
      }
      case 0xe:
      {
	pTrack->m_Pan += (pNote->m_Volume & 0xF);
	pTrack->m_RefreshFlag |= eBitRefresh_Pan;
	break;
      }
#ifdef eXMFx_3XX_PortamentoTo_COMPILE
      case 0xf:
      {
		GetPortamento(pTrack);
		break;
      }
 // endif eXMFx_3XX_PortamentoTo_COMPILE
#endif
    };
	// endif eXMFx_VOLUME_COMPILE
#endif
    switch (effect)
    {
#ifdef eXMFx_0XY_Arpeggio_COMPILE
      case eXMFx_0XY_Arpeggio:
      {
	if (pNote->m_EffectParameter > 0)
	{
	  switch (_pUniMiniXM->m_TickCount % 3)
	  {
	    case 1:
	      if (_pUniMiniXM->m_Flags & eBitFlagXM_LinearFreq)
		pTrack->m_FrequencyDelta = param_x << 6;
#ifdef UNIMINIXM_COMPILE_MODPERIOD
	      else
		pTrack->m_FrequencyDelta = GetModPeriod(pTrack->m_CurrentBaseNote + param_x, pSample->m_FineTune) - GetModPeriod(pTrack->m_CurrentBaseNote, pSample->m_FineTune);
#endif
	      break;
	    case 2:
	      if (_pUniMiniXM->m_Flags & eBitFlagXM_LinearFreq)
		pTrack->m_FrequencyDelta = param_y << 6;
#ifdef UNIMINIXM_COMPILE_MODPERIOD
	      else
		pTrack->m_FrequencyDelta = GetModPeriod(pTrack->m_CurrentBaseNote + param_y, pSample->m_FineTune) - GetModPeriod(pTrack->m_CurrentBaseNote, pSample->m_FineTune);
#endif
	      break;
	  };
	  pTrack->m_RefreshFlag |= eBitRefresh_Frequency;
	}
	break;
      }
#endif
#ifdef eXMFx_1XX_PortaUp_COMPILE
      case eXMFx_1XX_PortaUp:
      {
	pTrack->m_FrequencyDelta = 0;

	pTrack->m_Frequency -= pTrack->m_PortamentoUp << 2;	// sub freq
	if (pTrack->m_Frequency < 56)
	  pTrack->m_Frequency = 56;	// stop at B#8
	pTrack->m_RefreshFlag |= eBitRefresh_Frequency;
	break;
      }
#endif
#ifdef eXMFx_2XX_PortaDown_COMPILE
      case eXMFx_2XX_PortaDown:
      {
	pTrack->m_FrequencyDelta = 0;

	pTrack->m_Frequency += pTrack->m_PortamentoDown << 2; // sub freq
	pTrack->m_RefreshFlag |= eBitRefresh_Frequency;
	break;
      }
#endif
#ifdef eXMFx_3XX_PortamentoTo_COMPILE
      case eXMFx_3XX_PortamentoTo:
      {
		pTrack->m_FrequencyDelta = 0;
		GetPortamento(pTrack);
	break;
      }
#endif
#ifdef eXMFx_4XY_Vibrato_COMPILE
	  case eXMFx_4XY_Vibrato:
      {
	GetVibrato(pTrack);
	pTrack->m_VibratoPosition += pTrack->m_VibratoVector;
	if (pTrack->m_VibratoPosition > 31)
	  pTrack->m_VibratoPosition -= 64;
	break;
      }
#endif
#ifdef eXMFx_5XX_PortamentoToAndVolumeSlide_COMPILE
      case eXMFx_5XX_PortamentoToAndVolumeSlide:
      {
		pTrack->m_FrequencyDelta = 0;
		GetPortamento(pTrack);

	param_x = pTrack->m_VolumeSlide >> 4;	
	param_y = pTrack->m_VolumeSlide & 0xF;	

	if (param_x) pTrack->m_Volume += param_x;
	else if (param_y) pTrack->m_Volume -= param_y;

	pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	break;
      }
#endif
#ifdef eXMFx_6XX_VibratoAndVolumeSlide_COMPILE
	  case eXMFx_6XX_VibratoAndVolumeSlide:
      {
			GetVibrato(pTrack);
			pTrack->m_VibratoPosition += pTrack->m_VibratoVector;
			if (pTrack->m_VibratoPosition > 31)
				pTrack->m_VibratoPosition -= 64;

			param_x = pTrack->m_VolumeSlide >> 4;
			param_y = pTrack->m_VolumeSlide & 0xF;

			// slide up takes precedence over down
			if (param_x)  pTrack->m_Volume += param_x;
			else if (param_y) pTrack->m_Volume -= param_y;

			pTrack->m_RefreshFlag |= eBitRefresh_Volume;
			break;
      }
#endif
#ifdef eXMFx_TREMOLO_COMPILE
	  case eXMFx_7XX_Tremolo:
      {
			{
			unsigned char tt;

			tt = (pTrack->m_TremoloPosition & 31);

			switch ((pTrack->m_VibratoType >> 4) & 3)
			{
				case 0: //0, sin, 3 random. random=sin for the moment.
				case 3:
				pTrack->m_VolumeDelta = (int) 
					(fabsf((sinf((float) (pTrack->m_VibratoPosition) * m_FrequencyFXConstant2PiDiv64)) * 256.0f));
				break;
				case 1:
				tt <<= 3;		// ramp down
				if (pTrack->m_TremoloPosition < 0)
				tt = 255 - tt;
				pTrack->m_VolumeDelta = tt;
				break;
				case 2:
				pTrack->m_VolumeDelta = 255;	// square
				break;
			};

			pTrack->m_VolumeDelta *= pTrack->m_TremoloDepth;
			pTrack->m_VolumeDelta >>= 6;

			if (pTrack->m_TremoloPosition >= 0)
			{
				if (pTrack->m_Volume + pTrack->m_VolumeDelta > 64)
				pTrack->m_VolumeDelta = 64 - pTrack->m_Volume;
			}
			else
			{
				if ((short) (pTrack->m_Volume - pTrack->m_VolumeDelta) < 0)
				pTrack->m_VolumeDelta = pTrack->m_Volume;
				pTrack->m_VolumeDelta = -pTrack->m_VolumeDelta;
			}

			pTrack->m_TremoloPosition += pTrack->m_TremoloVector;
			if (pTrack->m_TremoloPosition > 31)
				pTrack->m_TremoloPosition -= 64;

			pTrack->m_RefreshFlag |= eBitRefresh_Volume;	
			}
		break;
      }
#endif
#ifdef eXMFx_AXY_VolumeSlide_COMPILE
      case eXMFx_AXY_VolumeSlide:
      {
		param_x = pTrack->m_VolumeSlide >> 4;
		param_y = pTrack->m_VolumeSlide & 0xF;

		// slide up more important than down
		if (param_x) pTrack->m_Volume += param_x;
		else if (param_y)  pTrack->m_Volume -= param_y;

		pTrack->m_RefreshFlag |= eBitRefresh_Volume;
		break;
      }
#endif
	// extended mod effects
      case eXMFx_EXX_SpecialEffect:
      {
	switch (param_x)
	{
#ifdef eXMFx_ECX_NoteCut_COMPILE
	  case eXMFx_ECX_NoteCut:
	  {
	    if (_pUniMiniXM->tick == param_y)
	    {
	      pTrack->volume = 0;
	      pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	    }
	    break;
	  }
#endif
#ifdef eXMFx_E9X_Retrig_COMPILE
	  case eXMFx_E9X_Retrig:
	  {
	    if (!param_y)
	      break;
	    if (!(_pUniMiniXM->tick % param_y))
	    {
	      pTrack->m_RefreshFlag |= eBitRefresh_TriggerSample;
	      pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	      pTrack->m_RefreshFlag |= eBitRefresh_Frequency;
	    }
	    break;
	  }
#endif
#ifdef eXMFx_EDX_NoteDelay_COMPILE
	  case eXMFx_EDX_NoteDelay:
	  {
	    if (_pUniMiniXM->m_TickCount == param_y)
	    {
	      // Process instrument
	      ResetTrack(pTrack, pSample);

	      pTrack->m_Frequency = pTrack->m_Period;
	      pTrack->m_RefreshFlag |= eBitRefresh_Frequency;
#ifdef eXMFx_VOLUME_COMPILE
	      if (pNote->m_Volume)
				UpdateVolume(pTrack, pNote->m_Volume);
#endif
	      pTrack->m_RefreshFlag |= eBitRefresh_TriggerSample;
	    }
	    else
	    {
	      pTrack->m_RefreshFlag &= ~eBitRefresh_Volume;
	      pTrack->m_RefreshFlag &= ~eBitRefresh_Frequency;
	      pTrack->m_RefreshFlag &= ~eBitRefresh_Pan;
	      pTrack->m_RefreshFlag &= ~eBitRefresh_TriggerSample;
	    }
	    break;
	  }
#endif // ->

	};
	break;
      }
#ifdef eXMFx_RXX_MultiRetrig_COMPILE
      case eXMFx_RXX_MultiRetrig:
      {
	if (!pTrack->m_RetrigYParam)
	  break;

	if (!(_pUniMiniXM->m_TickCount % pTrack->m_RetrigYParam))
	{
	  if (pTrack->m_RetrigXParam)
	  {
			// shortcode version:
		 unsigned char retx =( pTrack->m_RetrigXParam &0x07);
		 if(retx<6)
		 {
			int m_VolumeDelta = 1<<(retx-1); // case 1,2,3,4,5 + 9,a,b,c,d
			if(pTrack->m_RetrigXParam & 0x08) m_VolumeDelta=-m_VolumeDelta;
			pTrack->m_Volume += m_VolumeDelta;
		 }
		 else
		 switch(pTrack->m_RetrigXParam)	 
		 {
			case 6:
				pTrack->m_Volume = pTrack->m_Volume * 2 / 3;
			break;
			case 7:
				pTrack->m_Volume >>= 1;
			break;
			case 0xE:
				pTrack->m_Volume = pTrack->m_Volume * 3 / 2;
			break;
			case 0xF:
				pTrack->m_Volume <<= 1;
			break;
		 };

	    pTrack->m_RefreshFlag |= eBitRefresh_Volume;
	  }
	  pTrack->m_RefreshFlag |= eBitRefresh_TriggerSample;
	}
	break;
      }
#endif
#ifdef eXMFx_HXX_GlobalVolumeSlide_COMPILE
      case eXMFx_HXX_GlobalVolumeSlide:
      {
	param_x = _pUniMiniXM->m_GlobalVolumeSlide >> 4;
	param_y = _pUniMiniXM->m_GlobalVolumeSlide & 0xF;

	// up before down
	if (param_x)
	{
	  _pUniMiniXM->m_GlobalVolume += param_x;
	  if (_pUniMiniXM->m_GlobalVolume > 64)
	    _pUniMiniXM->m_GlobalVolume = 64;
	}
	else if (param_y)
	{
	  _pUniMiniXM->m_GlobalVolume -= param_y;
	  if (_pUniMiniXM->m_GlobalVolume < 0)
	    _pUniMiniXM->m_GlobalVolume = 0;
	}
	break;
      }
#endif
#ifdef eXMFx_PXX_PanSlide_COMPILE
      case eXMFx_PXX_PanSlide:
      {
	param_x = pTrack->panslide >> 4;	
	param_y = pTrack->panslide & 0xF;

	// right is more important than left
	if (param_x)
	{
	  pTrack->pan += param_x;
	  if (pTrack->pan > 255)
	    pTrack->pan = 255;
	}
	else if (param_y)
	{
	  pTrack->pan -= param_y;
	  if (pTrack->pan < 0)
	    pTrack->pan = 0;
	}

	pTrack->m_RefreshFlag |= eBitRefresh_Pan;
	break;
      }
#endif
#ifdef eXMFx_TREMOR_COMPILE
	case eXMFx_TXX_Tremor:
	{
			if (pTrack->m_TremorPosition >= pTrack->m_TremorKeepX)
				pTrack->m_VolumeDelta = -pTrack->m_Volume;
			pTrack->m_TremorPosition++;
			if (pTrack->m_TremorPosition >= (pTrack->m_TremorKeepX + pTrack->m_TremorKeepY))
				pTrack->m_TremorPosition = 0;
			pTrack->m_RefreshFlag |= eBitRefresh_Volume;
		break;
	}
#endif
    };

	// inst. Vibrato:
#ifdef eXMFx_INSTRUMENTVIBRATO_COMPILE
		GetInstrumentVibrato(pTrack, pInstr);	// this gets added to previous m_FrequencyDeltas
#endif
	// bound volume here, it is better than testing it everywhere:
	if( pTrack->m_Volume<0 ) pTrack->m_Volume = 0;
	else if(pTrack->m_Volume>64) pTrack->m_Volume=64;

    RefreshFlags(_pUniMiniXM,pTrack, pSample );
  }	
	// end of RefreshEffects()
}
/*
	\brief	Refresh Flags
*/
void RefreshFlags( sUniMiniXM *_pUniMiniXM, sUniMiniXM_TrackAutomat * pTrack, sUniMiniXM_Sample * pSampleToLaunch )
{
	unsigned char chanindex;
	sUniMiniXM_Channel *pOldMixChannel,*pNewMixChannel;

	chanindex = pTrack->m_ChannelIndex;
	pNewMixChannel = &(pTrack->m_Channel[chanindex]);

	if (!(pTrack->m_Frequency + pTrack->m_FrequencyDelta))
		pTrack->m_RefreshFlag &= ~eBitRefresh_Frequency;	// divide by 0 check

	// launch new sample on the track!
	if (pTrack->m_RefreshFlag & eBitRefresh_TriggerSample)
	{

		//if there was a sample before, makes the old channel fade out nicely.
		if (pNewMixChannel->m_pSample != NULL)
		{
			// Swaps between channels with old fading out:
			pOldMixChannel = pNewMixChannel;
			chanindex^=1; //0->1, 1->0
			pNewMixChannel = &(pTrack->m_Channel[chanindex]);
			pTrack->m_ChannelIndex = chanindex;
		
#ifdef	UNIMINIXM_TRACE_VOLUME_BY_SAMPLE
			UniMiniXM_CloseMixMonitor( pOldMixChannel->m_pSample,pNewMixChannel);
#endif
			memcpy(pNewMixChannel, pOldMixChannel, sizeof(sUniMiniXM_Channel));
			// bye bye!			
			pOldMixChannel->m_CurrentVolume = 0;
			pOldMixChannel->m_LeftVolume = 0;
			pOldMixChannel->m_RightVolume = 0;
		}

		pNewMixChannel->m_pSample = pSampleToLaunch;
#ifdef	UNIMINIXM_TRACE_VOLUME_BY_SAMPLE
		// use sample volume monitor:
		UniMiniXM_OpenMixMonitor(pSampleToLaunch,pNewMixChannel);
#endif
		// launch sample:
		if (pNewMixChannel->m_SampleOffset >= pSampleToLaunch->m_LoopStartIndex + pSampleToLaunch->m_LoopLength)
			pNewMixChannel->m_SampleOffset = 0;

		pNewMixChannel->m_SamplePositionHigh32 = pNewMixChannel->m_SampleOffset;
		pNewMixChannel->m_SamplePositionLow32 = 0;
		pNewMixChannel->m_CurrentVectorDirection = eSampleLoopForward;
		pNewMixChannel->m_SampleOffset = 0;	// reset it (in case other samples come in and get corrupted etc)

		// reset volume ramping
		pNewMixChannel->m_Slide_LeftVolume = 0;
		pNewMixChannel->m_Slide_RightVolume = 0;
		pNewMixChannel->m_Slide_Count = 0;
	}

	if (pTrack->m_RefreshFlag & eBitRefresh_Volume)
	{
		float finalvol;
		int volume;

		finalvol = (float) pTrack->m_EnvelopeVolumeHigh32;	//  6 bits (   64)
		finalvol *= (float)(pTrack->m_Volume + pTrack->m_VolumeDelta);	//  6 bits (   64)
		finalvol *= (float)(pTrack->m_FadeVolume);	// 16 bits (65536)
		finalvol *= (float)(_pUniMiniXM->m_GlobalVolume);	//  6 bits (   64)
		finalvol *= (255.0f / (64.0f * 64.0f * 65536.0f * 64.0f)) * 0.5f;
		volume = (int) finalvol;

		pNewMixChannel->m_CurrentVolume = volume;
		pNewMixChannel->m_LeftVolume = (volume * pNewMixChannel->m_Pan) / 255;
		pNewMixChannel->m_RightVolume = (volume * (255L - pNewMixChannel->m_Pan)) / 255;

	}
  if (pTrack->m_RefreshFlag & eBitRefresh_Pan)
  {
    int pan = pTrack->m_Pan + ((pTrack->m_EnvelopePanHigh32 - 32) * ((128 - abs(pTrack->m_Pan - 128)) / 32));

    if (pan < 0)
      pan = 0;
    if (pan > 255)
      pan = 255;

    if (pNewMixChannel)
    {
      pNewMixChannel->m_Pan = pan;
      pNewMixChannel->m_LeftVolume = (pNewMixChannel->m_CurrentVolume * pan) / 255;
      pNewMixChannel->m_RightVolume = (pNewMixChannel->m_CurrentVolume * (255L - pan)) / 255;
    }


  }

  if (pTrack->m_RefreshFlag & eBitRefresh_Frequency)
  {
    int freq;

    if (_pUniMiniXM->m_Flags & eBitFlagXM_LinearFreq)
      freq = 
		( (int)(8362.0f*powf(2.0f, ((6.0f*12.0f*16.0f*4.0f - (pTrack->m_Frequency + pTrack->m_FrequencyDelta)) /
		(12.0f*16.0f*4.0f)))) );
 
    else
	{ freq = (pTrack->m_Frequency + pTrack->m_FrequencyDelta);
		if(freq>0)
		 freq = 14317056L / freq; //? value certainly wrong. there was a dangerous macro here.
		else freq=0;
	}

    if (freq < 100) freq = 100;

	{	// this way, we got enough acuracy for the note speed, with
		// a64 int result:
		unsigned int divr = freq/_pUniMiniXM->m_MixFrequency ;
		unsigned int remr = freq-=(divr*_pUniMiniXM->m_MixFrequency);
		pNewMixChannel->m_SampleVectorHigh32 = divr;
		pNewMixChannel->m_SampleVectorLow32 = ((remr<<16)/_pUniMiniXM->m_MixFrequency)<<16 ;
	}

  }
  if (pTrack->m_RefreshFlag & eBitRefresh_Stop)
  {
    pNewMixChannel->m_SamplePositionLow32 = 0;
    pNewMixChannel->m_SamplePositionHigh32 = 0;
    pNewMixChannel->m_SampleOffset = 0;	// if this channel gets stolen it will be safe
  }	

}

void ResetTrack(sUniMiniXM_TrackAutomat * ptrack, sUniMiniXM_Sample * pSample)
{
  ptrack->m_Volume = (int) pSample->m_DefaultVolumeChar;
  ptrack->m_Pan = pSample->m_DefaultPan;
  ptrack->m_EnvelopeVolumeHigh32 = 64;
  ptrack->m_EnvelopeTickPosition = 0;
  ptrack->m_EnvelopeTickCount = 0;
  ptrack->m_EnvelopeVolumeDelta = 0;

  ptrack->m_EnvelopePanHigh32 = 32;
  ptrack->m_EnvelopeTickPanPosition = 0;
  ptrack->m_EnvelopeTickPanCount = 0;
  ptrack->m_EnvelopePanDelta = 0;

  ptrack->m_KeyOffFlag = FALSE;
  ptrack->m_FadeVolume = 65536;
  ptrack->m_EnvelopeVolumeStopFlag = FALSE;
  ptrack->m_EnvelopePanStopFlag = FALSE;
  ptrack->m_InstrumentVibratoSweepPosition = 0;
  ptrack->m_InstrumentVibratoPosition = 0;

  // retrigger tremolo and vibrato waveforms
  if ((ptrack->m_VibratoType & 0xF) < 4)
    ptrack->m_VibratoPosition = 0;
  if ((ptrack->m_VibratoType >> 4) < 4)
    ptrack->m_TremoloPosition = 0;

  ptrack->m_TremorPosition = 0;		// retrigger tremor count

  ptrack->m_RefreshFlag |= eBitRefresh_Volume;
  ptrack->m_RefreshFlag |= eBitRefresh_Pan;
}

/*
	\brief private use
*/
void SetBPM(sUniMiniXM *_pUniMiniXM, int bpm)
{
  float herz;
  if(_pUniMiniXM->m_FX0bpmSpeed==bpm) return;
  _pUniMiniXM->m_FX0bpmSpeed = bpm;
  herz = (float)bpm *( 2.0f / 5.0f );
   _pUniMiniXM->m_BPMSamplesPerTick = (int)((float)_pUniMiniXM->m_MixFrequency  * (1000.0f / herz) / 1000.0f);
}
/*
	\brief	Make a jump to a given pattern position.
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\param _patternIndex Index in the list of pattern to play.
*/
void	UniMiniXM_JumpToPattern( sUniMiniXM *_pUniMiniXM,
								unsigned int _patternIndex)
{
	_pUniMiniXM->m_GlobalVolume = _pUniMiniXM->m_DefaultGlobalVolume;
	_pUniMiniXM->m_F0XSpeed = _pUniMiniXM->m_DefaultF0XSpeed;
	_pUniMiniXM->m_LineIndex = 0;
	_pUniMiniXM->m_Position = 0;
	_pUniMiniXM->m_NextPosition = -1;
	_pUniMiniXM->m_NextLine = -1;
	_pUniMiniXM->m_TickCount = 0;
	_pUniMiniXM->m_PatternDelayCounter = 0;
	// reset the whole mixer state track:
	memset(_pUniMiniXM->m_pMusicChannelTable, 0, _pUniMiniXM->m_NumberOfChannels * sizeof(sUniMiniXM_TrackAutomat));

	// declare no sample generated:
	_pUniMiniXM->m_nsamplesStillToBeMixed = 0;
	// time play tick restart:
	_pUniMiniXM->m_TotalTickMixed = 0;	
}

#ifdef _UNIMINIXM_SEEKTIME_
/*
	\brief This function is only available if compiled with _UNIMINIXM_SEEKTIME_.
			It provides Random time access. 
			It will jump to a position given in seconds * 256, in normal play time.
			At the moment, it will jump to the begining of the pattern within this time,
			if it is not already in this pattern.
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\param	_secondX256 time in seconds, given with 8 bits after the dots (*256)		
*/
void	UniMiniXM_SeekTime(sUniMiniXM *_pUniMiniXM,int _secondX256)
{
	int end;
	int lastTime256;
	int	indexa,indexb;
	PatternTimeReference *pTimeState;
	if(_secondX256 <0) _secondX256 =0;
	// we don't manage end, if it is asked to reach the end.
	end = _pUniMiniXM->m_pPatternTimeReferenceTable[_pUniMiniXM->m_PatternTimeReferenceTableLength].m_Secx256 ;
	if( _secondX256>=end  ) return;

	// do nothing if near:
	// we use -200 and +150 because such time error should be forgivable
	// in a realtime continuous play.
	// without pattern test: -150 -> 100 : OK
	lastTime256 = ((_pUniMiniXM->m_TotalTickMixed<<8)/_pUniMiniXM->m_MixFrequency);
	if(_secondX256>=(lastTime256-96) && _secondX256<(lastTime256+96) ) return; // OK.

	// find new index by dichotomy, cause frametable can be very vast:	
	indexa=0;
	indexb=_pUniMiniXM->m_PatternTimeReferenceTableLength-1;
	while(indexa+1 != indexb )
	{
		int t_m;
		int middle = (indexa+indexb)>>1;
		if(middle==indexa) middle++;
		t_m = (int) _pUniMiniXM->m_pPatternTimeReferenceTable[middle].m_Secx256 ;
		if(_secondX256<t_m)
			indexb=middle;
		 else
			indexa=middle;
	}
	pTimeState = _pUniMiniXM->m_pPatternTimeReferenceTable+indexa ;
	// do nothing in the right lapse
	if(pTimeState->m_Position == _pUniMiniXM->m_Position ) return;
	//no, it stops previous sounds !! 
	UniMiniXM_JumpToPattern( _pUniMiniXM,0);
	_pUniMiniXM->m_F0XSpeed = pTimeState->m_Speed;
	SetBPM(_pUniMiniXM, pTimeState->m_BPM );

	_pUniMiniXM->m_LineIndex = pTimeState->m_Line;
	_pUniMiniXM->m_Position = pTimeState->m_Position;

	_pUniMiniXM->m_TotalTickMixed =pTimeState->m_timeTicks ;
		//searchTick;
}
#endif