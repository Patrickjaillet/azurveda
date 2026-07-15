/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "OGLMachineWinDxSound.h"
// cuno la muerte !!! Needed for gl.h under windows

#include <windows.h>        // Header File For Windows
#include <stdio.h>          // Header File For Standard Input/Output
#include <gl\gl.h>          // Header File For The OpenGL32 Library
//#include <gl\glu.h>         // Header File For The GLu32 Library
//#include <gl\glaux.h>       // Header File For The Glaux Library

OGLMachineWinDxSound	*OGLMachineWinDxSound::m_LastInstance=NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);   // Declaration For WndProc

//! tell if vOpenGL class is registered.
bool	OGLMachineWinDxSound::m_OGLClassIsRegistered=false;

OGLMachineWinDxSound::OGLMachineWinDxSound() : OGLMachine()
//	,m_fullscreen(true)
	,m_active(true)
	,m_hInstance(NULL)
	,m_Width(800)
	,m_Height(600)
	,m_SoundVolume(1.0f)
	,m_TotalPlaySize(0L)
	,m_LastStartPlayTime_SecondUnit(0L)
	,m_LastStartPlayTime_SecondFrac(0L)
	,m_MachineLocalization(eMachineLocalization_LocalisationNotImplemented)
	,m_AskSwitchScreen(false)
	,m_pMachineTitle(0L)
	,m_pCurrentScreen(0L)
	,m_DC(NULL)
	,m_hRC(NULL)
	,m_hWnd(NULL)
{
	m_FullScreen.m_IsActive = false;
	m_FullScreen.m_IsFullScreen = true;
	m_FullScreen.m_X = 0;
	m_FullScreen.m_Y = 0;
	m_FullScreen.m_Width = 800;
	m_FullScreen.m_Height = 600;
	m_FullScreen.m_dwExStyle = WS_EX_APPWINDOW;
	m_FullScreen.m_dwStyle = WS_POPUP;

	m_WindowScreen.m_IsActive = false;
	m_WindowScreen.m_IsFullScreen = false;
	m_WindowScreen.m_X = 128;
	m_WindowScreen.m_Y = 128;
	m_WindowScreen.m_Width = 640;
	m_WindowScreen.m_Height = 480;
	m_WindowScreen.m_dwExStyle =WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	m_WindowScreen.m_dwStyle =WS_OVERLAPPEDWINDOW; 

/*	m_WindowScreen.m_DC=NULL;
	m_WindowScreen.m_hRC=NULL;
	m_WindowScreen.m_hWnd= NULL;
	m_FullScreen.m_DC=NULL;
	m_FullScreen.m_hRC=NULL;
	m_FullScreen.m_hWnd= NULL;
*/
	m_LastInstance=this;

	// link to the machine list iof there are multiple machine:
	// so that the sound mix all machines.
#ifdef _ENGINE_EDITABLE_
	m_MachineIsLockedForSoundManagement = true;
#endif
		m_pNextMachine = m_pFirstMachine;
		m_pFirstMachine = this;
#ifdef _ENGINE_EDITABLE_
	m_MachineIsLockedForSoundManagement = false;
#endif
}

OGLMachineWinDxSound::~OGLMachineWinDxSound(void)
{
#ifdef _ENGINE_EDITABLE_
	m_MachineIsLockedForSoundManagement = true;
	while(m_MachineIsLockedForSoundUpdate){}
#endif
	SoundExit();
	KillGLWindow();
	//KillGLWindow(&m_WindowScreen);
	// if we are the last instance to be destroyed, unregister class:
	if(!(m_pFirstMachine->m_pNextMachine) && m_OGLClassIsRegistered )
	{
		UnregisterClass("OpenGL",m_hInstance);
		m_OGLClassIsRegistered = false;
	}

	// unlink this from the list of sound mixed machine:
	OGLMachineWinDxSound **pn = &m_pFirstMachine;
	while(*pn)
	{
		if( (*pn) == this )
		{
			*pn =  (*pn)->m_pNextMachine ;
			break;
		}
		pn = &((*pn)->m_pNextMachine) ;
	}
#ifdef _ENGINE_EDITABLE_
	m_MachineIsLockedForSoundManagement = false;
#endif
}
/*!
	\brief	Init
*/
VirtualMachine::eVMResult	OGLMachineWinDxSound::InitMachine()
{
	// create default root viewport object, for the whole screen:
	if( m_pDefaultViewPort ) return vmr_FAILED;

	OGLInternalViewPort *pglDefVP = new OGLInternalViewPort(0L,this);
	m_pDefaultViewPort = pglDefVP ;
	if( !pglDefVP ) return vmr_FAILED;
	// get system language.
	InitLocalization();

	//m_Width = 640;
	//m_Height = 480;
	//char *pTitle = " "; /*"http://www.m4nkind.com"*/;
	// Ask The User Which Screen Mode They Prefer
/*re	if (MessageBox(NULL,"Are you fullscreen ?" ,pTitle ,MB_YESNO|MB_ICONQUESTION )==IDNO)
    {
        m_fullscreen = false;                           // Windowed Mode
    }
*/
	//test:
	//m_fullscreen = false;   
	// Create our own windows class to handle messages and drawings:
	m_hInstance = GetModuleHandle(NULL); // Grab Application handler
	if(!m_OGLClassIsRegistered)
	{
		WNDCLASS	windowsClass;
		windowsClass.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;   // Redraw On Size, And Own DC For Window.
		windowsClass.lpfnWndProc      = (WNDPROC) WndProc;                    // WndProc Handles Messages
		windowsClass.cbClsExtra       =									 // No Extra Window Data
		windowsClass.cbWndExtra       = 0;                                    // No Extra Window Data
		windowsClass.hInstance        = m_hInstance;                            // Set The Instance
		windowsClass.hIcon            =  //LoadIcon(NULL,/*MAKEINTRESOURCE(101)*/IDI_HAND);
			LoadIcon(m_hInstance,MAKEINTRESOURCE(101));
			//IDI_ASTERISK);
			//LoadIcon(NULL,(LPCSTR)101 /*IDI_ICON1=101*//*IDI_WINLOGO*/);          // Load The Default Icon
		windowsClass.hCursor          = LoadCursor(NULL, IDC_ARROW);          // Load The Arrow Pointer
		windowsClass.hbrBackground    = NULL;								// No Background Required For GL
		windowsClass.lpszMenuName     = NULL;                                 // We Don't Want A Menu
		windowsClass.lpszClassName    = "OpenGL";                             // Set The Class Name
		if (!RegisterClass(&windowsClass))                                    // Attempt To Register The Window Class
		{
			MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return vmr_FAILED;
		}
		m_OGLClassIsRegistered = true;
	}


    // Create Our OpenGL Window
	if (!CreateGLWindow()) return vmr_FAILED; // Quit If Window Was Not Created
	//if (!CreateGLWindow(&m_FullScreen,true)) return vmr_FAILED; // Quit If Window Was Not Created
	//if (!CreateGLWindow(&m_WindowScreen,false)) return vmr_FAILED; // Quit If Window Was Not Created
	ActivateScreen( &m_FullScreen );
	// Usual OpenGL init stuff: (glclear color)
	VirtualMachine::eVMResult res= OGLMachine::InitMachine();
	if(vmr_OK != vmr_OK ) return res;
	// black screen now:
	pglDefVP->Clear();
	pglDefVP->SwapBuffer();

	// sound init:
	SoundInit( m_hWnd );

	// init openGL extension:
	if(!glActiveTextureARB)
		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
			wglGetProcAddress("glActiveTextureARB");
	if(!glClientActiveTextureARB)
		glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)
		wglGetProcAddress("glClientActiveTextureARB");
		
	return vmr_OK;
}
//! \brief	init localization :
void	OGLMachineWinDxSound::InitLocalization()
{
	LANGID sysdef = GetSystemDefaultLangID();
	sysdef &= 0x00ff; // exit sub languages.
	switch(sysdef)
	{
		default:
			m_MachineLocalization = eMachineLocalization_LocalisationNotImplemented;
			break;
		case 0x0009:
			m_MachineLocalization = eMachineLocalization_English;
			break;
		case 0x000c:
			m_MachineLocalization = eMachineLocalization_French;
			break;
		case 0x000a:
			m_MachineLocalization = eMachineLocalization_SpanishCastellano;
			break;
		case 0x0007:
			m_MachineLocalization = eMachineLocalization_German;
			break;
		case 0x0010:
			m_MachineLocalization = eMachineLocalization_Italian;
			break;
		case 0x0016:
			m_MachineLocalization = eMachineLocalization_Portuguese;
			break;
		case 0x0015:
			m_MachineLocalization = eMachineLocalization_Polish;
			break;
	}
}
/*
	\brief	After init,Set a general machine title, for example, to be displayed
		as a windows title. You can change the machine title at any time.
		Default behaviour: does nothing.
	\param	_pMachineTitle	const string to be displayed.
*/
void	OGLMachineWinDxSound::SetMachineTitle(const char *_pMachineTitle)
{
	m_pMachineTitle = _pMachineTitle ;
	if(m_hWnd) SetWindowText(m_hWnd,m_pMachineTitle);
}
int OGLMachineWinDxSound::ActivateScreen(RenderScreen *_pToActivate)
{
	// disable current screen
	if( m_pCurrentScreen )
	{
		ShowCursor(TRUE); // Show Mouse Pointer
		ShowWindow(m_hWnd,SW_HIDE); 
		if (m_pCurrentScreen->m_IsFullScreen) // Are We In Fullscreen Mode?
		{
			ChangeDisplaySettings(NULL,0);                  // If So Switch Back To The Desktop
		} else
		{ // keep windows position
			RECT rec;
			GetWindowRect( m_hWnd,&rec);
			m_pCurrentScreen->m_X = rec.left;
			m_pCurrentScreen->m_Y = rec.top;
		}
		m_pCurrentScreen->m_IsActive = false;
		m_pCurrentScreen = 0L;
	}
	// activate new screen
	if(_pToActivate)
	{
		if (_pToActivate->m_IsFullScreen) // Are We In Fullscreen Mode?
		{
			DEVMODE dmScreenSettings;                               // Device Mode
			memset(&dmScreenSettings,0,sizeof(dmScreenSettings));   // Makes Sure Memory's Cleared
			dmScreenSettings.dmSize=sizeof(dmScreenSettings);       // Size Of The Devmode Structure
			dmScreenSettings.dmPelsWidth    = _pToActivate->m_Width;  // Selected Screen Width
			dmScreenSettings.dmPelsHeight   = _pToActivate->m_Height; // Selected Screen Height
			dmScreenSettings.dmBitsPerPel   = 32;                 // Selected Bits Per Pixel
			dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
			// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
			if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
			{
					// back to Windowed Mode ?
					return 1;
			}
	        ShowCursor(FALSE);                                      // Hide Mouse Pointer
		} else
		{
			ShowCursor(TRUE); // Show Mouse Pointer
		}
		SetWindowLong(m_hWnd,GWL_STYLE,_pToActivate->m_dwStyle);
		SetWindowLong(m_hWnd,GWL_EXSTYLE,_pToActivate->m_dwExStyle);

		SetWindowPos(m_hWnd,HWND_TOP,_pToActivate->m_X,_pToActivate->m_Y,
						_pToActivate->m_Width,_pToActivate->m_Height,
						SWP_FRAMECHANGED);
		SetWindowText(m_hWnd,m_pMachineTitle);
		ShowWindow(m_hWnd,SW_SHOW);                       // Show The Window
		SetForegroundWindow(m_hWnd);                      // Slightly Higher Priority
		SetFocus(m_hWnd);                                 // Sets Keyboard Focus To The Window		          
		_pToActivate->m_IsActive = true;

		((OGLMachine::OGLInternalViewPort *) m_pDefaultViewPort)
				->ResetRootPixelSize(_pToActivate->m_Width,_pToActivate->m_Height);
	}
	m_pCurrentScreen = _pToActivate;

	// ok
	return 0;
}

void OGLMachineWinDxSound::KillGLWindow(/*RenderScreen *_pToDestroy*/) // Properly Kill The Window
{
	ActivateScreen(0L);

    if (m_hRC)    // Do We Have A Rendering Context?
    {
        if (!wglDeleteContext(m_hRC))   // Are We Able To Delete The RC?
        {
            MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        }
        m_hRC=NULL;   // Set RC To NULL
    }

    if (m_DC && !ReleaseDC(m_hWnd,m_DC))                    // Are We Able To Release The DC
    {
        MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        m_DC=NULL;   // Set DC To NULL
    }

    if (m_hWnd && !DestroyWindow(m_hWnd))                   // Are We Able To Destroy The Window?
    {
        MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        m_hWnd=NULL;   // Set hWnd To NULL
    }

}

BOOL OGLMachineWinDxSound::CreateGLWindow(/*RenderScreen *_pToCreate,bool fullscreen*/)
{
    GLuint      PixelFormat;            // Holds The Results After Searching For A Match

    // Create The Window like fullscreen first
	DEVMODE dmScreenSettings;                               // Device Mode
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));   // Makes Sure Memory's Cleared
	dmScreenSettings.dmSize=sizeof(dmScreenSettings);       // Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth    = m_FullScreen.m_Width;  // Selected Screen Width
	dmScreenSettings.dmPelsHeight   = m_FullScreen.m_Height; // Selected Screen Height
	dmScreenSettings.dmBitsPerPel   = 32;                 // Selected Bits Per Pixel
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
	// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
	if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{

			// back to Windowed Mode ?
			return FALSE;
	}
	ShowCursor(FALSE);                                      // Hide Mouse Pointer	

	RECT        WindowRect;             // Grabs Rectangle Upper Left / Lower Right Values
    WindowRect.left=(long)0;            // Set Left Value To 0
	WindowRect.right=(long)m_FullScreen.m_Width;       // Set Right Value To Requested Width
    WindowRect.top=(long)0;             // Set Top Value To 0
	WindowRect.bottom=(long)m_FullScreen.m_Height;     // Set Bottom Value To Requested Height
	AdjustWindowRectEx(&WindowRect, m_FullScreen.m_dwStyle, FALSE, m_FullScreen.m_dwExStyle);     // Adjust Window To True Requested Size

	if (!(m_hWnd =CreateWindowEx( m_FullScreen.m_dwExStyle , // Extended Style For The Window
                                "OpenGL",                           // Class Name
                                " ",                              // Window Title
                                m_FullScreen.m_dwStyle |                           // Defined Window Style
                                WS_CLIPSIBLINGS |                   // Required Window Style
                                WS_CLIPCHILDREN,                    // Required Window Style
                                0, 0,                               // Window Position
                                WindowRect.right-WindowRect.left,   // Calculate Window Width
                                WindowRect.bottom-WindowRect.top,   // Calculate Window Height
                                NULL,                               // No Parent Window
                                NULL,                               // No Menu
                                m_hInstance,                          // Instance
                                NULL)))                             // Dont Pass Anything To WM_CREATE
    {
        MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;                               // Return FALSE
    }

    static const PIXELFORMATDESCRIPTOR pfd=              // pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR),              // Size Of This Pixel Format Descriptor
        1,                                          // Version Number
        PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
        PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
        PFD_DOUBLEBUFFER,                           // Must Support Double Buffering
        PFD_TYPE_RGBA,                              // Request An RGBA Format
        32,                                       // Select Our Color Depth
        0, 0, 0, 0, 0, 0,                           // Color Bits Ignored
        0,                                          // No Alpha Buffer
        0,                                          // Shift Bit Ignored
        0,                                          // No Accumulation Buffer
        0, 0, 0, 0,                                 // Accumulation Bits Ignored
        16,                                         // 16Bit Z-Buffer (Depth Buffer)  
        0,                                          // No Stencil Buffer
        0,                                          // No Auxiliary Buffer
        PFD_MAIN_PLANE,                             // Main Drawing Layer
        0,                                          // Reserved
        0, 0, 0                                     // Layer Masks Ignored
    };
    
    if (!(m_DC=GetDC(m_hWnd)))  // Did We Get A Device Context?
    {
        MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;                               // Return FALSE
    }

    if (!(PixelFormat=ChoosePixelFormat(m_DC,&pfd)))   // Did Windows Find A Matching Pixel Format?
    {
        MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;                               // Return FALSE
    }

    if(!SetPixelFormat(m_DC,PixelFormat,&pfd))     // Are We Able To Set The Pixel Format?
    {
        MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;                               // Return FALSE
    }

    if (!(m_hRC=wglCreateContext(m_DC)))             // Are We Able To Get A Rendering Context?
    {
        MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;                               // Return FALSE
    }
	wglMakeCurrent(m_DC,m_hRC);
    return TRUE;                                    // Success
}

LRESULT CALLBACK OGLMachineWinDxSound::WndProc(   HWND    hWnd,           // Handle For This Window
                            UINT    uMsg,           // Message For This Window
                            WPARAM  wParam,         // Additional Message Information
                            LPARAM  lParam)         // Additional Message Information
{	
	switch (uMsg)                                   // Check For Windows Messages
    {

        case WM_ACTIVATE:                           // Watch For Window Activate Message
        {
            if (!HIWORD(wParam))                    // Check Minimization State
            {
                m_LastInstance->m_active=true;                        // Program Is Active
            }
            else
            {
                m_LastInstance->m_active=false;                       // Program Is No Longer Active
            }

            return 0;                               // Return To The Message Loop
        }

        case WM_SYSCOMMAND:                         // Intercept System Commands
        {
            switch (wParam)                         // Check System Calls
            {
                case SC_SCREENSAVE:                 // Screensaver Trying To Start?
                case SC_MONITORPOWER:               // Monitor Trying To Enter Powersave?
                return 0;                           // Prevent From Happening
            }
            break;                                  // Exit
        }
		case WM_LBUTTONDOWN:
		{
			if( m_LastInstance->m_pCurrentScreen == &(m_LastInstance->m_FullScreen) ) m_LastInstance->m_QuitMessage = true;	// Send A Quit Message
            return 0; 
		}
		case WM_CLOSE:                              // Did We Receive A Close Message?
        {
			m_LastInstance->m_QuitMessage = true;
            return 0;                               // Jump Back
        }

        case WM_KEYDOWN:                            // Is A Key Being Held Down?
        {
			if( wParam == VK_ESCAPE ) m_LastInstance->m_QuitMessage = true;
			if( wParam == VK_SPACE ||
				wParam == VK_RETURN )
				m_LastInstance->m_AskSwitchScreen = true;
//            return 0;                               // Jump Back
         } break;
		/*
        case WM_KEYUP:                              // Has A Key Been Released?
        {
             return 0;                               // Jump Back
        }
		*/
        case WM_SIZE:                               // Resize The OpenGL Window
        {
		
           //re? ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			m_LastInstance->m_Width = LOWORD(lParam);
			m_LastInstance->m_Height = HIWORD(lParam);
			if(m_LastInstance->m_pCurrentScreen)
			{
				m_LastInstance->m_pCurrentScreen->m_Width = LOWORD(lParam);
				m_LastInstance->m_pCurrentScreen->m_Height = HIWORD(lParam);

			}
			((OGLInternalViewPort *)(m_LastInstance->m_pDefaultViewPort))->ResetRootPixelSize(m_LastInstance->m_Width,m_LastInstance->m_Height);

            return 0;                               // Jump Back
        }
	}

    // Pass All Unhandled Messages To DefWindowProc
    return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

/*!
	\brief	Swap buffer so that what was drawn is shown.
*/
void	OGLMachineWinDxSound::SwapMainScreenBuffer()
{/* old
	if( m_active ) SwapBuffers(m_DC);                 // Swap Buffers (Double Buffering)
	*/
	if( m_pCurrentScreen )
	{
		SwapBuffers( m_DC );
		m_pDefaultViewPort->Clear();
	}
}

/*!
	\brief	If true, means that the user want to quit:
*/
void	OGLMachineWinDxSound::ProcessInterface()
{
    MSG     msg;    
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))   // Is There A Message Waiting?
	{
		if (msg.message==WM_QUIT)               // Have We Received A Quit Message?
		{
			m_QuitMessage = true;  
		}
		else                                    // If Not, Deal With Window Messages
		{
			TranslateMessage(&msg);             // Translate The Message
			DispatchMessage(&msg);              // Dispatch The Message
		}
	}
	// do switch screen if need:
	if(m_AskSwitchScreen)
	{
		if(m_pCurrentScreen== &m_FullScreen)
			ActivateScreen( &m_WindowScreen);
		else
			ActivateScreen( &m_FullScreen);
		m_AskSwitchScreen = false;
	}
}
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
bool	OGLMachineWinDxSound::FileRequester(const char *_pDisplayString,char *_pResultFileName, unsigned int _nameMaxLength )
{
/*	CFileDialog	cdiag(true,_pDisplayString,0,0,0,0,0);
	cdiag.DoModal();
	CString filename = cdiag.GetFileName();
	strncpy(_pResultFileName,filename.GetString(),_nameMaxLength);
*/
	OPENFILENAME ofn;       // common dialog box structure
	//char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = _pResultFileName;
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = _nameMaxLength ; //sizeof(szFile);
	ofn.lpstrFilter = "";//"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST /*| OFN_FILEMUSTEXIST*/;

	// Display the Open dialog box. 
	bool res =  (GetOpenFileName(&ofn)==TRUE)  ;

/*		hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
			0, (LPSECURITY_ATTRIBUTES) NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
			(HANDLE) NULL);
*/
	return res;
}
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
const char *OGLMachineWinDxSound::LoadFile(const char *_pFilePath,unsigned int _pFileStart,unsigned int &_LoadByteLength, unsigned int _maximumSizeLoad)
{

	return 0L;
}
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
bool OGLMachineWinDxSound::SaveFile(const char *_pFilePath,const char *_chunkToWrite,unsigned int _ChunkLength,bool _append)
{
	return false;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	in edition mode, redirection of machine task wait.
	\param	_milliseconds time to wait.
*/
void	OGLMachineWinDxSound::Sleep( unsigned int _milliseconds )
{
	// window's Sleep
	::Sleep(_milliseconds);
}
#endif
/*!
	\brief	Return the language of the machine, as a simple enum. It can be used
		by exemple
	\return an enum integer.
*/
VirtualMachine::eMachineLocalization	OGLMachineWinDxSound::GetCurrentWorldLocalizationEnum()
{
	return m_MachineLocalization;
}