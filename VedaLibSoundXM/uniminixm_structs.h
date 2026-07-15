/*! \file 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	INCLUDE_UNIMINIXM_STRUCTS_H
#define INCLUDE_UNIMINIXM_STRUCTS_H

#ifdef __cplusplus
extern "C" {
#endif


#define UNIMINIXM_TRACE_VOLUME_BY_SAMPLE

// _TRACE_EFFECT_USE_ fill a table of effect use...
// #define	_TRACE_EFFECT_USE_	1
#ifdef	_TRACE_EFFECT_USE_
#define	TRACE_EFFECT_USE(_enum) m_EffectUse[_enum]++;
#define	TRACE_EFFECT_USE_SPECIAL(_enum) m_EffectUseSpecial[_enum]++;
#endif
// if no trace:
#ifndef	_TRACE_EFFECT_USE_
#define	TRACE_EFFECT_USE(_enum) 
#define	TRACE_EFFECT_USE_SPECIAL(_enum) 
#endif


/*!
	\typedef	eXMMusicInitResult
	\brief		enum for the possibles return states of UniMiniXM_Init().
*/
typedef enum {
	//! means the init is correct.
	eResult_OK=0,
	//! means memory alloc failed.
	eResult_MemoryError,
	//! means File is not a valid XM.
	eResult_FileFormatError
} eXMMusicInitResult ;
/*!
	\typedef	eConstants
	\brief		Constants relatives to the XM format
*/
typedef	enum {
	eCte_MaximumPatternLength=256,
	//! minimum step to avoid sound ticks. (128 for 44khz)
	eCte_MixerMinimumSampleStep=128, 
	//! size of m_pAntiAliasTable.
	eCte_AntiAliasTableLength=256
} eConstants;

/*!
	\struct	FloatStereoSample
	\brief	Sound Buffer Format Sample for UniMiniXM_AddSoundOnStereoFloatBuffer().			
*/
typedef struct {
	float	m_Left;
	float	m_Right;
} sFloatStereoSample;

struct _sUniMiniXM_Channel;
#ifdef UNIMINIXM_TRACE_VOLUME_BY_SAMPLE
/*!
	\struct	sUniMiniXM_SampleMonitor
	\brief	monitor how much a sample is mixed at a given moment.
*/
#define MAXNUMBEROFMIXINGMONITORABLE	4
typedef struct {
	struct _sUniMiniXM_Channel *m_pChannelMixed[MAXNUMBEROFMIXINGMONITORABLE];
} sUniMiniXM_SampleMonitor ;
#endif
/*!
	\struct	sUniMiniXM_Sample
	\brief	describe a XM Sample.
*/
typedef struct {
	//! pointer to sound data, an aligned float table.
	float *m_pBuffer;
	//! sample length
	unsigned int m_Length;         
	//! sample loop start
	unsigned int m_LoopStartIndex;      
	//! sample loop length
	unsigned int m_LoopLength;       
	//! the allocation version, non-aligned.
	float *m_pAllocBuffer;
	//! sample default volume
	unsigned char m_DefaultVolumeChar;        
	//! finetune value from -128 to 127
	signed char m_FineTune;       
	//! bits per sample
	unsigned char m_BitsPerSample;
	//! loop type
	unsigned char m_LoopMode;
	//! default pan
	int m_DefaultPan; 
	//! relative note
	signed char m_RelativeNote;       
#ifdef UNIMINIXM_TRACE_VOLUME_BY_SAMPLE
	//! Monitor the amount of volume a sample is mixed.
	sUniMiniXM_SampleMonitor	m_Monitor;
#endif
} sUniMiniXM_Sample;
/*!
	\struct	sUniMiniXM_Channel
	\brief	describe a XM Channel.
*/
typedef struct _sUniMiniXM_Channel {
	//! Current Volume
	int m_CurrentVolume;
	//! panning value (00-FFh).
	int m_Pan;
	//! sample offset (sample starts playing from here).
	unsigned int m_SampleOffset;  
	//! sample currently played
	sUniMiniXM_Sample *m_pSample;        
	//! mixer: final volume for left channel 
	unsigned int m_LeftVolume;     
	//! mixer: final volume for right channel 
	unsigned int m_RightVolume;
	//! mixer: 32bits high parts of pixer position
	unsigned int m_SamplePositionHigh32;
	//! mixer: 32bits low parts of pixer position
	unsigned int m_SamplePositionLow32;
	//! mixer: 32bits High parts of vector position
	unsigned int m_SampleVectorHigh32;
	//! mixer: 32bits low parts of vector position
	unsigned int m_SampleVectorLow32;
	//! mixer: current sample read direction for ping pong mode.
	unsigned int m_CurrentVectorDirection; 
	//! mixer: target value for volume ramp
	unsigned int m_Slide_LeftTarget;
	//! mixer: target value for volume ramp
	unsigned int m_Slide_RightTarget;
	//! mixer: current volume ramp 
	unsigned int m_Slide_LeftVolume;
	//! mixer: current volume ramp 
	unsigned int m_Slide_RightVolume;
	//! mixer: current volume ramp vector
	float		 m_Slide_LeftSpeed;
	//! mixer: current volume ramp vector
	float		 m_Slide_RightSpeed;
	//! number of sample still to ramp
	unsigned int m_Slide_Count;
} sUniMiniXM_Channel;
/*!
	\struct	sUniMiniXM_Note
	\brief	describe a Note for sUniMiniXM_Pattern.
*/
typedef struct 
{        
	//! note to play at (0-133) 132=none,133=keyoff
	unsigned char m_PlayNote;                 
	//! instrument being played (0-99)
	unsigned char m_InstrumentIndex;              
	//! volume column value (0-64)  255=no volume
	unsigned char m_Volume;
	//! Effect number (0-1Ah)
	unsigned char m_Effect;
	//! Effect parameter (0-255)
	unsigned char m_EffectParameter;
} sUniMiniXM_Note;
/*!
	\struct	sUniMiniXM_Pattern
	\brief	describe a XM pattern.
*/
typedef struct {
	//! number of lines of notes in pathe pattern.
	int m_NumberOfLines;
	//! table of all the notes in the patterns. Size if nbChan * m_NumberOfLines.
	sUniMiniXM_Note *m_pNoteTable;
} sUniMiniXM_Pattern;
/*!
	\struct	sUniMiniXM_Instrument
	\brief	describe a XM instrument.
*/
typedef struct 
{ 
	//! number of samples in the instr.
	int m_NumberOfSamples;
	//! table of the 16 samples of the instrument. 
	sUniMiniXM_Sample *m_SampleTable[16];
	//! Key table for envelope.
	unsigned char m_KeyTable[96];
	//! Volume envelope points
	unsigned short m_VolumePoints[40];
	//! Pan envelope points
	unsigned short m_PanPoints[40];
	//! Number of volume envelope points
	unsigned char m_NumberOfVolumePoints;
	//! Number of panning envelope points
	unsigned char m_NumberOfPanPoints;
	//! Volume sustain points
	unsigned char m_NumberOfSustainPoints;
	//! Volume envelope loop start
	unsigned char m_VolumeEnvelopeLoopStartIndex;
	//! Volume envelope loop end
	unsigned char m_VolumeEnvelopeLoopEnd;
	//! Panning sustain point
	unsigned char m_NumberOfPanSustainPoints;
	//! Panning envelope loop start
	unsigned char m_PanLoopStartIndex;
	//! Panning envelope loop end
	unsigned char m_PanLoopEnd;
	//! Envelope type: 0:On 1:Sustain 2:Loop
	unsigned char m_VolumeEnvelopeLoopType;
	//! Envelope type: 0:On 1:Sustain 2:Loop
	unsigned char m_PanEnvelopeLoopType;
	//! Instrument Vibrato type
	unsigned char m_VibratoType;
	//! vibrato time delay to come
	unsigned char m_VibratoSweep;
	//! Vibrato effect depth
	unsigned char m_VibratoDepth;
	//! Vibrato effect freq
	unsigned char m_VibratoFreq;
	//! Volume fade out
	unsigned short m_VolumeFadeOut;
} sUniMiniXM_Instrument;

/*!
	\struct	PatternTimeReference
	\brief	precalculation to allow random time positionning.
			It describes keys in the times.
*/
typedef struct {
	//! date of the state, in "<<8" seconds
	int	m_Secx256;	
	//! same date, given in sample units.
	int	m_timeTicks;	
	//! pattern list index.
	int	m_Position;
	//! Line in the pattern.
	int	m_Line;		
	//! speed at the beginning of the pattern.
	int	m_Speed;
	//! bitperminute
	int	m_BPM;
} PatternTimeReference;

/*!
	\struct		sUniMiniXM_TrackAutomat
	\brief		Private struct that handles the mixing of a track
				on a channel.				
*/
typedef struct 
{
	//! here are the current mixing channel: during sample change, a second channel is swapped with volume slide to avoid clicks:
	sUniMiniXM_Channel	m_Channel[2];
	//! last channel used in m_Channel,0 or 1:
	unsigned char	m_ChannelIndex;
	//! Current note.
	unsigned char m_Note;       
	//! aspect being refreshed. Use eBitRefreshNote enum bits.
	unsigned char m_RefreshFlag;
	//! end process flag for volume envelope. Better here with its char friend :-)
	signed char m_EnvelopeVolumeStopFlag; 
	//! Current Frequency period
	int m_Frequency;
	//! Current Volume
	int m_Volume;
	//! current Pan
	int m_Pan;
	//! Current effect volume delta
	int m_VolumeDelta;
	//! Current effect freq delta for vibrato
	int m_FrequencyDelta;
	//! delta for pan.
	int m_PanDelta;
	//! envelp tick count
	int m_EnvelopeTickCount;
	//! envelp tick position
	int m_EnvelopeTickPosition;
	//! current computed volume, Integer value
	int m_EnvelopeVolumeHigh32;
	//! current computed volume, low value
	int m_EnvelopeVolumeLow32;
	//! samples between volume envlp points
	int m_EnvelopeVolumeDelta;
	//! pan envelp tick count
	int m_EnvelopeTickPanCount;
	//! pan envelp tick position
	int m_EnvelopeTickPanPosition;
	//! current computed pan, Integer value
	int m_EnvelopePanHigh32;
	//! current computed pan, low value
	int m_EnvelopePanLow32;
	//!  samples between pan envlp points
	int m_EnvelopePanDelta;
	//! fade volume value
	int m_FadeVolume;
	//! Vibrato position
	int m_InstrumentVibratoPosition;
	//! Vibrato Sweep Position
	int m_InstrumentVibratoSweepPosition;
	//! Current period
	unsigned int m_Period;
	//! fx E6x: line index of the start of pattern loop
	int m_PatternLoopStartLine;
	//! fx E6x: number of lines still to loop.
	int m_PatternLoopCount;
	//! Portamento target value
	int m_PortamentoTarget;
	//! key off start flag
	signed char m_KeyOffFlag;
	//! Current instrument index
	unsigned char m_InstrumentIndex; 
	//! Current base note in channel
	unsigned char m_CurrentBaseNote;
	//! previous line effect, for some tuning.
	unsigned char m_PreviousEffect;
	//! End process flag for pan envelope. Better here with its char friend :-)
	signed char m_EnvelopePanStopFlag;
	//! Current Portamento Down Value
	unsigned char m_PortamentoDown;
	//! Current Portamento Up Value
	unsigned char m_PortamentoUp;
	//!Current Portamento Down Value level 2
	unsigned char m_ExtraPortamentoDown;
	//!Current Portamento Up Value level 2
	unsigned char m_ExtraPortamentoUp;
	//! Previous Volume slide value
	unsigned char m_VolumeSlide;
	//! Retrig Volume X parameter
	unsigned char m_RetrigXParam;
	//! Retrig Volume Y parameter
	unsigned char m_RetrigYParam;
	//! Current Portamento vector
	unsigned char m_PortamentoVector;
	//! Vibrato position
	signed char m_VibratoPosition;
	//! Vibrato Vector
	unsigned char m_VibratoVector;
	//! Vibrato amplitude
	unsigned char m_VibratoDepth;
	//! Tremolo Position
	signed char m_TremoloPosition;
	//! Tremolo Vector
	unsigned char m_TremoloVector;
	//! Tremolo depth
	unsigned char m_TremoloDepth;
	//! Last Tremor Position
	unsigned char m_TremorPosition;
	//! keep last tremor X
	unsigned char m_TremorKeepX;
	//! keep last tremor Y
	unsigned char m_TremorKeepY;
	//! shape type of vibrato & tremolo
	unsigned char m_VibratoType;
	//! Fine Volume Slide Value
	unsigned char m_FineVolumeSlide;
	//! Fine Portamento up value
	unsigned char m_FinePortamentoUp;
	//! Fine Portamento down value
	unsigned char m_FinePortamentoDown;

} sUniMiniXM_TrackAutomat;

/*
	\typedef	eBitFlagXM
	\brief		XM main music Flags.
*/
typedef	enum {
	eBitFlagXM_LinearFreq=1
} eBitFlagXM;
 
/*!
	\struct		sUniMiniXM
	\brief		Main Structure that handles the Music States,
				and the sample mixing. You should allocate it yourself,
				and then use UniMiniXM_Init(), UniMiniXM_Close() and
				other UniMiniXM_XXX() functions.
*/
typedef struct _sUniMiniXM{
	//! number of samples still to be mixed after last refresh.
	unsigned int		m_nsamplesStillToBeMixed;
	//! inition state after UniMiniXM_Init().
	eXMMusicInitResult	m_InitState;
	//! herz rate of sound signal.
	unsigned int m_MixFrequency;  
	//! float inverse version: 1/m_MixFrequency
	float	m_fInvMixRate;
	//! The minimum samples length a mixer can go on:
	unsigned int m_MinimumSampleStep;
	//! 1/m_MinimumSampleStep
	float	m_fInvSampleStep;
	//! anti-aliasing optimisation: this table prevent a int->float cast and a multiply.
	float	*m_pAntiAliasTable;
	//! total sample mixed since the begining of the song. We guess the time play with it.
	unsigned int	m_TotalTickMixed;
	/*! we alloc the structure objects for channels, because some tools
	can save XM with more than 32 channels:*/
	sUniMiniXM_TrackAutomat *m_pMusicChannelTable;
	//! patterns table
	sUniMiniXM_Pattern	*m_pPattern;
	//! Instrument table
	sUniMiniXM_Instrument *m_pInstrument;
	//! Samples Per Tick according to BPM speed;
	int m_BPMSamplesPerTick;
	//! Pattern Length
	short m_PatternLength;
	// restart position
	short m_RestartPosition;
	//! number of channels
	short m_NumberOfChannels;
	//! Number of instruments
	short m_NumberOfInstruments;
	//! Number of physical patterns
	int m_NumberOPatterns;
	//! Number of allocated patterns
	int m_NumberOPatternsMem;
	//! XM flags, defined by eXMFlags
	unsigned short m_Flags;
	//! default tick speed.
	short m_DefaultF0XSpeed;
	//! Default BPM speed for the XM.
	short m_DefaultBPMSpeed;
	//! 0-63 default global volume for the XM
	unsigned char m_DefaultGlobalVolume;
	//! For special global volume slide effect
	unsigned char m_GlobalVolumeSlide;
	//! Current global volume
	int m_GlobalVolume;
	//! Current tick count
	int m_TickCount;
	//! F0X speed of song in ticks per line
	int m_F0XSpeed;                                   
	//! FX0 bpm speed
	int m_FX0bpmSpeed;
	//! Current line in pattern
	int m_LineIndex;                                      
	//! next valid line after m_LineIndex. 
	int m_NextLine;
	//! current song index position
	int m_Position;
	//! Next Pattern Position for jumping.
	int m_NextPosition;
	//! Pattern Delay Counter
	int m_PatternDelayCounter;
	//! XM pattern list. Table at the end for better cache handling.
	unsigned char m_pPatternIndexList[eCte_MaximumPatternLength];

#ifdef _UNIMINIXM_SEEKTIME_
	//! precalculation to allow time random positionning.
	PatternTimeReference	*m_pPatternTimeReferenceTable;
	//! total number of element filled in m_pPatternTimeReferenceTable. could be diffent from order length.
	unsigned int 			m_PatternTimeReferenceTableLength;
	//! total time length in seconds:
	float					m_TimeLength;
#endif
#ifdef _TRACE_EFFECT_USE_ 
	//! trace effect use
	int	m_EffectUse[eXMFx_ZXX+1];
	int	m_EffectUseSpecial[eXMFx_EFX_FunkRepeat+1];	 
#endif
} sUniMiniXM;
/*!
	\typedef eBitFlag_Effects
	\brief	Classic mod/xm Obarski code effect list enum:
*/
typedef enum 
{
	//! make a chord by changing between 3 notes quickly. X and Y are the numer of offset tones from the base note.
	eXMFx_0XY_Arpeggio=0,
	//! slide note up of XX units, according to tick speed (given in eXMFx_FXY_SetSpeed)
	eXMFx_1XX_PortaUp,
	//! slide note down of XX units, according to tick speed (given in eXMFx_FXY_SetSpeed)
	eXMFx_2XX_PortaDown,
	//! slide note to the new note, at XX unit speed, according to tick speed (given in eXMFx_FXY_SetSpeed)
	eXMFx_3XX_PortamentoTo,
	//! Vibrato at speed X and amplitude Y.
	eXMFx_4XY_Vibrato,
	//! slide note like eXMFx_3XX_PortamentoTo, but change volume with XY like eXMFx_AXY_VolumeSlide
	eXMFx_5XX_PortamentoToAndVolumeSlide,
	//! Vibrato like eXMFx_4XY_Vibrato, but change volume with XY like eXMFx_AXY_VolumeSlide.
	eXMFx_6XX_VibratoAndVolumeSlide,
	//! eXMFx_7XX_Tremolo
	eXMFx_7XX_Tremolo,
	//! Set Pan Position
	eXMFx_8XX_SetPanPosition,
	//! Start sample at offset "XX" * 256 
	eXMFx_9XX_SampleOffset,
	//! Slide volume down with Y, OR uyp with X, at units given by eXMFx_FXY_SetSpeed.
	eXMFx_AXY_VolumeSlide,
	//! next line jump to this position in pattern list:
	eXMFx_BXX_PatternJump,
	//! set volume, from 0 to 63.
	eXMFx_CXX_SetVolume,
	//! Pattern Break
	eXMFx_DXX_PatternBreak,
	//! process effects as described in enum eBitFlag_SpecialEffects.
	eXMFx_EXX_SpecialEffect,
	//! set speed, to number of ticks-per-line Y with F0Y, for change main BPM speed with FX0.
	eXMFx_FXY_SetSpeed,
	eXMFx_GXX_SetGlobalVolume,
	eXMFx_HXX_GlobalVolumeSlide,
	eXMFx_IXX,
	eXMFx_JXX,
	eXMFx_KXX_KeyOff,
	eXMFx_LXX_SetEnvelopePosition,
	eXMFx_MXX,
	eXMFx_NXX,
	eXMFx_OXX,
	eXMFx_PXX_PanSlide,
	eXMFx_QXX,
	eXMFx_RXX_MultiRetrig,
	eXMFx_SXX,
	eXMFx_TXX_Tremor,
	eXMFx_UXX,
	eXMFx_VXX,
	eXMFx_WXX,
	eXMFx_XXX_ExtraFinePortamento,
	eXMFx_YXX,
	eXMFx_ZXX
} eBitFlag_Effects;
/*!
	\typedef eBitFlag_SpecialEffects
	\brief	Classic mod/xm obarski code effect
		 for second parameter for special effect eXMFx_EXX_SpecialEffect
*/
typedef enum {
	//! Set Filter on/off, originally the amiga paula chip hardware filter.
	eXMFx_E0X_SetFilter=0,
	//! 
	eXMFx_E1X_FinePortamentoUp,
	eXMFx_E2X_FinePortamentoDown,
	eXMFx_E3X_SetGlissando,
	eXMFx_E4X_VibratoToWave,
	eXMFx_E5X_SetFineTune,
	eXMFx_E6X_PatternLoop,
	eXMFx_E7X_SetTremoloWave,
	eXMFx_E8X_SetPanPosition16,
	eXMFx_E9X_Retrig,
	eXMFx_EAX_FineVolumeSlideUp,
	eXMFx_EBX_FineVolumeSlideDown,
	eXMFx_ECX_NoteCut,
	eXMFx_EDX_NoteDelay,
	eXMFx_EEX_m_PatternDelayCounter,
	eXMFx_EFX_FunkRepeat
} eBitFlag_SpecialEffects;

/*
	\typedef eBitRefreshNote
	\brief	 enum use by sUniMiniXM_TrackAutomat::m_RefreshFlag
*/
typedef enum {
	//! frequency changed
	eBitRefresh_Frequency=1,
	//! volume changed
	eBitRefresh_Volume=2,
	//! Pan changed
	eBitRefresh_Pan=4,
	//! sample just trigerred.
	eBitRefresh_TriggerSample=8,
	//! sample just stopping.
	eBitRefresh_Stop=32,
	//! special mask
	eBitRefresh_KeyOffMask=255
} eBitRefreshNote;
/*
	\typedef eEnvelopeBit
	\brief	envelope bits
*/
typedef enum {
	eEnvelopeBit_On=1,
	eEnvelopeBit_Sustain=2,
	eEnvelopeBit_Loop=4
} eEnvelopeBit;
/*
	\typedef	eBitFlag_SampleLoopType
	\brief	sample loop type bits
*/
typedef enum {
	//! No loop
	eLoopType_NoLoop=0,
	//! circular loop
	eLoopType_Circular,
	//! ping pong loop.
	eLoopType_PingPong
} eSampleLoopType;
/*
	\typedef	eBitFlag_SampleLoopState
	\brief	for the mixer state
*/
typedef enum {
	//! ->
	eSampleLoopForward=0,
	//! <-
	eSampleLoopBackward
} eSampleLoopState;

// end of the extern C patch for cpp:
#ifdef __cplusplus
};

#endif
//end of header file 
#endif
