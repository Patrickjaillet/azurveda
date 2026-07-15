#pragma once

#include "Dialog_AbstractPackSerializable.h"
// Boîte de dialogue CDialog_PackStringMultiple

class CDialog_PackStringMultiple : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackStringMultiple)

public:
	CDialog_PackStringMultiple(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialog_PackStringMultiple();

// Données de boîte de dialogue
	enum { IDD = IDD_PACKSTRINGMULTIPLE };

	static const char *GetManagedClassName(){ return("PackString::PackString_MultipleLines");   };
	
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
	CEdit m_EditCtrl;
	afx_msg void OnEnKillfocusStringctrl();
};
