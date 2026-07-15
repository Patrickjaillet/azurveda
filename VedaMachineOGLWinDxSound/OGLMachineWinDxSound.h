// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_OGLMachineWinDxSound_H
#define COM_M4NKIND_OGLMachineWinDxSound_H

#include "OGLMachine.h"
#include <windows.h>

struct IXAudio2;
struct IXAudio2MasteringVoice;
struct IXAudio2SourceVoice;

class OGLMachineWinDxSound : public OGLMachine
{

public:

	OGLMachineWinDxSound(void);

	virtual ~OGLMachineWinDxSound(void);

	virtual eVMResult	InitMachine();

	virtual	void	SwapMainScreenBuffer();

	virtual void	ProcessInterface();

	static	void	UpdateSoundBuffers();

	static DWORD WINAPI NotificationProc( LPVOID lpParameter );
#ifdef _ENGINE_EDITABLE_

	virtual	void	EnableMediaSound( SoundInterface *_pNoisyObject, bool _enable=true );
#endif

	virtual	void	SetSoundCurrentTime(int _seconds,unsigned int _fraction );

	virtual	void	GetCurrentSoundBufferImage(float ** const _ppSoundBuffer,unsigned int *_pLength) const ;

	virtual	void	SetSoundVolume(float _newVolumeValue);

	virtual	float	GetSoundVolume();

	virtual	float	GetPlayFrequency();

	virtual eMachineLocalization	GetCurrentWorldLocalizationEnum();

#ifdef _ENGINE_EDITABLE_

	virtual	bool	FileRequester(const char *_pDisplayString,char *_pResultFileName, unsigned int _nameMaxLength );

#endif
#ifdef _ENGINE_EDITABLE_

	virtual	const char *LoadFile(const char *_pFilePath,unsigned int _pFileStart,unsigned int &_LoadByteLength, unsigned int _maximumSizeLoad=0xffffffff);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual	bool SaveFile(const char *_pFilePath,const char *_chunkToWrite,unsigned int _ChunkLength,bool _append=false);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void	Sleep( unsigned int _milliseconds );
#endif

	virtual	void	SetMachineTitle(const char *_pMachineTitle);

protected:
	typedef struct {

		volatile unsigned int	m_X;

		volatile unsigned int	m_Y;

		volatile unsigned int	m_Width;

		volatile unsigned int	m_Height;
		DWORD       m_dwExStyle;
		DWORD       m_dwStyle;

		bool		m_IsFullScreen;

		bool		m_IsActive;
	} RenderScreen ;

	RenderScreen	m_FullScreen;

	RenderScreen	m_WindowScreen;

	RenderScreen	*m_pCurrentScreen;

		HWND		m_hWnd;

		HDC			m_DC;

		HGLRC       m_hRC;

	static bool	m_OGLClassIsRegistered;

	static	OGLMachineWinDxSound	*m_LastInstance;

	const char	*m_pMachineTitle;

	HINSTANCE   m_hInstance;

	bool		m_active;

	volatile unsigned int	m_Width;

	volatile unsigned int	m_Height;

	int ActivateScreen(RenderScreen *_pToActivate);
	void KillGLWindow();
	BOOL CreateGLWindow();
	static	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	void	SoundInit( HWND _window );

	void	SoundExit();

	float			m_SoundVolume;

	bool			m_AskSwitchScreen;

	#define	m_dwPrimaryChannels		2

	static const unsigned int	m_NumSoundBuffers = 4;
	static const unsigned int	m_SoundQueueDepth = 3;
	static const unsigned int	m_FramesPerSoundBuffer = 1024;

	class	MiniXAudio2Sound
	{ public:
		IXAudio2				*m_pXAudio2;
		IXAudio2MasteringVoice	*m_pMasteringVoice;
		IXAudio2SourceVoice	*m_pSourceVoice;
		short					m_PCMBuffer[m_NumSoundBuffers][m_FramesPerSoundBuffer*m_dwPrimaryChannels];
		unsigned int			m_NextBufferToFill;
	};
	static	MiniXAudio2Sound	*m_pSoundManager;
	static	unsigned int		m_NumberOfMachine;
	static	HANDLE				m_dwNotifyThreadID;
	static	volatile BOOL		m_ThreadIsAlive;

	static	DWORD	m_dwSoundBufferSize;

	static	DWORD	m_dwPrimaryFreq;

	static double	m_d32InvPrimaryFreq;

	#define	m_nBlockAlign	((16/8) * m_dwPrimaryChannels)

	static float	* volatile m_pStaticSoundBuffer;

	static float	* volatile m_pStaticSoundBuffer2;

	static volatile unsigned int	m_LastSoundBufferLength;

	unsigned int	m_TotalPlaySize;

	int				m_LastStartPlayTime_SecondUnit;

	unsigned int	m_LastStartPlayTime_SecondFrac;

	static	OGLMachineWinDxSound	*m_pFirstMachine;
	OGLMachineWinDxSound			*m_pNextMachine;

#ifdef _ENGINE_EDITABLE_

	volatile static	bool	m_MachineIsLockedForSoundManagement;
	volatile static	bool	m_MachineIsLockedForSoundUpdate;
#endif
	VirtualMachine::eMachineLocalization	m_MachineLocalization;

	void	InitLocalization();

};

#endif
