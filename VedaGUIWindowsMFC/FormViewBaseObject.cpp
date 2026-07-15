// FormViewBaseObject.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "FormViewBaseObject.h"
#include ".\formviewbaseobject.h"
#include "VedaDemoOGLMfcGuiDoc.h"

#include "BaseContext.h"
#include "ClassNode.h"

//re#include "PackStruct.h"
#include "Dialog_PackSerializable.h"
#include "Dialog_PackStruct.h"

// CFormViewBaseObject

IMPLEMENT_DYNCREATE(CFormViewBaseObject, CFormView)

CFormViewBaseObject::CFormViewBaseObject()
	: CFormView(CFormViewBaseObject::IDD)
	,m_pBaseObjectDialog(NULL)
	,m_pViewShapedForMe(NULL)
	,m_initied(false)
	,m_ForbidUpdateRecursion(false)
{
}

CFormViewBaseObject::~CFormViewBaseObject()
{

}

void CFormViewBaseObject::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLASSINFO, m_ClassInfoText);
	DDX_Control(pDX, IDC_PREVIEWOB, m_ButtonPreview);
	//DDX_Control(pDX, IDC_OBJECTOUTPUT, m_InfoLine);
	DDX_Control(pDX, IDC_TOOLFUNCBT1, m_BtToolFunction[0]);
	DDX_Control(pDX, IDC_TOOLFUNCBT2, m_BtToolFunction[1]);
	DDX_Control(pDX, IDC_TOOLFUNCBT3, m_BtToolFunction[2]);
	DDX_Control(pDX, IDC_TOOLFUNCBT4, m_BtToolFunction[3]);
	DDX_Control(pDX, IDC_TOOLFUNCBT5, m_BtToolFunction[4]);
}

BEGIN_MESSAGE_MAP(CFormViewBaseObject, CFormView)
	ON_COMMAND(ID_UNDO, CFormViewBaseObject::OnUndo)
	ON_COMMAND(ID_REDO, CFormViewBaseObject::OnRedo)
	ON_COMMAND(ID_MERGE, CFormViewBaseObject::OnMerge)
	ON_BN_CLICKED(IDC_PREVIEWOB, OnBnClickedPreviewob)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_MPREVIEW, OnMpreview)
	ON_COMMAND(ID_MPREVIEWB1, OnPreviewPrevious)
	ON_COMMAND(ID_MEDITPreview, OnEditPreview)
	ON_COMMAND(ID_MEDITB1, OnEditPrevious)
	ON_COMMAND(ID_BASEOBJECT_CLONE, OnBaseobjectClone)
	ON_COMMAND(ID_BASEOBJECT_DESTROY, OnBaseobjectDestroy)
	ON_COMMAND(ID_BASEOBJECT_DESTROYINDEPENDANT, OnBaseobjectDestroyindependant)
	ON_BN_CLICKED(IDC_TOOLFUNCBT1, OnBnClickedToolfuncbt1)
	ON_BN_CLICKED(IDC_TOOLFUNCBT2, OnBnClickedToolfuncbt2)
	ON_BN_CLICKED(IDC_TOOLFUNCBT3, OnBnClickedToolfuncbt3)
	ON_BN_CLICKED(IDC_TOOLFUNCBT4, OnBnClickedToolfuncbt4)
	ON_BN_CLICKED(IDC_TOOLFUNCBT5, OnBnClickedToolfuncbt5)
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// Diagnostics CFormViewBaseObject

#ifdef _DEBUG
void CFormViewBaseObject::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormViewBaseObject::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// Gestionnaires de messages CFormViewBaseObject

void CFormViewBaseObject::OnInitialUpdate()
{
	m_initied = true;
	CFormView::OnInitialUpdate();

	/*
	// enregistre les constructeurs de dialogues possibles pour les membres:
	RegisterPackDialog( CDialog_PackString::GetManagedClassName(),CDialog_PackString::NewInstance );
	RegisterPackDialog( CDialog_PackULong::GetManagedClassName(),CDialog_PackULong::NewInstance );
	RegisterPackDialog( CDialog_PackULongFlags::GetManagedClassName(),CDialog_PackULongFlags::NewInstance );
	RegisterPackDialog( CDialog_PackReference::GetManagedClassName(),CDialog_PackReference::NewInstance );
	RegisterPackDialog( CDialog_PackLong_WithLimits::GetManagedClassName(),CDialog_PackLong_WithLimits::NewInstance );
	RegisterPackDialog( CDialog_PackListDynamic::GetManagedClassName(),CDialog_PackListDynamic::NewInstance );
*/
}
void CFormViewBaseObject::ShapeGUIForBaseObject( BaseObject *_pBaseObject)
{	
	// already done ?
	if( m_pViewShapedForMe == _pBaseObject) return;
	// erase all previous dialogs:
	DestroyAllDialogs();
	m_ButtonPreview.ShowWindow( SW_HIDE );

	m_pViewShapedForMe = _pBaseObject;


	// hide tool bouttons:
	unsigned int	btnb=0;
	while(btnb<m_NumberOfButton)
	{
		m_BtToolFunction[btnb].ShowWindow(SW_HIDE);
		btnb++;
	}
	// clean class info text:
	m_ClassInfoText.SetWindowText("");
	// _pBaseObject can be null (deselect)
	if(_pBaseObject == NULL ) return;
	// set class info text:
	CString cstrInfo = _pBaseObject->GetDisplayClassName();
	cstrInfo +=": ";
	cstrInfo +=_pBaseObject->GetClassHelpDescription();
	m_ClassInfoText.SetWindowText(cstrInfo.GetString());

	m_ButtonPreview.ShowWindow( SW_SHOW );

	//old: BaseObject are PackStruct, and drawn as:
	// BaseObject are PackSerializables, and drawn as:

	CRect rec;
	m_pBaseObjectDialog = new CDialog_PackStruct();
	if(m_pBaseObjectDialog )
	{	// create mfc dialog:
		m_pBaseObjectDialog->Create(this);
		// place dialog in view:
		// CView

		// set Veda object to edit, it occurs subcreations:
		m_pBaseObjectDialog->SetPackSerializable( _pBaseObject, (CVedaDemoOGLMfcGuiDoc *) GetDocument(),this );
	
	}
	// set buttons for tool methods:
	//m_BtToolFunction1
	const char *pMethodShortDisplayName,*pMethodHelp;
	unsigned int	methodFlagInfo = 0;
	unsigned int	methodID = 0;
	btnb=0;
	while( 
		(methodID = _pBaseObject->GetNextToolMethod(methodID,methodFlagInfo,pMethodShortDisplayName,pMethodHelp )) != 0
		&& btnb<m_NumberOfButton
		)
	{
		m_BtToolFunction[btnb].SetWindowText( pMethodShortDisplayName );
		m_ToolFunctionID[btnb] = methodID;
		m_BtToolFunction[btnb].ShowWindow(SW_SHOW);
		btnb++;
	}



	// unsigned int GetNextToolMethod( unsigned int _PreviousMethodID,unsigned int &_NextMethodflagInfo ,const char *&_pNextShortDisplayName,const char *&_pNextDisplayHelp );

}

void CFormViewBaseObject::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
{	
	if(!m_initied) return;
	if(m_ForbidUpdateRecursion)
	{
		return;
	}
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc)
	{
		m_ForbidUpdateRecursion = false;
		return ;
	}
	m_ForbidUpdateRecursion = true;
	bool sendUpdate=false;
	// if member change because of undo, change selection:
	if( pdoc->GetUpdateType() == BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo
		&& pdoc->GetObjectUpdated() != 0L
		// test so we don't put interfacepref object after a focus (quite awful test, can do better)
		&& pdoc->GetObjectUpdated()->GetClassDescription().m_NewMethod != InterfacePrefObject::m_Description.m_NewMethod
		)
	{
		pdoc->SetEditionSelectedBaseObject( pdoc->GetObjectUpdated() );
	}

	// manage object selection and reshape:
	if( m_pViewShapedForMe != pdoc->GetEditionSelectedBaseObject() )
	{
		ShapeGUIForBaseObject(pdoc->GetEditionSelectedBaseObject());
		sendUpdate = true;
	}
	if( !pdoc->GetEditionSelectedBaseObject()  )
	{
		m_ForbidUpdateRecursion = false;
		return;
	}
	// manage veda updates:
	switch( pdoc->GetUpdateType() )
	{
		case BaseContext::eVedaUpdate_ObjectNew:
		case BaseContext::eVedaUpdate_ObjectDestroyed:
		case BaseContext::eVedaUpdate_MemberChanged:
		case BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo:
		{		
			sendUpdate = true;
			//m_InfoLine.SetWindowText( pdoc->GetEditionSelectedBaseObject()->GetObjectInfoLine() );

		}
		break;
		case BaseContext::eVedaUpdate_ObjectReCreated:
		{
			//m_InfoLine.SetWindowText( pdoc->GetEditionSelectedBaseObject()->GetObjectInfoLine() );
		} break;
		case BaseContext::eVedaUpdate_ObjectClosed:
		{	
		}
		break;	
	}
	if(sendUpdate)
	{
		// veda objects recursive scan for updates:
		m_pBaseObjectDialog->Update();
		// position+ size reset after construction or modify:
		CRect	rec;
		CPoint scrollpos = GetScrollPosition();
		m_pBaseObjectDialog->GetClientRect( &rec);
		m_pBaseObjectDialog->ClientToScreen( &rec);
		ScreenToClient(&rec);
		rec.MoveToX( 0 - scrollpos.x);
		rec.MoveToY(46 - scrollpos.y ); //28
		m_pBaseObjectDialog->MoveWindow( &rec);// 16+130,verticalline,200,30);
		m_pBaseObjectDialog->ShowWindow(SW_SHOW);	

		CSize cz(rec.Width()+16, rec.Height()+ 46 );
		SetScrollSizes(MM_TEXT, cz );	
	}
	m_ForbidUpdateRecursion = false;
}
//! enregistre les gadgets capable d'editer les membres: 
/*
void	CFormViewBaseObject::RegisterPackDialog( const char *_pSuportedClass,CDialog_AbstractPackSerializable *(*_CreatorFunc)(CWnd *) )
{
	//CMapStringToPtr
	m_PackDialogMap.SetAt(_pSuportedClass,(void *)_CreatorFunc);

}*/
void	CFormViewBaseObject::DestroyAllDialogs()
{
	//m_ButtonPreview.SetFocus(); // send kill focus to others.
	//CPtrList
	CDialog_PackStruct *pDialogToDelete = m_pBaseObjectDialog;
	if( pDialogToDelete )
	{
		m_pBaseObjectDialog = NULL; //important to do it now.
		pDialogToDelete->DestroyWindow();
		delete pDialogToDelete;
	}

}

void CFormViewBaseObject::OnUndo()
{
	GetDocument()->GetVedaContext()->Undo(); 
}
void CFormViewBaseObject::OnRedo()
{
	GetDocument()->GetVedaContext()->Redo(); 
}
void CFormViewBaseObject::OnMerge()
{
	GetDocument()->OnMerge();	
}

void CFormViewBaseObject::OnBnClickedPreviewob()
{
	// clickin on the preview button set the object to preview:
	GetDocument()->SetPreviewSelectedBaseObject(
		GetDocument()->GetEditionSelectedBaseObject() );

}

void CFormViewBaseObject::OnRButtonDown(UINT nFlags, CPoint point)
{
//	CFormView::OnRButtonDown(nFlags, point);
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc) return;
	BaseObject *pOb = pdoc->GetEditionSelectedBaseObject() ;
	if( !pOb ) return;// CFormView::OnRButtonDown(nFlags, point);

	CMenu menu;
	CMenu *pPopUp;
	menu.LoadMenu( IDR_MENU_BASEOBJECT );	
	pPopUp = menu.GetSubMenu(0);
	
	// track:
	CPoint pt;
	GetCursorPos( &pt);
	CPoint ppoint = pt;
	SetForegroundWindow();
	pPopUp->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, ppoint.x, ppoint.y,this);

}

void CFormViewBaseObject::OnMpreview()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;
	BaseObject *pOb = pdoc->GetEditionSelectedBaseObject() ;
	if(  !pOb ) return;
	pdoc->SetPreviewSelectedBaseObject( pOb );

}

void CFormViewBaseObject::OnBaseobjectClone()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;
	BaseObject *pOb = pdoc->GetEditionSelectedBaseObject() ;
	if(  !pOb ) return;
	pOb->Clone();

}

void CFormViewBaseObject::OnBaseobjectDestroy()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;
	BaseObject *pOb = pdoc->GetEditionSelectedBaseObject() ;
	if(  !pOb ) return;
	// we really got to do that:
	if( pdoc->GetEditionSelectedBaseObject() == pOb)  pdoc->SetEditionSelectedBaseObject(NULL);
	if( pdoc->GetPreviewSelectedBaseObject() == pOb ) pdoc->SetPreviewSelectedBaseObject(NULL);
	pOb->GetContext()->DestroyObject(pOb);
}

void CFormViewBaseObject::OnBaseobjectDestroyindependant()
{
	if (AfxGetMainWnd()->MessageBox("This Function Destroy all objects that are not used by this object. Do we do that ?" ,"Watch out!" ,MB_YESNO|MB_ICONQUESTION  /*QUESTION*/)==IDNO)
     return;
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;
	BaseObject *pOb = pdoc->GetEditionSelectedBaseObject() ;
	if(  !pOb ) return;
	pOb->GetContext()->DestroyAllIndependantObjects( pOb );
}

void CFormViewBaseObject::OnBnClickedToolfuncbt1()
{
	if(!m_pViewShapedForMe) return;
	m_pViewShapedForMe->ExecuteToolMethod(m_ToolFunctionID[0]);
}
void CFormViewBaseObject::OnBnClickedToolfuncbt2()
{
	if(!m_pViewShapedForMe) return;
	m_pViewShapedForMe->ExecuteToolMethod(m_ToolFunctionID[1]);

}
void CFormViewBaseObject::OnBnClickedToolfuncbt3()
{
	if(!m_pViewShapedForMe) return;
	m_pViewShapedForMe->ExecuteToolMethod(m_ToolFunctionID[2]);

}
void CFormViewBaseObject::OnBnClickedToolfuncbt4()
{
	if(!m_pViewShapedForMe) return;
	m_pViewShapedForMe->ExecuteToolMethod(m_ToolFunctionID[3]);

}
void CFormViewBaseObject::OnBnClickedToolfuncbt5()
{
	if(!m_pViewShapedForMe) return;
	m_pViewShapedForMe->ExecuteToolMethod(m_ToolFunctionID[4]);

}
void CFormViewBaseObject::OnPreviewPrevious()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;	
	pdoc->OnPreviewPrevious();
}

void CFormViewBaseObject::OnEditPreview()
{

	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;	
	BaseObject *pPreview = pdoc->GetPreviewSelectedBaseObject();
	if(pPreview) pdoc->SetEditionSelectedBaseObject(pPreview);
}

void CFormViewBaseObject::OnEditPrevious()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;	
	pdoc->OnEditPrevious();
}
/*
You can handle mouse wheel scrolling yourself by overriding the message-mapped 
OnMouseWheel and OnRegisteredMouseWheel member functions. As they are for CScrollView, these member functions support the recommended behaviour for WM_MOUSEWHEEL, the wheel rotation message. 


*/
BOOL CFormViewBaseObject::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels
/*
nFlags :
MK_CONTROL   Set if the CTRL key is down. 
MK_LBUTTON   Set if the left mouse button is down. 
MK_MBUTTON   Set if the middle mouse button is down. 
MK_RBUTTON   Set if the right mouse button is down. 
MK_SHIFT   Set if the SHIFT key is down. 
zDelta 
Indicates distance rotated. The zDelta value is expressed in multiples or divisions
of WHEEL_DELTA, which is 120. A value less than zero indicates rotating back
(toward the user) while a value greater than zero indicates rotating forward 
(away from the user). The user can reverse this response by changing the Wheel 
setting in the mouse software. See the Remarks for more information about this 
parameter. 
pt 
Specifies the x- and y-coordinate of the cursor. These coordinates are 
always relative to the upper-left corner of the screen. 
*/
	//CFormView
	// CScrollView
		CPoint bef;
// bef = GetScrollPosition( );
//	SetScrollPos(SB_VERT,bef.y - zDelta,FALSE);

	bef = GetDeviceScrollPosition( );
	bef.y -=zDelta;
	ScrollToPosition(bef);
	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}

void CFormViewBaseObject::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFormViewBaseObject::OnDestroy()
{
	DestroyAllDialogs();
	CFormView::OnDestroy();
	// TODO : ajoutez ici le code de votre gestionnaire de messages
}

void CFormViewBaseObject::OnDropFiles(HDROP hDropInfo)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	CFormView::OnDropFiles(hDropInfo);
}
