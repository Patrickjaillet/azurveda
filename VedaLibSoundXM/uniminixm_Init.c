/*! \file 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/

#include "uniminixm_functions.h"
// for memcpy() memset(),  string.h is the more platform independant.
#include <string.h>

// use of optimized memset from the current devkit:
#ifndef NULL
#define	NULL	0L
#endif

extern	void *uniminixm_CleanAlloc(unsigned int _bytesize);
extern	void uniminixm_FreeAlloc(void *_pAlloc);

extern void SetBPM(sUniMiniXM *_pUniMiniXM, int bpm);

static signed short ReadSignedShortLE(const unsigned char *pBin)
{ 
	return (signed short)((unsigned short)(*pBin) | ((signed short)(pBin[1])<<8) ); 
};
static signed int ReadSignedIntLE(const unsigned char *pBin)
{ return 
		(signed int)
			( (unsigned int)(*pBin) | (((unsigned int)pBin[1])<<8)
			| (((unsigned int)pBin[2])<<16)	| (((signed int)pBin[3])<<24)  
			); 
};	
// global are OK if they are static const:
static const char *g_xmtag = "Extended Mod";
/*
	\brief	Init by reading a XM binary file into memory.
			You must alloc a sUniMiniXM struct in memory or on the stack.
			UniMiniXM_Close() must be called to free the memory and end
			the thread, except if the init fails.
			You also got to manage the way the file binary is set in memory.
			(UniMiniXM and XM Format can't handle streaming).
			Once the result is eResult_OK, you will be able to use the other 
			UniMiniXM_XXXX functions.
	\param	_pUniMiniXM	the struct object that manage the whole player.
	\param	_pFileBinary binary pointer to the beginning of the memory.
	\param	_FilebinaryLength length of memory pointed by _pFileBinary.
	\param	_outputHerzFrequency	play frequency for the output(22050,44100,...)
	\return a eXMMusicInitResult enum for the result.
*/
eXMMusicInitResult	UniMiniXM_Init( sUniMiniXM *_pUniMiniXM,
								  const unsigned char *_pFileBinary,
								  const unsigned int _FilebinaryLength,
								  const	unsigned int _outputHerzFrequency
								  )
{
	// note: this function take care of endianness of the code.
	// the xm files are always LE, this code can be LE or BE.
	unsigned short fileNumberOPatterns = 0;
	int ii,seek=0;
	unsigned int FirstChunk;
	// first clean up the struct:
	memset(_pUniMiniXM, 0, sizeof(sUniMiniXM) );

	// some constants. play frequency could'nt change on the fly:
	//! herz rate of sound signal.
	_pUniMiniXM->m_MixFrequency = _outputHerzFrequency;  

	_pUniMiniXM->m_fInvMixRate = 1.0f/((float)_outputHerzFrequency);
	_pUniMiniXM->m_MinimumSampleStep = (_outputHerzFrequency * eCte_MixerMinimumSampleStep) / 44100;
	_pUniMiniXM->m_fInvSampleStep = 1.0f / (float)_pUniMiniXM->m_MinimumSampleStep;

	// verify XM tag:
	for (ii = 0; ii < 12; ii++)
	if (_pFileBinary[ii] != g_xmtag[ii])
	{
		return(eResult_FileFormatError);
	}
	_pUniMiniXM->m_DefaultGlobalVolume = 64;
	// jump over pattern info: 
	seek = 60;
	FirstChunk = ReadSignedIntLE( &_pFileBinary[seek] );
	seek+=4;
	_pUniMiniXM->m_PatternLength = ReadSignedShortLE( &_pFileBinary[seek] );
	seek+=2;
	_pUniMiniXM->m_RestartPosition = ReadSignedShortLE( &_pFileBinary[seek] );
	seek+=2;
	_pUniMiniXM->m_NumberOfChannels = ReadSignedShortLE( &_pFileBinary[seek] );
	seek+=2;
	fileNumberOPatterns = ReadSignedShortLE( &_pFileBinary[seek] );
	seek+=2;
	_pUniMiniXM->m_NumberOfInstruments = ReadSignedShortLE( &_pFileBinary[seek] );
	seek+=2;
	_pUniMiniXM->m_Flags = ReadSignedShortLE( &_pFileBinary[seek] );
	seek+=2;
	_pUniMiniXM->m_DefaultF0XSpeed = ReadSignedShortLE( &_pFileBinary[seek] );
	seek+=2;
	_pUniMiniXM->m_DefaultBPMSpeed = ReadSignedShortLE( &_pFileBinary[seek] );
	seek+=2;
	memcpy(&(_pUniMiniXM->m_pPatternIndexList),&_pFileBinary[seek],256); 

	// jump to pattern data
	seek = 60 + FirstChunk;

	_pUniMiniXM->m_NumberOPatterns = 0;
	for (ii = 0; ii < (int) _pUniMiniXM->m_PatternLength; ii++)
	{
		if (_pUniMiniXM->m_pPatternIndexList[ii] >= _pUniMiniXM->m_NumberOPatterns)
			_pUniMiniXM->m_NumberOPatterns = _pUniMiniXM->m_pPatternIndexList[ii] + 1;
	}
	ii = _pUniMiniXM->m_NumberOPatterns;
	if(fileNumberOPatterns>ii) ii=fileNumberOPatterns;
	_pUniMiniXM->m_NumberOPatternsMem = ii;

	_pUniMiniXM->m_pPattern = uniminixm_CleanAlloc( _pUniMiniXM->m_NumberOPatternsMem * sizeof(sUniMiniXM_Pattern)); 
	if(!(_pUniMiniXM->m_pPattern)) goto memoryfail;

  // extract patterns
  for (ii = 0; ii < fileNumberOPatterns; ii++)
  {
		sUniMiniXM_Pattern *pPattern;
		unsigned short patternsize, nblines;
		unsigned int patternHDRsize;
		unsigned char tempchar = 0;
		pPattern = (_pUniMiniXM->m_pPattern)+ii;
		patternHDRsize = ReadSignedIntLE( &_pFileBinary[seek]);
		seek+=4;
		tempchar = _pFileBinary[seek];
		seek+=1;
		nblines = (unsigned short) ReadSignedShortLE( &_pFileBinary[seek]);
		seek+=2;
		pPattern->m_NumberOfLines = nblines;
		patternsize = (unsigned short) ReadSignedShortLE( &_pFileBinary[seek]);
		seek+=2;
		// pattern alloc		
		pPattern->m_pNoteTable = uniminixm_CleanAlloc(pPattern->m_NumberOfLines*_pUniMiniXM->m_NumberOfChannels*sizeof(sUniMiniXM_Note));
		if(! pPattern->m_pNoteTable) goto memoryfail;
	 
		if (patternsize > 0)
		{
			int ii2;
			sUniMiniXM_Note *nptr;

			nptr = pPattern->m_pNoteTable;

			for (ii2 = 0; ii2 < (pPattern->m_NumberOfLines * _pUniMiniXM->m_NumberOfChannels); ii2++)
			{
				unsigned char dat;
				dat =  _pFileBinary[seek];
				seek++;
				if (dat & 0x80)
				{
					if (dat & 1){ nptr->m_PlayNote =_pFileBinary[seek]; seek++; }
					if (dat & 2){ nptr->m_InstrumentIndex =_pFileBinary[seek]; seek++; }
					if (dat & 4){ nptr->m_Volume =_pFileBinary[seek]; seek++; }
					if (dat & 8){ nptr->m_Effect =_pFileBinary[seek]; seek++; }
					if (dat & 16){ nptr->m_EffectParameter =_pFileBinary[seek]; seek++; }
				}
				else
				{
					if (dat)
					nptr->m_PlayNote = dat;
					nptr->m_InstrumentIndex = ReadSignedIntLE( &_pFileBinary[seek]);
					seek+=4;
				}
				if (nptr->m_PlayNote == 97) nptr->m_PlayNote = eBitRefresh_KeyOffMask;

				if (nptr->m_InstrumentIndex > 0x80) nptr->m_InstrumentIndex = 0;

				nptr++;
			} // end loop numchannel
		} // if (patternsize > 0)
	} // end loop /fileNumberOPatterns

  // allocate and clean out any extra patterns 
  // note: we continue the table init.
	if (_pUniMiniXM->m_NumberOPatterns > fileNumberOPatterns)
	{
		for (ii = fileNumberOPatterns; ii < _pUniMiniXM->m_NumberOPatterns; ii++)
		{
			sUniMiniXM_Pattern *pPattern;
			pPattern = &_pUniMiniXM->m_pPattern[ii];
			pPattern->m_NumberOfLines = 64;
			// pattern alloc
			pPattern->m_pNoteTable = uniminixm_CleanAlloc(_pUniMiniXM->m_NumberOfChannels * pPattern->m_NumberOfLines * sizeof(sUniMiniXM_Note));
			if(!pPattern->m_pNoteTable ) goto memoryfail;
		}
	}
  _pUniMiniXM->m_pInstrument = uniminixm_CleanAlloc(_pUniMiniXM->m_NumberOfInstruments * sizeof(sUniMiniXM_Instrument));
	if(!_pUniMiniXM->m_pInstrument) goto memoryfail;

  // load instrument information
  for (ii = 0; ii < (int) _pUniMiniXM->m_NumberOfInstruments; ii++)
  {
    unsigned int ii2;
	sUniMiniXM_Instrument *pInstr;
    unsigned int instHDRsize;
    unsigned short numsamples;
    int firstsampleoffset;

    // point a pointer to that particular instrument
    pInstr = &_pUniMiniXM->m_pInstrument[ii];

    firstsampleoffset = seek; 
	// inst size
	instHDRsize = ReadSignedIntLE( &_pFileBinary[seek]);
	seek+=4;
    firstsampleoffset += instHDRsize;

	// inst name
	seek += 23;
	// nb sample in instrument
	numsamples = ReadSignedShortLE( &_pFileBinary[seek]);
	seek+=2;
    if (numsamples > 16) goto formatfail;

	pInstr->m_NumberOfSamples = numsamples;
	if (numsamples > 0)
	{
		int jj;
		unsigned int sampHDRsize;
		unsigned char tempchar = 0;
		sampHDRsize = ReadSignedIntLE( &_pFileBinary[seek]);
		seek+=4;
		memcpy( &(pInstr->m_KeyTable[0]),&_pFileBinary[seek],96);
		seek+=96;
		for(jj=0 ; jj<24 ; jj++)
		{
			pInstr->m_VolumePoints[jj] = (unsigned short) ReadSignedShortLE(&_pFileBinary[seek]);
			seek+=2;
		}
		for(jj=0 ; jj<24 ; jj++)
		{
			pInstr->m_PanPoints[jj] = (unsigned short) ReadSignedShortLE(&_pFileBinary[seek]);
			seek+=2;
		}

		pInstr->m_NumberOfVolumePoints = _pFileBinary[seek] ;
		seek++;
		pInstr->m_NumberOfPanPoints = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_NumberOfSustainPoints = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_VolumeEnvelopeLoopStartIndex = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_VolumeEnvelopeLoopEnd = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_NumberOfPanSustainPoints = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_PanLoopStartIndex = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_PanLoopEnd = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_VolumeEnvelopeLoopType = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_PanEnvelopeLoopType = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_VibratoType = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_VibratoSweep = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_VibratoDepth = _pFileBinary[seek] ;  
		seek++;
		pInstr->m_VibratoFreq = _pFileBinary[seek] ;  
		seek++;

		pInstr->m_VolumeFadeOut = (unsigned short) ReadSignedShortLE(_pFileBinary+seek);
		// useless seek +=2;
		pInstr->m_VolumeFadeOut *= 2;
		if (pInstr->m_NumberOfVolumePoints < 2) pInstr->m_VolumeEnvelopeLoopType = 0;
		if (pInstr->m_NumberOfPanPoints < 2) pInstr->m_PanEnvelopeLoopType = 0;

		// seek to first sample
		seek = firstsampleoffset;
		for (ii2 = 0; ii2 < numsamples; ii2++)
		{
			sUniMiniXM_Sample *pSample;
			unsigned char flag;
			pSample = uniminixm_CleanAlloc( sizeof(sUniMiniXM_Sample));
			if(!pSample)	goto memoryfail;
			pInstr->m_SampleTable[ii2] = pSample ;
			
			pSample->m_Length = (unsigned int)ReadSignedIntLE( &_pFileBinary[seek]);
			seek+=4;
			pSample->m_LoopStartIndex = (unsigned int)ReadSignedIntLE( &_pFileBinary[seek]);
			seek+=4;
			pSample->m_LoopLength = (unsigned int)ReadSignedIntLE( &_pFileBinary[seek]);
			seek+=4;
			pSample->m_DefaultVolumeChar = _pFileBinary[seek]; 
			seek++;
			pSample->m_FineTune = (signed char)_pFileBinary[seek];	//  finetune -128to +127
			seek++;
			//of course: pSample->m_LoopStartIndex = 0;
			//of course: pSample->loopmode = eLoopType_NoLoop;
			flag = _pFileBinary[seek];
			seek++;
			if (flag & 1) pSample->m_LoopMode = eLoopType_Circular ;
			if (flag & 2) pSample->m_LoopMode = eLoopType_PingPong ;
			if (!(flag & 3)) pSample->m_LoopLength = pSample->m_Length ;
			pSample->m_BitsPerSample = 8;
			if (flag & 16)
			{
				pSample->m_BitsPerSample = 16;
				pSample->m_Length >>= 1;
				pSample->m_LoopStartIndex >>= 1;
				pSample->m_LoopLength >>= 1;
			}

			pSample->m_DefaultPan = (int) _pFileBinary[seek];// uchar to int.
			seek++;

			pSample->m_RelativeNote = _pFileBinary[seek];
			seek += 24 ;
			// we continue the buffer read later on "Load sample data" pass.
		} // end of loop by sample

		// extract sample and cast it to float.
		for (ii2 = 0; ii2 < numsamples; ii2++)
		{
			sUniMiniXM_Sample *pSample = pInstr->m_SampleTable[ii2];
			float *buff = NULL;
			int ii3;

		if (pSample->m_Length)
		{
			buff = (float*)uniminixm_CleanAlloc((pSample->m_Length+16) * sizeof(float));
			if(!buff) goto memoryfail;
			pSample->m_pAllocBuffer = buff;
			// align float buffer on 4byte, for PPC processor and others...
			buff = (float*)(((int)buff+3)& 0xfffffffc);
			pSample->m_pBuffer = buff;

			if (pSample->m_BitsPerSample == 8)
			{
				static const float fdiv128 = 1.0f/128.0f;
				//8 bits: delta unpack and conversion to float.
				// delta must be done in integer, before div conversion.
				signed char oldval, newval;
				oldval = 0;

				for (ii3=0; ii3 < pSample->m_Length; ii3++)
				{
					newval = ((signed char)_pFileBinary[seek]) + oldval;
					*buff++ = ((float)newval)* fdiv128 ;
					oldval = newval;
					seek++;
				}
			}
			else
			{	
				static const float fdiv32768 = 1.0f/32768.0f;				
				signed short oldval, newval;
				oldval = 0;
				// 16bits
				// note: watch out ! LitlleEndian 16 bit to be read
				// delta unpack must be done in integer, before div conversion.
				for(ii3=0 ; ii3<pSample->m_Length ; ii3++)
				{
					newval = (signed short)ReadSignedShortLE(&_pFileBinary[seek]) + oldval;
					*buff++ = ((float)(newval))* fdiv32768 ;
					oldval = newval;
					seek+=2;
				}
			}
		{
			float endvalue;
			buff = pSample->m_pBuffer;
			// clamp sample a bit after and before, because of antialiasing.
			if (pSample->m_LoopMode == eLoopType_PingPong)
			{
				endvalue = buff[pSample->m_LoopStartIndex + pSample->m_LoopLength - 1];
				// write clamp at end 2 times for antialias.
				buff[pSample->m_LoopStartIndex + pSample->m_LoopLength] = endvalue;
				buff[pSample->m_LoopStartIndex + pSample->m_LoopLength+1] =endvalue;	
			}
			else if (pSample->m_LoopMode == eLoopType_Circular)
			{
				endvalue = buff[pSample->m_LoopStartIndex];	
				// write clamp at end 2 times for antialias.
				buff[pSample->m_LoopStartIndex + pSample->m_LoopLength] = endvalue;
				buff[pSample->m_LoopStartIndex + pSample->m_LoopLength+1] =endvalue;	
			}

		} // end loop sample init.
	} // end if (pSample->length)
		} // end  for (ii2 = 0; ii2 < numsamples; ii2++)
	} // end if (numsamples > 0)
	else
	{
		seek = firstsampleoffset;
	}
	}
	// allocate channels to the exact needed number:
	_pUniMiniXM->m_pMusicChannelTable = (sUniMiniXM_TrackAutomat *)
				uniminixm_CleanAlloc(_pUniMiniXM->m_NumberOfChannels * sizeof(sUniMiniXM_TrackAutomat));
	if(!_pUniMiniXM->m_pMusicChannelTable) goto memoryfail;

	//! allocation and inition of the antialiasing table trick
	_pUniMiniXM->m_pAntiAliasTable = (float *)uniminixm_CleanAlloc(eCte_AntiAliasTableLength * sizeof(float));
	if(!_pUniMiniXM->m_pAntiAliasTable) goto memoryfail;
	{
		float	*pt = _pUniMiniXM->m_pAntiAliasTable;
		float aav=0.0f;
		const float aaf = 1.0f/256.0f;
		for(ii=0;ii<eCte_AntiAliasTableLength ; ii++)
		{
			*pt++ = aav;
			aav +=aaf;
		}
	}
#ifdef _UNIMINIXM_SEEKTIME_
	// precalculation table to allow random time positionning.
	// we got +1 because we start at time 0, and last element gives the final time. 
	_pUniMiniXM->m_pPatternTimeReferenceTable = (PatternTimeReference *)
		uniminixm_CleanAlloc((_pUniMiniXM->m_PatternLength+1) * sizeof(PatternTimeReference));
	if(!_pUniMiniXM->m_pPatternTimeReferenceTable) goto memoryfail;
	UniMiniXM_JumpToPattern( _pUniMiniXM,0);
	SetBPM(_pUniMiniXM, _pUniMiniXM->m_DefaultBPMSpeed );
	// --------------- precalculate song length and time run: -------------
	{
		int lastpattern=-1;
		int currentpattern=-1;
		int tableIndex = 0;
		unsigned int timeticks=0;
		// just refresh without 
		PatternTimeReference *pfill = _pUniMiniXM->m_pPatternTimeReferenceTable;
		while(UniMiniXM_RefreshTick(_pUniMiniXM)) // quit if classic song pattern end 
		{
			//currentpattern = _pUniMiniXM->order;
			if(currentpattern != _pUniMiniXM->m_Position)
			{
				lastpattern = currentpattern;
				currentpattern = _pUniMiniXM->m_Position;
				// we got one pattern 
				if(tableIndex>=_pUniMiniXM->m_PatternLength) break; // shouldn't happen, but with this special jump salsa who knows ?
				
				pfill->m_timeTicks = timeticks;
				pfill->m_Secx256 = 
					((timeticks<<8)/_pUniMiniXM->m_MixFrequency) ;
				pfill->m_Position = currentpattern ;
				pfill->m_Line = _pUniMiniXM->m_LineIndex ;
				pfill->m_Speed = _pUniMiniXM->m_F0XSpeed;
				pfill->m_BPM = _pUniMiniXM->m_FX0bpmSpeed;
				pfill++;
				tableIndex++;

			}
			timeticks += (unsigned int) _pUniMiniXM->m_BPMSamplesPerTick ; // renderable sample length after update.
		}
		// keep last date, which should stand for the total length.
		_pUniMiniXM->m_pPatternTimeReferenceTable[tableIndex].m_Secx256 = 
						((timeticks<<8)/_pUniMiniXM->m_MixFrequency) ;
		_pUniMiniXM->m_pPatternTimeReferenceTable[tableIndex].m_Position = 0L ; // useless ?
		_pUniMiniXM->m_PatternTimeReferenceTableLength = tableIndex;
		// set computed time length as the serialized time length value !
		_pUniMiniXM->m_TimeLength = ((double)timeticks ) * _pUniMiniXM->m_fInvMixRate ;
	}
#endif
#ifdef	_TRACE_EFFECT_USE_	
	memset( &m_EffectUse[0],0,sizeof(m_EffectUse));
	memset( &m_EffectUseSpecial[0],0,sizeof(m_EffectUseSpecial));
#endif
	
	// these inits are common with the jump pattern functions:
	UniMiniXM_JumpToPattern( _pUniMiniXM,0);
	SetBPM(_pUniMiniXM, _pUniMiniXM->m_DefaultBPMSpeed );
	// return OK:
	_pUniMiniXM->m_InitState = eResult_OK;
	return(eResult_OK);
memoryfail:
	UniMiniXM_Close(_pUniMiniXM);
	_pUniMiniXM->m_InitState = eResult_MemoryError;
	return(eResult_MemoryError);
formatfail:
	UniMiniXM_Close(_pUniMiniXM);
	_pUniMiniXM->m_InitState = eResult_FileFormatError;
	return(eResult_FileFormatError);
}
/*
	\brief	Close everything opened by UniMiniXM_Init(). Must
			be called at quitting after a successful Init, will be useless after 
			a failure.
	\param	_pUniMiniXM	the struct object that manage the whole player.
*/
void	UniMiniXM_Close( sUniMiniXM *_pUniMiniXM )
{
	int ii,jj;
#ifdef _UNIMINIXM_SEEKTIME_
	if(_pUniMiniXM->m_pPatternTimeReferenceTable)
	{
		uniminixm_FreeAlloc(_pUniMiniXM->m_pPatternTimeReferenceTable);
		_pUniMiniXM->m_pPatternTimeReferenceTable = 0L;
	}
#endif
	if(_pUniMiniXM->m_pAntiAliasTable)
	{
		uniminixm_FreeAlloc(_pUniMiniXM->m_pAntiAliasTable);
		_pUniMiniXM->m_pAntiAliasTable = 0L;
	}
/*	if(m_pPatternTimeReferenceTable)
	{
		delete [] m_pPatternTimeReferenceTable;
		m_pPatternTimeReferenceTable = 0L;
	}*/
	if(_pUniMiniXM->m_pMusicChannelTable)
	{
		uniminixm_FreeAlloc(_pUniMiniXM->m_pMusicChannelTable);
		_pUniMiniXM->m_pMusicChannelTable = 0L;
	}

	if( _pUniMiniXM->m_pInstrument != 0L)
	{
		for (ii = 0; ii < (int) _pUniMiniXM->m_NumberOfInstruments; ii++)
		{
			sUniMiniXM_Instrument *pInstr;
			pInstr = &_pUniMiniXM->m_pInstrument[ii];
			for(jj=0 ; jj<pInstr->m_NumberOfSamples ; jj++ )
			{
				if(pInstr->m_SampleTable[jj] != 0L )
				{
					sUniMiniXM_Sample *pSample=pInstr->m_SampleTable[jj];
					if( pSample->m_pAllocBuffer != 0L )
						uniminixm_FreeAlloc( pSample->m_pAllocBuffer );
					uniminixm_FreeAlloc( pSample );
				}
			}
		}
		uniminixm_FreeAlloc( _pUniMiniXM->m_pInstrument );
		_pUniMiniXM->m_pInstrument = 0L;
	}
	// clear pattern base
	if( _pUniMiniXM->m_pPattern)
	{
		// clear unpacked pattern base:
		for( ii=0 ; ii < _pUniMiniXM->m_NumberOPatternsMem ; ii++ )
		{
			if( _pUniMiniXM->m_pPattern[ii].m_pNoteTable != 0L ) // not systematic.
			{
				uniminixm_FreeAlloc( _pUniMiniXM->m_pPattern[ii].m_pNoteTable);
			}
		}
		uniminixm_FreeAlloc( _pUniMiniXM->m_pPattern );
		_pUniMiniXM->m_pPattern = 0L;
	}	
	_pUniMiniXM->m_NumberOPatternsMem = 0;
}
/*
	\brief	You can replace a sample after UniMiniXM_Init(), 
		You must provide a mono continuous float buffer.
		\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
		\param	_instrument instrument of the sample to replace
		\param	_sample		sample to replace.
		\param	_pNewSample	pointer to the beginning of a float buffer.
		\param	_SampleLength	length of _pNewSample
		\param	_relativeNote	base tone delta of the sample for note A.
*/
void UniMiniXM_ReplaceSample( sUniMiniXM *_pUniMiniXM,
									 unsigned int _instrumentIndex,
									 unsigned int _sampleIndex,
									 float	*_pNewSample,
									 unsigned int _SampleLength,
									 unsigned int _relativeNote)
{
	sUniMiniXM_Instrument *pInstrument;
	sUniMiniXM_Sample *pSample;
	float			  *pBuffer;
	if( !_pUniMiniXM->m_pInstrument) return;
	if(_instrumentIndex>=_pUniMiniXM->m_NumberOfInstruments) return;

	pInstrument = &_pUniMiniXM->m_pInstrument[_instrumentIndex];
	if(_sampleIndex>= pInstrument->m_NumberOfSamples) return;
	pSample = pInstrument->m_SampleTable[_sampleIndex];
	if(pSample == 0L ) return;
	//! delete old buffer if there was one there:
	if( pSample->m_pAllocBuffer != 0L )
		uniminixm_FreeAlloc( pSample->m_pAllocBuffer );
	pSample->m_pAllocBuffer=0L;
	// alloc float buffer
	pBuffer = (float *)uniminixm_CleanAlloc( sizeof(float)*(_SampleLength+1));
	if(!pBuffer) return;

	pSample->m_pAllocBuffer = pBuffer ;
	pBuffer = (float*)(((int)pBuffer+3)& 0xfffffffc);
	pSample->m_pBuffer = pBuffer ;

	pSample->m_Length = _SampleLength;
	pSample->m_LoopStartIndex = 0;
	pSample->m_LoopLength = _SampleLength;
	pSample->m_RelativeNote = _relativeNote;
	// copy mono sample:
	{
	//unsigned int hhh=0x48421157;
	while(_SampleLength>0)
	{
		//float vv =  (((float)(hhh>>16))/65536.0f) -0.5f;
		*pBuffer++= *_pNewSample++;
		//hhh=(hhh*0x9af86b95)+0x9a186b97;
		_SampleLength--;
	}
	}

}

#ifdef	UNIMINIXM_TRACE_VOLUME_BY_SAMPLE
/*
	\brief return the current volume mixed for a given sample.
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\param	_instrument index.
	\param	_sample index.
	\param	_volume current volume is add there. 
*/
void	UniMiniXM_AddCurrentMixedVolume( sUniMiniXM *_pUniMiniXM,
								unsigned int _instrument,
								unsigned int _sample,
								float *_volume)
{
	const float v1d =	1.0f/512.0f;
	int ii;
	sUniMiniXM_Instrument *pinst;
	sUniMiniXM_Sample *psamp;
	if(_instrument>= _pUniMiniXM->m_NumberOfInstruments) return;
	pinst = &(_pUniMiniXM->m_pInstrument[_instrument]);
	if(_sample>=pinst->m_NumberOfSamples) return;
	psamp = pinst->m_SampleTable[_sample] ;
	
	for(ii=0; ii<MAXNUMBEROFMIXINGMONITORABLE ; ii++ )
	{
		if(psamp->m_Monitor.m_pChannelMixed[ii])
		{	
			*_volume += (((float)(psamp->m_Monitor.m_pChannelMixed[ii]->m_LeftVolume+
								 psamp->m_Monitor.m_pChannelMixed[ii]->m_RightVolume
							))*v1d) ;
			return;
		}
	}

}
#endif

