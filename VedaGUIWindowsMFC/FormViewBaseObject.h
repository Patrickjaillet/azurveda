#pragma once
#include "afxwin.h"



// Mode formulaire CFormViewBaseObject

class BaseObject;
class CVedaDemoOGLMfcGuiDoc;
class CDialog_AbstractPackSerializable;
class CDialog_PackStruct;
class CDialog_PackSerializable;

class CFormViewBaseObject : public CFormView
{
	DECLARE_DYNCREATE(CFormViewBaseObject)

protected:
	CFormViewBaseObject();           // constructeur protégé utilisé par la création dynamique
	virtual ~CFormViewBaseObject();

public:
	enum { IDD = IDD_FORMVIEWBASEOBJECT };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
inline CVedaDemoOGLMfcGuiDoc* GetDocument()
   { return reinterpret_cast<CVedaDemoOGLMfcGuiDoc*>(m_pDocument); }
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnMerge();
protected:
	bool m_initied;
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV
	DECLARE_MESSAGE_MAP()
	void	OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	// the object dialog as a PackStruct:
	CDialog_PackStruct	*m_pBaseObjectDialog;
	//! enregistre les gadgets capable d'editer les membres: 
	//void	RegisterPackDialog( const char *_pSuportedClass,CDialog_AbstractPackSerializable *(*_CreatorFunc)(CWnd *) );

	// the known possible dialogs constructors:
	//CMapStringToPtr	m_PackDialogMap;

	// the base of the current built dialogs:
	//CPtrList	m_CurrentDialogList;
	//CPtrList	m_CurrentLabelMemberList;
	// the object for which the view is currently shaped:
	BaseObject *m_pViewShapedForMe;
public:
	//! this bool is used to forbid double recursion in some rare case:
	//! an update can generate a recursive update, like for exemple,
	//! a form deletion can throw a "Edit KillFocus"-> a set -> another update.
	bool			m_ForbidUpdateRecursion;
	virtual void	ShapeGUIForBaseObject( BaseObject *_pBaseObject);
	virtual	void	DestroyAllDialogs();

	CStatic m_ClassInfoText;
	afx_msg void OnBnClickedPreviewob();
	CButton m_ButtonPreview;
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMpreview();
	afx_msg void OnPreviewPrevious();
	afx_msg void OnEditPreview();
	afx_msg void OnEditPrevious();
	afx_msg void OnBaseobjectClone();
	afx_msg void OnBaseobjectDestroy();
	afx_msg void OnBaseobjectDestroyindependant();
	CStatic m_InfoLine;
	static	const	unsigned int	m_NumberOfButton=5;
	CButton m_BtToolFunction[m_NumberOfButton];
	unsigned int	m_ToolFunctionID[m_NumberOfButton];
	afx_msg void OnBnClickedToolfuncbt1();
	afx_msg void OnBnClickedToolfuncbt2();
	afx_msg void OnBnClickedToolfuncbt3();
	afx_msg void OnBnClickedToolfuncbt4();
	afx_msg void OnBnClickedToolfuncbt5();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};


