// SPDX-License-Identifier: LGPL-2.1-only

#include "OGLMachineWinDxSound.h"

#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>

OGLMachineWinDxSound	*OGLMachineWinDxSound::m_LastInstance=NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool	OGLMachineWinDxSound::m_OGLClassIsRegistered=false;

OGLMachineWinDxSound::OGLMachineWinDxSound() : OGLMachine()

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

	m_LastInstance=this;

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

	if(!(m_pFirstMachine->m_pNextMachine) && m_OGLClassIsRegistered )
	{
		UnregisterClass("OpenGL",m_hInstance);
		m_OGLClassIsRegistered = false;
	}

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

VirtualMachine::eVMResult	OGLMachineWinDxSound::InitMachine()
{

	if( m_pDefaultViewPort ) return vmr_FAILED;

	OGLInternalViewPort *pglDefVP = new OGLInternalViewPort(0L,this);
	m_pDefaultViewPort = pglDefVP ;
	if( !pglDefVP ) return vmr_FAILED;

	InitLocalization();

	m_hInstance = GetModuleHandle(NULL);
	if(!m_OGLClassIsRegistered)
	{
		WNDCLASS	windowsClass;
		windowsClass.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowsClass.lpfnWndProc      = (WNDPROC) WndProc;
		windowsClass.cbClsExtra       =
		windowsClass.cbWndExtra       = 0;
		windowsClass.hInstance        = m_hInstance;
		windowsClass.hIcon            =
			LoadIcon(m_hInstance,MAKEINTRESOURCE(101));

		windowsClass.hCursor          = LoadCursor(NULL, IDC_ARROW);
		windowsClass.hbrBackground    = NULL;
		windowsClass.lpszMenuName     = NULL;
		windowsClass.lpszClassName    = "OpenGL";
		if (!RegisterClass(&windowsClass))
		{
			MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return vmr_FAILED;
		}
		m_OGLClassIsRegistered = true;
	}

	if (!CreateGLWindow()) return vmr_FAILED;

	ActivateScreen( &m_FullScreen );

	VirtualMachine::eVMResult res= OGLMachine::InitMachine();
	if(vmr_OK != vmr_OK ) return res;

	pglDefVP->Clear();
	pglDefVP->SwapBuffer();

	SoundInit( m_hWnd );

	if(!glActiveTextureARB)
		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
			wglGetProcAddress("glActiveTextureARB");
	if(!glClientActiveTextureARB)
		glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)
		wglGetProcAddress("glClientActiveTextureARB");

	return vmr_OK;
}

void	OGLMachineWinDxSound::InitLocalization()
{
	LANGID sysdef = GetSystemDefaultLangID();
	sysdef &= 0x00ff;
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

void	OGLMachineWinDxSound::SetMachineTitle(const char *_pMachineTitle)
{
	m_pMachineTitle = _pMachineTitle ;
	if(m_hWnd) SetWindowText(m_hWnd,m_pMachineTitle);
}
int OGLMachineWinDxSound::ActivateScreen(RenderScreen *_pToActivate)
{

	if( m_pCurrentScreen )
	{
		ShowCursor(TRUE);
		ShowWindow(m_hWnd,SW_HIDE);
		if (m_pCurrentScreen->m_IsFullScreen)
		{
			ChangeDisplaySettings(NULL,0);
		} else
		{
			RECT rec;
			GetWindowRect( m_hWnd,&rec);
			m_pCurrentScreen->m_X = rec.left;
			m_pCurrentScreen->m_Y = rec.top;
		}
		m_pCurrentScreen->m_IsActive = false;
		m_pCurrentScreen = 0L;
	}

	if(_pToActivate)
	{
		if (_pToActivate->m_IsFullScreen)
		{
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
			dmScreenSettings.dmSize=sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth    = _pToActivate->m_Width;
			dmScreenSettings.dmPelsHeight   = _pToActivate->m_Height;
			dmScreenSettings.dmBitsPerPel   = 32;
			dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

			if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
			{

					return 1;
			}
	        ShowCursor(FALSE);
		} else
		{
			ShowCursor(TRUE);
		}
		SetWindowLong(m_hWnd,GWL_STYLE,_pToActivate->m_dwStyle);
		SetWindowLong(m_hWnd,GWL_EXSTYLE,_pToActivate->m_dwExStyle);

		SetWindowPos(m_hWnd,HWND_TOP,_pToActivate->m_X,_pToActivate->m_Y,
						_pToActivate->m_Width,_pToActivate->m_Height,
						SWP_FRAMECHANGED);
		SetWindowText(m_hWnd,m_pMachineTitle);
		ShowWindow(m_hWnd,SW_SHOW);
		SetForegroundWindow(m_hWnd);
		SetFocus(m_hWnd);
		_pToActivate->m_IsActive = true;

		((OGLMachine::OGLInternalViewPort *) m_pDefaultViewPort)
				->ResetRootPixelSize(_pToActivate->m_Width,_pToActivate->m_Height);
	}
	m_pCurrentScreen = _pToActivate;

	return 0;
}

void OGLMachineWinDxSound::KillGLWindow()
{
	ActivateScreen(0L);

    if (m_hRC)
    {
        if (!wglDeleteContext(m_hRC))
        {
            MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        }
        m_hRC=NULL;
    }

    if (m_DC && !ReleaseDC(m_hWnd,m_DC))
    {
        MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        m_DC=NULL;
    }

    if (m_hWnd && !DestroyWindow(m_hWnd))
    {
        MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        m_hWnd=NULL;
    }

}

BOOL OGLMachineWinDxSound::CreateGLWindow()
{
    GLuint      PixelFormat;

	DEVMODE dmScreenSettings;
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
	dmScreenSettings.dmSize=sizeof(dmScreenSettings);
	dmScreenSettings.dmPelsWidth    = m_FullScreen.m_Width;
	dmScreenSettings.dmPelsHeight   = m_FullScreen.m_Height;
	dmScreenSettings.dmBitsPerPel   = 32;
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{

			return FALSE;
	}
	ShowCursor(FALSE);

	RECT        WindowRect;
    WindowRect.left=(long)0;
	WindowRect.right=(long)m_FullScreen.m_Width;
    WindowRect.top=(long)0;
	WindowRect.bottom=(long)m_FullScreen.m_Height;
	AdjustWindowRectEx(&WindowRect, m_FullScreen.m_dwStyle, FALSE, m_FullScreen.m_dwExStyle);

	if (!(m_hWnd =CreateWindowEx( m_FullScreen.m_dwExStyle ,
                                "OpenGL",
                                " ",
                                m_FullScreen.m_dwStyle |
                                WS_CLIPSIBLINGS |
                                WS_CLIPCHILDREN,
                                0, 0,
                                WindowRect.right-WindowRect.left,
                                WindowRect.bottom-WindowRect.top,
                                NULL,
                                NULL,
                                m_hInstance,
                                NULL)))
    {
        MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

    static const PIXELFORMATDESCRIPTOR pfd=
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL |
        PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        16,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    if (!(m_DC=GetDC(m_hWnd)))
    {
        MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!(PixelFormat=ChoosePixelFormat(m_DC,&pfd)))
    {
        MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

    if(!SetPixelFormat(m_DC,PixelFormat,&pfd))
    {
        MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!(m_hRC=wglCreateContext(m_DC)))
    {
        MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }
	wglMakeCurrent(m_DC,m_hRC);
    return TRUE;
}

LRESULT CALLBACK OGLMachineWinDxSound::WndProc(   HWND    hWnd,
                            UINT    uMsg,
                            WPARAM  wParam,
                            LPARAM  lParam)
{
	switch (uMsg)
    {

        case WM_ACTIVATE:
        {
            if (!HIWORD(wParam))
            {
                m_LastInstance->m_active=true;
            }
            else
            {
                m_LastInstance->m_active=false;
            }

            return 0;
        }

        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                return 0;
            }
            break;
        }
		case WM_LBUTTONDOWN:
		{
			if( m_LastInstance->m_pCurrentScreen == &(m_LastInstance->m_FullScreen) ) m_LastInstance->m_QuitMessage = true;
            return 0;
		}
		case WM_CLOSE:
        {
			m_LastInstance->m_QuitMessage = true;
            return 0;
        }

        case WM_KEYDOWN:
        {
			if( wParam == VK_ESCAPE ) m_LastInstance->m_QuitMessage = true;
			if( wParam == VK_SPACE ||
				wParam == VK_RETURN )
				m_LastInstance->m_AskSwitchScreen = true;

         } break;

        case WM_SIZE:
        {

			m_LastInstance->m_Width = LOWORD(lParam);
			m_LastInstance->m_Height = HIWORD(lParam);
			if(m_LastInstance->m_pCurrentScreen)
			{
				m_LastInstance->m_pCurrentScreen->m_Width = LOWORD(lParam);
				m_LastInstance->m_pCurrentScreen->m_Height = HIWORD(lParam);

			}
			((OGLInternalViewPort *)(m_LastInstance->m_pDefaultViewPort))->ResetRootPixelSize(m_LastInstance->m_Width,m_LastInstance->m_Height);

            return 0;
        }
	}

    return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void	OGLMachineWinDxSound::SwapMainScreenBuffer()
{
	if( m_pCurrentScreen )
	{
		SwapBuffers( m_DC );
		m_pDefaultViewPort->Clear();
	}
}

void	OGLMachineWinDxSound::ProcessInterface()
{
    MSG     msg;
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		if (msg.message==WM_QUIT)
		{
			m_QuitMessage = true;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	if(m_AskSwitchScreen)
	{
		if(m_pCurrentScreen== &m_FullScreen)
			ActivateScreen( &m_WindowScreen);
		else
			ActivateScreen( &m_FullScreen);
		m_AskSwitchScreen = false;
	}
}

#ifdef _ENGINE_EDITABLE_

bool	OGLMachineWinDxSound::FileRequester(const char *,char *_pResultFileName, unsigned int _nameMaxLength )
{

	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = _pResultFileName;

	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = _nameMaxLength ;
	ofn.lpstrFilter = "";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST ;

	bool res =  (GetOpenFileName(&ofn)==TRUE)  ;

	return res;
}
#endif
#ifdef _ENGINE_EDITABLE_

const char *OGLMachineWinDxSound::LoadFile(const char *,unsigned int ,unsigned int &, unsigned int )
{

	return 0L;
}
#endif
#ifdef _ENGINE_EDITABLE_

bool OGLMachineWinDxSound::SaveFile(const char *,const char *,unsigned int ,bool )
{
	return false;
}
#endif
#ifdef _ENGINE_EDITABLE_

void	OGLMachineWinDxSound::Sleep( unsigned int _milliseconds )
{

	::Sleep(_milliseconds);
}
#endif

VirtualMachine::eMachineLocalization	OGLMachineWinDxSound::GetCurrentWorldLocalizationEnum()
{
	return m_MachineLocalization;
}
