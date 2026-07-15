// VedaDemoOGLMfcGuiView.h : interface de la classe CVedaDemoOGLMfcGuiView
//


#pragma once


class CVedaDemoOGLMfcGuiView : public CListView
{
protected: // création à partir de la sérialisation uniquement
	CVedaDemoOGLMfcGuiView();
	DECLARE_DYNCREATE(CVedaDemoOGLMfcGuiView)

// Attributs
public:
	CVedaDemoOGLMfcGuiDoc* GetDocument() const;

// Opérations
public:

// Substitutions
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // premier appel après la construction

// Implémentation
public:
	virtual ~CVedaDemoOGLMfcGuiView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions générées de la table des messages
protected:
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};

#ifndef _DEBUG  // version de débogage dans VedaDemoOGLMfcGuiView.cpp
inline CVedaDemoOGLMfcGuiDoc* CVedaDemoOGLMfcGuiView::GetDocument() const
   { return reinterpret_cast<CVedaDemoOGLMfcGuiDoc*>(m_pDocument); }
#endif

