// MainFrm.cpp : implémentation de la classe CMainFrame
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "VedaDemoOGLMfcGuiView.h"
#include "FormViewBaseObject.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_UNDO, OnUndo)
	ON_COMMAND(ID_REDO, OnRedo)
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
//	ON_COMMAND(ID_FILE_NEW, OnFileNew)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // indicateur de la ligne d'état
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// construction ou destruction de CMainFrame

CMainFrame::CMainFrame()
{
	// TODO : ajoutez ici le code d'une initialisation de membre
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Impossible de créer toolbar\n");
		return -1;      // échec de la création
	}
	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Impossible de créer dialogbar\n");
		return -1;		// échec de la création
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("Impossible de créer rebar\n");
		return -1;      // échec de la création
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Impossible de créer la barre d'état\n");
		return -1;      // échec de la création
	}
	// TODO : Supprimez ceci si vous ne souhaitez pas afficher les info-bulles
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs,
	CCreateContext* pContext)
{

	// créer une fenêtre fractionnée
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(200, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CFormViewBaseObject), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO : Changez ici la classe ou les styles Window en modifiant
	//  CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX;

	return TRUE;
}


// diagnostics pour CMainFrame

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// gestionnaires de messages pour CMainFrame

CFormViewBaseObject* CMainFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CFormViewBaseObject* pView = DYNAMIC_DOWNCAST(CFormViewBaseObject, pWnd);
	return pView;
}

void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	if (!pCmdUI)
		return;

	// TODO : personnalisez ou étendez ce code en vue de la gestion des options dans le menu Affichage

	CFormViewBaseObject* pView = GetRightPane(); 

	// si le volet de droite n'a pas été créé ou n'est pas une vue,
	// désactiver les commandes dans notre étendue

	if (pView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;

		// si la commande est ID_VIEW_LINEUP, activer la commande
		// uniquement en mode LVS_ICON ou LVS_SMALLICON

		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
		{
			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			// sinon, utiliser des pointillés pour refléter le style de la vue
			pCmdUI->Enable();
			BOOL bChecked = FALSE;

			switch (pCmdUI->m_nID)
			{
			case ID_VIEW_DETAILS:
				bChecked = (dwStyle == LVS_REPORT);
				break;

			case ID_VIEW_SMALLICON:
				bChecked = (dwStyle == LVS_SMALLICON);
				break;

			case ID_VIEW_LARGEICON:
				bChecked = (dwStyle == LVS_ICON);
				break;

			case ID_VIEW_LIST:
				bChecked = (dwStyle == LVS_LIST);
				break;

			default:
				bChecked = FALSE;
				break;
			}

			pCmdUI->SetRadio(bChecked ? 1 : 0);
		}
	}
}


void CMainFrame::OnViewStyle(UINT nCommandID)
{
	// TODO : personnalisez ou étendez ce code en vue de la gestion des options dans le menu Affichage
	CFormViewBaseObject* pView = GetRightPane();

	// si le volet de droite a été créé et qu'il s'agit d'une vue CFormViewBaseObject,
	// les commandes de menu sont exécutées...
/* TODO: refaire
	if (pView != NULL)
	{
		DWORD dwStyle = -1;

		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
				// demander au contrôle List de s'aligner sur la grille
				CListCtrl& refListCtrl = pView->GetListCtrl();
				refListCtrl.Arrange(LVA_SNAPTOGRID);
			}
			break;

		// d'autres commandes changent le style du contrôle List
		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			break;

		case ID_VIEW_SMALLICON:
			dwStyle = LVS_SMALLICON;
			break;

		case ID_VIEW_LARGEICON:
			dwStyle = LVS_ICON;
			break;

		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}

		// changer le style et repeindre automatiquement la fenêtre
		if (dwStyle != -1)
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);
	}
	*/
}
BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// la classe de base effectue le travail

	if (!CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	CWinApp* pApp = AfxGetApp();
	if (pApp->m_pMainWnd == NULL)
		pApp->m_pMainWnd = this;

	// remplacer la commande Quitter du menu Fichier par la commande Fermer pour les fenêtres secondaires
	if (AfxGetApp()->m_pMainWnd != this)
	{
		CMenu *pMenu = GetMenu();
		ASSERT(pMenu);
		pMenu->ModifyMenu(ID_APP_EXIT, MF_BYCOMMAND | MF_STRING, ID_FILE_CLOSE, "&Fermer");
	}

	return TRUE;
}

void CMainFrame::OnFileClose()
{
   DestroyWindow();
}
/*
void CMainFrame::OnFileNew()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
	//CFrameWnd::onfile
	getsel
}
*/
void CMainFrame::OnUndo()
{

}
void CMainFrame::OnRedo()
{


}

