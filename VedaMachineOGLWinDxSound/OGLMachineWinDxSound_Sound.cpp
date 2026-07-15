// SPDX-License-Identifier: LGPL-2.1-only

#include "OGLMachineWinDxSound.h"

#include <windows.h>
#include <math.h>
#include <stdlib.h>
#define STRICT
#include <mmsystem.h>
#include <dsound.h>

OGLMachineWinDxSound::MiniDirectSound		*OGLMachineWinDxSound::m_pSoundManager=NULL;
unsigned int		OGLMachineWinDxSound::m_NumberOfMachine=0L;
HANDLE				OGLMachineWinDxSound::m_dwNotifyThreadID=0;

OGLMachineWinDxSound	*OGLMachineWinDxSound::m_pFirstMachine=0L;

DWORD	OGLMachineWinDxSound::m_dwDSBufferSize;

DWORD	OGLMachineWinDxSound::m_dwPrimaryFreq;

DWORD	OGLMachineWinDxSound::m_dwNextWriteOffset=0L;

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
	HRESULT		hr;

	m_dwDSBufferSize	= 1024*16;
	m_dwPrimaryFreq		=  44100 ;

	m_d32InvPrimaryFreq = 4294967296.0/((double)m_dwPrimaryFreq);

	if( m_NumberOfMachine == 0 )
	{
		if(m_pStaticSoundBuffer == NULL)
		{
			m_pStaticSoundBuffer = new float[ m_dwPrimaryChannels *m_dwDSBufferSize ];
			m_pStaticSoundBuffer2 = new float[ m_dwPrimaryChannels *m_dwDSBufferSize ];
		}
		if(m_pStaticSoundBuffer == NULL ||
			m_pStaticSoundBuffer2 == NULL) return;

		LPDIRECTSOUND8	pDS;
		hr = DirectSoundCreate8(NULL, &pDS, NULL);
		if( FAILED(hr) ) return;

		hr = pDS->SetCooperativeLevel(_window,
			DSSCL_PRIORITY) ;

		WAVEFORMATEX	wfx;
		ZeroMemory( &wfx, sizeof(WAVEFORMATEX) );
		wfx.wFormatTag		= WAVE_FORMAT_PCM;
		wfx.nChannels		= (WORD) m_dwPrimaryChannels;
		wfx.nSamplesPerSec	= m_dwPrimaryFreq;
		wfx.wBitsPerSample	= (WORD) 16;
		wfx.nBlockAlign		= (WORD) m_nBlockAlign ;
		wfx.nAvgBytesPerSec = m_dwPrimaryFreq * m_nBlockAlign;

		DSBUFFERDESC dsbd;
		ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
		dsbd.dwSize			= sizeof(DSBUFFERDESC);
		{

			LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

			dsbd.dwFlags		= DSBCAPS_PRIMARYBUFFER;
			dsbd.dwBufferBytes	= 0;
			dsbd.lpwfxFormat	= NULL;

			hr = pDS->CreateSoundBuffer(&dsbd, &pDSBPrimary, NULL);
			if( FAILED(hr) ) return;

			pDSBPrimary->SetFormat(&wfx);

			if( pDSBPrimary ) pDSBPrimary->Release();
		}
		LPDIRECTSOUNDBUFFER		pSoundBuffer;

		dsbd.dwFlags		=
								 DSBCAPS_GLOBALFOCUS |  DSBCAPS_GETCURRENTPOSITION2;
		dsbd.dwBufferBytes	=  m_dwDSBufferSize;
		dsbd.lpwfxFormat	= &wfx;
		hr = pDS->CreateSoundBuffer(&dsbd, &pSoundBuffer, NULL);
		if( FAILED(hr) )
		{
			pDS->Release();
			return;
		}

		m_pSoundManager = new MiniDirectSound;
		m_pSoundManager->m_pDS = pDS ;
		m_pSoundManager->m_pSoundBuffer = pSoundBuffer;
	}

	m_NumberOfMachine++;

	LPDIRECTSOUNDBUFFER		pSoundBuffer = m_pSoundManager->m_pSoundBuffer;
	VOID* pDSLockedBuffer = NULL;
	DWORD dwDSLockedBufferSize = 0;

	if (FAILED(hr = pSoundBuffer->Lock(0, m_dwDSBufferSize, &pDSLockedBuffer,
								&dwDSLockedBufferSize, NULL, NULL, 0L))) return;
	short *pbuf = (short *)pDSLockedBuffer;
	for( unsigned int ii=0; ii<dwDSLockedBufferSize/m_nBlockAlign ; ii++)
	{
		*(pbuf++) = 0;
		*(pbuf++) = 0;
	}

	pSoundBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );
	pSoundBuffer->Play(0,0, DSBPLAY_LOOPING );

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
		MiniDirectSound		*pToDestroy = m_pSoundManager;
		m_pSoundManager = NULL;
			pToDestroy->m_pSoundBuffer->Stop();
			pToDestroy->m_pSoundBuffer->Release();

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
	DWORD dwDSLockedBufferSize,dwDSLockedBufferSize2;
	DWORD	currentplayPos,sizeToWrite;
	unsigned int ii;

	if( m_pSoundManager == NULL) return;

	LPDIRECTSOUNDBUFFER		pSoundBuffer = m_pSoundManager->m_pSoundBuffer;

	pSoundBuffer->GetCurrentPosition( &currentplayPos ,NULL );

	if(m_dwNextWriteOffset<currentplayPos)
	{
		sizeToWrite = currentplayPos-m_dwNextWriteOffset;
	} else
	{
		sizeToWrite = m_dwDSBufferSize-m_dwNextWriteOffset;
		sizeToWrite += currentplayPos;
	}

    while (DS_OK != pSoundBuffer->Lock(m_dwNextWriteOffset,sizeToWrite,
								&pDSLockedBuffer,&dwDSLockedBufferSize,
								&pDSLockedBuffer2,&dwDSLockedBufferSize2,
								 0L))
	{
		pSoundBuffer->Restore();
		pSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
	}

	m_dwNextWriteOffset += sizeToWrite;
	if(m_dwNextWriteOffset>=m_dwDSBufferSize ) m_dwNextWriteOffset -= m_dwDSBufferSize;

	dwDSLockedBufferSize /=m_nBlockAlign;
	dwDSLockedBufferSize2 /=m_nBlockAlign;

	sizeToWrite = dwDSLockedBufferSize + dwDSLockedBufferSize2 ;

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
		soundBufferToAddYourSignal.m_LengthToRender = (unsigned int) sizeToWrite ;
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

		short *pbuf = (short *)pDSLockedBuffer;
		float vv;
		float	vm=-1.0f;
		float	vp=1.0f;
		float	v16m1 = 32767.0f;

		for( ii=0; ii<dwDSLockedBufferSize ; ii++)
		{
			vv = *(pfl++);
			if( vv<vm ) vv = vm; else if(vv>vp) vv=vp;
			*(pbuf++) = (WORD) (vv*v16m1);
			vv = *(pfl++);
			if( vv<vm ) vv = vm; else if(vv>vp) vv=vp;
			*(pbuf++) = (WORD) (vv*v16m1);
		}

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

		pSoundBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, pDSLockedBuffer2, dwDSLockedBufferSize2 );

		pfl = m_pStaticSoundBuffer2 ;
		m_pStaticSoundBuffer2 = m_pStaticSoundBuffer;
		m_pStaticSoundBuffer = pfl;
		m_LastSoundBufferLength = sizeToWrite;

}
DWORD WINAPI OGLMachineWinDxSound::NotificationProc( LPVOID lpParameter )
{

	HWND hDlg = (HWND) lpParameter;

	unsigned int	traceNbLock = 0;
	while( m_ThreadIsAlive )
	{
		::Sleep(20);
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
