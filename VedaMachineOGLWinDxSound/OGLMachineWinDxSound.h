/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_OGLMachineWinDxSound_H
#define COM_M4NKIND_OGLMachineWinDxSound_H

#include "OGLMachine.h"
#include <windows.h>
//#include <dsound.h>

typedef struct IDirectSound8                *LPDIRECTSOUND8;
typedef struct IDirectSoundBuffer           *LPDIRECTSOUNDBUFFER;
/*!
	\class	OGLMachineWinDxSound 
	\ingroup	VirtualMachineDocGroup
	\brief	Defines a \mainprojectname Machine for Windows systems, using OGLMachine and OpenGL
			for 3D rendering, and DirectSound for the sound server.			
*/

class OGLMachineWinDxSound : public OGLMachine
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
	*/
	OGLMachineWinDxSound(void);
	/*!
		\brief	Destructor.
	*/
	virtual ~OGLMachineWinDxSound(void);

	/*!
		\brief	Init
	*/
	virtual eVMResult	InitMachine();	

	/*!
		\brief	Swap buffer so that what was drawn is shown.
	*/
	virtual	void	SwapMainScreenBuffer();

	/*!
		\brief	Check the Process the message of the interface, in the real time:
	*/
	virtual void	ProcessInterface();

	/*!
		\brief	Method that update the cicular buffer. It is to be thrown
				by the main task message handling, for event GetSoundBufferEventHandle()
	 */
	static	void	UpdateCircularSoundBuffer();


	static DWORD WINAPI NotificationProc( LPVOID lpParameter );
#ifdef _ENGINE_EDITABLE_
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
	virtual	void	EnableMediaSound( SoundInterface *_pNoisyObject, bool _enable=true );
#endif

	/*!
		\brief	force a date to the clock passed to the sound objects.	
		\param	_seconds	in second units.
		\param	_fraction	the following after-the-point-units of _seconds, like if the whole were a 64 bits integer.
	*/
	virtual	void	SetSoundCurrentTime(int _seconds,unsigned int _fraction );
	/*!
		\brief	return an image of the current short sound buffer which is currently played.
				This is always a 2 float(stereo left,right) table.
				the play frequency of this sound signal is given by GetPlayFrequency() .
				The buffer size may vary in time and implementation, and this table is refresh on an undefined rate.
				It can be used by sound preview methods or live effects. 
		\param	_ppSoundBuffer	pointer on pointer on the buffer to return.
		\param	_pLength	pointer on a unsigned int that return the length.
	*/
	virtual	void	GetCurrentSoundBufferImage(float ** const _ppSoundBuffer,unsigned int *_pLength) const ;

	/*!
		\brief	tool: change main volume [0,1].
				Yes, you can saturate when >1.
		\param	_newVolumeValue [0,1]
	*/
	virtual	void	SetSoundVolume(float _newVolumeValue);
	//{ m_SoundVolume = _newVolumeValue; };
	/*!
		\brief	tool: get main volume value [0,1].
		\return	VolumeValue [0,1]
	*/
	virtual	float	GetSoundVolume();
	//{ return m_SoundVolume;  };
	/*!
		\brief	Return the machine main play frequency, in herz, (tick per sec).
		\return freq (44100.0,22050.0,...)
	*/
	virtual	float	GetPlayFrequency();	

/*==================================================================
			Cultural Stuff
==================================================================*/

	/*!
		\brief	Return the language of the machine, as a simple enum. It can be used
			by exemple
		\return an enum integer.
	*/
	virtual eMachineLocalization	GetCurrentWorldLocalizationEnum();
	//old: { return eMachineLocalization_LocalisationNotImplemented; };

/*==================================================================
			Disk Operating System Stuff.
==================================================================*/
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	In edition mode, ask a file path to a file requester.
			the string format should vary, but will be compatible with LoadFile() and SaveFile()
		\param	_pDisplayString	a string to inform to what's the file for.
		\param	_pResultFileName the file path , returned.
		\param	_nameMaxLength the maximum buffer size to write.
		\return	true if _pDisplayString valid.
	*/
	virtual	bool	FileRequester(const char *_pDisplayString,char *_pResultFileName, unsigned int _nameMaxLength );

#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	load a file into a memory chunk. You can load all in one row, or stream.
				memory is to be deleted by the CALLER with operator: "delete [] pointer", for each 
				succesfull call. return 0L if failed.
		\param	_pFilePath			the path of the file to load
		\param	_pFileStart			an offset of where to start reading the file. 0 if all file.
		\param	_LoadByteLength		return the exact real length of what was read and returned, if OK.
		\param	_maximumSizeLoad	the maximum length to read
		\return	the memory chunk, or 0L if failed.
	*/
	virtual	const char *LoadFile(const char *_pFilePath,unsigned int _pFileStart,unsigned int &_LoadByteLength, unsigned int _maximumSizeLoad=0xffffffff);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	write a file from a memory chunk. You can save all in one row, or stream.
		\param	_pFilePath		the path of the file to write
		\param	_chunkToWrite	your memory chunk to save, as char *.
		\param	_ChunkLength	the length of _chunkToWrite
		\param	_append			if true, file is continued. if false, file is rewritten. default to false.
		\return	true if OK, false if not.
	*/
	virtual	bool SaveFile(const char *_pFilePath,const char *_chunkToWrite,unsigned int _ChunkLength,bool _append=false);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	in edition mode, redirection of machine task wait.
		\param	_milliseconds time to wait.
	*/
	virtual void	Sleep( unsigned int _milliseconds );
#endif
	/*
		\brief	After init,Set a general machine title, for example, to be displayed
			as a windows title. You can change the machine title at any time.
		\param	_pMachineTitle	const string to be displayed.
	*/
	virtual	void	SetMachineTitle(const char *_pMachineTitle);
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	typedef struct {
		//! screen dimension: can change when resizing:
		volatile unsigned int	m_X;
		//! screen dimension: can change when resizing:
		volatile unsigned int	m_Y;
		//! screen dimension: can change when resizing:
		volatile unsigned int	m_Width;
		//! screen dimension: can change when resizing:
		volatile unsigned int	m_Height;
		DWORD       m_dwExStyle;              // Window Extended Style
		DWORD       m_dwStyle;                // Window Style
		//!
		bool		m_IsFullScreen;
		//!
		bool		m_IsActive;
	} RenderScreen ;
	//!
	RenderScreen	m_FullScreen;
	//!
	RenderScreen	m_WindowScreen;
	//!
	RenderScreen	*m_pCurrentScreen;

		//! handle to the window:
		HWND		m_hWnd;     
		//! window or screen drawing object
		HDC			m_DC;			
		//! the OpenGL context:
		HGLRC       m_hRC; 
	//! tell if vOpenGL class is registered.
	static bool	m_OGLClassIsRegistered;
	//! to be reached from process functions
	static	OGLMachineWinDxSound	*m_LastInstance;
	//! true if fullscreen.
	//bool		m_fullscreen;
	//! keep display title from a context:
	const char	*m_pMachineTitle;
	//! Holds The Instance Of The Application
	HINSTANCE   m_hInstance;      
	//!  true when windows or screen is shown, so we got to draw:
	bool		m_active;
	//! screen dimension: can change when resizing:
	volatile unsigned int	m_Width;
	//! screen dimension: can change when resizing:
	volatile unsigned int	m_Height;
	/*!
		\brief
		\return 0 OK, 1 ChangeDisplaySettings failed (fullscreen impossible)
	*/
	int ActivateScreen(RenderScreen *_pToActivate);
	void KillGLWindow(/*RenderScreen *_pToDestroy*/);
	BOOL CreateGLWindow(/* RenderScreen *_pToCreate,bool fullscreen*/);
	static	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);   // Declaration For WndProc
	/*!
		\brief protected sound init
		\param	_window 
	*/
	void	SoundInit( HWND _window );
	/*!
		\brief protected sound exit
	*/
	void	SoundExit();

	//! the current main sound volume of the machine.[0,1]
	float			m_SoundVolume;
	//! a switch screen is to be done
	bool			m_AskSwitchScreen;

	/*! the direct sound object is static TO THE TASK, and 
		- inited, re-inited at first call, as there can be many machines by tasks, using one sound object.
		(every sound of all machines are mixed.
		- destroyed at last context destruction.
	*/
	class	MiniDirectSound
	{ public:
		LPDIRECTSOUND8			m_pDS;
		LPDIRECTSOUNDBUFFER		m_pSoundBuffer;
	};
	static	MiniDirectSound		*m_pSoundManager;
	static	unsigned int		m_NumberOfMachine;
	static	HANDLE				m_dwNotifyThreadID; 
	static	volatile BOOL		m_ThreadIsAlive;
	//! Figure out how big the DSound buffer should be 1 * 2 channel * sizeof(short)
	static	DWORD	m_dwDSBufferSize;
	//! stereo = 2
	#define	m_dwPrimaryChannels		2
	//! sec freq.
	static	DWORD	m_dwPrimaryFreq;
	//! 4Go/ freq for double -> lon long conversion.
	static double	m_d32InvPrimaryFreq;

	//! bytesize of one sound sample unit: (  2 channel * sizeof(short) = 4)
	#define	m_nBlockAlign	((16/8) * m_dwPrimaryChannels)
	//! circular sound buffer: offset where to write.
	static	DWORD	m_dwNextWriteOffset;

	//! the m_dwDSBufferSize * 2 * sizeof(float) table passed to AzurVeda contextes:
	static float	* volatile m_pStaticSoundBuffer;
	//! another one for double buffer. We use dbf to read a previously done chunk and return the curve !
	static float	* volatile m_pStaticSoundBuffer2;
	//! the length on which the last sound buffer(2) was written 
	static volatile unsigned int	m_LastSoundBufferLength;
	//! the total of amount played by a machine, in sample frequency. (to calculate cuurrent sound date.)
	unsigned int	m_TotalPlaySize;
	//! 
	int				m_LastStartPlayTime_SecondUnit;
	//! 
	unsigned int	m_LastStartPlayTime_SecondFrac;

	//! link the machine to a static machine list
	static	OGLMachineWinDxSound	*m_pFirstMachine;
	OGLMachineWinDxSound			*m_pNextMachine;

#ifdef _ENGINE_EDITABLE_
	//!	true if MachineIsLockedForSoundManagement. so that we don't use a list that is worked on.
	volatile static	bool	m_MachineIsLockedForSoundManagement;
	volatile static	bool	m_MachineIsLockedForSoundUpdate;
#endif
	VirtualMachine::eMachineLocalization	m_MachineLocalization;
	//! \brief	init localization :
	void	InitLocalization();

};


#endif
