#pragma once

#include "Dialog_AbstractPackSerializable.h"

class CDialog_PackStringMultiple : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackStringMultiple)

public:
	CDialog_PackStringMultiple(CWnd* pParent = NULL);
	virtual ~CDialog_PackStringMultiple();

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
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	CEdit m_EditCtrl;
	afx_msg void OnEnKillfocusStringctrl();
};
