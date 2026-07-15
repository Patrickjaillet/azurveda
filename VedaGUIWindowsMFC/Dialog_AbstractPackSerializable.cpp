// Dialog_AbstractPackSerializable.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_AbstractPackSerializable.h"
#include ".\dialog_abstractpackserializable.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "InterfacePrefObject.h"
#include "BaseObject.h"
#include "BaseContext.h"
// Boîte de dialogue CDialog_AbstractPackSerializable

CDialog_AbstractPackSerializable::CDialog_AbstractPackSerializable(UINT nIDTemplate,CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
	,m_pObjectToManage(NULL)
	,m_pDoc(NULL)
{
	m_ActionFocusable.m_pManager = this ;
}

CDialog_AbstractPackSerializable::~CDialog_AbstractPackSerializable()
{
	if(m_pDoc && IsFocused())  m_pDoc->SetActionFocusObject(0L);
}

void CDialog_AbstractPackSerializable::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
														   )
{
	m_pObjectToManage = _objToManage;
	m_pDoc = _pDoc;
	m_pView = _pView ;
}
void	CDialog_AbstractPackSerializable::Update(void)
{

}
void	CDialog_AbstractPackSerializable::Resize(int width, int height)
{
	CRect trc;
	GetClientRect(&trc);
	ClientToScreen(&trc);
	GetParent()->ScreenToClient(&trc);
	if(width == -1 ) width=trc.Width();
	if(height == -1 ) height=trc.Height();
	MoveWindow( trc.left, trc.top,width ,height );
}
BEGIN_MESSAGE_MAP(CDialog_AbstractPackSerializable, CDialog)
	ON_COMMAND(ID_MPREVIEW, OnMpreview)
	ON_COMMAND(ID_MPREVIEWB1, OnPreviewPrevious)
	ON_COMMAND(ID_MEDITPreview, OnEditPreview)
	ON_COMMAND(ID_MEDITB1, OnEditPrevious)
	ON_COMMAND(ID_BASEOBJECT_CLONE, OnBaseobjectClone)
//	ON_COMMAND(ID_BASEOBJECT_DESTROY, OnBaseobjectDestroy)
	ON_COMMAND(ID_BASEOBJECT_DESTROYINDEPENDANT, OnBaseobjectDestroyindependant)
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CDialog_AbstractPackSerializable::OnMpreview()
{
	if( !m_pDoc ) return;
	BaseObject *pOb = m_pDoc->GetEditionSelectedBaseObject() ;
	if(  !pOb ) return;
	m_pDoc->SetPreviewSelectedBaseObject( pOb );
}

void CDialog_AbstractPackSerializable::OnBaseobjectClone()
{
	if( !m_pDoc ) return;
	BaseObject *pOb = m_pDoc->GetEditionSelectedBaseObject() ;
	if(  !pOb ) return;
	pOb->Clone();
}

void CDialog_AbstractPackSerializable::OnBaseobjectDestroy()
{
	if( !m_pDoc ) return;
	BaseObject *pOb = m_pDoc->GetEditionSelectedBaseObject() ;
	if(  !pOb ) return;
/*
	if( m_pDoc->GetEditionSelectedBaseObject() == pOb)  m_pDoc->SetEditionSelectedBaseObject(NULL);
	if( m_pDoc->GetPreviewSelectedBaseObject() == pOb ) m_pDoc->SetPreviewSelectedBaseObject(NULL);
	pOb->GetContext()->DestroyObject(pOb);
	*/
}

void CDialog_AbstractPackSerializable::OnBaseobjectDestroyindependant()
{
if (AfxGetMainWnd()->MessageBox("This Function Destroy all objects that are not used by this object. Do we do that ?" ,"Watch out!" ,MB_YESNO|MB_ICONQUESTION  /*QUESTION*/)==IDNO)
     return;
	if( !m_pDoc ) return;
	BaseObject *pOb = m_pDoc->GetEditionSelectedBaseObject() ;
	if(  !pOb ) return;
	pOb->GetContext()->DestroyAllIndependantObjects( pOb );
}

void CDialog_AbstractPackSerializable::OnRButtonDown(UINT nFlags, CPoint point)
{
//	CDialog::OnRButtonDown(nFlags, point);
	if( !m_pDoc ) return;

	BaseObject *pOb = m_pDoc->GetEditionSelectedBaseObject() ;
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
void CDialog_AbstractPackSerializable::SetMemberName(	const char *_pMemberName )
{
	/*SS_BITMAP
	CStatic*/
/*	CFont
	m_MemberName.SetFont
	ModifyStyle(*/
	//CWnd

	m_MemberName.SetWindowText( _pMemberName );
}

void CDialog_AbstractPackSerializable::OnPreviewPrevious()
{
	if( !m_pDoc ) return;
	m_pDoc->OnPreviewPrevious();
}

void CDialog_AbstractPackSerializable::OnEditPreview()
{
	if( !m_pDoc ) return;
	BaseObject *pPreview = m_pDoc->GetPreviewSelectedBaseObject();
	if(pPreview) m_pDoc->SetEditionSelectedBaseObject(pPreview);
}

void CDialog_AbstractPackSerializable::OnEditPrevious()
{
	if( !m_pDoc ) return;
	m_pDoc->OnEditPrevious();
}
void CDialog_AbstractPackSerializable::OnActionEditClicks()
{
	if( !m_pDoc ) return;
	if( IsFocused() )
		m_pDoc->SetActionFocusObject();
	else
		m_pDoc->SetActionFocusObject( &m_ActionFocusable );
}
bool	CDialog_AbstractPackSerializable::IsFocused()
{
	return m_ActionFocusable.IsActionFocusable();
}
/*
	\brief	if an object of any type is managed, it could be changed by a cursor state.
			by default, it does nothing. It has to be extended.
	\param	_validityBits tels if _x,_y,_z and _time are  valid
	\param	_x x position.
	\param	_y y position.
	\param	_z z position.
	\param	_time the time of the cursor in seconds.
*/
void CDialog_AbstractPackSerializable::ChangeValueOfManagedObjectWithCursor(
					VirtualMachine::InternalViewPort *_pClickedViewPort,
					unsigned int _validityBits,
					float _x,float _y,float _z,double _time )
{
	// not impl. by def. !
	return;
}
/*
	\brief	sent when the object become the actionfocused one.	
*/
void	CDialog_AbstractPackSerializable::CActionFocusableDialog::
			NotifySetActionFocusable(bool _isActionFocusable)
{
	CActionFocusable::NotifySetActionFocusable(_isActionFocusable);
	m_pManager->Update();
}
/*
	\brief	a viewport click will affect something in a way or another
				according to the active CActionFocusable. It is a 
				virtual method to be overriden.
	\param	_pClickedViewPort the viewport that has been clicked.
	\param	_xClic the position clicked in _pClickedViewPort's ratio
	\param	_yClic the position clicked in _pClickedViewPort's ratio
	\param	_previewtimesec preview time position.
	\param  _mouseButton index of the mouse buttonXXXXXXXXXXXXXXXXXXXXXx
	\param	_mouseButtonEvent XXXXXXXXXXXXXXXXXXXXXXXX
*/
void	CDialog_AbstractPackSerializable::CActionFocusableDialog::ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
									float _xClic,float _yClic,double _previewtimesec,
									unsigned int _mouseButton,
									unsigned int _mouseButtonEvent
										)
{
	unsigned int validBits=0;
	//if(_mouseButtonEvent==0)
	//{
	if(_mouseButton & 1)
	{
		validBits |=
		CDialog_AbstractPackSerializable::m_cursorbit_XValid |
		CDialog_AbstractPackSerializable::m_cursorbit_YValid |
		CDialog_AbstractPackSerializable::m_cursorbit_TimeValid 
		;
	}
	if(_mouseButton & 2)
	{
		validBits |=
		CDialog_AbstractPackSerializable::m_cursorbit_ZValid |
		CDialog_AbstractPackSerializable::m_cursorbit_TimeValid 
		;
	}
	
	if(!validBits) return;
	m_pManager->ChangeValueOfManagedObjectWithCursor(
		_pClickedViewPort,
			validBits,_xClic,_yClic,_yClic,_previewtimesec);

}
/*
	\brief	OVERRIDE change the focused object from a 3D cursor.
	\param	_x position
	\param	_y position
	\param	_z position
	\param	_timesec time in second.
*//*
void CDialog_AbstractPackSerializable::CActionFocusableDialog::Set3DCursorOnActionFocusedObject( float _x,float _y,float _z,double _timesec)
{
	m_pManager->ChangeValueOfManagedObjectWithCursor(_x,_y,_z,_timesec);

}*/

/*void CDialog_AbstractPackSerializable::OnDestroy()
{
	// declare we manage no more.
	//no need finnaly.(not thrown  before) m_pObjectToManage = NULL;
	CDialog::OnDestroy();
}*/
