
#pragma once

class CVedaDemoOGLMfcGuiView : public CListView
{
protected:
	CVedaDemoOGLMfcGuiView();
	DECLARE_DYNCREATE(CVedaDemoOGLMfcGuiView)

public:
	CVedaDemoOGLMfcGuiDoc* GetDocument() const;

public:

	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate();

public:
	virtual ~CVedaDemoOGLMfcGuiView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

protected:
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* , LPARAM , CObject* );
};

#ifndef _DEBUG
inline CVedaDemoOGLMfcGuiDoc* CVedaDemoOGLMfcGuiView::GetDocument() const
   { return reinterpret_cast<CVedaDemoOGLMfcGuiDoc*>(m_pDocument); }
#endif
