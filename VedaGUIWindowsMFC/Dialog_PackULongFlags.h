#pragma once

#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"

class CDialog_PackULongFlags : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackULongFlags)

public:
	CDialog_PackULongFlags(CWnd* pParent = NULL);
	virtual ~CDialog_PackULongFlags();

	enum { IDD = IDD_PACKULONG_FLAGS };
	static const char *GetManagedClassName(){ return("PackULong::PackULong_Flags");   };
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
	unsigned int	m_NumberOfBitManaged;
public:
#define NBCHECKBOX	10
	CButton m_c[NBCHECKBOX];
	afx_msg void OnBnClickedCheck0();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnBnClickedCheck5();
	afx_msg void OnBnClickedCheck6();
	afx_msg void OnBnClickedCheck7();
	afx_msg void OnBnClickedCheck8();
	afx_msg void OnBnClickedCheck9();
};
