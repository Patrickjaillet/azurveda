
#pragma once
#include ".\xsplitterwndfixed.h"
class CFormViewBaseObject;

class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

protected:
	CSplitterWnd m_wndSplitter;
	CSplitterWnd m_wndSplitter2;
	XSplitterWndFixed m_wndSplitter3;
public:

public:

	public:
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CFormViewBaseObject* GetRightPane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMove(int x, int y);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnExportcfile();
	afx_msg void OnExportexecutable();

	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnMerge();
	afx_msg void OnFlushNames();
	afx_msg void OnAddLibImage();
	afx_msg void OnAddLibBAOOM();
	afx_msg void OnAddLibDemo();

	afx_msg void OnForceLang_English();
	afx_msg void OnForceLang_French();
	afx_msg void OnForceLang_Spanish();
	afx_msg void OnForceLang_German();
	afx_msg void OnForceLang_Italian();
	afx_msg void OnForceLang_Polish();
	afx_msg void OnForceLang_Portuguese();

};
