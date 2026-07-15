// OpenGLWnd.cpp : implementation file
//

#include "stdafx.h"
//#include "Viewer.h"
#include "OpenGLWnd.h"

#include <gl/gl.h>
#include <gl/glu.h>

#include "VedaDemoOGLMfcGuiDoc.h"
#include "openglwnd.h"
#include "PreviewMachine.h"
#include "BaseObject.h"
#include "BaseContext.h"
#include "InterfacePrefObject.h"
#include "Dialog_AbstractPackSerializable.h"
#include ".\openglwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	TIMER_ID	50	

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd

IMPLEMENT_DYNCREATE(COpenGLWnd, CView)

COpenGLWnd::COpenGLWnd()
{	m_pLastDrawn = NULL;
	m_pPreviewMachine = NULL;
	m_LastDateDrawn = 0.0;
	m_initied = false;
	//m_RenderScene = DrawDocWire;	// Set the default draw function.
}

COpenGLWnd::~COpenGLWnd()
{
}

// Standard OpenGL Init Stuff

BOOL COpenGLWnd::SetupPixelFormat()
{
  
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd
		1,                                // version number
		PFD_DRAW_TO_WINDOW |              // support window
		PFD_SUPPORT_OPENGL |              // support OpenGL
		PFD_DOUBLEBUFFER,                 // double buffered
		PFD_TYPE_RGBA,                    // RGBA type
		32,                               // 24-bit color depth
		0, 0, 0, 0, 0, 0,                 // color bits ignored
		0,                                // no alpha buffer
		0,                                // shift bit ignored
		0,                                // no accumulation buffer
		0, 0, 0, 0,                       // accumulation bits ignored
		16,                               // 16-bit z-buffer
		0,                                // no stencil buffer
		0,                                // no auxiliary buffer
		PFD_MAIN_PLANE,                   // main layer
		0,                                // reserved
		0, 0, 0                           // layer masks ignored
	};

   int m_nPixelFormat = ::ChoosePixelFormat( m_pDC->GetSafeHdc(), &pfd );

    if ( m_nPixelFormat == 0 )
        return FALSE;

    return ::SetPixelFormat( m_pDC->GetSafeHdc(), m_nPixelFormat, &pfd );
}

BOOL COpenGLWnd::InitOpenGL()
{
	//Get a DC for the Client Area
	m_pDC = new CClientDC(this);

	//Failure to Get DC
	if( m_pDC == NULL )
		return FALSE;

	if( !SetupPixelFormat() )
		return FALSE;

    //Create Rendering Context
	m_hRC = ::wglCreateContext( m_pDC->GetSafeHdc() );

    //Failure to Create Rendering Context
    if( m_hRC == 0 )
        return FALSE;

	//Make the RC Current
	if( ::wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC ) == FALSE )
		return FALSE;
	

	return TRUE;
}

void COpenGLWnd::RenderScene()
{/* re
	if ( m_RenderScene != NULL )
		m_RenderScene( (CViewerDoc*)(GetDocument()) );
	else
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
*/
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

}

BEGIN_MESSAGE_MAP(COpenGLWnd, CView)
	//{{AFX_MSG_MAP(COpenGLWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd drawing

void COpenGLWnd::OnDraw(CDC* pDC)
{
	if( !m_initied ) return;
	SetContext();
	//? glClearColor(0.2f,0.05f,0.05f,1.0f); // test
	/*glDisable( GL_SCISSOR_TEST );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_SCISSOR_TEST );
	glClearColor(0.0f,0.0f,0.0f,0.0f); // test*/
	// draw the preview of the object selected:
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc ) return ;
	if( pdoc->IsCurrentlySerializing() ) return; // optimization.
	// this test would exit if the context is being edited, and update
	// be sent during the changes: in some cases, object coherences is not valid.
	// WARNING: it matches a RemoveLock...() before quitting:
	BaseContext *pContext = pdoc->GetVedaContext();

	/* no need to lock, this is the context's thread.
		 pPreviewObject->Create(false) reported to generate BaseContext::Notify()
		during creation,( in a merge)
	if( !(pContext->AddOneLockForThreadUse()))
	{	// AddOneLockForThreadUse() will fail if the object is currently changing its state.
		return;
	}*/
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(pdoc->GetVedaContext());
	VirtualMachine::InternalViewPort *pChildVP;
	VirtualMachine::InternalViewPort *pPositionVP;	
	m_pPreviewMachine->GetChildViewport(pChildVP,pPositionVP);
	if( ppref ) 
	{
		double CurrentTime = pdoc->GetPreviewCurrentTime();// ppref->GetPreviewCurrentTime();
		//BaseObject *pPreviewObject = pdoc->GetPreviewSelectedBaseObject();
		InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
		BaseObject *pPreviewObject = ppref->GetContextPreviewObject( 0,&pInterfacePreviewConfiguration);
		if( pPreviewObject )
		{	
			int creation= pPreviewObject->Create(false);
			if(creation >0) creation=pPreviewObject->Create(false); // try two times, objects are created faster.
			if(creation == -2)
			{
				pdoc->SetCurrentSoundObject(NULL);
				pChildVP->Clear(); // error: clear.
			}
			if(creation ==0) 
			{
				//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
				long long itime = (long long)( CurrentTime * 4294967296.0);
				pPreviewObject->GetMachine()->SetSoundCurrentTime( (int)(itime>>32),(unsigned int)itime);
				// note: the object's context machine is the same as m_pPreviewMachine.
				if(pChildVP) // always true ?
				{
					// reset child viewport according to clipping and aspect ratio if needed.
					//todo 
					float	borderwidth= 0.0f;// 0.05f;
					float	borderwidthEnd= 1.0f;//1.0f-0.01f;
					pPositionVP->SetClippingCoordinates(borderwidth,borderwidth,borderwidthEnd,borderwidthEnd);
					pPositionVP->SetScaleCoordinates(borderwidth,borderwidth,borderwidthEnd,borderwidthEnd);
					// apply previewconfiguration scale to viewport scale by default:
					float x1,y1,x2,y2;
					pInterfacePreviewConfiguration->GetViewportScale(x1,y1,x2,y2);
					pChildVP->SetScaleCoordinates(x1,y1,x2,y2);
					pChildVP->Clear();//0.5f,0.45f,0.5f);
					// draw preview for this object.
					pdoc->SetCurrentSoundObject(pPreviewObject);
					pPreviewObject->ProcessPreview( CurrentTime,pChildVP,pInterfacePreviewConfiguration );
				}
			}
		} 
		else
		{
			pdoc->SetCurrentSoundObject(NULL);
			pChildVP->Clear();
		}	
	} 
	else
	{	
		pdoc->SetCurrentSoundObject(NULL);
		pChildVP->Clear();
	}
	// watch AddOneLock...() upon
	/* no need to lock, this is the context's thread.
	pContext->RemoveOneLockForThreadUse();
	*/
	SwapGLBuffers();

}

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd diagnostics

#ifdef _DEBUG
void COpenGLWnd::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLWnd::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd message handlers

// Initialize OpenGL when window is created.
int COpenGLWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( !InitOpenGL() )
	{
		MessageBox( "Error setting up OpenGL!", "Init Error!",
			MB_OK | MB_ICONERROR );
		return -1;
	}

	return 0;
}

// Shutdown this view when window is destroyed.
void COpenGLWnd::OnDestroy()
{
	if(m_pPreviewMachine) m_pPreviewMachine->SetGLMFCView(NULL);
	wglMakeCurrent(0,0);
	wglDeleteContext(m_hRC);
	if( m_pDC )
	{
		delete m_pDC;
	}
	m_pDC = NULL;
	CView::OnDestroy();
}

// Override the errase background function to
// do nothing to prevent flashing.
BOOL COpenGLWnd::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}

// Set a few flags to make sure OpenGL only renders
// in its viewport.
BOOL COpenGLWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.lpszClass = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC,
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	return CView::PreCreateWindow(cs);
}

void COpenGLWnd::OnInitialUpdate()
{
	// get document machine linked with the veda context:
	m_pPreviewMachine = GetDocument()->GetPreviewMachine();
	m_pPreviewMachine->InitFromMFCView( this);

	CRect cr;
	GetClientRect( &cr );
	m_pPreviewMachine->OnSize( cr.Width(), cr.Height() );

	CView::OnInitialUpdate();

	this->SetContext();
	glClearColor(0.2f,0.05f,0.05f,1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	SetTimer( TIMER_ID,40, NULL); // 1000/n=freq
	m_initied = true;
}

void COpenGLWnd::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc) return;
	if( pdoc->IsCurrentlySerializing() ) return;
	if( pdoc->GetPreviewSelectedBaseObject() != m_pLastDrawn )
	{
		m_pLastDrawn = pdoc->GetPreviewSelectedBaseObject() ;
		OnDraw(NULL);
		return;
	}

	// manage veda updates:
	switch( pdoc->GetUpdateType() )
	{
		case BaseContext::eVedaUpdate_ObjectNew:
		{
		
		} break;
		case BaseContext::eVedaUpdate_ObjectDestroyed:
		{	
		}
		break;
		case BaseContext::eVedaUpdate_ObjectReCreated:
		{
			// not needed: occur stupid 2X recursions:
			//OnDraw(NULL);
			
		} break;
		case BaseContext::eVedaUpdate_ObjectClosed:
		{	
			// this update comes from a deletion... no redraw.
			// this is an optimisation.
			return;
		}
		break;
		case BaseContext::eVedaUpdate_MemberChanged:
		case BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo:
		{
			// ... OnDraw(NULL);
			//---------
		}
		break;
	}
	// else, update if date changed:
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(pdoc->GetVedaContext());
	if( ppref )
	{
		if( m_LastDateDrawn != pdoc->GetPreviewCurrentTime() )
		{
			m_LastDateDrawn = pdoc->GetPreviewCurrentTime();
			OnDraw(NULL);
			return;
		}
	}

}

void COpenGLWnd::OnTimer(UINT nIDEvent)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	CView::OnTimer(nIDEvent);
	switch( nIDEvent )
	{
		case TIMER_ID:
		{
			CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
			if( !pdoc) return;
			if( pdoc->IsCurrentlySerializing() ) break;
			OnDraw(NULL);
		
		} break;
		default: break;	
	}

}

void COpenGLWnd::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if(m_pPreviewMachine) m_pPreviewMachine->OnSize(cx,cy);
	SetContext();
	glClearColor(0.2f,0.05f,0.05f,1.0f); // test
	glDisable( GL_SCISSOR_TEST );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	
	glClearColor(0.2f,0.05f,0.05f,1.0f); // test
}

void COpenGLWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	int mouseID=0;
	if(nFlags & MK_LBUTTON)mouseID |=1;
	if(nFlags & MK_MBUTTON)mouseID |=2;
	if(nFlags & MK_RBUTTON)mouseID |=4;
	// set the viewport cursor:
	if((nFlags & MK_LBUTTON) ||(nFlags & MK_MBUTTON))
	{
		SetCursorPositionByMouseClick(point.x,point.y,mouseID,1);
	}
	CView::OnLButtonDown(nFlags, point);
}

void COpenGLWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	CView::OnLButtonUp(nFlags, point);
}

void COpenGLWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels
/*
MK_CONTROL   Set if the CTRL key is down. 
MK_LBUTTON   Set if the left mouse button is down. 
MK_MBUTTON   Set if the middle mouse button is down. 
MK_RBUTTON   Set if the right mouse button is down. 
MK_SHIFT   Set if the SHIFT key is down. 
*/

	int mouseID=0;
	if(nFlags & MK_LBUTTON)mouseID |=1;
	if(nFlags & MK_MBUTTON)mouseID |=2;
	if(nFlags & MK_RBUTTON)mouseID |=4;
	// set the viewport cursor:
	if((nFlags & MK_LBUTTON) ||(nFlags & MK_MBUTTON))
	{
		SetCursorPositionByMouseClick(point.x,point.y,mouseID,0);
	}

	CView::OnMouseMove(nFlags, point);
}
/*!
	\brief	change a viewport cursor position, according to mouse events.
			this call must be applied on a root viewport; then it is
			thrown down to all VP hierarchy,and activate all viewport which
			clip rectangle contains the mouse position.
	\param	_xPosition pixel position for this event.
	\param	_yPosition pixel position for this event.
	\param  _mouseButton index of the mouse button. 1=left,2=center,4=right...
	\param	_mouseButtonEvent 0=clickdown, 1=move when down,2=up.
*/	
void	COpenGLWnd::SetCursorPositionByMouseClick( 
			int _xPosition,
			int _yPosition, 
			unsigned int _mouseButton,
			unsigned int _mouseButtonEvent )
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if(!pdoc) return;

	CPreviewMachine::PreviewOGLInternalViewPort *pMainViewport =
		(CPreviewMachine::PreviewOGLInternalViewPort *)
		m_pPreviewMachine->GetDefaultViewPort();
	if(!pMainViewport) return;
	//pMainViewport->SetCursorPositionByMouseClick(_xPosition,_yPosition,_mouseButton,_mouseButtonEvent);
	float	xcoord,ycoord; // the pixel coord in the viewport ratio.
	CPreviewMachine::PreviewOGLInternalViewPort *pAffectedViewPort=
		pMainViewport->GetViewportAtPixel(_xPosition,_yPosition,xcoord,ycoord);
	if(!pAffectedViewPort) return; // no child viewport found.

	CVedaDemoOGLMfcGuiDoc::CActionFocusable *pFocused = pdoc->GetActionFocusObject();
	if( !pFocused ) return; // no actionFocus to get the clic.
	
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(pdoc->GetVedaContext());
	if( !ppref )  return;

	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration 
						*pInterfacePreviewConfiguration=NULL;
	BaseObject *pPreviewObject = ppref->GetContextPreviewObject( 0,&pInterfacePreviewConfiguration);
	if( !pPreviewObject ) return;

		// let the focused object do anything with this click:
		pFocused->ManageViewPortClickAction(pAffectedViewPort,
								pInterfacePreviewConfiguration,
								xcoord,ycoord,
								pdoc->GetPreviewCurrentTime(),
								_mouseButton,_mouseButtonEvent );

}
void COpenGLWnd::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels
	int mouseID=0;
	if(nFlags & MK_LBUTTON)mouseID |=1;
	if(nFlags & MK_MBUTTON)mouseID |=2;
	if(nFlags & MK_RBUTTON)mouseID |=4;
	// set the viewport cursor:
	if((nFlags & MK_LBUTTON) ||(nFlags & MK_MBUTTON))
	{
		SetCursorPositionByMouseClick(point.x,point.y,mouseID,2);
	}
	CView::OnMButtonDown(nFlags, point);
}
