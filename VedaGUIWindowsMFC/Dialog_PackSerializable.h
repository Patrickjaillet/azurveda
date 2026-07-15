#pragma once
#include "Dialog_AbstractPackSerializable.h"

class CDialog_PackSerializable : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackSerializable)

public:
	CDialog_PackSerializable(CWnd* pParent = NULL);
	virtual ~CDialog_PackSerializable();

	enum { IDD = IDD_PACKSERIALIZABLE };
	static const char *GetManagedClassName(){ return("BaseType");   };
	static CDialog_AbstractPackSerializable *NewInstance(CWnd* pParent);
	virtual void Create(CWnd* pParent = NULL);
	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);

	virtual	void	Update(void);
	virtual	void SetMemberName(	const char *_pMemberName );

	void	RegisterPackDialog( const char *_pSuportedClass,CDialog_AbstractPackSerializable *(*_CreatorFunc)(CWnd *) );

	virtual	void	DestroyAllDialogs();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();

	CMapStringToPtr	m_PackDialogMap;

	CPtrList	m_CurrentDialogList;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};
