// VedaDemoOGLMfcGui.cpp : Définit les comportements de classe pour l'application.
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "LeftView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVedaDemoOGLMfcGuiApp

BEGIN_MESSAGE_MAP(CVedaDemoOGLMfcGuiApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Commandes de fichier standard
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_EMPTYCONTEXT, CVedaDemoOGLMfcGuiApp::OnEmptyContext)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
/*
	ON_COMMAND(ID_UNDO, CVedaDemoOGLMfcGuiApp::OnUndo)
	ON_COMMAND(ID_REDO, CVedaDemoOGLMfcGuiApp::OnRedo)
	ON_COMMAND(ID_MERGE, CVedaDemoOGLMfcGuiApp::OnMerge)
	ON_COMMAND(ID_FLUSHNAMES, CLeftView::OnFlushNames)
	*/
END_MESSAGE_MAP()


// construction CVedaDemoOGLMfcGuiApp

CVedaDemoOGLMfcGuiApp::CVedaDemoOGLMfcGuiApp()
{
	m_NewFilePref = 0;
	// TODO : ajoutez ici du code de construction,
	// Placez toutes les initialisations significatives dans InitInstance
}


// Seul et unique objet CVedaDemoOGLMfcGuiApp

CVedaDemoOGLMfcGuiApp theApp;

// initialisation CVedaDemoOGLMfcGuiApp

BOOL CVedaDemoOGLMfcGuiApp::InitInstance()
{
	// InitCommonControls() est requis sur Windows XP si le manifeste de l'application
	// spécifie l'utilisation de ComCtl32.dll version 6 ou ultérieure pour activer
	// les styles visuels.  Dans le cas contraire, la création de fenêtres échouera.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialiser les bibliothèques OLE
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Initialisation standard
	// Si vous n'utilisez pas ces fonctionnalités et que vous souhaitez réduire la taille
	// de votre exécutable final, vous devez supprimer ci-dessous
	// les routines d'initialisation spécifiques dont vous n'avez pas besoin.
	// Changez la clé de Registre sous laquelle nos paramètres sont enregistrés.
	// TODO : Modifiez cette chaîne avec des informations appropriées,
	// telles que le nom de votre société ou organisation
	SetRegistryKey(_T("AzurVeda Context Editor with MFC by www.m4nkind.com"));
	LoadStdProfileSettings(8);  // Charge les options de fichier INI standard (y compris les derniers fichiers utilisés)
	// Inscrire les modèles de document de l'application. Ces modèles
	//  lient les documents, fenêtres frame et vues entre eux
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_VedaDemoOGLMfcGTYPE,
		RUNTIME_CLASS(CVedaDemoOGLMfcGuiDoc),
		RUNTIME_CLASS(CChildFrame), // frame enfant MDI personnalisé
		RUNTIME_CLASS(CLeftView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// crée la fenêtre frame MDI principale
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// appelle DragAcceptFiles uniquement s'il y a un suffixe
	//  Dans une application MDI, cet appel doit avoir lieu juste après la définition de m_pMainWnd
	// Activer les ouvertures via glisser-déplacer
	m_pMainWnd->DragAcceptFiles();
	// Activer les ouvertures d'exécution DDE
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// Analyser la ligne de commande pour les commandes shell standard, DDE, ouverture de fichiers
	CCommandLineInfo cmdInfo;

	//cmdInfo.get
	ParseCommandLine(cmdInfo);
	// Distribue les commandes spécifiées dans la ligne de commande. Retourne la valeur FALSE si
	// l'application a été démarrée avec /RegServer, /Register, /Unregserver ou /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// La fenêtre principale a été initialisée et peut donc être affichée et mise à jour

	pMainFrame->SetWindowPos(NULL,128,64,800,600,0);
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	

	return TRUE;
}



// boîte de dialogue CAboutDlg utilisée pour la boîte de dialogue 'À propos de' pour votre application

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

// Implémentation
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

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_COMMAND(ID_BASEOBJECT_CLONE, OnBaseobjectClone)
END_MESSAGE_MAP()

// Commande App pour exécuter la boîte de dialogue
void CVedaDemoOGLMfcGuiApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
/*
void CVedaDemoOGLMfcGuiApp::OnUndo()
{
	this->Getm MainWnd() Focus();
	POSITION pos= m_pDocManager->GetFirstDocTemplatePosition();
	while(pos)
	{
		CDocTemplate *pTemplate = m_pDocManager->GetNextDocTemplate( pos );
		pTemplate->get
	}
}
void CVedaDemoOGLMfcGuiApp::OnRedo()
{

}
void CVedaDemoOGLMfcGuiApp::OnMerge()
{

}
*/
// gestionnaires de messages pour CVedaDemoOGLMfcGuiApp


void CAboutDlg::OnBaseobjectClone()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}
void CVedaDemoOGLMfcGuiApp::OnEmptyContext()
{
	m_NewFilePref = 1;
	CWinApp::OnFileNew();

}
