// Dialog_PackString.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackStringMultiple.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "PackString_MultipleLines.h"

void CDialog_PackStringMultiple::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PACKSTRINGMULTIPLE,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackStringMultiple::NewInstance(CWnd* pParent)
{
	return new CDialog_PackStringMultiple(pParent);

}
// Boîte de dialogue CDialog_PackStringMultiple

IMPLEMENT_DYNAMIC(CDialog_PackStringMultiple, CDialog)
CDialog_PackStringMultiple::CDialog_PackStringMultiple(CWnd* pParent /*=NULL*/)
: CDialog_AbstractPackSerializable(CDialog_PackStringMultiple::IDD, pParent)
{
}

CDialog_PackStringMultiple::~CDialog_PackStringMultiple()
{
}

void CDialog_PackStringMultiple::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_StringCtrl, m_EditCtrl);
}


BEGIN_MESSAGE_MAP(CDialog_PackStringMultiple, CDialog)
	ON_EN_KILLFOCUS(IDC_StringCtrl, OnEnKillfocusStringctrl)
END_MESSAGE_MAP()



// Gestionnaires de messages CDialog_PackStringMultiple

void CDialog_PackStringMultiple::OnOK()
{
	if( m_pObjectToManage )
	{
		PackString *pVedaString =(PackString *) m_pObjectToManage;
		CString ctxt;
		m_EditCtrl.GetWindowText(ctxt);
		ctxt.Replace("\r\n","\n");
		pVedaString->Set(ctxt.GetString() );
	}
	//CDialog_AbstractPackSerializable::OnOK();
}

void CDialog_PackStringMultiple::OnCancel()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	//CDialog_AbstractPackSerializable::OnCancel();
}
void CDialog_PackStringMultiple::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView													   
														   )
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);
	// cast veda type:
	PackString *pVedaString =(PackString *)_objToManage;
	// set the value of the veda string to the edit ctrl:
	m_EditCtrl.SetWindowText( pVedaString->Get() );


}

void CDialog_PackStringMultiple::OnEnKillfocusStringctrl()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
	if( m_pObjectToManage )
	{
		PackString *pVedaString =(PackString *) m_pObjectToManage;
		CString ctxt;
		m_EditCtrl.GetWindowText(ctxt);
		ctxt.Replace("\r\n","\n");
		pVedaString->Set(ctxt.GetString() );
	}
}
void	CDialog_PackStringMultiple::Update(void)
{
	PackString *pVedaString =(PackString *) m_pObjectToManage;
	if(!pVedaString)return;
	CString str;
	str.SetString(pVedaString->Get());
	str.Replace("\n","\r\n");
	// set the value to the edit ctrl:
	m_EditCtrl.SetWindowText(str.GetString() );

}