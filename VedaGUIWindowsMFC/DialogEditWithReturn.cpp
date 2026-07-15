// DialogEditWithReturn.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "DialogEditWithReturn.h"
#include ".\dialogeditwithreturn.h"


// Boîte de dialogue CDialogEditWithReturn

IMPLEMENT_DYNAMIC(CDialogEditWithReturn, CDialog)
CDialogEditWithReturn::CDialogEditWithReturn(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogEditWithReturn::IDD, pParent)
{
}

CDialogEditWithReturn::~CDialogEditWithReturn()
{
}

void CDialogEditWithReturn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITDLG1, m_edit);
}


BEGIN_MESSAGE_MAP(CDialogEditWithReturn, CDialog)
END_MESSAGE_MAP()


// Gestionnaires de messages CDialogEditWithReturn

void CDialogEditWithReturn::OnOK()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	//CDialog::OnOK();
}

void CDialogEditWithReturn::OnCancel()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	//CDialog::OnCancel();
}
