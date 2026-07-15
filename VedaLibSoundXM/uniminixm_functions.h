/*! \file 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	INCLUDE_UNIMINIXM_FUNCTIONS_H
#define INCLUDE_UNIMINIXM_FUNCTIONS_H
#include "uniminixm_structs.h"
/*!
	\page UniMiniXM UniMiniXM
	\version	0.1
	\brief	 UniMiniXM is a portable minimal OpenSource XM music format player,
			 with a minimal API, using C.\n
			The aim of this player is:
				- To be the most portable possible. Which means
				 we possibly target folkloric hardwares with no specific code.
				- To be bug-free. (well, of course !)
				- To generate a short code.
				- To generate a good sound signal.
				- Being optimized and fast, as long as the previous items allow it.
				- Having an Open-API which does not use directly a sound server,
				 and which you can implement elsewhere.
			So,it may not play exactly xm files as it should, especially if they
			have been created with recent music tools (XMs done
			with linux's soundtracker will have effects problems. ),
			but it will do its job for open source or portable demo or intro projects.\n
			If you search more accurate and complete music players,
			you should have a look at fmod from firelight multimedia or some other projects.
			The programming rules goes like that:
				- full Professional doxygen-like comments !
				- full re-entrant, and multicontext code (no global variable,
				 more than one Music can be inited and played at a time ! 
				- Uniminixm use a C code that can be compiled for BigEndian
				 Processors (PowerPC,68060), or little endian processors (Intel,...).
				- It is assembler free, which doesn't mean it is necessarily slower.
				- You need a compiler that supports 64Bit integer operations.
				 All int64 operation has been separated in a struct which code can change, in case your compiler
				 would not support it.
				- It has no "int-to-float casts" in its mixing loops, which
				 is a CPU critic operation on PowerPC: Samples are casted to float
				 at inition.
				- It ensures floats are 4 bytes-aligned, also for PowerPC specific optimizations.
			- And here is a current bug list:
				- in loop mode, it seems there is a "one-sample" shift in some cases.
				- some effects are not inited correctly if XM was saved with some tools.
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
	\brief	You got to provide this clean memory allocator C function:
*/
extern	void *uniminixm_CleanAlloc(unsigned int _bytesize);
/*
	\brief	You got to provide this clean memory allocator C function:
*/
extern	void uniminixm_FreeAlloc(void *_pAlloc);

/*
	\brief	Init by reading a XM binary file into memory.
			You must alloc a sUniMiniXM struct in memory or on the stack.
			UniMiniXM_Close() must be called to free the memory and end
			the thread, except if the init fails.
			You also got to manage the way the file binary is set in memory.
			(UniMiniXM and XM Format can't handle streaming).
			Once the result is eResult_OK, you will be able to use the other 
			UniMiniXM_XXXX functions.
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\param	_pFileBinary binary pointer to the beginning of the memory.
	\param	_FilebinaryLength length of memory pointed by _pFileBinary.
	\param	_outputHerzFrequency	play frequency for the output(22050,44100,...)
	\return a eXMMusicInitResult enum for the result.
*/
eXMMusicInitResult	UniMiniXM_Init( sUniMiniXM *_pUniMiniXM,		  
								  const unsigned char *_pFileBinary,
								  const unsigned int _FilebinaryLength,
								  const	unsigned int _outputHerzFrequency
								  );
/*
	\brief	Close everything opened by UniMiniXM_Init(). Must
			be called at quitting after a successful Init, will be useless after 
			a failure.
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
*/
void	UniMiniXM_Close( sUniMiniXM *_pUniMiniXM );
/*
	\brief	You can replace a sample after UniMiniXM_Init().
		You must provide a mono continuous float buffer, and the instrument
		and sample numbers must already exist in the XM.
		\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
		\param	_instrumentIndex instrument of the sample to replace
		\param	_sampleIndex		sample to replace.
		\param	_pNewSample	pointer to the beginning of a float buffer.
		\param	_SampleLength	length of _pNewSample
		\param	_relativeNote	base tone delta of the sample for note A.
*/
void UniMiniXM_ReplaceSample( sUniMiniXM *_pUniMiniXM,
									 unsigned int _instrumentIndex,
									 unsigned int _sampleIndex,
									 float	*_pNewSample,
									 unsigned int _SampleLength,
									 unsigned int _relativeNote);
/*
	\typedef	bRefreshTickReportBits
	\brief		flag bits describing an event that could happen during after a call to
				UniMiniXM_RefreshTick()
*/
typedef enum {
	//! means the music finished because it reached the end.
	eRefresh_SongEnd=1
} bRefreshTickReportBits ;
/*
	\brief	Refresh the music effects for a tick time. 
			UniMiniXM_GetSamplesLeftToMix() will then returns
			the amount of sample you will be able to mix with 
			UniMiniXM_AddSoundOnStereoFloatBuffer().	
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\return a bRefreshTickReportBits flag bit set, that indicates if the music
			ended, or entered a never ending loop.
*/
unsigned int UniMiniXM_RefreshTick( sUniMiniXM *_pUniMiniXM );
/*
	\brief	Returns the samples unit length that you can Mix, after
			a call to UniMiniXM_RefreshTick(). If the result is bigger than 0,
			you can use UniMiniXM_AddSoundOnStereoFloatBuffer() to create the sound signal.
			You can subdivide to the mixer with more little length, and ask
			the remaining mixable size with this function. When it reaches zero,
			you should refresh again.
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\return	the number of unit of sample left to mix.
*/
#define		UniMiniXM_GetSamplesLeftToMix(_pUniMiniXM) ( _pUniMiniXM->m_nsamplesStillToBeMixed )
//unsigned int	UniMiniXM_GetSamplesLeftToMix( sUniMiniXM *_pUniMiniXM );

/*
	\brief	Mix the sound signal after a call to UniMiniXM_RefreshTick().
			It will <b>add</b> the signal over a pre-inited float buffer,
			which is a stereo 2 float by sample table.(see struct sFloatStereoSample)
			This buffer should be set to zeroes, but you can mix other noises in the same signal,
			before or after.
			UniMiniXM_GetSamplesLeftToMix() returned the maximum number of sample you can mix
			before a next call to UniMiniXM_RefreshTick(), but you can perfectly subdivide the
			calls to this mixer function by asking less samples.
			The remaining samples to mix before a refresh are returned by UniMiniXM_GetSamplesLeftToMix().
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\param	_pSoundBufferToAddSignalOn pointer to the beginning of the buffer to write.
	\param	_soundBufferMaxLength sample length of  _pSoundBufferToAddSignalOn
*/
void	UniMiniXM_AddSoundOnStereoFloatBuffer(sUniMiniXM *_pUniMiniXM,
						sFloatStereoSample *_pSoundBufferToAddSignalOn,
							unsigned int _soundBufferMaxLength);
/*
	\brief	Make a jump to a given pattern position.
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\param _patternIndex Index in the list of pattern to play.
*/
void	UniMiniXM_JumpToPattern( sUniMiniXM *_pUniMiniXM,
								unsigned int _patternIndex);
/*
	\brief when playing, return the current pattern number which is played.
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\return pattern index.
*/
unsigned int UniMiniXM_GetCurrentPatternNumber(const sUniMiniXM *_pUniMiniXM);
/*
	\brief when playing, return the current row index in the pattern.
		Watch out, the maximum row by pattern vary.
	\param	_pUniMiniXM	pointer to the struct object that manage the whole player.
	\return the row.
*/
unsigned int UniMiniXM_GetCurrentRowIndex(const  sUniMiniXM *_pUniMiniXM );

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
void	UniMiniXM_SeekTime(sUniMiniXM *_pUniMiniXM,int _secondX256);
#endif
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
								float *_volume);
#endif


#ifdef __cplusplus
};
#endif
//end of file:
#endif
