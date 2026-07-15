#include "StdAfx.h"
#include ".\xsplitterwndfixed.h"

XSplitterWndFixed::XSplitterWndFixed(void)
{
}

XSplitterWndFixed::~XSplitterWndFixed(void)
{
}
BEGIN_MESSAGE_MAP(XSplitterWndFixed, CSplitterWnd)
	ON_WM_MOVING()
	ON_WM_MOVE()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void XSplitterWndFixed::OnMoving(UINT , LPRECT )
{

}

void XSplitterWndFixed::OnMove(int x, int y)
{
	CSplitterWnd::OnMove(x, y);

}

void XSplitterWndFixed::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

	CSplitterWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL XSplitterWndFixed::DoScroll(CView* , UINT , BOOL )
{

	return TRUE;
}

BOOL XSplitterWndFixed::DoScrollBy(CView* , CSize ,	BOOL )
{

	return TRUE;
}

BOOL XSplitterWndFixed::OnNotify(WPARAM , LPARAM , LRESULT* )
{

	return TRUE;
}

void XSplitterWndFixed::OnLButtonDown(UINT nFlags, CPoint point)
{

	CWnd::OnLButtonDown(nFlags, point);
}

void XSplitterWndFixed::OnMouseMove(UINT nFlags, CPoint point)
{

	CWnd::OnMouseMove(nFlags, point);
}

BOOL XSplitterWndFixed::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL XSplitterWndFixed::PreCreateWindow(CREATESTRUCT& cs)
{

	return CSplitterWnd::PreCreateWindow(cs);
}
