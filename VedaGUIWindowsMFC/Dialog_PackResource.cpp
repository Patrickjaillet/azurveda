// Dialog_PackResource.cpp : fichier d'implémentation
//

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
// Boîte de dialogue CDialog_PackResource

IMPLEMENT_DYNAMIC(CDialog_PackResource, CDialog)
CDialog_PackResource::CDialog_PackResource(CWnd* pParent /*=NULL*/)
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



// Gestionnaires de messages CDialog_PackResource

void CDialog_PackResource::OnOK()
{
	if( m_pObjectToManage )
	{
		OnEnKillfocusStringctrl();
	}
	//CDialog_AbstractPackSerializable::OnOK();
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
	// cast veda type:
	PackResource *pVedaString =(PackResource *)_objToManage;
	// set the value of the veda string to the edit ctrl:
	//re m_EditCtrl.SetWindowText( pVedaString->Get() );


}

void CDialog_PackResource::OnEnKillfocusStringctrl()
{
	PackResource *pVedaRes =(PackResource *) m_pObjectToManage;
	if(!pVedaRes)return;
	if(!m_updated) return;
	CString ctxt;
	m_EditCtrl.GetWindowText(ctxt);
	pVedaRes->SetFile(ctxt.GetString() );
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
	if( m_pObjectToManage )
	{/*re
		PackResource *pVedaString =(PackResource *) m_pObjectToManage;
		CString ctxt;
		m_EditCtrl.GetWindowText(ctxt);
		pVedaString->Set(ctxt.GetString() );*/
	}
}
void	CDialog_PackResource::Update(void)
{
	PackResource *pVedaRes =(PackResource *) m_pObjectToManage;
	if(!pVedaRes)return;
	// set the value to the edit ctrl:
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
	// ask a file
	CString cFileFilter="*.";
	/*const char *pFileFilter = pVedaRes->GetFileExtension(); // can be nul.
	if(pFileFilter)
	{
		cFileFilter += pFileFilter;
		pFileFilter = cFileFilter.GetString();
	}*/
	CFileDialog	cdiag(true,0,0,0,0,/*this*/0L,0);

	if(cdiag.DoModal() == IDCANCEL ) return;
	// get app dir:
	char    szAppPath[MAX_PATH] = "";
	CString strAppDirectory;
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
	// Extract directory
	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));
	::SetCurrentDirectory(strAppDirectory);

	//CWinApp
	//AfxGetApp()->
	//CString filename = cdiag.GetFileName();

	pVedaRes->SetFile( cdiag.GetPathName() );
	//CFile	cf;
	//cf.Open( filename.GetString() , CFile::modeRead );
	//unsigned int ll = (unsigned int)( cf.GetLength() );
	//unsigned char *pbuf = new unsigned char[ll];
	/*if( pbuf )
	{
		//cf.Read(pbuf,ll);
		//m_pContextManaged->MergeSerialize_In(pbuf);
		//delete [] pbuf;
	}*/
}

BOOL CDialog_PackResource::OnInitDialog()
{
	CDialog_AbstractPackSerializable::OnInitDialog();

	m_Bm_Open.LoadBitmap( IDC_B_FILEOPEN );
	m_FileProp.SetBitmap( (HBITMAP)m_Bm_Open.m_hObject );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
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
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

	CDialog_AbstractPackSerializable::OnDropFiles(hDropInfo);
}
