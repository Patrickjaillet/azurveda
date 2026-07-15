#pragma once
#include "afxwin.h"


// Mode formulaire CPreviewView

class BaseObject;
class CVedaDemoOGLMfcGuiDoc;
class CDialog_AbstractPackSerializable;

class CPreviewView : public CFormView
{
	DECLARE_DYNCREATE(CPreviewView)

protected:
	CPreviewView();           // constructeur protégé utilisé par la création dynamique
	virtual ~CPreviewView();

public:
	enum { IDD = IDD_FORMVIEWPREVIEW };
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
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV
	DECLARE_MESSAGE_MAP()
	void	OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	// the object for which the view is currently shaped:
	BaseObject *m_pObjectPreView;

public:


protected:
	virtual void OnDraw(CDC* /*pDC*/);
	CImageList	m_ImgList; 

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	CStatic m_objname;
	CStatic m_ObjectName;
};


