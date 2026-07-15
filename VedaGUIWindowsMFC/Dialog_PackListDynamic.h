#pragma once

#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"
// Boîte de dialogue CDialog_PackListDynamic

class CDialog_PackSerializable;

class CDialog_PackListDynamic : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackListDynamic)

public:
	CDialog_PackListDynamic(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialog_PackListDynamic();
	virtual	void	Update(void);
// Données de boîte de dialogue
	enum { IDD = IDD_PackListDynamic };
	static const char *GetManagedClassName(){ return("PackList");   };
	
	static CDialog_AbstractPackSerializable *NewInstance(CWnd* pParent);

	virtual void Create(CWnd* pParent = NULL);
	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
	CListCtrl m_listctrl;
	bool	m_ShapeInited;
	void	InitListShape( BaseType *_pObjectToShapeList  );
	void	SetElementEditor( BaseType *_pObjectToShape);
	virtual	void SetMemberName(	const char *_pMemberName );
	CDialog_PackSerializable	*m_pMemberEditor;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonPlus();
	afx_msg void OnBnClickedButtonMinus();
	afx_msg void OnDestroy();
	afx_msg void OnLvnItemActivateList1(NMHDR *pNMHDR, LRESULT *pResult);
	//CStatic m_MemberName;
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedButtonClone();
	afx_msg void OnBnClickedButtonSelectatptime();
	afx_msg void OnBnClickedButtonAddatptime();
	CButton m_BtUp;
	CButton m_BtDown;
	CButton m_BtClone;
	CButton m_BtSelectAtPreviewTime;
	CButton m_BtAddAtPreviewTime;
};
