
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "FormViewBaseObject.h"
#include ".\formviewbaseobject.h"
#include "VedaDemoOGLMfcGuiDoc.h"

#include "BaseContext.h"
#include "ClassNode.h"

#include "Dialog_PackSerializable.h"
#include "Dialog_PackStruct.h"

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

#ifdef _DEBUG
void CFormViewBaseObject::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormViewBaseObject::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif

void CFormViewBaseObject::OnInitialUpdate()
{
	m_initied = true;
	CFormView::OnInitialUpdate();

}
void CFormViewBaseObject::ShapeGUIForBaseObject( BaseObject *_pBaseObject)
{

	if( m_pViewShapedForMe == _pBaseObject) return;

	DestroyAllDialogs();
	m_ButtonPreview.ShowWindow( SW_HIDE );

	m_pViewShapedForMe = _pBaseObject;

	unsigned int	btnb=0;
	while(btnb<m_NumberOfButton)
	{
		m_BtToolFunction[btnb].ShowWindow(SW_HIDE);
		btnb++;
	}

	m_ClassInfoText.SetWindowText("");

	if(_pBaseObject == NULL ) return;

	CString cstrInfo = _pBaseObject->GetDisplayClassName();
	cstrInfo +=": ";
	cstrInfo +=_pBaseObject->GetClassHelpDescription();
	m_ClassInfoText.SetWindowText(cstrInfo.GetString());

	m_ButtonPreview.ShowWindow( SW_SHOW );

	CRect rec;
	m_pBaseObjectDialog = new CDialog_PackStruct();
	if(m_pBaseObjectDialog )
	{
		m_pBaseObjectDialog->Create(this);

		m_pBaseObjectDialog->SetPackSerializable( _pBaseObject, (CVedaDemoOGLMfcGuiDoc *) GetDocument(),this );

	}

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

}

void CFormViewBaseObject::OnUpdate(CView* , LPARAM , CObject* )
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

	if( pdoc->GetUpdateType() == BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo
		&& pdoc->GetObjectUpdated() != 0L

		&& pdoc->GetObjectUpdated()->GetClassDescription().m_NewMethod != InterfacePrefObject::m_Description.m_NewMethod
		)
	{
		pdoc->SetEditionSelectedBaseObject( pdoc->GetObjectUpdated() );
	}

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

	switch( pdoc->GetUpdateType() )
	{
		case BaseContext::eVedaUpdate_ObjectNew:
		case BaseContext::eVedaUpdate_ObjectDestroyed:
		case BaseContext::eVedaUpdate_MemberChanged:
		case BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo:
		{
			sendUpdate = true;

		}
		break;
		case BaseContext::eVedaUpdate_ObjectReCreated:
		{

		} break;
		case BaseContext::eVedaUpdate_ObjectClosed:
		{
		}
		break;
	}
	if(sendUpdate)
	{

		m_pBaseObjectDialog->Update();

		CRect	rec;
		CPoint scrollpos = GetScrollPosition();
		m_pBaseObjectDialog->GetClientRect( &rec);
		m_pBaseObjectDialog->ClientToScreen( &rec);
		ScreenToClient(&rec);
		rec.MoveToX( 0 - scrollpos.x);
		rec.MoveToY(46 - scrollpos.y );
		m_pBaseObjectDialog->MoveWindow( &rec);
		m_pBaseObjectDialog->ShowWindow(SW_SHOW);

		CSize cz(rec.Width()+16, rec.Height()+ 46 );
		SetScrollSizes(MM_TEXT, cz );
	}
	m_ForbidUpdateRecursion = false;
}

void	CFormViewBaseObject::DestroyAllDialogs()
{

	CDialog_PackStruct *pDialogToDelete = m_pBaseObjectDialog;
	if( pDialogToDelete )
	{
		m_pBaseObjectDialog = NULL;
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

	GetDocument()->SetPreviewSelectedBaseObject(
		GetDocument()->GetEditionSelectedBaseObject() );

}

void CFormViewBaseObject::OnRButtonDown(UINT , CPoint )
{

	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc) return;
	BaseObject *pOb = pdoc->GetEditionSelectedBaseObject() ;
	if( !pOb ) return;

	CMenu menu;
	CMenu *pPopUp;
	menu.LoadMenu( IDR_MENU_BASEOBJECT );
	pPopUp = menu.GetSubMenu(0);

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

	if( pdoc->GetEditionSelectedBaseObject() == pOb)  pdoc->SetEditionSelectedBaseObject(NULL);
	if( pdoc->GetPreviewSelectedBaseObject() == pOb ) pdoc->SetPreviewSelectedBaseObject(NULL);
	pOb->GetContext()->DestroyObject(pOb);
}

void CFormViewBaseObject::OnBaseobjectDestroyindependant()
{
	if (AfxGetMainWnd()->MessageBox("This Function Destroy all objects that are not used by this object. Do we do that ?" ,"Watch out!" ,MB_YESNO|MB_ICONQUESTION  )==IDNO)
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

BOOL CFormViewBaseObject::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

		CPoint bef;

	bef = GetDeviceScrollPosition( );
	bef.y -=zDelta;
	ScrollToPosition(bef);
	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}

void CFormViewBaseObject::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFormViewBaseObject::OnDestroy()
{
	DestroyAllDialogs();
	CFormView::OnDestroy();

}

void CFormViewBaseObject::OnDropFiles(HDROP hDropInfo)
{

	CFormView::OnDropFiles(hDropInfo);
}
