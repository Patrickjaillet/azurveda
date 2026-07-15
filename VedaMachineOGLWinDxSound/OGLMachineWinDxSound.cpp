// SPDX-License-Identifier: LGPL-2.1-only

#include "OGLMachineWinDxSound.h"

#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

bool	OGLMachineWinDxSound::m_GLFWIsInitialized=false;

OGLMachineWinDxSound::OGLMachineWinDxSound() : OGLMachine()

	,m_active(true)
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
	,m_pGLFWWindow(NULL)
{
	m_FullScreen.m_IsActive = false;
	m_FullScreen.m_IsFullScreen = true;
	m_FullScreen.m_X = 0;
	m_FullScreen.m_Y = 0;
	m_FullScreen.m_Width = 800;
	m_FullScreen.m_Height = 600;

	m_WindowScreen.m_IsActive = false;
	m_WindowScreen.m_IsFullScreen = false;
	m_WindowScreen.m_X = 128;
	m_WindowScreen.m_Y = 128;
	m_WindowScreen.m_Width = 640;
	m_WindowScreen.m_Height = 480;

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

	if(!(m_pFirstMachine->m_pNextMachine) && m_GLFWIsInitialized )
	{
		glfwTerminate();
		m_GLFWIsInitialized = false;
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

	if (!CreateGLWindow()) return vmr_FAILED;

	ActivateScreen( &m_FullScreen );

	VirtualMachine::eVMResult res= OGLMachine::InitMachine();
	if(res != vmr_OK ) return res;

	pglDefVP->Clear();
	pglDefVP->SwapBuffer();

	SoundInit( glfwGetWin32Window(m_pGLFWWindow) );

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
	if(m_pGLFWWindow) glfwSetWindowTitle(m_pGLFWWindow,m_pMachineTitle);
}
int OGLMachineWinDxSound::ActivateScreen(RenderScreen *_pToActivate)
{

	if( m_pCurrentScreen )
	{
		if (!m_pCurrentScreen->m_IsFullScreen)
		{
			int x,y;
			glfwGetWindowPos(m_pGLFWWindow,&x,&y);
			m_pCurrentScreen->m_X = (unsigned int)x;
			m_pCurrentScreen->m_Y = (unsigned int)y;
		}
		m_pCurrentScreen->m_IsActive = false;
		m_pCurrentScreen = 0L;
	}

	if(_pToActivate)
	{
		if (_pToActivate->m_IsFullScreen)
		{
			GLFWmonitor *pMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *pMode = glfwGetVideoMode(pMonitor);
			glfwSetWindowMonitor(m_pGLFWWindow,pMonitor,0,0,
				(int)_pToActivate->m_Width,(int)_pToActivate->m_Height,pMode->refreshRate);
			glfwSetInputMode(m_pGLFWWindow,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
		} else
		{
			glfwSetWindowMonitor(m_pGLFWWindow,NULL,(int)_pToActivate->m_X,(int)_pToActivate->m_Y,
				(int)_pToActivate->m_Width,(int)_pToActivate->m_Height,0);
			glfwSetInputMode(m_pGLFWWindow,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
		}
		glfwSetWindowTitle(m_pGLFWWindow,m_pMachineTitle ? m_pMachineTitle : " ");
		glfwShowWindow(m_pGLFWWindow);
		glfwFocusWindow(m_pGLFWWindow);
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

	if(m_pGLFWWindow)
	{
		glfwDestroyWindow(m_pGLFWWindow);
		m_pGLFWWindow=NULL;
	}
}

BOOL OGLMachineWinDxSound::CreateGLWindow()
{
	if(!m_GLFWIsInitialized)
	{
		if(!glfwInit())
		{
			MessageBox(NULL,"Failed To Initialize GLFW.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;
		}
		m_GLFWIsInitialized = true;
	}

	glfwWindowHint(GLFW_CLIENT_API,GLFW_OPENGL_API);
	glfwWindowHint(GLFW_DEPTH_BITS,16);
	glfwWindowHint(GLFW_DOUBLEBUFFER,GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);

	m_pGLFWWindow = glfwCreateWindow(
		(int)m_FullScreen.m_Width,(int)m_FullScreen.m_Height,
		m_pMachineTitle ? m_pMachineTitle : " ",
		NULL,NULL);
	if(!m_pGLFWWindow)
	{
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	glfwSetWindowUserPointer(m_pGLFWWindow,this);
	glfwMakeContextCurrent(m_pGLFWWindow);

	glfwSetKeyCallback(m_pGLFWWindow,KeyCallback);
	glfwSetMouseButtonCallback(m_pGLFWWindow,MouseButtonCallback);
	glfwSetWindowCloseCallback(m_pGLFWWindow,WindowCloseCallback);
	glfwSetFramebufferSizeCallback(m_pGLFWWindow,FramebufferSizeCallback);
	glfwSetWindowFocusCallback(m_pGLFWWindow,WindowFocusCallback);

    return TRUE;
}

void	OGLMachineWinDxSound::KeyCallback(GLFWwindow *_pWindow,int _key,int ,int _action,int )
{
	OGLMachineWinDxSound *pMachine = (OGLMachineWinDxSound *)glfwGetWindowUserPointer(_pWindow);
	if(_action == GLFW_RELEASE) return;
	if(_key == GLFW_KEY_ESCAPE) pMachine->m_QuitMessage = true;
	if(_key == GLFW_KEY_SPACE || _key == GLFW_KEY_ENTER) pMachine->m_AskSwitchScreen = true;
}

void	OGLMachineWinDxSound::MouseButtonCallback(GLFWwindow *_pWindow,int _button,int _action,int )
{
	OGLMachineWinDxSound *pMachine = (OGLMachineWinDxSound *)glfwGetWindowUserPointer(_pWindow);
	if( _button == GLFW_MOUSE_BUTTON_LEFT && _action == GLFW_PRESS &&
		pMachine->m_pCurrentScreen == &(pMachine->m_FullScreen) )
		pMachine->m_QuitMessage = true;
}

void	OGLMachineWinDxSound::WindowCloseCallback(GLFWwindow *_pWindow)
{
	OGLMachineWinDxSound *pMachine = (OGLMachineWinDxSound *)glfwGetWindowUserPointer(_pWindow);
	pMachine->m_QuitMessage = true;
}

void	OGLMachineWinDxSound::FramebufferSizeCallback(GLFWwindow *_pWindow,int _width,int _height)
{
	OGLMachineWinDxSound *pMachine = (OGLMachineWinDxSound *)glfwGetWindowUserPointer(_pWindow);
	pMachine->m_Width = (unsigned int)_width;
	pMachine->m_Height = (unsigned int)_height;
	if(pMachine->m_pCurrentScreen)
	{
		pMachine->m_pCurrentScreen->m_Width = (unsigned int)_width;
		pMachine->m_pCurrentScreen->m_Height = (unsigned int)_height;
	}
	((OGLInternalViewPort *)(pMachine->m_pDefaultViewPort))->ResetRootPixelSize(pMachine->m_Width,pMachine->m_Height);
}

void	OGLMachineWinDxSound::WindowFocusCallback(GLFWwindow *_pWindow,int _focused)
{
	OGLMachineWinDxSound *pMachine = (OGLMachineWinDxSound *)glfwGetWindowUserPointer(_pWindow);
	pMachine->m_active = (_focused != 0);
}

void	OGLMachineWinDxSound::SwapMainScreenBuffer()
{
	if( m_pCurrentScreen )
	{
		glfwSwapBuffers( m_pGLFWWindow );
		m_pDefaultViewPort->Clear();
	}
}

void	OGLMachineWinDxSound::ProcessInterface()
{
	glfwPollEvents();

	if(m_pGLFWWindow && glfwWindowShouldClose(m_pGLFWWindow))
		m_QuitMessage = true;

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
	ofn.hwndOwner = m_pGLFWWindow ? glfwGetWin32Window(m_pGLFWWindow) : NULL;
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
