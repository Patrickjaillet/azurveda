
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "LeftView.h"
#include "Version.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CVedaDemoOGLMfcGuiApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)

	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_EMPTYCONTEXT, CVedaDemoOGLMfcGuiApp::OnEmptyContext)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)

END_MESSAGE_MAP()

CVedaDemoOGLMfcGuiApp::CVedaDemoOGLMfcGuiApp()
{
	m_NewFilePref = 0;

}

CVedaDemoOGLMfcGuiApp theApp;

BOOL CVedaDemoOGLMfcGuiApp::InitInstance()
{

	InitCommonControls();

	CWinApp::InitInstance();

	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	SetRegistryKey(_T("AzurVeda Context Editor with MFC by www.m4nkind.com"));
	LoadStdProfileSettings(8);

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_VedaDemoOGLMfcGTYPE,
		RUNTIME_CLASS(CVedaDemoOGLMfcGuiDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CLeftView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	m_pMainWnd->DragAcceptFiles();

	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	CCommandLineInfo cmdInfo;

	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	pMainFrame->SetWindowPos(NULL,128,64,800,600,0);
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBaseobjectClone();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString versionText;
	versionText.Format(_T("UVeda MFC GUI v%s (%s)"),
		_T(AZURVEDA_VERSION_STRING), _T(AZURVEDA_VERSION_GIT_HASH));
	SetDlgItemText(IDC_ABOUT_VERSION, versionText);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_COMMAND(ID_BASEOBJECT_CLONE, OnBaseobjectClone)
END_MESSAGE_MAP()

void CVedaDemoOGLMfcGuiApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CAboutDlg::OnBaseobjectClone()
{

}
void CVedaDemoOGLMfcGuiApp::OnEmptyContext()
{
	m_NewFilePref = 1;
	CWinApp::OnFileNew();

}
