
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackResource.h"
#include ".\dialog_PackResource.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "PackResource.h"
#include <stdlib.h>

void CDialog_PackResource::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PACKRESOURCE,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackResource::NewInstance(CWnd* pParent)
{
	return new CDialog_PackResource(pParent);

}

IMPLEMENT_DYNAMIC(CDialog_PackResource, CDialog)
CDialog_PackResource::CDialog_PackResource(CWnd* pParent )
	: CDialog_AbstractPackSerializable(CDialog_PackResource::IDD, pParent)
		,m_updated(false)
{
}

CDialog_PackResource::~CDialog_PackResource()
{
}

void CDialog_PackResource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_StringCtrl, m_EditCtrl);
	DDX_Control(pDX, IDC_FILEPROP, m_FileProp);
	DDX_Control(pDX, IDC_CHECK1, m_LinkCheckBox);
}

BEGIN_MESSAGE_MAP(CDialog_PackResource, CDialog)
	ON_EN_KILLFOCUS(IDC_StringCtrl, OnEnKillfocusStringctrl)
	ON_BN_CLICKED(IDC_FILEPROP, OnBnClickedFileprop)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

void CDialog_PackResource::OnOK()
{
	if( m_pObjectToManage )
	{
		OnEnKillfocusStringctrl();
	}

}

void CDialog_PackResource::OnCancel()
{
	if( m_pObjectToManage )
	{
		OnEnKillfocusStringctrl();
	}
}
void CDialog_PackResource::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
														   )
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);

}

void CDialog_PackResource::OnEnKillfocusStringctrl()
{
	PackResource *pVedaRes =(PackResource *) m_pObjectToManage;
	if(!pVedaRes)return;
	if(!m_updated) return;
	CString ctxt;
	m_EditCtrl.GetWindowText(ctxt);
	pVedaRes->SetFile(ctxt.GetString() );

	if( m_pObjectToManage )
	{
	}
}
void	CDialog_PackResource::Update(void)
{
	PackResource *pVedaRes =(PackResource *) m_pObjectToManage;
	if(!pVedaRes)return;

	if(pVedaRes->GetFilePath())
		m_EditCtrl.SetWindowText(pVedaRes->GetFilePath() );
	else
		m_EditCtrl.SetWindowText("");

	if( pVedaRes->GetLinkMode() )
			m_LinkCheckBox.SetCheck( BST_CHECKED );
		else
			m_LinkCheckBox.SetCheck( BST_UNCHECKED );
	m_updated = true;
}
void CDialog_PackResource::OnBnClickedFileprop()
{
	PackResource *pVedaRes =(PackResource *) m_pObjectToManage;
	if(!pVedaRes)return;

	CString cFileFilter="*.";

	CFileDialog	cdiag(true,0,0,0,0,0L,0);

	if(cdiag.DoModal() == IDCANCEL ) return;

	char    szAppPath[MAX_PATH] = "";
	CString strAppDirectory;
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));
	::SetCurrentDirectory(strAppDirectory);

	pVedaRes->SetFile( cdiag.GetPathName() );

}

BOOL CDialog_PackResource::OnInitDialog()
{
	CDialog_AbstractPackSerializable::OnInitDialog();

	m_Bm_Open.LoadBitmap( IDC_B_FILEOPEN );
	m_FileProp.SetBitmap( (HBITMAP)m_Bm_Open.m_hObject );

	return TRUE;

}
void CDialog_PackResource::OnBnClickedCheck()
{
	PackResource *pVedaRes =(PackResource *) m_pObjectToManage;
	if(!pVedaRes)return;
	if(pVedaRes->GetLinkMode())
		pVedaRes->SetLinkMode(false);
	else
		pVedaRes->SetLinkMode(true);
}
void CDialog_PackResource::OnDropFiles(HDROP hDropInfo)
{

	CDialog_AbstractPackSerializable::OnDropFiles(hDropInfo);
}
