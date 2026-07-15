// PreviewManagerView.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "PreviewManagerView.h"
#include ".\previewmanagerview.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "BaseContext.h"
#include "InterfacePrefObject.h"
#include "PreviewMachine.h"
// CPreviewManagerView

IMPLEMENT_DYNCREATE(CPreviewManagerView, CFormView)

#define	SLIDERTIME_NBTICK	300

#define TIMER_ID	50

CPreviewManagerView::CPreviewManagerView()
	: CFormView(CPreviewManagerView::IDD)
	, m_isInited(false)
	, m_pPref(0L)
{
	m_focusMove.m_pManager = this;
	m_focusRotate.m_pManager = this;
	m_focusVPMove.m_pManager = this;
	m_focusVPReset.m_pManager = this;

}

CPreviewManagerView::~CPreviewManagerView()
{
}

void CPreviewManagerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OBJECTNAME, m_ObjectName);
	DDX_Control(pDX, IDC_MINSEC, m_EditStartTime);
	DDX_Control(pDX, IDC_MAXSEC, m_EditEndTime);
	DDX_Control(pDX, IDC_BPLAYPAUSE, m_ButtonPlay);
	DDX_Control(pDX, IDC_BRETURNSTOP, m_ButtonRewind);
	DDX_Control(pDX, IDC_DATE, m_EditDate);
	DDX_Control(pDX, IDC_Speed, m_SpeedStatic);
	DDX_Control(pDX, IDC_SLIDERTIME, m_SliderTime);
	DDX_Control(pDX, IDC_SLIDERSPEED, m_SliderSpeed);
	DDX_Control(pDX, IDC_SLIDERSOUND, m_SliderVolume);
	DDX_Control(pDX, IDC_PROGRESSCONSTRUCT, m_Progressbar);

	DDX_Control(pDX, IDC_BMOVE, m_butonMove);
	DDX_Control(pDX, IDC_BROTATE, m_butonRotate);
	DDX_Control(pDX, IDC_BZOOM, m_butonVPMove);
	DDX_Control(pDX, IDC_BRESETLAYOUT, m_butonVPReset);
}

BEGIN_MESSAGE_MAP(CPreviewManagerView, CFormView)
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_MINSEC, OnEnKillfocusMinsec)
	ON_EN_KILLFOCUS(IDC_MAXSEC, OnEnKillfocusMaxsec)
	ON_STN_CLICKED(IDC_Speed, OnStnClickedSpeed)
	ON_STN_CLICKED(IDC_BPLAYPAUSE, OnStnClickedBplaypause)
	ON_WM_TIMER()
	ON_STN_CLICKED(IDC_BRETURNSTOP, OnStnClickedBreturnstop)

	ON_STN_CLICKED(IDC_BMOVE, OnStnClickedBMove)
	ON_STN_CLICKED(IDC_BROTATE, OnStnClickedBRotate)
	ON_STN_CLICKED(IDC_BZOOM, OnStnClickedBVPMove)
	ON_STN_CLICKED(IDC_BRESETLAYOUT, OnStnClickedBVPReset)
	ON_WM_KEYDOWN()
	ON_WM_SYSDEADCHAR()
END_MESSAGE_MAP()

// Diagnostics CPreviewManagerView

#ifdef _DEBUG
void CPreviewManagerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPreviewManagerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// Gestionnaires de messages CPreviewManagerView

void CPreviewManagerView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	if( !m_isInited ) return;
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc) return;
	if( pdoc->IsCurrentlySerializing() ) return;

	BaseObject *pObjecUpdated = pdoc->GetObjectUpdated();
	BaseContext *pContext = pdoc->GetVedaContext();

	// 
	if( pdoc->IsActiveDoc() && pdoc->GetPreviewSelectedBaseObject() != NULL )
	{
		CString str,str2;
		//pdoc->GetDocTemplate()->GetDocString(str, CDocTemplate::windowTitle );
		str2.Format(" - UVeda - wwww.m4nkind.com - Beta: %s : %s",
			pdoc->GetPreviewSelectedBaseObject()->GetName(),
			pdoc->GetPreviewSelectedBaseObject()->GetObjectInfoLine()
			);
		AfxGetApp()->GetMainWnd()->SetWindowText( str2.GetString() );
	}
	/*else
	{
		m_ObjectName.SetWindowText("-");
	}*/
	// manage veda updates:
	switch( pdoc->GetUpdateType() )
	{
		case BaseContext::eVedaUpdate_ObjectNew:
		{
			if( pObjecUpdated->GetClassDescription().m_NewMethod == InterfacePrefObject::m_Description.m_NewMethod )
			{
				InterfacePrefObject *pPref = (InterfacePrefObject *)pObjecUpdated ;
				if( pPref->IsActive() ) UpdateFromPreferenceObject(pPref);			
			}
		} break;
		case BaseContext::eVedaUpdate_ObjectDestroyed:
		{		
			//BaseObject *pobj = pdoc->GetPreviewSelectedBaseObject();
		}
		break;
		case BaseContext::eVedaUpdate_ObjectReCreated:
		case BaseContext::eVedaUpdate_ObjectClosed:
		{	
		}
		break;
		case BaseContext::eVedaUpdate_MemberChanged:
		case BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo:
		{
			if( pObjecUpdated->GetClassDescription().m_NewMethod == InterfacePrefObject::m_Description.m_NewMethod )
			{
				InterfacePrefObject *pPref = (InterfacePrefObject *)pObjecUpdated ;
				if( pPref->IsActive() ) UpdateFromPreferenceObject(pPref);			
			}
		}
		break;
	}

	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(pContext);
	if(m_pPref != ppref)
	{
		m_pPref = ppref;
	}
	if(m_pPref) UpdateFromPreferenceObject(m_pPref);
	// update action buttons:

	if(  ! m_focusMove.IsActionFocusable() )
		m_butonMove.SetBitmap( (HBITMAP)m_Bm_Move.m_hObject );
	else
		m_butonMove.SetBitmap( (HBITMAP)m_Bm_MoveFocus.m_hObject );

	if( ! m_focusRotate.IsActionFocusable() )
		m_butonRotate.SetBitmap( (HBITMAP)m_Bm_Rotate.m_hObject );
	else
		m_butonRotate.SetBitmap( (HBITMAP)m_Bm_RotateFocus.m_hObject );

	if( ! m_focusVPMove.IsActionFocusable() )
		m_butonVPMove.SetBitmap( (HBITMAP)m_Bm_Zoom.m_hObject );
	else
		m_butonVPMove.SetBitmap( (HBITMAP)m_Bm_ZoomFocus.m_hObject );

	if( ! m_focusVPReset.IsActionFocusable() )
		m_butonVPReset.SetBitmap( (HBITMAP)m_Bm_VPReset.m_hObject );
	else
		m_butonVPReset.SetBitmap( (HBITMAP)m_Bm_VPResetFocus.m_hObject );

}
void	CPreviewManagerView::UpdateFromPreferenceObject(InterfacePrefObject *ppref)
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	BaseObject *pobj = pdoc->GetPreviewSelectedBaseObject();

	if(pobj )
	{
		//m_ObjectName.SetWindowText(pobj->GetName() );
		float creationprogress = pobj->GetCreationRate();
		if( creationprogress != 0.0f )
		{
			m_ObjectName.ShowWindow(SW_HIDE);
			m_Progressbar.ShowWindow(SW_SHOW);
			m_Progressbar.SetPos( (int)( 100- (creationprogress * 100.0f)) );
			//m_Progressbar.SetWindowText("tt");
		} else
		{
			m_ObjectName.ShowWindow(SW_SHOW);
			m_Progressbar.ShowWindow(SW_HIDE);
			m_ObjectName.SetWindowText(pobj->GetName() );
		}
	}
	else
	{	// CProgressCtrl
		m_Progressbar.SetPos( 0);
		m_ObjectName.SetWindowText("-");	
	}

		
	// ----------
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	BaseObject *pPreviewObject = ppref->GetContextPreviewObject( 0,&pInterfacePreviewConfiguration);
	float	playspeed = 1.0;
	float	timeend = 360.0;
	float	timestart = 1.0;
	if(pInterfacePreviewConfiguration)
	{
		playspeed = pInterfacePreviewConfiguration->GetPlaySpeed();
		timeend = pInterfacePreviewConfiguration->GetPreviewEndTime();
		timestart = pInterfacePreviewConfiguration->GetPreviewStartTime();
	}
	// ----------
	CString	str;
	str.Format("%d",(int)(timestart) );
	m_EditStartTime.SetWindowText(str.GetString());
	str.Format("%d",(int)(timeend) );
	m_EditEndTime.SetWindowText(str.GetString());
	str.Format("X:%.02f",playspeed );
	m_SpeedStatic.SetWindowText(str.GetString());
	int currdate = (int)pdoc->GetPreviewCurrentTime();
	int hh = currdate/3600;
	currdate -= (hh*3600);
	int mm = currdate/60;
	currdate -= (mm*60);
	str.Format("%02d:%02d:%02d",hh,mm,currdate );
	m_EditDate.SetWindowText( str.GetString() );

	double rate = ( pdoc->GetPreviewCurrentTime() - timestart)
				/( timeend - timestart ) ;
	m_SliderTime.SetPos( (int)(rate * SLIDERTIME_NBTICK) );

	m_SliderSpeed.SetPos( (int) (playspeed*2.0) );

	if( pdoc->GetPreviewPlayState() )
		m_ButtonPlay.SetBitmap( (HBITMAP)m_Bm_Pause.m_hObject );
	else
		m_ButtonPlay.SetBitmap( (HBITMAP)m_Bm_Play.m_hObject );
}

void CPreviewManagerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
		m_Progressbar.SetRange(0,100);
//	m_ObjectName.SetWindowText(" " );

	m_SliderSpeed.SetRange(-16,15 );

	m_SliderTime.SetRange(0, SLIDERTIME_NBTICK,TRUE );

	m_Bm_Play.LoadBitmap( IDC_B_PLAY );
	m_Bm_Pause.LoadBitmap( IDC_B_PAUSE );

	m_Bm_Move.LoadBitmap( IDC_B_MOVEPREVIEW );
	m_Bm_MoveFocus.LoadBitmap( IDC_B_MOVEPREVIEWFOCUS );

	m_Bm_Rotate.LoadBitmap( IDC_B_ROTATEPREVIEW );
	m_Bm_RotateFocus.LoadBitmap( IDC_B_ROTATEPREVIEWFOCUS );

	m_Bm_Zoom.LoadBitmap( IDC_B_VPZOOMIN );
	m_Bm_ZoomFocus.LoadBitmap( IDC_B_VPZOOMINFOCUS );

	m_Bm_VPReset.LoadBitmap( IDC_B_VPRESET );
	m_Bm_VPResetFocus.LoadBitmap( IDC_B_VPRESETFOCUS );

/*
	CBitmap	m_Bm_Move;
	CBitmap	m_Bm_MoveFocus;
	CBitmap	m_Bm_Rotate;
	CBitmap	m_Bm_RotateFocus;
	CBitmap	m_Bm_Zoom;
	CBitmap	m_Bm_ZoomFocus;
	CBitmap	m_Bm_VPReset;
	CBitmap	m_Bm_VPResetFocus;
*/
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( pdoc ) 
	{
		// initial focus.
		pdoc->SetDefaultActionFocusObject( &m_focusMove );
		pdoc->SetActionFocusObject( &m_focusMove );
	}
	//pdoc->PreviewZoomIn();
	


	SetTimer( TIMER_ID,500, NULL);
	m_isInited = true;
}

void CPreviewManagerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc ) return ;
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(pdoc->GetVedaContext());
	if( !ppref ) return;
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	BaseObject *pPreviewObject = ppref->GetContextPreviewObject( 0,&pInterfacePreviewConfiguration);
	if(!pInterfacePreviewConfiguration) return;

	if(pScrollBar)
	{
		int id = pScrollBar->GetDlgCtrlID();
		switch(id)
		{
			case	IDC_SLIDERTIME:
			{	
				//m_SliderTime.SetRange((int)(ppref->GetPreviewStartTime()),(int)(ppref->GetPreviewEndTime()),TRUE );

				double rate = ((double) m_SliderTime.GetPos())/ SLIDERTIME_NBTICK ;
				pdoc->SetPreviewCurrentTime(
					pInterfacePreviewConfiguration->GetPreviewStartTime() +
					( pInterfacePreviewConfiguration->GetPreviewEndTime()-pInterfacePreviewConfiguration->GetPreviewStartTime() )*rate
											);
			} break;
			case	IDC_SLIDERSPEED:
			{	
				int pos = m_SliderSpeed.GetPos();
				pdoc->SetPreviewCurrentTime( pdoc->GetPreviewCurrentTime() );
				pInterfacePreviewConfiguration->SetPlaySpeed( ((float)pos * 0.5f) );
			} break;
			default:
				break;
		}
	}

}
void CPreviewManagerView::OnEnKillfocusMinsec()
{

	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc ) return ;
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(pdoc->GetVedaContext());
	if( !ppref ) return;
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	BaseObject *pPreviewObject = ppref->GetContextPreviewObject( 0,&pInterfacePreviewConfiguration);
	if(!pInterfacePreviewConfiguration) return;

	CString	str;
	m_EditStartTime.GetWindowText(str);
	pInterfacePreviewConfiguration->SetPreviewStartTime((float) atof(str.GetString()) );

}
void CPreviewManagerView::OnEnKillfocusMaxsec()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc ) return ;
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(pdoc->GetVedaContext());
	if( !ppref ) return;
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	BaseObject *pPreviewObject = ppref->GetContextPreviewObject( 0,&pInterfacePreviewConfiguration);
	if(!pInterfacePreviewConfiguration) return;

	CString	str;
	m_EditEndTime.GetWindowText(str);
	pInterfacePreviewConfiguration->SetPreviewEndTime( (float) atof(str.GetString()) );

}

void CPreviewManagerView::OnStnClickedSpeed()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void CPreviewManagerView::OnStnClickedBplaypause()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc ) return ;
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(pdoc->GetVedaContext());
	if( !ppref ) return ;
	// switch:
	pdoc->SetPreviewPlayState( !pdoc->GetPreviewPlayState() );
	UpdateFromPreferenceObject(ppref);
}

void CPreviewManagerView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels
	
	CFormView::OnTimer(nIDEvent);
	//CView::OnTimer(nIDEvent);
	switch( nIDEvent )
	{
		case TIMER_ID:
		{
			CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
			if( !pdoc ) return ;
			if( pdoc->IsCurrentlySerializing() ) break;
			if( pdoc->GetPreviewPlayState() )	
				OnUpdate(NULL,NULL,NULL);
		
		} break;
		default: break;	
	}

}

void CPreviewManagerView::OnStnClickedBreturnstop()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc ) return ;
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(pdoc->GetVedaContext());
	if( !ppref ) return;
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	BaseObject *pPreviewObject = ppref->GetContextPreviewObject( 0,&pInterfacePreviewConfiguration);
	if(!pInterfacePreviewConfiguration) return;

	//pdoc->SetPreviewCurrentTime( ppref->GetPreviewStartTime() ); // 1 time before to reset update.
	pdoc->SetPreviewPlayState( false );
	pdoc->SetPreviewCurrentTime( pInterfacePreviewConfiguration->GetPreviewStartTime() );
	UpdateFromPreferenceObject(ppref);
}
void CPreviewManagerView::OnStnClickedBMove()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc ) return ;
	//pdoc->PreviewZoomIn();
	pdoc->SetActionFocusObject( &m_focusMove );

}

void CPreviewManagerView::OnStnClickedBRotate()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc ) return ;
	//pdoc->PreviewZoomOut();
	pdoc->SetActionFocusObject( &m_focusRotate );

}

void CPreviewManagerView::OnStnClickedBVPMove()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc ) return ;
	//pdoc->PreviewScroll();
	pdoc->SetActionFocusObject( &m_focusVPMove );
}
void CPreviewManagerView::OnStnClickedBVPReset()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc ) return ;

	//pdoc->PreviewScroll();
	pdoc->SetActionFocusObject( &m_focusVPReset );

}
/*
	CBitmap	m_Bm_SetNormal;
	CBitmap	m_Bm_SetNormalFocus;
	CBitmap	m_Bm_ZoomIn;
	CBitmap	m_Bm_ZoomInFocus;
	CBitmap	m_Bm_ZoomOut;
	CBitmap	m_Bm_ZoomOutFocus;
	CBitmap	m_Bm_Scroll;
	CBitmap	m_Bm_ScrollFocus;
*/
/*
	\brief	sent when the object become the actionfocused one.	
*/
void	CPreviewManagerView::CActionFocusable_Main::NotifySetActionFocusable(bool _isActionFocusable)
{
	//super:
	CVedaDemoOGLMfcGuiDoc::CActionFocusable::NotifySetActionFocusable(_isActionFocusable);
	m_pManager->OnUpdate(NULL,NULL,NULL);		
}

/*
	\brief	a viewport click will affect something in a way or another
				according to the active CActionFocusable. It is a 
				virtual method to be overriden.
	\param	_pClickedViewPort the viewport that has been clicked.
	\param	_xClic the position clicked in _pClickedViewPort's ratio
	\param	_yClic the position clicked in _pClickedViewPort's ratio
	\param	_previewtimesec preview time position.
	\param  _mouseButton index of the mouse button. 1=left,2=center,4=right...
	\param	_mouseButtonEvent 0=clickdown, 1=move when down,2=up.
*/
void	CPreviewManagerView::CActionFocusable_Move::ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
										float _xClic,float _yClic,double _previewtimesec,
										unsigned int _mouseButton,
										unsigned int _mouseButtonEvent
											)
{
	CPreviewMachine::PreviewOGLInternalViewPort *pVP =
	(CPreviewMachine::PreviewOGLInternalViewPort*) _pClickedViewPort ;

	if(_mouseButtonEvent == 0 &&
		 _pClickedViewPort == m_pLastViewPortClicked // if move on same viewport:
		) // this is a move after a first clic.
	{
		// apply move on the viewport's cursor:
		float	xx,yy,zz;
		_pPreviewConfToModify->GetPreviewPosition(xx,yy,zz);
		//re pVP->GetCursorCoordinates(xx,yy,zz);
		float deltaX = _xClic - m_LastXClick; // mouse delta.
		float deltaY = _yClic - m_LastYClick; 
		if( _mouseButton & 1 )
		{
			xx += deltaX;
			yy += deltaY;
		}
		if( _mouseButton & 2 )
		{
			zz += deltaY;
		}
		_pPreviewConfToModify->SetPreviewPosition(xx,yy,zz);
	}
	// recall last mouse state in down or move state:
	m_pLastViewPortClicked = _pClickedViewPort;
	m_LastXClick = _xClic;
	m_LastYClick = _yClic;
}
/*
	\brief	a viewport click will affect something in a way or another
				according to the active CActionFocusable. It is a 
				virtual method to be overriden.
	\param	_pClickedViewPort the viewport that has been clicked.
	\param	_xClic the position clicked in _pClickedViewPort's ratio
	\param	_yClic the position clicked in _pClickedViewPort's ratio
	\param	_previewtimesec preview time position.
	\param  _mouseButton index of the mouse button. 1=left,2=center,4=right...
	\param	_mouseButtonEvent 0=clickdown, 1=move when down,2=up.
*/
void	CPreviewManagerView::CActionFocusable_Rotate::ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
										float _xClic,float _yClic,double _previewtimesec,
										unsigned int _mouseButton,
										unsigned int _mouseButtonEvent
											)
{
	CPreviewMachine::PreviewOGLInternalViewPort *pVP =
	(CPreviewMachine::PreviewOGLInternalViewPort*) _pClickedViewPort ;

	if(_mouseButtonEvent == 0 &&
		 _pClickedViewPort == m_pLastViewPortClicked // if move on same viewport:
		) // this is a move after a first clic.
	{
		// apply move on the viewport's cursor:
		float	xx,yy,zz;
		_pPreviewConfToModify->GetPreviewRotation(xx,yy,zz);
		//re pVP->GetCursorCoordinates(xx,yy,zz);
		float deltaX = _xClic - m_LastXClick; // mouse delta.
		float deltaY = _yClic - m_LastYClick; 
		if( _mouseButton & 1 )
		{
			xx += deltaX;
			yy += deltaY;
		}
		if( _mouseButton & 2 )
		{
			zz += deltaY;
		}
		_pPreviewConfToModify->SetPreviewRotation(xx,yy,zz);
	}
	// recall last mouse state in down or move state:
	m_pLastViewPortClicked = _pClickedViewPort;
	m_LastXClick = _xClic;
	m_LastYClick = _yClic;
}
/*
	\brief	a viewport click will affect something in a way or another
				according to the active CActionFocusable. It is a 
				virtual method to be overriden.
	\param	_pClickedViewPort the viewport that has been clicked.
	\param	_xClic the position clicked in _pClickedViewPort's ratio
	\param	_yClic the position clicked in _pClickedViewPort's ratio
	\param	_previewtimesec preview time position.
	\param  _mouseButton index of the mouse button.1=left,2=center,4=right...
	\param	_mouseButtonEvent 0=clickdown, 1=move when down,2=up.
*/
void	CPreviewManagerView::CActionFocusable_VPMove::ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
										float _xClic,float _yClic,double _previewtimesec,
										unsigned int _mouseButton,
										unsigned int _mouseButtonEvent
											)
{

	if(_mouseButtonEvent == 0 && // this is a move after a first clic.
		 _pClickedViewPort == m_pLastViewPortClicked )// if move on same viewport:
	{
		// apply move on the viewport's cursor:
		float deltaX = _xClic - m_LastXClick; // mouse delta.
		float deltaY = _yClic - m_LastYClick; 

		// get previous position rectangle of the viewport.
		/*old
		float px1 = _pClickedViewPort->GetPositionX1() ;
		float py1 = _pClickedViewPort->GetPositionY1() ;
		float px2 = _pClickedViewPort->GetPositionX2() ;
		float py2 = _pClickedViewPort->GetPositionY2() ;
		*/
		float	px1,py1,px2,py2;
		_pPreviewConfToModify->GetViewportScale(px1,py1,px2,py2) ;
		float xds1= (-px1 ) / (px2-px1);
		float xds2= (1.0f-px1) / (px2-px1) ;
		float yds1= (-py1 ) / (py2-py1) ;
		float yds2= (1.0f-py1) / (py2-py1) ;

		float centerx = (xds1+xds2)*0.5f;
		float centery = (yds1+yds2)*0.5f;
		float	lengthX = xds2-xds1;
		float	lengthY = yds2-yds1;
		if(lengthX == 0.0f) lengthX = 0.001f;
		if(lengthY == 0.0f) lengthY = 0.001f;

		if( _mouseButton & 1 )
		{
			centerx -= deltaX*(lengthX);
			centery -= deltaY*(lengthY);
		}
		if( _mouseButton & 2 )
		{

			float xzoom = 1.0f;
			if(px2-px1 > 0.001f) xzoom += deltaX*(lengthX);
			if(xzoom<0.8f) xzoom=0.8f;
			else if(xzoom>1.25f) xzoom=1.25f;
			//if( xzoom<1.0f) xzoom=1.0f;
			lengthX *= xzoom;
			if(lengthX<0.005f) lengthX = 0.005f;

			float yzoom = 1.0f;
			if(py2-py1 > 0.001f) yzoom += deltaY*(lengthY);
			if(yzoom<0.8f) yzoom=0.8f;
			else if(yzoom>1.25f) yzoom=1.25f;
			//if( yzoom<1.0f) yzoom=1.0f;
			lengthY *= yzoom;
			if(lengthY<0.005f) lengthY = 0.005f;

		}
		px1 = centerx-(lengthX*0.5f);
		py1 = centery-(lengthY*0.5f);
		px2 = centerx+(lengthX*0.5f);
		py2 = centery+(lengthY*0.5f);
		
		xds1= (-px1 ) / (px2-px1);
		 xds2= (1.0f-px1) / (px2-px1) ;
		 yds1= (-py1 ) / (py2-py1) ;
		 yds2= (1.0f-py1) / (py2-py1) ;

		_pPreviewConfToModify->SetViewportScale(
			xds1,yds1,xds2,yds2);
				/*centerx-(lengthX*0.5f),
				centery-(lengthY*0.5f),
				centerx+(lengthX*0.5f),
				centery+(lengthY*0.5f)			
			) ;*/

		if( _mouseButton & 4 )
		{
			// last bt: to normal:
			_pClickedViewPort->SetScaleCoordinates(0.0f,0.0f,1.0f,1.0f);
			
		}
	}

	// recall last mouse state:
	m_pLastViewPortClicked = _pClickedViewPort;
	m_LastXClick = _xClic;
	m_LastYClick = _yClic;
}
/*
	\brief	a viewport click will affect something in a way or another
				according to the active CActionFocusable. It is a 
				virtual method to be overriden.
	\param	_pClickedViewPort the viewport that has been clicked.
	\param	_xClic the position clicked in _pClickedViewPort's ratio
	\param	_yClic the position clicked in _pClickedViewPort's ratio
	\param	_previewtimesec preview time position.
	\param  _mouseButton index of the mouse button.1=left,2=center,4=right...
	\param	_mouseButtonEvent 0=clickdown, 1=move when down,2=up.
*/
void	CPreviewManagerView::CActionFocusable_VPReset::ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
										float _xClic,float _yClic,double _previewtimesec,
										unsigned int _mouseButton,
										unsigned int _mouseButtonEvent
											)
{
		_pPreviewConfToModify->SetViewportScale( 0.0f,0.0f,1.0f,1.0f ) ;
		_pPreviewConfToModify->SetPreviewPosition(0.0f,0.0f,0.0f);
		_pPreviewConfToModify->SetPreviewRotation(0.0f,0.0f,0.0f);

}
BOOL CPreviewManagerView::DestroyWindow()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( pdoc ) 
	{
		// focus.
		pdoc->SetDefaultActionFocusObject( 0L );
		pdoc->SetActionFocusObject( 0L );
	}
	return CFormView::DestroyWindow();
}

void CPreviewManagerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CPreviewManagerView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	return CFormView::OnNotify(wParam, lParam, pResult);
}

void CPreviewManagerView::OnSysDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	CFormView::OnSysDeadChar(nChar, nRepCnt, nFlags);
}
