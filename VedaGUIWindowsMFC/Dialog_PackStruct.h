#pragma once

#include "Dialog_AbstractPackSerializable.h"

class CDialog_PackStruct : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackStruct)

public:
	CDialog_PackStruct(CWnd* pParent = NULL);
	virtual ~CDialog_PackStruct();

	enum { IDD = IDD_PackStruct };
	virtual void Create(CWnd* pParent = NULL);
	virtual	void	Update(void);
	static const char *GetManagedClassName(){ return("PackStruct");   };
	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);
	static CDialog_AbstractPackSerializable *NewInstance(CWnd* pParent);

	virtual	void	DestroyAllDialogs();

	void	RegisterPackDialog( const char *_pSuportedClass,CDialog_AbstractPackSerializable *(*_CreatorFunc)(CWnd *) );
	virtual	void SetMemberName(	const char *_pMemberName );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	CMapStringToPtr	m_PackDialogMap;

	CPtrList	m_CurrentDialogList;
	CPtrList	m_CurrentLabelMemberList;

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
};
