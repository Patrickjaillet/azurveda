#pragma once

#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"
#include "afxcmn.h"

class CDialog_PackULong : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackULong)

public:
	CDialog_PackULong(CWnd* pParent = NULL);
	virtual ~CDialog_PackULong();

	enum { IDD = IDD_PACKULONG };

	static const char *GetManagedClassName(){ return("PackULong");   };
	static CDialog_AbstractPackSerializable *NewInstance(CWnd* pParent);

	virtual void Create(CWnd* pParent = NULL);
	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);
	virtual	void	Update(void);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	CEdit m_editCtrl;
	CSpinButtonCtrl m_Spin;
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnEnKillfocusEdit1();
};
