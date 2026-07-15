// Dialog_PackFloat.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackFloat.h"
#include "PackFloat.h"
#include ".\dialog_packfloat.h"


void CDialog_PackFloat::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PACKFLOAT,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackFloat::NewInstance(CWnd* pParent)
{
	return new CDialog_PackFloat(pParent);

}

// Boîte de dialogue CDialog_PackFloat

IMPLEMENT_DYNAMIC(CDialog_PackFloat, CDialog)
CDialog_PackFloat::CDialog_PackFloat(CWnd* pParent /*=NULL*/)
	: CDialog_AbstractPackSerializable(CDialog_PackFloat::IDD, pParent)
{
}

CDialog_PackFloat::~CDialog_PackFloat()
{
}

void CDialog_PackFloat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_EDIT1, m_editCtrl);
	DDX_Control(pDX, IDC_EDIT2, m_editCtrlY);
	DDX_Control(pDX, IDC_EDIT3, m_editCtrlZ);
	DDX_Control(pDX, IDC_STATICX, m_StaticX);
	DDX_Control(pDX, IDC_STATICY, m_StaticY);
	DDX_Control(pDX, IDC_STATICZ, m_StaticZ);
	DDX_Control(pDX, IDC_BEDIT_X, m_EditBT_X);
	DDX_Control(pDX, IDC_STATICD, m_StaticD);
	DDX_Control(pDX, IDC_EDIT4, m_editCtrlD);
}


BEGIN_MESSAGE_MAP(CDialog_PackFloat, CDialog)
	ON_STN_CLICKED(IDC_BEDIT_X, OnActionEditClicks)
END_MESSAGE_MAP()


// Gestionnaires de messages CDialog_PackFloat

void CDialog_PackFloat::OnOK()
{
	//CDialog::OnOK();
	PackFloat *pPackFloat =(PackFloat *) m_pObjectToManage;
	if( !pPackFloat ) return;
	CString str;
	m_editCtrl.GetWindowText(str);
	float vv = (float)atof( str.GetString() );
	pPackFloat->Set(vv);

	PackFloat::VectorDimension  dim= pPackFloat->GetVectorDimension();
	if( dim>PackFloat::vd_X )
	{	// Y
		m_editCtrlY.GetWindowText(str);
		float vv = (float)atof( str.GetString() );
		pPackFloat->Set(1,vv);
	}
	if( dim>PackFloat::vd_XY )
	{
		// XY
		m_editCtrlZ.GetWindowText(str);
		float vv = (float)atof( str.GetString() );
		pPackFloat->Set(2,vv);
	}

	if( dim>PackFloat::vd_XYZ )
	{
		// XYZ
		m_editCtrlD.GetWindowText(str);
		float vv = (float)atof( str.GetString() );
		pPackFloat->Set(3,vv);
	}

}

void CDialog_PackFloat::OnCancel()
{
	OnOK();
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base
	//CDialog::OnCancel();
}

void	CDialog_PackFloat::Update(void)
{
	// cast AzurVeda type:
	PackFloat *pPackFloat =(PackFloat *) m_pObjectToManage;
	// are we focused ?
	if( IsFocused() )
		m_EditBT_X.SetBitmap( (HBITMAP)m_Bm_Focused.m_hObject );
	else
		m_EditBT_X.SetBitmap( (HBITMAP)m_Bm_UnFocused.m_hObject );
	// set the value to the edit ctrl:
	CString str;
	str.Format( "%f", pPackFloat->Get() );
	m_editCtrl.SetWindowText( str.GetString() );

	PackFloat::VectorDimension  dim= pPackFloat->GetVectorDimension();
	if( dim>PackFloat::vd_X )
	{	// Y
		str.Format( "%f", pPackFloat->Get(1) );
		m_editCtrlY.SetWindowText( str.GetString() );
	}
	if( dim>PackFloat::vd_XY )
	{
		// XY
		str.Format( "%f", pPackFloat->Get(2) );
		m_editCtrlZ.SetWindowText( str.GetString() );
	}
	if( dim>PackFloat::vd_XYZ )
	{
		// XY
		str.Format( "%f", pPackFloat->Get(3) );
		m_editCtrlD.SetWindowText( str.GetString() );
	}

	
}
void CDialog_PackFloat::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView														   
														   )
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);
	PackFloat *pPackFloat =(PackFloat *) m_pObjectToManage;

	PackFloat::VectorDimension  dim= pPackFloat->GetVectorDimension();

	m_StaticX.ShowWindow( SW_HIDE );
	m_EditBT_X.ShowWindow( SW_SHOW );

	m_StaticY.ShowWindow( SW_HIDE );
	m_editCtrlY.ShowWindow( SW_HIDE );
	m_StaticZ.ShowWindow( SW_HIDE );
	m_editCtrlZ.ShowWindow( SW_HIDE );
	m_StaticD.ShowWindow( SW_HIDE );
	m_editCtrlD.ShowWindow( SW_HIDE );
	switch(dim)
	{
		default:
		case PackFloat::vd_X:
		{
			//m_EditBT_X.ShowWindow( SW_SHOW );
		}
		break;
		case PackFloat::vd_XY:
		{
			m_StaticX.ShowWindow( SW_SHOW );
			m_StaticY.ShowWindow( SW_SHOW );
			m_editCtrlY.ShowWindow( SW_SHOW );

		}
		break;
		case PackFloat::vd_XYZ:
		{
			m_StaticX.ShowWindow( SW_SHOW );
			m_StaticY.ShowWindow( SW_SHOW );
			m_editCtrlY.ShowWindow( SW_SHOW );
			m_StaticZ.ShowWindow( SW_SHOW );
			m_editCtrlZ.ShowWindow( SW_SHOW );
		}
		break;
		case PackFloat::vd_XYZD:
		{
			m_StaticX.ShowWindow( SW_SHOW );
			m_StaticY.ShowWindow( SW_SHOW );
			m_editCtrlY.ShowWindow( SW_SHOW );
			m_StaticZ.ShowWindow( SW_SHOW );
			m_editCtrlZ.ShowWindow( SW_SHOW );
			m_StaticD.ShowWindow( SW_SHOW );
			m_editCtrlD.ShowWindow( SW_SHOW );
		}
		break;
	}
	Update();
}

BOOL CDialog_PackFloat::OnInitDialog()
{
	CDialog_AbstractPackSerializable::OnInitDialog();

	m_Bm_Focused.LoadBitmap( IDC_B_RESETVPFOCUS );
	m_Bm_UnFocused.LoadBitmap( IDC_B_RESETVP );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}
/*
	\brief	if an object of any type is managed, it could be changed by a cursor state.
			by default, it does nothing. It has to be extended.
	\param	_x x position.
	\param	_y y position.
	\param	_z z position.
	\param	_time the time of the cursor in seconds.
*/
void  CDialog_PackFloat::ChangeValueOfManagedObjectWithCursor(
						VirtualMachine::InternalViewPort *_pClickedViewPort,
						unsigned int _validityBits,
						float _x,float _y,float _z,double _time )
{
	PackFloat *pPackFloat =(PackFloat *) m_pObjectToManage;
	if( !pPackFloat ) return;

	if( _validityBits & m_cursorbit_XValid ) pPackFloat->Set(_x); // in all cases.
	PackFloat::VectorDimension  dim= pPackFloat->GetVectorDimension();
	if( (_validityBits & m_cursorbit_YValid) && dim>PackFloat::vd_X ) pPackFloat->Set(1,_y); // Y
	if( (_validityBits & m_cursorbit_ZValid) && dim>PackFloat::vd_XY ) pPackFloat->Set(2,_z); // Z
	if( (_validityBits & m_cursorbit_TimeValid) && dim>PackFloat::vd_XYZ ) pPackFloat->Set(3,(float)_time); // time.

}

