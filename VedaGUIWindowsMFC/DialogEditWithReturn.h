#pragma once
#include "afxwin.h"

class CDialogEditWithReturn : public CDialog
{
	DECLARE_DYNAMIC(CDialogEditWithReturn)

public:
	CDialogEditWithReturn(CWnd* pParent = NULL);
	virtual ~CDialogEditWithReturn();

	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edit;
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
