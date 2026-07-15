#pragma once
#include "afxwin.h"


// Boîte de dialogue CDialogEditWithReturn

class CDialogEditWithReturn : public CDialog
{
	DECLARE_DYNAMIC(CDialogEditWithReturn)

public:
	CDialogEditWithReturn(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialogEditWithReturn();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edit;
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
