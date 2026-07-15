#pragma once

#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"
// Boîte de dialogue CDialog_PackResource

class CDialog_PackResource : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackResource)

public:
	CDialog_PackResource(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialog_PackResource();

// Données de boîte de dialogue
	enum { IDD = IDD_PACKSTRING };

	static const char *GetManagedClassName(){ return("PackStruct::PackResource");   };
	
	static CDialog_AbstractPackSerializable *NewInstance(CWnd* pParent);

	virtual void Create(CWnd* pParent = NULL);
	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);
	virtual	void	Update(void);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	CBitmap	m_Bm_Open;
	CEdit m_EditCtrl;
	bool	m_updated;
	CButton	m_FileProp;
	CButton m_LinkCheckBox;
	afx_msg void OnBnClickedCheck();
	afx_msg void OnEnKillfocusStringctrl();
	afx_msg void OnBnClickedFileprop();
	virtual BOOL OnInitDialog();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
