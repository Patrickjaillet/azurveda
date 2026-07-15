#pragma once
#include "afxext.h"

class XSplitterWndFixed :
	public CSplitterWnd
{
public:
	XSplitterWndFixed(void);
	virtual ~XSplitterWndFixed(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL DoScroll(CView* pViewFrom, UINT nScrollCode, BOOL bDoScroll = TRUE);
	virtual BOOL DoScrollBy(CView* pViewFrom, CSize sizeScroll,	BOOL bDoScroll = TRUE);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};
