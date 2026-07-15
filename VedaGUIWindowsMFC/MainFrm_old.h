// MainFrm.h : interface de la classe CMainFrame
//


#pragma once

class CVedaDemoOGLMfcGuiView;
class CFormViewBaseObject;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
// Attributs
protected:
	CSplitterWnd m_wndSplitter;
public:

// Opérations
public:

// Substitutions
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implémentation
public:
	virtual ~CMainFrame();
	CFormViewBaseObject* GetRightPane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // membres incorporés de la barre de contrôle
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	CDialogBar      m_wndDlgBar;

// Fonctions générées de la table des messages
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileClose();
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnFileNew();
};


