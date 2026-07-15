
#pragma once

class CVedaDemoOGLMfcGuiDoc;
class ClassNode;

class CLeftView : public CTreeView
{
protected:
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

public:
	CVedaDemoOGLMfcGuiDoc* GetDocument();

public:

	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate();

public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void	InsertClassNode( const ClassNode *_pClassNode, HTREEITEM _fatheritem, CString &_classlevel );
	CImageList	m_ImgList;
	CToolTipCtrl	m_ToolTipCtrl;

protected:
	DECLARE_MESSAGE_MAP()

	CMapPtrToPtr	m_ClassNodeMap;
	CMapPtrToPtr	m_BaseObjectMap;
	virtual void OnUpdate(CView* , LPARAM , CObject* );
public:
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNewobject();
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBaseobjectDestroy();
	afx_msg void OnDestroyIndependants();
	afx_msg void OnBaseobjectClone();
	afx_msg void OnMpreview();
	afx_msg void OnPreviewPrevious();
	afx_msg void OnEditPreview();
	afx_msg void OnEditPrevious();
	BOOL OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	void OnTvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:

};

#ifndef _DEBUG
inline CVedaDemoOGLMfcGuiDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CVedaDemoOGLMfcGuiDoc*>(m_pDocument); }
#endif
