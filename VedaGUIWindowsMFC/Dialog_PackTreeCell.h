#pragma once

#include "Dialog_AbstractPackSerializable.h"
#include "afxcmn.h"

class PackTreeCell;
class BaseType;
class CDialog_PackSerializable;
class CDialog_PackTreeCell : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackTreeCell)

public:
	CDialog_PackTreeCell(CWnd* pParent = NULL);
	virtual ~CDialog_PackTreeCell();

	enum { IDD = IDD_PACKTREECELL };
	static const char *GetManagedClassName(){ return("PackTreeCell");   };
	static CDialog_AbstractPackSerializable *NewInstance(CWnd* pParent);

	virtual void Create(CWnd* pParent = NULL);
	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);
	virtual	void	Update(void);
	void		FillTreeWithCell(PackTreeCell *_pCell,HTREEITEM treeitem);
	void		SetElementEditor( BaseType *_pObjectToShape);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();

	CDialog_PackSerializable	*m_pMemberEditor;
	PackTreeCell				*m_pCurrentSelection;

	CMapPtrToPtr			m_MapTreeCellToItem;
public:
	virtual BOOL OnInitDialog();
	CTreeCtrl m_treectrl;
	afx_msg void OnBnClickedButtonPlus();
	afx_msg void OnBnClickedButtonMinus();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
};
