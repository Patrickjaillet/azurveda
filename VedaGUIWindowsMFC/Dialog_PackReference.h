#pragma once

#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"

class ClassNode;

class CDialog_PackReference : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackReference)

public:
	CDialog_PackReference(CWnd* pParent = NULL);
	virtual ~CDialog_PackReference();

	enum { IDD = IDD_PACKREFERENCE };
static const char *GetManagedClassName(){ return("PackObjectReference");   };

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
	virtual void OnCancel();
	virtual void OnOK();

	void	UpdateComboList();
	void	AddClassObjectsInCombo( const ClassNode *_pFatherClassNode);
	void	InsertNewClassNode( const ClassNode *_pClassNode );

public:
	bool m_isBeingUpdated;
	CStatic m_SelectedName;
	CComboBox m_ObjectCombo;
	afx_msg void OnCbnSelchangeObjectcombo();
	afx_msg void OnBnClickedSelectedname();
	CButton m_PreviewButton;
	CMapPtrToWord	m_IndexMap;
	afx_msg void OnBnClickedPreview();

};
