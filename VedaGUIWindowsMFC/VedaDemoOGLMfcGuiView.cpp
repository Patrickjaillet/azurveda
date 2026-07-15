// VedaDemoOGLMfcGuiView.cpp : implémentation de la classe CVedaDemoOGLMfcGuiView
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"

#include "VedaDemoOGLMfcGuiDoc.h"
#include "VedaDemoOGLMfcGuiView.h"
#include ".\vedademooglmfcguiview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVedaDemoOGLMfcGuiView

IMPLEMENT_DYNCREATE(CVedaDemoOGLMfcGuiView, CListView)

BEGIN_MESSAGE_MAP(CVedaDemoOGLMfcGuiView, CListView)
	ON_WM_STYLECHANGED()
END_MESSAGE_MAP()

// construction ou destruction de CVedaDemoOGLMfcGuiView

CVedaDemoOGLMfcGuiView::CVedaDemoOGLMfcGuiView()
{
	// TODO : ajoutez ici le code d'une construction

}

CVedaDemoOGLMfcGuiView::~CVedaDemoOGLMfcGuiView()
{
}

BOOL CVedaDemoOGLMfcGuiView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO : Changez ici la classe ou les styles Window en modifiant
	//  CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}

void CVedaDemoOGLMfcGuiView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO : Vous pouvez remplir votre contrôle ListView avec des éléments en accédant directement
	//  au contrôle List par un appel à GetListCtrl().
	GetListCtrl().SetExtendedStyle( LVS_EX_FULLROWSELECT |  LVS_EX_GRIDLINES );

	LV_COLUMN lvc;
	lvc.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH ;
	lvc.cx = 400 ;
	lvc.iSubItem = 1;
	lvc.pszText = _T("test");
	GetListCtrl().InsertColumn( 0 , &lvc );

	LVITEM lvi;
	int itemindex;

	lvi.iItem = 1; // nb list
	lvi.iSubItem = 0;
	lvi.pszText = _T("mfff");
	lvi.cchTextMax = (int) strlen(lvi.pszText);
	itemindex = GetListCtrl().InsertItem( &lvi );

	lvi.iItem = 1; // nb list
	lvi.iSubItem = 1;
	lvi.pszText = _T("mfff2");
	lvi.cchTextMax = (int) strlen(lvi.pszText);
	itemindex = GetListCtrl().SetItem( &lvi );
}


// diagnostics pour CVedaDemoOGLMfcGuiView

#ifdef _DEBUG
void CVedaDemoOGLMfcGuiView::AssertValid() const
{
	CListView::AssertValid();
}

void CVedaDemoOGLMfcGuiView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CVedaDemoOGLMfcGuiDoc* CVedaDemoOGLMfcGuiView::GetDocument() const // la version non déboguée est en ligne
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVedaDemoOGLMfcGuiDoc)));
	return (CVedaDemoOGLMfcGuiDoc*)m_pDocument;
}
#endif //_DEBUG


// gestionnaires de messages pour CVedaDemoOGLMfcGuiView
void CVedaDemoOGLMfcGuiView::OnStyleChanged(int /*nStyleType*/, LPSTYLESTRUCT /*lpStyleStruct*/)
{
	//TODO : ajoutez ici le code qui définit le comportement de votre application lorsque l'utilisateur change le style d'affichage de la fenêtre
	
	Default();
}

void CVedaDemoOGLMfcGuiView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	PackSerializableObject *pMemberEdited = (PackSerializableObject *)lHint;



}
