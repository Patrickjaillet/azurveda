/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
// note: this file is to implement the direct sound functions in the machine.
#include "OGLMachineWinDxSound.h"
// cuno la muerte !!! Needed for gl.h under windows

#include <windows.h>        // Header File For Windows
#include <math.h>
#include <stdlib.h>
#define STRICT
#include <mmsystem.h>
#include <dxerr8.h>
#include <dsound.h>
//#include "VirtualMedia.h"
//#include "BaseContext.h"
OGLMachineWinDxSound::MiniDirectSound		*OGLMachineWinDxSound::m_pSoundManager=NULL;
unsigned int		OGLMachineWinDxSound::m_NumberOfMachine=0L;
HANDLE				OGLMachineWinDxSound::m_dwNotifyThreadID=0; 

//! link the machine to a static machine list
OGLMachineWinDxSound	*OGLMachineWinDxSound::m_pFirstMachine=0L;

//! Figure out how big the DSound buffer should be 1 * 2 channel * sizeof(short)
DWORD	OGLMachineWinDxSound::m_dwDSBufferSize;

//! sec freq.
DWORD	OGLMachineWinDxSound::m_dwPrimaryFreq;
//! bytesize of one sound sample unit: (  2 channel * sizeof(short) = 4)
DWORD	OGLMachineWinDxSound::m_dwNextWriteOffset=0L;

//! 4Go/ freq for double -> lon long conversion.
double	OGLMachineWinDxSound::m_d32InvPrimaryFreq;

//HANDLE	OGLMachineWinDxSound::m_hNotificationEvent=0L;

float * volatile OGLMachineWinDxSound::m_pStaticSoundBuffer=NULL;
//! another one for double buffer. We use dbf to read a previously done chunk and return the curve !
float * volatile OGLMachineWinDxSound::m_pStaticSoundBuffer2=NULL;
volatile unsigned int	OGLMachineWinDxSound::m_LastSoundBufferLength=0L;

volatile BOOL		OGLMachineWinDxSound::m_ThreadIsAlive=TRUE;
#ifdef _ENGINE_EDITABLE_
//!	true if MachineIsLockedForSoundManagement. so that we don't use a list that is worked on.
volatile bool	OGLMachineWinDxSound::m_MachineIsLockedForSoundManagement=false;
volatile bool	OGLMachineWinDxSound::m_MachineIsLockedForSoundUpdate=false;
#endif


void	OGLMachineWinDxSound::SoundInit(HWND _window)
{
	HRESULT		hr;

	m_dwDSBufferSize	= 1024*16;	// Figure out how big the DSound buffer should be 
	m_dwPrimaryFreq		=  44100 ;
							//11025 ;
						// 22050 ;
	m_d32InvPrimaryFreq = 4294967296.0/((double)m_dwPrimaryFreq);

	// init sound if not done: m_NumberOfMachine ==0
	if( m_NumberOfMachine == 0 )
	{
		if(m_pStaticSoundBuffer == NULL)
		{	// *2 for stereo left+right:(m_dwPrimaryChannels=2)
			m_pStaticSoundBuffer = new float[ m_dwPrimaryChannels *m_dwDSBufferSize ];
			m_pStaticSoundBuffer2 = new float[ m_dwPrimaryChannels *m_dwDSBufferSize ];
		}
		if(m_pStaticSoundBuffer == NULL ||
			m_pStaticSoundBuffer2 == NULL) return;

		LPDIRECTSOUND8	pDS;
		hr = DirectSoundCreate8(NULL, &pDS, NULL);
		if( FAILED(hr) ) return;
		// Set DirectSound coop level
		hr = pDS->SetCooperativeLevel(_window,
			DSSCL_PRIORITY) ;

		WAVEFORMATEX	wfx;
		ZeroMemory( &wfx, sizeof(WAVEFORMATEX) );
		wfx.wFormatTag		= WAVE_FORMAT_PCM;
		wfx.nChannels		= (WORD) m_dwPrimaryChannels;
		wfx.nSamplesPerSec	= m_dwPrimaryFreq; 
		wfx.wBitsPerSample	= (WORD) 16; 
		wfx.nBlockAlign		= (WORD) m_nBlockAlign ;// (wfx.wBitsPerSample / 8 * wfx.nChannels);
		wfx.nAvgBytesPerSec = m_dwPrimaryFreq * m_nBlockAlign;

		DSBUFFERDESC dsbd;
		ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
		dsbd.dwSize			= sizeof(DSBUFFERDESC);
		{	
			// Get the primary buffer 
			LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

			dsbd.dwFlags		= DSBCAPS_PRIMARYBUFFER;
			dsbd.dwBufferBytes	= 0; // this is managed somewhere else.
			dsbd.lpwfxFormat	= NULL;

			hr = pDS->CreateSoundBuffer(&dsbd, &pDSBPrimary, NULL);
			if( FAILED(hr) ) return;
					
			pDSBPrimary->SetFormat(&wfx); // don't care if it fails.
			// destroy it, cause it was just made to set the primary format:
			// (I guess that way, the secondary play format will use it more directly)
			if( pDSBPrimary ) pDSBPrimary->Release();
		}
		LPDIRECTSOUNDBUFFER		pSoundBuffer;

		dsbd.dwFlags		= /*DSBCAPS_ PRIMARYBUFFER | DSBCAPS_CTRLPOSITIONNOTIFY | */
								 DSBCAPS_GLOBALFOCUS |  DSBCAPS_GETCURRENTPOSITION2;
		dsbd.dwBufferBytes	=  m_dwDSBufferSize; 
		dsbd.lpwfxFormat	= &wfx; //  pWaveFile->m_pwfx;
		hr = pDS->CreateSoundBuffer(&dsbd, &pSoundBuffer, NULL);
		if( FAILED(hr) ) 
		{
			pDS->Release();
			return;
		}
		//OK:
		m_pSoundManager = new MiniDirectSound;
		m_pSoundManager->m_pDS = pDS ;
		m_pSoundManager->m_pSoundBuffer = pSoundBuffer;
	}
	// OK:
	m_NumberOfMachine++;

	//------------- test "play" ------------------
	LPDIRECTSOUNDBUFFER		pSoundBuffer = m_pSoundManager->m_pSoundBuffer;
	VOID* pDSLockedBuffer = NULL;
	DWORD dwDSLockedBufferSize = 0; // Size of the locked DirectSound buffer
	// Lock the buffer down
	if (FAILED(hr = pSoundBuffer->Lock(0, m_dwDSBufferSize, &pDSLockedBuffer,
								&dwDSLockedBufferSize, NULL, NULL, 0L))) return;
	short *pbuf = (short *)pDSLockedBuffer;
	for( unsigned int ii=0; ii<dwDSLockedBufferSize/m_nBlockAlign ; ii++)
	{
		*(pbuf++) = 0;
		*(pbuf++) = 0;
	}
	// Unlock the buffer, we don't need it anymore.
	pSoundBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );
	pSoundBuffer->Play(0,0, DSBPLAY_LOOPING );
	// start thread to get notifications,  after play():
	// create the thread once if not done.
	if(m_dwNotifyThreadID ==0)	
	{
		DWORD id;
		m_dwNotifyThreadID = CreateThread( NULL, 0, NotificationProc, _window, 0, &id );
	}

	// OK
	return;

}
void	OGLMachineWinDxSound::SoundExit()
{
	if(m_NumberOfMachine>0) m_NumberOfMachine--;
	// destroy sound if it was last machine that needed it:
	if(m_NumberOfMachine ==0 && m_pSoundManager)
	{
#ifndef _ENGINE_EDITABLE_
		// in non-editable release mode, kill thread, or thread might try to use context when dead.
		m_ThreadIsAlive = FALSE;
		Sleep(100);
#endif
		MiniDirectSound		*pToDestroy = m_pSoundManager;
		m_pSoundManager = NULL; // declare useless first in case of thread use.
			pToDestroy->m_pSoundBuffer->Stop();
			pToDestroy->m_pSoundBuffer->Release();
			//CloseHandle( pToDestroy->m_hNotificationEvent );
			pToDestroy->m_pDS->Release();
			delete pToDestroy;
		if(m_pStaticSoundBuffer != NULL)
		{
			delete [] m_pStaticSoundBuffer;
			m_pStaticSoundBuffer = NULL;
		}
		if(m_pStaticSoundBuffer2 != NULL)
		{
			delete [] m_pStaticSoundBuffer2;
			m_pStaticSoundBuffer2 = NULL;
		}

	} 


}
void	OGLMachineWinDxSound::UpdateCircularSoundBuffer()
{
	VOID* pDSLockedBuffer,*pDSLockedBuffer2;
	DWORD dwDSLockedBufferSize,dwDSLockedBufferSize2; // Size of the locked DirectSound buffer
	DWORD	currentplayPos,sizeToWrite;
	unsigned int ii;
//	HRESULT		hr;

	if( m_pSoundManager == NULL) return;
	//------------- test "play" ------------------
	LPDIRECTSOUNDBUFFER		pSoundBuffer = m_pSoundManager->m_pSoundBuffer;

	// the tedchnique used is: we write from last position wrote,  till current play pos.
	pSoundBuffer->GetCurrentPosition( &currentplayPos ,NULL );

	// m_dwNextWriteOffset == last in fact.
	if(m_dwNextWriteOffset<currentplayPos)
	{
		sizeToWrite = currentplayPos-m_dwNextWriteOffset;
	} else
	{	// player is behind us !!! -> managed in 2 time:
		sizeToWrite = m_dwDSBufferSize-m_dwNextWriteOffset;	// length till end of the  buffer.
		sizeToWrite += currentplayPos; // + begining of the buffer to player.
	}

	// Lock the buffer down
    while (DS_OK != pSoundBuffer->Lock(m_dwNextWriteOffset,sizeToWrite,
								&pDSLockedBuffer,&dwDSLockedBufferSize, // size returned in bytes.
								&pDSLockedBuffer2,&dwDSLockedBufferSize2,
								 0L))
	{
		pSoundBuffer->Restore();
		pSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
	}

	m_dwNextWriteOffset += sizeToWrite; 
	if(m_dwNextWriteOffset>=m_dwDSBufferSize ) m_dwNextWriteOffset -= m_dwDSBufferSize; // Circular buffer

	dwDSLockedBufferSize /=m_nBlockAlign;
	dwDSLockedBufferSize2 /=m_nBlockAlign;

	sizeToWrite = dwDSLockedBufferSize + dwDSLockedBufferSize2 ;
	// ------- create buffer data ---------------	

		// clear static sound buffer: (silence)
		float	*pfl = m_pStaticSoundBuffer;

		float	zero = 0.0f;
		for( ii=0; ii<sizeToWrite ; ii++)
		{
			*(pfl++) = zero;
			*(pfl++) = zero;
		}

		// pass sound buffer to AzurVeda machine context(es):
		pfl = m_pStaticSoundBuffer;

		VirtualMachine::SoundBufferToAddYourSignal	soundBufferToAddYourSignal;
		soundBufferToAddYourSignal.m_pSoundBuffer	= pfl ;
		soundBufferToAddYourSignal.m_LengthToRender = (unsigned int) sizeToWrite ;
		soundBufferToAddYourSignal.m_PlayFrequency	= (float) m_dwPrimaryFreq ;

		OGLMachineWinDxSound	*pMachine =m_pFirstMachine;
		
		while(pMachine)
		{	
			/*
			long long totalTime = ((long long )pMachine->m_TotalPlaySize)<<32;
	
			soundBufferToAddYourSignal.m_StartPlayTime	= ((double)pMachine->m_TotalPlaySize) * dInvPrimaryFreq;
			soundBufferToAddYourSignal.m_EndPlayTime	= ((double)(pMachine->m_TotalPlaySize+sizeToWrite)) * dInvPrimaryFreq;
			*/
			int			 nextSecUnit = pMachine->m_LastStartPlayTime_SecondUnit;
			unsigned int nextSecFrac = pMachine->m_LastStartPlayTime_SecondFrac;
			soundBufferToAddYourSignal.m_StartPlayTime_SecondUnit = nextSecUnit ;
			soundBufferToAddYourSignal.m_StartPlayTime_SecondFrac = nextSecFrac ;
			// note: this should give best precision/speed:
			pMachine->m_TotalPlaySize	+= sizeToWrite ;
			double endplay =  ((double)pMachine->m_TotalPlaySize) * m_d32InvPrimaryFreq;
			// note we know the compiler use long long as 64bits:
			long long wholeEnd = (long long)(endplay);
			soundBufferToAddYourSignal.m_EndPlayTime_SecondUnit =
			pMachine->m_LastStartPlayTime_SecondUnit = (int)(wholeEnd>>32);	
			soundBufferToAddYourSignal.m_EndPlayTime_SecondFrac = 
			pMachine->m_LastStartPlayTime_SecondFrac =  (unsigned int)(wholeEnd);	
			// loop by function to mix in a single context: 
			SoundUpdateListCell *pcell = pMachine->m_pFirstSoundObject;
			while(pcell)
			{
				SoundInterface *pSoundMedia = pcell->m_pMediaObject;
				// ask for a signal to add. Can fail if object not created or threadlock
				// (can't play while changing data.)
				pSoundMedia->ProcessSoundInterupt( soundBufferToAddYourSignal );

				pcell = pcell->m_pNext;
			}

			pMachine = pMachine->m_pNextMachine ;
		}
		// ---------------- end of create buffer data ---------------


		// convert to 16bit buffer:
		short *pbuf = (short *)pDSLockedBuffer;
		float vv;
		float	vm=-1.0f;
		float	vp=1.0f;
		float	v16m1 = 32767.0f; // main volume btw

		for( ii=0; ii<dwDSLockedBufferSize ; ii++)
		{
			vv = *(pfl++);
			if( vv<vm ) vv = vm; else if(vv>vp) vv=vp;
			*(pbuf++) = (WORD) (vv*v16m1);
			vv = *(pfl++);
			if( vv<vm ) vv = vm; else if(vv>vp) vv=vp;
			*(pbuf++) = (WORD) (vv*v16m1);
		}
		// maybe we have to continue with buffer2 if needed: (discontinuous buffer lock)
		if(pDSLockedBuffer2)
		{
			short *pbuf = (short *)pDSLockedBuffer2;
			for( ii=0; ii<dwDSLockedBufferSize2 ; ii++)
			{
				vv = *(pfl++);
				if( vv<vm ) vv = vm; else if(vv>vp) vv=vp;
				*(pbuf++) = (WORD) (vv*v16m1);
				vv = *(pfl++);
				if( vv<vm ) vv = vm; else if(vv>vp) vv=vp;
				*(pbuf++) = (WORD) (vv*v16m1);
			}

		}
		// Unlock the buffer, we don't need it anymore.
		pSoundBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, pDSLockedBuffer2, dwDSLockedBufferSize2 );
		// just do the double buffering swap here, after all write:
		pfl = m_pStaticSoundBuffer2 ;
		m_pStaticSoundBuffer2 = m_pStaticSoundBuffer;
		m_pStaticSoundBuffer = pfl;
		m_LastSoundBufferLength = sizeToWrite;

}
DWORD WINAPI OGLMachineWinDxSound::NotificationProc( LPVOID lpParameter )
{
//	HRESULT hr;
	HWND hDlg = (HWND) lpParameter;
//	MSG msg;
//	DWORD dwResult;
	// BOOL bLooped;
	unsigned int	traceNbLock = 0;
	while( m_ThreadIsAlive )
	{
		::Sleep(20); // 20
		#ifdef _ENGINE_EDITABLE_				
				if( !m_MachineIsLockedForSoundManagement) 
				{
					m_MachineIsLockedForSoundUpdate = true;
					if(!m_MachineIsLockedForSoundManagement ) UpdateCircularSoundBuffer();
				} else
				{
					traceNbLock++;
				}
				m_MachineIsLockedForSoundUpdate = false;
		#else
					UpdateCircularSoundBuffer();
		#endif
				
	}
	return 0;
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Register a media object that will then generate a mixed sound through
			VirtualMedia::ProcessSoundInterupt() The sound will start instantly.
			if false, Unregister a media object previously registered
			with EnableMediaSound(). The sound stops instantly.

				OVERRIDED to handle a lock when editing the machine list of
				for what have to be mixed, and have no mutex.
	\param	_pNoisyObject 
*/
/*!
	\brief	Register a SoundInterface object that will then generate a mixed sound through
			SoundInterface::ProcessSoundInterupt() The sound will start instantly.
			if false, Unregister a media object previously registered
			with EnableMediaSound(). The sound stops instantly.

				OVERRIDED to handle a lock when editing the machine list of
				for what have to be mixed, and have no mutex.
	\param	_pNoisyObject the object that can generate noise.
	\param	_enable	true if you want to enable, false to disable.
*/
void	OGLMachineWinDxSound::EnableMediaSound( SoundInterface *_pNoisyObject, bool _enable )
{
	m_MachineIsLockedForSoundManagement = true;
	while(m_MachineIsLockedForSoundUpdate){}

		OGLMachine::EnableMediaSound(_pNoisyObject,_enable);
	m_MachineIsLockedForSoundManagement = false;
}
#endif
/*!
	\brief	return an image of the current short sound buffer which is currently played.
			This is always a 2 float(stereo left,right) table.
			the play frequency of this sound signal is given by GetPlayFrequency() .
			The buffer size may vary in time and implementation, and this table is refresh on an undefined rate.
			It can be used by sound preview methods or live effects. 
	\param	_ppSoundBuffer	pointer on pointer on the buffer to return.
	\param	_pLength	pointer on a unsigned int that return the length.
*/
void	OGLMachineWinDxSound::GetCurrentSoundBufferImage(float ** const _ppSoundBuffer,unsigned int *_pLength) const
{
	*_ppSoundBuffer = m_pStaticSoundBuffer2 ;
	*_pLength		= m_LastSoundBufferLength;
}
/*!
	\brief	tool: change main volume [0,1].
			Yes, you can saturate when >1.
	\param	_newVolumeValue [0,1]
*/
void	OGLMachineWinDxSound::SetSoundVolume(float _newVolumeValue)
{
	m_SoundVolume		= _newVolumeValue;
}
/*!
	\brief	tool: get main volume value [0,1].
	\return	VolumeValue [0,1]
*/
float	OGLMachineWinDxSound::GetSoundVolume()
{
	return m_SoundVolume;
}
/*!
	\brief	Return the machine main play frequency, in herz, (tick per sec).
	\return freq (44100.0,22050.0,...)
*/
float	OGLMachineWinDxSound::GetPlayFrequency() 
{
	return (float)m_dwPrimaryFreq ;

}
/*!
	\brief	force a date to the clock passed to the sound objects.	
	\param	_seconds	in second units.
	\param	_fraction	the following after-the-point-units of _seconds, like if the whole were a 64 bits integer.
*/
void	OGLMachineWinDxSound::SetSoundCurrentTime(int _seconds,unsigned int _fraction )
{
	unsigned int	newPlay = //(unsigned int)(_date * (double)m_dwPrimaryFreq ) ;
						(_seconds * m_dwPrimaryFreq)
						+(((_fraction>>16)*m_dwPrimaryFreq)>>16) ;
	// don't change if time is average:
	if( newPlay > (m_TotalPlaySize+(m_dwPrimaryFreq>>1))  ||
		newPlay < (m_TotalPlaySize-(m_dwPrimaryFreq>>1)) )
	{
		m_TotalPlaySize = newPlay ;
		m_LastStartPlayTime_SecondUnit = _seconds;
		m_LastStartPlayTime_SecondFrac = _fraction;
	}
}
