#pragma once
#include "Dialog_AbstractPackSerializable.h"
// Boîte de dialogue CDialog_PackStruct

class CDialog_PackSerializable : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackSerializable)

public:
	CDialog_PackSerializable(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialog_PackSerializable();

// Données de boîte de dialogue
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
	//! enregistre les gadgets capable d'editer les membres: 
	void	RegisterPackDialog( const char *_pSuportedClass,CDialog_AbstractPackSerializable *(*_CreatorFunc)(CWnd *) );
	// if not done:
	virtual	void	DestroyAllDialogs();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();

	// the known possible dialogs constructors:
	CMapStringToPtr	m_PackDialogMap;
	// the base of the current built dialogs:
	CPtrList	m_CurrentDialogList;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};
