#pragma once

#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"
// Boîte de dialogue CDialog_PackString

class CDialog_PackString : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackString)

public:
	CDialog_PackString(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialog_PackString();

// Données de boîte de dialogue
	enum { IDD = IDD_PACKSTRING };

	static const char *GetManagedClassName(){ return("PackString");   };
	
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
