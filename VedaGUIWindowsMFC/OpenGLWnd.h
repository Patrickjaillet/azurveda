#if !defined(AFX_OPENGLWND_H__83310F64_5D53_44AE_9D08_E29D6A961D6F__INCLUDED_)
#define AFX_OPENGLWND_H__83310F64_5D53_44AE_9D08_E29D6A961D6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CVedaDemoOGLMfcGuiDoc;
class CPreviewMachine;
class BaseObject;

class COpenGLWnd : public CView
{
protected:
	COpenGLWnd();
	DECLARE_DYNCREATE(COpenGLWnd)

public:
	void (*m_RenderScene) ( CVedaDemoOGLMfcGuiDoc* doc );

protected:
	HGLRC m_hRC;
	CDC* m_pDC;

public:

	BOOL SetupPixelFormat();
	BOOL InitOpenGL();

	void RenderScene();

	void SetContext() { wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC ); }
	void SwapGLBuffers() { 	SwapBuffers( m_pDC->GetSafeHdc() ); }

	protected:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual ~COpenGLWnd();

	CPreviewMachine	*m_pPreviewMachine;
	BaseObject		*m_pLastDrawn;
	float			m_LastDateDrawn;
	bool			m_initied;
	int	m_currentWidth;
	int	m_currentHeight;

void	SetCursorPositionByMouseClick(
			int _xPosition,
			int _yPosition,
			unsigned int _mouseButton,
			unsigned int _mouseButtonEvent );
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

inline CVedaDemoOGLMfcGuiDoc* GetDocument()
   { return reinterpret_cast<CVedaDemoOGLMfcGuiDoc*>(m_pDocument); }

protected:
	virtual void OnUpdate(CView* , LPARAM , CObject* );
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
};

#endif
