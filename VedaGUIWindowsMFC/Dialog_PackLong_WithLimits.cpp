// Dialog_PackLong_WithLimits.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackLong_WithLimits.h"
#include ".\dialog_packlong_withlimits.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "PackLong_WithLimits.h"

void CDialog_PackLong_WithLimits::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PACKLONG_WITHLIMITS,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackLong_WithLimits::NewInstance(CWnd* pParent)
{
	return new CDialog_PackLong_WithLimits(pParent);

}
// Boîte de dialogue CDialog_PackLong_WithLimits

IMPLEMENT_DYNAMIC(CDialog_PackLong_WithLimits, CDialog)
CDialog_PackLong_WithLimits::CDialog_PackLong_WithLimits(CWnd* pParent /*=NULL*/)
	: CDialog_AbstractPackSerializable(CDialog_PackLong_WithLimits::IDD, pParent)
{
}

CDialog_PackLong_WithLimits::~CDialog_PackLong_WithLimits()
{
}

void CDialog_PackLong_WithLimits::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_EDIT1, m_editctrl);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
	DDX_Control(pDX, IDC_STATICMIN, m_mintext);
	DDX_Control(pDX, IDC_STATICMAX, m_maxtext);
	DDX_Control(pDX, IDC_SPIN1, m_Spin);
	DDX_Control(pDX, IDC_BEDIT_X, m_Focusedit);
}

BEGIN_MESSAGE_MAP(CDialog_PackLong_WithLimits, CDialog)
	ON_WM_HSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_EN_KILLFOCUS(IDC_EDIT1, OnEnKillfocusEdit1)
	ON_STN_CLICKED(IDC_BEDIT_X, OnActionEditClicks)
END_MESSAGE_MAP()


// Gestionnaires de messages CDialog_PackLong_WithLimits

// Gestionnaires de messages CDialog_PackULongFlags
void CDialog_PackLong_WithLimits::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView														   
														   )
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);
	// cast veda type:
	PackLong_WithLimits *pVedaLong =(PackLong_WithLimits *)_objToManage;
	int min = pVedaLong->GetMin();
	int max = pVedaLong->GetMax();
	// CSliderCtrl
	m_slider.SetRange(min,max,true);

	CString text ;
	text.Format("%d",min);
	m_mintext.SetWindowText(text.GetString());
	text.Format("%d",max);
	m_maxtext.SetWindowText(text.GetString());

	m_Spin.SetRange32(min,max);

	// 
	Update();
}
void CDialog_PackLong_WithLimits::OnOK()
{
	if( m_pObjectToManage )
	{
		PackLong_WithLimits *pVedaOb =(PackLong_WithLimits *) m_pObjectToManage;
		CString ctxt;
		m_editctrl.GetWindowText(ctxt);
		unsigned int value = atoi(ctxt.GetString());
		pVedaOb->Set(value );
	}
	//CDialog_AbstractPackSerializable::OnOK();
}

void CDialog_PackLong_WithLimits::OnCancel()
{

	//CDialog_AbstractPackSerializable::OnCancel();
}

void CDialog_PackLong_WithLimits::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog_AbstractPackSerializable::OnHScroll(nSBCode, nPos, pScrollBar);
	if(pScrollBar)
	{
		int id = pScrollBar->GetDlgCtrlID();
		if( id == IDC_SLIDER1 )
		{
			int pos = m_slider.GetPos();
			PackLong_WithLimits *pVedaOb =(PackLong_WithLimits *) m_pObjectToManage;
			if( pVedaOb )	 pVedaOb->Set(pos);
		}
	}
}
void	CDialog_PackLong_WithLimits::Update(void)
{
	PackLong_WithLimits *pVedaLong =(PackLong_WithLimits *) m_pObjectToManage;
	if(!pVedaLong)return;

	// are we focused ?
	if( IsFocused() )
		m_Focusedit.SetBitmap( (HBITMAP)m_Bm_Focused.m_hObject );
	else
		m_Focusedit.SetBitmap( (HBITMAP)m_Bm_UnFocused.m_hObject );


	// set the value to the edit ctrl:
	CString str;
	str.Format( "%d", pVedaLong->Get() );
	m_editctrl.SetWindowText( str.GetString() );
	if( m_slider.GetPos() != pVedaLong->Get() )
		m_slider.SetPos(pVedaLong->Get());
}
BOOL CDialog_PackLong_WithLimits::OnInitDialog()
{
	CDialog_AbstractPackSerializable::OnInitDialog();

	m_Bm_Focused.LoadBitmap( IDC_B_RESETVPFOCUS );
	m_Bm_UnFocused.LoadBitmap( IDC_B_RESETVP );

	UDACCEL acctable[4]={
		{1,1},
		{2,10},
		{3,20},
		{4,100}
	};

	m_Spin.SetBase(10);
	m_Spin.SetAccel(4,acctable);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialog_PackLong_WithLimits::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if( m_pObjectToManage )
	{
		PackLong_WithLimits *pVedaOb =(PackLong_WithLimits *) m_pObjectToManage;
		CString ctxt;
		m_editctrl.GetWindowText(ctxt);
		unsigned int value = atoi(ctxt.GetString());
		pVedaOb->Set(value );
	}	
	
	*pResult = 0;
}

void CDialog_PackLong_WithLimits::OnEnKillfocusEdit1()
{	/*
	if( m_pObjectToManage )
	{
		PackLong_WithLimits *pVedaOb =(PackLong_WithLimits *) m_pObjectToManage;
		CString ctxt;
		m_editctrl.GetWindowText(ctxt);
		unsigned int value = atoi(ctxt.GetString());
		pVedaOb->Set(value );
	}	*/

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
void CDialog_PackLong_WithLimits::ChangeValueOfManagedObjectWithCursor(
	VirtualMachine::InternalViewPort *_pClickedViewPort,
	unsigned int _validityBits,
	float _x,float _y,float _z,double _time )
{
	PackLong_WithLimits *pVedaLong =(PackLong_WithLimits *) m_pObjectToManage;
	if( !pVedaLong ) return;

	_x  = (_x-_pClickedViewPort->GetPositionX1()) /
	(_pClickedViewPort->GetPositionX2() - _pClickedViewPort->GetPositionX1() );
	_y  = (_y-_pClickedViewPort->GetPositionY1()) /
	(_pClickedViewPort->GetPositionY2() - _pClickedViewPort->GetPositionY1() );
	_z  = (_z-_pClickedViewPort->GetPositionY1()) /
	(_pClickedViewPort->GetPositionY2() - _pClickedViewPort->GetPositionY1() );

	if( _validityBits & m_cursorbit_XValid ) pVedaLong->Set(
		pVedaLong->GetMin()+ (int)
					(_x * (float)(pVedaLong->GetMax()-pVedaLong->GetMin()))
					); // in all cases.
/*	PackFloat::VectorDimension  dim= pVedaLong->GetVectorDimension();
	if( (_validityBits & m_cursorbit_YValid) && dim>PackFloat::vd_X ) 
				pVedaLong->Set(1,pVedaLong->GetMin()+ _y * (pVedaLong->GetMax()-pVedaLong->GetMin())); // Y
	if( (_validityBits & m_cursorbit_ZValid) && dim>PackFloat::vd_XY ) 
				pVedaLong->Set(2,pVedaLong->GetMin()+ _y * (pVedaLong->GetMax()-pVedaLong->GetMin())); // Z
	if( (_validityBits & m_cursorbit_TimeValid) && dim>PackFloat::vd_XYZ ) pVedaLong->Set(3,_time); // time.
*/
}