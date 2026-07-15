
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackULong.h"
#include "PackULong.h"
#include ".\dialog_packulong.h"
#include "VedaDemoOGLMfcGuiDoc.h"

void CDialog_PackULong::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PACKULONG,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackULong::NewInstance(CWnd* pParent)
{
	return new CDialog_PackULong(pParent);

}

IMPLEMENT_DYNAMIC(CDialog_PackULong, CDialog)
CDialog_PackULong::CDialog_PackULong(CWnd* pParent )
	: CDialog_AbstractPackSerializable(CDialog_PackULong::IDD, pParent)
{
}

CDialog_PackULong::~CDialog_PackULong()
{
}

void CDialog_PackULong::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_EDIT1, m_editCtrl);
	DDX_Control(pDX, IDC_SPIN1, m_Spin);
}

BEGIN_MESSAGE_MAP(CDialog_PackULong, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_EN_KILLFOCUS(IDC_EDIT1, OnEnKillfocusEdit1)
END_MESSAGE_MAP()

void CDialog_PackULong::OnOK()
{
	if( m_pObjectToManage )
	{
		PackULong *pVedaOb =(PackULong *) m_pObjectToManage;
		CString ctxt;
		m_editCtrl.GetWindowText(ctxt);
		unsigned int value = atoi(ctxt.GetString());
		pVedaOb->Set(value );
	}

}

void CDialog_PackULong::OnCancel()
{

}
void CDialog_PackULong::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
														   )
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);

	PackULong *pVedaULong =(PackULong *)_objToManage;

	CString str;
	str.Format( "%d", pVedaULong->Get() );
	m_editCtrl.SetWindowText( str.GetString() );

	m_Spin.SetRange32(0,(1<<28)-1);
}

void CDialog_PackULong::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	if( m_pObjectToManage )
	{
		PackULong *pVedaOb =(PackULong *) m_pObjectToManage;
		CString ctxt;
		m_editCtrl.GetWindowText(ctxt);
		unsigned int value = atoi(ctxt.GetString());
		pVedaOb->Set(value );
	}
	*pResult = 0;
}

BOOL CDialog_PackULong::OnInitDialog()
{
	CDialog_AbstractPackSerializable::OnInitDialog();

	UDACCEL acctable[4]={
		{1,1},
		{2,10},
		{3,20},
		{4,100}
	};

	m_Spin.SetBase(10);
	m_Spin.SetAccel(4,acctable);

	return TRUE;

}

void CDialog_PackULong::OnEnKillfocusEdit1()
{

	if( m_pObjectToManage )
	{
		PackULong *pVedaOb =(PackULong *) m_pObjectToManage;
		CString ctxt;
		m_editCtrl.GetWindowText(ctxt);
		unsigned int value = atoi(ctxt.GetString());
		pVedaOb->Set(value );
	}

}
void	CDialog_PackULong::Update(void)
{
	PackULong *pVedaLong =(PackULong *) m_pObjectToManage;
	if(!pVedaLong)return;

	CString str;
	str.Format( "%d", pVedaLong->Get() );
	m_editCtrl.SetWindowText( str.GetString() );

}
