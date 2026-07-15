// ChildFrm.cpp : implémentation de la classe CChildFrame
//
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "OpenGLWnd.h"
#include "ChildFrm.h"
#include "LeftView.h"
#include "FormViewBaseObject.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "PreviewView.h"
#include ".\childfrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "PreviewManagerView.h"

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_WM_MOVE()
	ON_WM_HSCROLL()
	ON_COMMAND(ID_EXPORTCFILE, OnExportcfile)
	ON_COMMAND(ID_EXPORTEXECUTABLE, OnExportexecutable)

	ON_COMMAND(ID_UNDO, OnUndo)
	ON_COMMAND(ID_REDO, OnRedo)
	ON_COMMAND(ID_MERGE, OnMerge)
	ON_COMMAND(ID_FLUSHNAMES, OnFlushNames)

	ON_COMMAND(ID_FILE_LIBIMAGE, OnAddLibImage)
	ON_COMMAND(ID_FILE_LIBBAOOM, OnAddLibBAOOM)

	ON_COMMAND(ID_FORCELANGUAGE_ENGLISH, OnForceLang_English)	
	ON_COMMAND(ID_FORCEPREVIEWLANGUAGE_FRENCH, OnForceLang_French)	
	ON_COMMAND(ID_FORCEPREVIEWLANGUAGE_SPANISH, OnForceLang_Spanish)	
	ON_COMMAND(ID_FORCEPREVIEWLANGUAGE_GERMAN, OnForceLang_German)	
	ON_COMMAND(ID_FORCEPREVIEWLANGUAGE_ITALIAN, OnForceLang_Italian)	
	ON_COMMAND(ID_FORCEPREVIEWLANGUAGE_POLISH, OnForceLang_Polish)	
	ON_COMMAND(ID_FORCEPREVIEWLANGUAGE_PORTU, OnForceLang_Portuguese)	
	
	//	ON_COMMAND(ID_FILE_LIBDEMO, OnAddLibDemo)

END_MESSAGE_MAP()


// construction ou destruction de CChildFrame

CChildFrame::CChildFrame()
{
	// TODO : ajoutez ici le code d'une initialisation de membre
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	// créer une fenêtre fractionnée
	if (!m_wndSplitter.CreateStatic( this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(200, 100), pContext) 
		)
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}
	m_wndSplitter.EnableToolTips(TRUE);   // enable tool tips for view


		// add the second splitter pane - which is a nested splitter with 2 rows
	if (!m_wndSplitter2.CreateStatic(
		&m_wndSplitter,     // our parent window is the first splitter
		2, 1,               // the new splitter is 2 rows, 1 column
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndSplitter.IdFromRowCol(0, 1)
			// new splitter is in the first row, 2nd column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}
	// ------ create the  right up object edition view:
	if (!m_wndSplitter2.CreateView(0, 0,
		RUNTIME_CLASS(CFormViewBaseObject), CSize(600, 180), pContext)) // 600,200
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	// ----- split again horizontaly for preview magneto:
	if (!m_wndSplitter3.CreateStatic(
		&m_wndSplitter2,     // our parent window is the first splitter
		2, 1,               // the new splitter is 2 rows, 1 column
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndSplitter2.IdFromRowCol(1, 0)
			// new splitter is in the first row, 2nd column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}
	if (!m_wndSplitter3.CreateView(0, 0,
		//RUNTIME_CLASS(CPreviewView), CSize(600, 400), pContext))
		RUNTIME_CLASS(CPreviewManagerView), CSize(560, 43), pContext))		
	{
		TRACE0("Failed to create third pane\n");
		return FALSE;
	}
	if (!m_wndSplitter3.CreateView(1, 0,
		//RUNTIME_CLASS(CPreviewView), CSize(600, 400), pContext))
		RUNTIME_CLASS(COpenGLWnd), CSize(600, 400), pContext))		
	{
		TRACE0("Failed to create third pane\n");
		return FALSE;
	}
	// || !m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CFormViewBaseObject), CSize(100, 100), pContext) 
	/*	||		
		!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CLeftView), CSize(200, 100), pContext) ||
		!m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(CFormViewBaseObject), CSize(100, 100), pContext)
	*/	

	// CWnd* GetPane(int row, int col) const;
/*
	if (!m_wndSplitter2.CreateStatic( m_wndSplitter.GetPane(0,1) , 2, 1))
		return FALSE;

	if (!m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(200, 100), pContext) ||
		!m_wndSplitter2.CreateView(1, 0, RUNTIME_CLASS(CFormViewBaseObject), CSize(100, 100), pContext) 
		*/
	/*	||		
		!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CLeftView), CSize(200, 100), pContext) ||
		!m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(CFormViewBaseObject), CSize(100, 100), pContext)
	
		)
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}
*/

	return TRUE;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO : Changez ici la classe ou les styles Window en modifiant CREATESTRUCT cs
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	return TRUE;
}


// diagnostics pour CChildFrame

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


// gestionnaires de messages pour CChildFrame


void CChildFrame::OnMove(int x, int y)
{
	if(true) CMDIChildWnd::OnMove(x, y);

	// TODO : ajoutez ici le code de votre gestionnaire de messages
}

void CChildFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	if(true) 	CMDIChildWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CChildFrame::OnExportcfile()
{
	
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnExportcfile();

}

void CChildFrame::OnExportexecutable()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnExportExecutable();

}
void CChildFrame::OnUndo()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnUndo(); 
}
void CChildFrame::OnRedo()
{	
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnRedo(); 
}	
void CChildFrame::OnMerge()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnMerge(); 
}
void CChildFrame::OnFlushNames()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnFlushNames(); 
}
void CChildFrame::OnAddLibImage()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnAddLibImage(); 
}
void CChildFrame::OnAddLibBAOOM()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnAddLibBAOOM(); 
}
void CChildFrame::OnAddLibDemo()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
//	pdoc->OnAddLibDemo(); 
}
void CChildFrame::OnForceLang_English()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnForceLang(1); 
}

void CChildFrame::OnForceLang_French()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnForceLang(2); 
}
void CChildFrame::OnForceLang_Spanish()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnForceLang(3); 
}
void CChildFrame::OnForceLang_German()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnForceLang(4);
}
void CChildFrame::OnForceLang_Italian()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnForceLang(5); 
}
void CChildFrame::OnForceLang_Polish()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnForceLang(6); 
}
void CChildFrame::OnForceLang_Portuguese()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetActiveDocument();
	if(!pdoc) return;
	pdoc->OnForceLang(7);
}

