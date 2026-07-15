
#include "stdafx.h"

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

IMPLEMENT_DYNCREATE(COpenGLWnd, CView)

COpenGLWnd::COpenGLWnd()
{	m_pLastDrawn = NULL;
	m_pPreviewMachine = NULL;
	m_LastDateDrawn = 0.0;
	m_initied = false;

}

COpenGLWnd::~COpenGLWnd()
{
}

BOOL COpenGLWnd::SetupPixelFormat()
{

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

   int m_nPixelFormat = ::ChoosePixelFormat( m_pDC->GetSafeHdc(), &pfd );

    if ( m_nPixelFormat == 0 )
        return FALSE;

    return ::SetPixelFormat( m_pDC->GetSafeHdc(), m_nPixelFormat, &pfd );
}

BOOL COpenGLWnd::InitOpenGL()
{

	m_pDC = new CClientDC(this);

	if( m_pDC == NULL )
		return FALSE;

	if( !SetupPixelFormat() )
		return FALSE;

	m_hRC = ::wglCreateContext( m_pDC->GetSafeHdc() );

    if( m_hRC == 0 )
        return FALSE;

	if( ::wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC ) == FALSE )
		return FALSE;

	return TRUE;
}

void COpenGLWnd::RenderScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

}

BEGIN_MESSAGE_MAP(COpenGLWnd, CView)

	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()

	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
END_MESSAGE_MAP()

void COpenGLWnd::OnDraw(CDC* pDC)
{
	if( !m_initied ) return;
	SetContext();

	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc ) return ;
	if( pdoc->IsCurrentlySerializing() ) return;

	BaseContext *pContext = pdoc->GetVedaContext();

	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(pdoc->GetVedaContext());
	VirtualMachine::InternalViewPort *pChildVP;
	VirtualMachine::InternalViewPort *pPositionVP;
	m_pPreviewMachine->GetChildViewport(pChildVP,pPositionVP);
	if( ppref )
	{
		double CurrentTime = pdoc->GetPreviewCurrentTime();

		InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
		BaseObject *pPreviewObject = ppref->GetContextPreviewObject( 0,&pInterfacePreviewConfiguration);
		if( pPreviewObject )
		{
			int creation= pPreviewObject->Create(false);
			if(creation >0) creation=pPreviewObject->Create(false);
			if(creation == -2)
			{
				pdoc->SetCurrentSoundObject(NULL);
				pChildVP->Clear();
			}
			if(creation ==0)
			{

				long long itime = (long long)( CurrentTime * 4294967296.0);
				pPreviewObject->GetMachine()->SetSoundCurrentTime( (int)(itime>>32),(unsigned int)itime);

				if(pChildVP)
				{

					float	borderwidth= 0.0f;
					float	borderwidthEnd= 1.0f;
					pPositionVP->SetClippingCoordinates(borderwidth,borderwidth,borderwidthEnd,borderwidthEnd);
					pPositionVP->SetScaleCoordinates(borderwidth,borderwidth,borderwidthEnd,borderwidthEnd);

					float x1,y1,x2,y2;
					pInterfacePreviewConfiguration->GetViewportScale(x1,y1,x2,y2);
					pChildVP->SetScaleCoordinates(x1,y1,x2,y2);
					pChildVP->Clear();

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

	SwapGLBuffers();

}

#ifdef _DEBUG
void COpenGLWnd::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLWnd::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif

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

BOOL COpenGLWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL COpenGLWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC,
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

void COpenGLWnd::OnInitialUpdate()
{

	m_pPreviewMachine = GetDocument()->GetPreviewMachine();
	m_pPreviewMachine->InitFromMFCView( this);

	CRect cr;
	GetClientRect( &cr );
	m_pPreviewMachine->OnSize( cr.Width(), cr.Height() );

	CView::OnInitialUpdate();

	this->SetContext();
	glClearColor(0.2f,0.05f,0.05f,1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	SetTimer( TIMER_ID,40, NULL);
	m_initied = true;
}

void COpenGLWnd::OnUpdate(CView* , LPARAM , CObject* )
{

	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc) return;
	if( pdoc->IsCurrentlySerializing() ) return;
	if( pdoc->GetPreviewSelectedBaseObject() != m_pLastDrawn )
	{
		m_pLastDrawn = pdoc->GetPreviewSelectedBaseObject() ;
		OnDraw(NULL);
		return;
	}

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

		} break;
		case BaseContext::eVedaUpdate_ObjectClosed:
		{

			return;
		}
		break;
		case BaseContext::eVedaUpdate_MemberChanged:
		case BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo:
		{

		}
		break;
	}

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

void COpenGLWnd::OnTimer(UINT_PTR nIDEvent)
{

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
	glClearColor(0.2f,0.05f,0.05f,1.0f);
	glDisable( GL_SCISSOR_TEST );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor(0.2f,0.05f,0.05f,1.0f);
}

void COpenGLWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	int mouseID=0;
	if(nFlags & MK_LBUTTON)mouseID |=1;
	if(nFlags & MK_MBUTTON)mouseID |=2;
	if(nFlags & MK_RBUTTON)mouseID |=4;

	if((nFlags & MK_LBUTTON) ||(nFlags & MK_MBUTTON))
	{
		SetCursorPositionByMouseClick(point.x,point.y,mouseID,1);
	}
	CView::OnLButtonDown(nFlags, point);
}

void COpenGLWnd::OnLButtonUp(UINT nFlags, CPoint point)
{

	CView::OnLButtonUp(nFlags, point);
}

void COpenGLWnd::OnMouseMove(UINT nFlags, CPoint point)
{

	int mouseID=0;
	if(nFlags & MK_LBUTTON)mouseID |=1;
	if(nFlags & MK_MBUTTON)mouseID |=2;
	if(nFlags & MK_RBUTTON)mouseID |=4;

	if((nFlags & MK_LBUTTON) ||(nFlags & MK_MBUTTON))
	{
		SetCursorPositionByMouseClick(point.x,point.y,mouseID,0);
	}

	CView::OnMouseMove(nFlags, point);
}

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

	float	xcoord,ycoord;
	CPreviewMachine::PreviewOGLInternalViewPort *pAffectedViewPort=
		pMainViewport->GetViewportAtPixel(_xPosition,_yPosition,xcoord,ycoord);
	if(!pAffectedViewPort) return;

	CVedaDemoOGLMfcGuiDoc::CActionFocusable *pFocused = pdoc->GetActionFocusObject();
	if( !pFocused ) return;

	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(pdoc->GetVedaContext());
	if( !ppref )  return;

	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration
						*pInterfacePreviewConfiguration=NULL;
	BaseObject *pPreviewObject = ppref->GetContextPreviewObject( 0,&pInterfacePreviewConfiguration);
	if( !pPreviewObject ) return;

		pFocused->ManageViewPortClickAction(pAffectedViewPort,
								pInterfacePreviewConfiguration,
								xcoord,ycoord,
								pdoc->GetPreviewCurrentTime(),
								_mouseButton,_mouseButtonEvent );

}
void COpenGLWnd::OnMButtonDown(UINT nFlags, CPoint point)
{

	int mouseID=0;
	if(nFlags & MK_LBUTTON)mouseID |=1;
	if(nFlags & MK_MBUTTON)mouseID |=2;
	if(nFlags & MK_RBUTTON)mouseID |=4;

	if((nFlags & MK_LBUTTON) ||(nFlags & MK_MBUTTON))
	{
		SetCursorPositionByMouseClick(point.x,point.y,mouseID,2);
	}
	CView::OnMButtonDown(nFlags, point);
}
