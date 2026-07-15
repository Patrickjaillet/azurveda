
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackString.h"
#include ".\dialog_packstring.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "PackString.h"

void CDialog_PackString::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PACKSTRING,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackString::NewInstance(CWnd* pParent)
{
	return new CDialog_PackString(pParent);

}

IMPLEMENT_DYNAMIC(CDialog_PackString, CDialog)
CDialog_PackString::CDialog_PackString(CWnd* pParent )
	: CDialog_AbstractPackSerializable(CDialog_PackString::IDD, pParent)
{
}

CDialog_PackString::~CDialog_PackString()
{
}

void CDialog_PackString::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_StringCtrl, m_EditCtrl);
}

BEGIN_MESSAGE_MAP(CDialog_PackString, CDialog)
	ON_EN_KILLFOCUS(IDC_StringCtrl, OnEnKillfocusStringctrl)
END_MESSAGE_MAP()

void CDialog_PackString::OnOK()
{
	if( m_pObjectToManage )
	{
		PackString *pVedaString =(PackString *) m_pObjectToManage;
		CString ctxt;
		m_EditCtrl.GetWindowText(ctxt);
		pVedaString->Set(ctxt.GetString() );
	}

}

void CDialog_PackString::OnCancel()
{

}
void CDialog_PackString::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
														   )
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);

	PackString *pVedaString =(PackString *)_objToManage;

	m_EditCtrl.SetWindowText( pVedaString->Get() );

}

void CDialog_PackString::OnEnKillfocusStringctrl()
{

	if( m_pObjectToManage )
	{
		PackString *pVedaString =(PackString *) m_pObjectToManage;
		CString ctxt;
		m_EditCtrl.GetWindowText(ctxt);
		pVedaString->Set(ctxt.GetString() );
	}
}
void	CDialog_PackString::Update(void)
{
	PackString *pVedaString =(PackString *) m_pObjectToManage;
	if(!pVedaString)return;

	m_EditCtrl.SetWindowText(pVedaString->Get() );

}
