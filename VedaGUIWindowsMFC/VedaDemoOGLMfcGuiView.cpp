
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"

#include "VedaDemoOGLMfcGuiDoc.h"
#include "VedaDemoOGLMfcGuiView.h"
#include ".\vedademooglmfcguiview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CVedaDemoOGLMfcGuiView, CListView)

BEGIN_MESSAGE_MAP(CVedaDemoOGLMfcGuiView, CListView)
	ON_WM_STYLECHANGED()
END_MESSAGE_MAP()

CVedaDemoOGLMfcGuiView::CVedaDemoOGLMfcGuiView()
{

}

CVedaDemoOGLMfcGuiView::~CVedaDemoOGLMfcGuiView()
{
}

BOOL CVedaDemoOGLMfcGuiView::PreCreateWindow(CREATESTRUCT& cs)
{

	return CListView::PreCreateWindow(cs);
}

void CVedaDemoOGLMfcGuiView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	GetListCtrl().SetExtendedStyle( LVS_EX_FULLROWSELECT |  LVS_EX_GRIDLINES );

	LV_COLUMN lvc;
	lvc.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH ;
	lvc.cx = 400 ;
	lvc.iSubItem = 1;
	lvc.pszText = _T("test");
	GetListCtrl().InsertColumn( 0 , &lvc );

	LVITEM lvi;
	int itemindex;

	lvi.iItem = 1;
	lvi.iSubItem = 0;
	lvi.pszText = _T("mfff");
	lvi.cchTextMax = (int) strlen(lvi.pszText);
	itemindex = GetListCtrl().InsertItem( &lvi );

	lvi.iItem = 1;
	lvi.iSubItem = 1;
	lvi.pszText = _T("mfff2");
	lvi.cchTextMax = (int) strlen(lvi.pszText);
	itemindex = GetListCtrl().SetItem( &lvi );
}

#ifdef _DEBUG
void CVedaDemoOGLMfcGuiView::AssertValid() const
{
	CListView::AssertValid();
}

void CVedaDemoOGLMfcGuiView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CVedaDemoOGLMfcGuiDoc* CVedaDemoOGLMfcGuiView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVedaDemoOGLMfcGuiDoc)));
	return (CVedaDemoOGLMfcGuiDoc*)m_pDocument;
}
#endif

void CVedaDemoOGLMfcGuiView::OnStyleChanged(int , LPSTYLESTRUCT )
{

	Default();
}

void CVedaDemoOGLMfcGuiView::OnUpdate(CView* , LPARAM lHint, CObject* )
{
	PackSerializableObject *pMemberEdited = (PackSerializableObject *)lHint;

}
