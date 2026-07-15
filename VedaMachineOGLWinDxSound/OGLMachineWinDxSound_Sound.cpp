// SPDX-License-Identifier: LGPL-2.1-only

#include "OGLMachineWinDxSound.h"

#include <windows.h>
#include <math.h>
#include <stdlib.h>
#define STRICT
#include <mmsystem.h>
#include <xaudio2.h>

OGLMachineWinDxSound::MiniXAudio2Sound		*OGLMachineWinDxSound::m_pSoundManager=NULL;
unsigned int		OGLMachineWinDxSound::m_NumberOfMachine=0L;
HANDLE				OGLMachineWinDxSound::m_dwNotifyThreadID=0;

OGLMachineWinDxSound	*OGLMachineWinDxSound::m_pFirstMachine=0L;

DWORD	OGLMachineWinDxSound::m_dwSoundBufferSize;

DWORD	OGLMachineWinDxSound::m_dwPrimaryFreq;

double	OGLMachineWinDxSound::m_d32InvPrimaryFreq;

float * volatile OGLMachineWinDxSound::m_pStaticSoundBuffer=NULL;

float * volatile OGLMachineWinDxSound::m_pStaticSoundBuffer2=NULL;
volatile unsigned int	OGLMachineWinDxSound::m_LastSoundBufferLength=0L;

volatile BOOL		OGLMachineWinDxSound::m_ThreadIsAlive=TRUE;
#ifdef _ENGINE_EDITABLE_

volatile bool	OGLMachineWinDxSound::m_MachineIsLockedForSoundManagement=false;
volatile bool	OGLMachineWinDxSound::m_MachineIsLockedForSoundUpdate=false;
#endif

void	OGLMachineWinDxSound::SoundInit(HWND _window)
{
	m_dwSoundBufferSize	= m_NumSoundBuffers * m_FramesPerSoundBuffer * m_nBlockAlign;
	m_dwPrimaryFreq		=  44100 ;

	m_d32InvPrimaryFreq = 4294967296.0/((double)m_dwPrimaryFreq);

	if( m_NumberOfMachine == 0 )
	{
		if(m_pStaticSoundBuffer == NULL)
		{
			m_pStaticSoundBuffer = new float[ m_dwPrimaryChannels *m_FramesPerSoundBuffer ];
			m_pStaticSoundBuffer2 = new float[ m_dwPrimaryChannels *m_FramesPerSoundBuffer ];
		}
		if(m_pStaticSoundBuffer == NULL ||
			m_pStaticSoundBuffer2 == NULL) return;

		if( FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)) ) return;

		IXAudio2 *pXAudio2 = NULL;
		if( FAILED(XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)) ) return;

		IXAudio2MasteringVoice *pMasteringVoice = NULL;
		if( FAILED(pXAudio2->CreateMasteringVoice(&pMasteringVoice)) )
		{
			pXAudio2->Release();
			return;
		}

		WAVEFORMATEX	wfx;
		ZeroMemory( &wfx, sizeof(WAVEFORMATEX) );
		wfx.wFormatTag		= WAVE_FORMAT_PCM;
		wfx.nChannels		= (WORD) m_dwPrimaryChannels;
		wfx.nSamplesPerSec	= m_dwPrimaryFreq;
		wfx.wBitsPerSample	= (WORD) 16;
		wfx.nBlockAlign		= (WORD) m_nBlockAlign ;
		wfx.nAvgBytesPerSec = m_dwPrimaryFreq * m_nBlockAlign;

		IXAudio2SourceVoice		*pSourceVoice = NULL;
		if( FAILED(pXAudio2->CreateSourceVoice(&pSourceVoice, &wfx)) )
		{
			pXAudio2->Release();
			return;
		}

		pSourceVoice->Start(0);

		m_pSoundManager = new MiniXAudio2Sound;
		m_pSoundManager->m_pXAudio2			= pXAudio2 ;
		m_pSoundManager->m_pMasteringVoice		= pMasteringVoice;
		m_pSoundManager->m_pSourceVoice		= pSourceVoice;
		m_pSoundManager->m_NextBufferToFill	= 0;
		for( unsigned int bb=0; bb<m_NumSoundBuffers ; bb++)
		{
			ZeroMemory(m_pSoundManager->m_PCMBuffer[bb], sizeof(m_pSoundManager->m_PCMBuffer[bb]));
		}
	}

	m_NumberOfMachine++;

	if(m_dwNotifyThreadID ==0)
	{
		DWORD id;
		m_dwNotifyThreadID = CreateThread( NULL, 0, NotificationProc, _window, 0, &id );
	}

	return;

}
void	OGLMachineWinDxSound::SoundExit()
{
	if(m_NumberOfMachine>0) m_NumberOfMachine--;

	if(m_NumberOfMachine ==0 && m_pSoundManager)
	{
#ifndef _ENGINE_EDITABLE_

		m_ThreadIsAlive = FALSE;
		Sleep(100);
#endif
		MiniXAudio2Sound		*pToDestroy = m_pSoundManager;
		m_pSoundManager = NULL;
			pToDestroy->m_pSourceVoice->Stop();
			pToDestroy->m_pSourceVoice->DestroyVoice();
			pToDestroy->m_pMasteringVoice->DestroyVoice();
			pToDestroy->m_pXAudio2->Release();
			delete pToDestroy;
		CoUninitialize();
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
void	OGLMachineWinDxSound::UpdateSoundBuffers()
{
	if( m_pSoundManager == NULL) return;

	IXAudio2SourceVoice		*pSourceVoice = m_pSoundManager->m_pSourceVoice;

	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState(&state);

	while( state.BuffersQueued < m_SoundQueueDepth )
	{
		const unsigned int sizeToWrite = m_FramesPerSoundBuffer;
		unsigned int ii;

		float	*pfl = m_pStaticSoundBuffer;

		float	zero = 0.0f;
		for( ii=0; ii<sizeToWrite ; ii++)
		{
			*(pfl++) = zero;
			*(pfl++) = zero;
		}

		pfl = m_pStaticSoundBuffer;

		VirtualMachine::SoundBufferToAddYourSignal	soundBufferToAddYourSignal;
		soundBufferToAddYourSignal.m_pSoundBuffer	= pfl ;
		soundBufferToAddYourSignal.m_LengthToRender = sizeToWrite ;
		soundBufferToAddYourSignal.m_PlayFrequency	= (float) m_dwPrimaryFreq ;

		OGLMachineWinDxSound	*pMachine =m_pFirstMachine;

		while(pMachine)
		{

			int			 nextSecUnit = pMachine->m_LastStartPlayTime_SecondUnit;
			unsigned int nextSecFrac = pMachine->m_LastStartPlayTime_SecondFrac;
			soundBufferToAddYourSignal.m_StartPlayTime_SecondUnit = nextSecUnit ;
			soundBufferToAddYourSignal.m_StartPlayTime_SecondFrac = nextSecFrac ;

			pMachine->m_TotalPlaySize	+= sizeToWrite ;
			double endplay =  ((double)pMachine->m_TotalPlaySize) * m_d32InvPrimaryFreq;

			long long wholeEnd = (long long)(endplay);
			soundBufferToAddYourSignal.m_EndPlayTime_SecondUnit =
			pMachine->m_LastStartPlayTime_SecondUnit = (int)(wholeEnd>>32);
			soundBufferToAddYourSignal.m_EndPlayTime_SecondFrac =
			pMachine->m_LastStartPlayTime_SecondFrac =  (unsigned int)(wholeEnd);

			SoundUpdateListCell *pcell = pMachine->m_pFirstSoundObject;
			while(pcell)
			{
				SoundInterface *pSoundMedia = pcell->m_pMediaObject;

				pSoundMedia->ProcessSoundInterupt( soundBufferToAddYourSignal );

				pcell = pcell->m_pNext;
			}

			pMachine = pMachine->m_pNextMachine ;
		}

		unsigned int bufIndex = m_pSoundManager->m_NextBufferToFill;
		short *pbuf = m_pSoundManager->m_PCMBuffer[bufIndex];
		float vv;
		float	vm=-1.0f;
		float	vp=1.0f;
		float	v16m1 = 32767.0f;

		for( ii=0; ii<sizeToWrite ; ii++)
		{
			vv = *(pfl++);
			if( vv<vm ) vv = vm; else if(vv>vp) vv=vp;
			*(pbuf++) = (short) (vv*v16m1);
			vv = *(pfl++);
			if( vv<vm ) vv = vm; else if(vv>vp) vv=vp;
			*(pbuf++) = (short) (vv*v16m1);
		}

		XAUDIO2_BUFFER xbuf;
		ZeroMemory(&xbuf, sizeof(xbuf));
		xbuf.AudioBytes = sizeToWrite * m_nBlockAlign;
		xbuf.pAudioData = (const BYTE *)m_pSoundManager->m_PCMBuffer[bufIndex];
		pSourceVoice->SubmitSourceBuffer(&xbuf);

		m_pSoundManager->m_NextBufferToFill = (bufIndex+1) % m_NumSoundBuffers;

		pfl = m_pStaticSoundBuffer2 ;
		m_pStaticSoundBuffer2 = m_pStaticSoundBuffer;
		m_pStaticSoundBuffer = pfl;
		m_LastSoundBufferLength = sizeToWrite;

		pSourceVoice->GetState(&state);
	}

}
DWORD WINAPI OGLMachineWinDxSound::NotificationProc( LPVOID lpParameter )
{

	HWND hDlg = (HWND) lpParameter;

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	unsigned int	traceNbLock = 0;
	while( m_ThreadIsAlive )
	{
		::Sleep(20);
		#ifdef _ENGINE_EDITABLE_
				if( !m_MachineIsLockedForSoundManagement)
				{
					m_MachineIsLockedForSoundUpdate = true;
					if(!m_MachineIsLockedForSoundManagement ) UpdateSoundBuffers();
				} else
				{
					traceNbLock++;
				}
				m_MachineIsLockedForSoundUpdate = false;
		#else
					UpdateSoundBuffers();
		#endif

	}
	CoUninitialize();
	return 0;
}
#ifdef _ENGINE_EDITABLE_

void	OGLMachineWinDxSound::EnableMediaSound( SoundInterface *_pNoisyObject, bool _enable )
{
	m_MachineIsLockedForSoundManagement = true;
	while(m_MachineIsLockedForSoundUpdate){}

		OGLMachine::EnableMediaSound(_pNoisyObject,_enable);
	m_MachineIsLockedForSoundManagement = false;
}
#endif

void	OGLMachineWinDxSound::GetCurrentSoundBufferImage(float ** const _ppSoundBuffer,unsigned int *_pLength) const
{
	*_ppSoundBuffer = m_pStaticSoundBuffer2 ;
	*_pLength		= m_LastSoundBufferLength;
}

void	OGLMachineWinDxSound::SetSoundVolume(float _newVolumeValue)
{
	m_SoundVolume		= _newVolumeValue;
}

float	OGLMachineWinDxSound::GetSoundVolume()
{
	return m_SoundVolume;
}

float	OGLMachineWinDxSound::GetPlayFrequency()
{
	return (float)m_dwPrimaryFreq ;

}

void	OGLMachineWinDxSound::SetSoundCurrentTime(int _seconds,unsigned int _fraction )
{
	unsigned int	newPlay =
						(_seconds * m_dwPrimaryFreq)
						+(((_fraction>>16)*m_dwPrimaryFreq)>>16) ;

	if( newPlay > (m_TotalPlaySize+(m_dwPrimaryFreq>>1))  ||
		newPlay < (m_TotalPlaySize-(m_dwPrimaryFreq>>1)) )
	{
		m_TotalPlaySize = newPlay ;
		m_LastStartPlayTime_SecondUnit = _seconds;
		m_LastStartPlayTime_SecondFrac = _fraction;
	}
}
