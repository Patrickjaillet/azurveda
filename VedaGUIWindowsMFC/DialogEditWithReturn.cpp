
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "DialogEditWithReturn.h"
#include ".\dialogeditwithreturn.h"

IMPLEMENT_DYNAMIC(CDialogEditWithReturn, CDialog)
CDialogEditWithReturn::CDialogEditWithReturn(CWnd* pParent )
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

void CDialogEditWithReturn::OnOK()
{

}

void CDialogEditWithReturn::OnCancel()
{

}
