#pragma once

#include "Dialog_AbstractPackSerializable.h"
#include "Dialog_PackSerializable.h"
#include "afxwin.h"

class CDialog_PackDynamicType  : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackDynamicType)

public:
	CDialog_PackDynamicType(CWnd* pParent = NULL);
	virtual ~CDialog_PackDynamicType();

	enum { IDD = IDD_PACKDYNAMICTYPE };
	static const char *GetManagedClassName(){ return("PackDynamicType");   };

	static CDialog_AbstractPackSerializable *NewInstance(CWnd* pParent);

	virtual void Create(CWnd* pParent = NULL);
	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);

	virtual	void	Update(void);
protected:
	void	SetCurrentShape(BaseType *_pCurrentShape);
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
	BaseType		*m_pCurrentShapeObject;
	CDialog_PackSerializable	*m_pCurrentShapeDialog;
public:
	CComboBox m_ComboBox;
	afx_msg void OnCbnSelchangeComboenums();
	afx_msg void OnDestroy();
};
