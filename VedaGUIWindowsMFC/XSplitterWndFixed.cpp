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

void XSplitterWndFixed::OnMoving(UINT fwSide, LPRECT pRect)
{
//	CSplitterWnd::OnMoving(fwSide, pRect);

	// TODO : ajoutez ici le code de votre gestionnaire de messages
}

void XSplitterWndFixed::OnMove(int x, int y)
{
	CSplitterWnd::OnMove(x, y);

	// TODO : ajoutez ici le code de votre gestionnaire de messages
}

void XSplitterWndFixed::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	CSplitterWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL XSplitterWndFixed::DoScroll(CView* pViewFrom, UINT nScrollCode, BOOL bDoScroll)
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	return TRUE; //CSplitterWnd::DoScroll(pViewFrom, nScrollCode, bDoScroll);
}

BOOL XSplitterWndFixed::DoScrollBy(CView* pViewFrom, CSize sizeScroll,	BOOL bDoScroll)
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	return TRUE; // CSplitterWnd::DoScrollBy(pViewFrom, sizeScroll, bDoScroll);
}

BOOL XSplitterWndFixed::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	return TRUE; //CSplitterWnd::OnNotify(wParam, lParam, pResult);
}

void XSplitterWndFixed::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	CWnd::OnLButtonDown(nFlags, point);
}

void XSplitterWndFixed::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	CWnd::OnMouseMove(nFlags, point);
}

BOOL XSplitterWndFixed::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL XSplitterWndFixed::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	return CSplitterWnd::PreCreateWindow(cs);
}
