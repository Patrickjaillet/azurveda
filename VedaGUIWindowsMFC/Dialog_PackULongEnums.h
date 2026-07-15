#pragma once


#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"
// Boîte de dialogue CDialog_PackULongEnums

class CDialog_PackULongEnums  : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackULongEnums)

public:
	CDialog_PackULongEnums(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialog_PackULongEnums();

// Données de boîte de dialogue
	enum { IDD = IDD_PACKULONG_ENUMS };
	static const char *GetManagedClassName(){ return("PackULong::PackULong_Enums");   };
	
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
	CComboBox m_ComboBox;
	afx_msg void OnCbnSelchangeComboenums();
};


