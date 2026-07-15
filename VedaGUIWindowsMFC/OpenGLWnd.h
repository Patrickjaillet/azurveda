#if !defined(AFX_OPENGLWND_H__83310F64_5D53_44AE_9D08_E29D6A961D6F__INCLUDED_)
#define AFX_OPENGLWND_H__83310F64_5D53_44AE_9D08_E29D6A961D6F__INCLUDED_



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenGLWnd.h : header file
//

//#include "RenderFunctions.h"
//#include "ViewerDoc.h"
class CVedaDemoOGLMfcGuiDoc;
class CPreviewMachine;
class BaseObject;
/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd view

// The base class for all our viewport views.
// Handles all the basic OpenGL init stuff for
// MFC. A similar method could be used for
// another API like Direct 3D.

class COpenGLWnd : public CView
{
protected:
	COpenGLWnd();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(COpenGLWnd)

// Attributes
public:
	void (*m_RenderScene) ( CVedaDemoOGLMfcGuiDoc* doc );	// void function pointer to the rendering
												// function. Used to change to easily
												// change what a viewport displays.
protected:
	HGLRC m_hRC; //Rendering Context
	CDC* m_pDC;  //Device Context

// Operations
public:
	// OpenGL init stuff
	BOOL SetupPixelFormat();
	BOOL InitOpenGL();

	// A couple of functions to allow outside
	// forces to manipulate the class.
	//void SetRenderFunc( void (*func) ( VedaDemoOGLMfcGuiDoc* ) ) { m_RenderScene = func; }
	void RenderScene();
	// Each viewport uses its own context
	// so we need to make sure the correct
	// context is set whenever we make an
	// OpenGL command.
	void SetContext() { wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC ); }
	void SwapGLBuffers() { 	SwapBuffers( m_pDC->GetSafeHdc() ); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenGLWnd)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~COpenGLWnd();

	//! the preview machine object linked to the object displayed:
	CPreviewMachine	*m_pPreviewMachine;
	BaseObject		*m_pLastDrawn;
	float			m_LastDateDrawn;
	bool			m_initied;
	int	m_currentWidth;
	int	m_currentHeight;
/*!
	\brief	change a viewport cursor position, according to mouse events.
			this call must be applied on a root viewport; then it is
			thrown down to all VP hierarchy,and activate all viewport which
			clip rectangle contains the mouse position.
	\param	_xPosition pixel position for this event.
	\param	_yPosition pixel position for this event.
	\param  _mouseButton index of the mouse button. 0=left,1=center,...
	\param	_mouseButtonEvent 0=clickdown, 1=move when down,2=up.
*/	
void	SetCursorPositionByMouseClick( 
			int _xPosition,
			int _yPosition, 
			unsigned int _mouseButton,
			unsigned int _mouseButtonEvent );
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(COpenGLWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

inline CVedaDemoOGLMfcGuiDoc* GetDocument()
   { return reinterpret_cast<CVedaDemoOGLMfcGuiDoc*>(m_pDocument); }

protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENGLWND_H__83310F64_5D53_44AE_9D08_E29D6A961D6F__INCLUDED_)
