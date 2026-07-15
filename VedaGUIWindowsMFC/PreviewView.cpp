// FormViewBaseObject.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "PreviewView.h"
#include ".\formviewbaseobject.h"
#include "VedaDemoOGLMfcGuiDoc.h"

#include "BaseContext.h"
#include "ClassNode.h"

#include "Dialog_PackString.h"
#include "Dialog_PackULong.h"
#include "Dialog_PackULongFlags.h"
#include "Dialog_PackReference.h"
#include "Dialog_PackLong_WithLimits.h"
#include "Dialog_PackListDynamic.h"
#include ".\previewview.h"
// CPreviewView

#include <gl\gl.h>          // Header File For The OpenGL32 Library
#include <gl\glu.h>         // Header File For The GLu32 Library
#include <gl\glaux.h>       // Header File For The Glaux Library

IMPLEMENT_DYNCREATE(CPreviewView, CFormView)

CPreviewView::CPreviewView()
	: CFormView(CPreviewView::IDD)
{
}

CPreviewView::~CPreviewView()
{

}

void CPreviewView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	//	DDX_Control(pDX, IDC_OBJECTNAME, m_objname);
	DDX_Control(pDX, IDC_OBJECTNAME, m_ObjectName);
}

BEGIN_MESSAGE_MAP(CPreviewView, CFormView)
	ON_COMMAND(ID_UNDO, CPreviewView::OnUndo)
	ON_COMMAND(ID_REDO, CPreviewView::OnRedo)
	ON_COMMAND(ID_MERGE, CPreviewView::OnMerge)
	ON_WM_CREATE()

END_MESSAGE_MAP()


// Diagnostics CPreviewView

#ifdef _DEBUG
void CPreviewView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPreviewView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// Gestionnaires de messages CPreviewView

void CPreviewView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
/*
	m_ImgList.Create( IDC_B_RETURNSTOP,16,3,1);
*/
	/*
typedef struct {
    HIMAGELIST himl;
    RECT margin;
    UINT uAlign;
} BUTTON_IMAGELIST, *PBUTTON_IMAGELIST;
*/
/*	BUTTON_IMAGELIST iml;
	iml.himl = m_ImgList.m_hImageList ;
	iml.margin.top =0;
	iml.margin.left =0;
	iml.margin.right =16;
	iml.margin.bottom =16;
	iml.uAlign = BUTTON_IMAGELIST_ALIGN_CENTER;
	//CButton
	m_b_ResetPreview.SetImageList(  &iml );
	m_b_ResetPreview.Setim Bitmap(
	//CreateGLWindow( 640,400,32);
	*/
	// CBitmap
	//m_bm_ResetPreview.LoadBitmap(IDC_B_RETURNSTOP);
	//m_b_ResetPreview.SetBitmap( m_bm_ResetPreview.t );
	//m_b_ResetPreview.RedrawWindow();

}
void CPreviewView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
{	
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc) return;

	BaseObject *pobj = pdoc->GetPreviewSelectedBaseObject();
/*	if(pobj )
		m_objname.SetWindowText(pobj->GetName() );
	else
		m_objname.SetWindowText("oups?");
*/

	// manage veda updates:
	switch( pdoc->GetUpdateType() )
	{
		case BaseContext::eVedaUpdate_NewObject:
		case BaseContext::eVedaUpdate_ObjectDestroyed:
		{		
		
		}
		break;
		case BaseContext::eVedaUpdate_ObjectReCreated:
		case BaseContext::eVedaUpdate_ObjectClosed:
		{	
		}
		break;
		case BaseContext::eVedaUpdate_MemberChanged:
		case BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo:
		{

		}
		break;
	}


}

void CPreviewView::OnUndo()
{
	GetDocument()->GetVedaContext()->Undo(); 
}
void CPreviewView::OnRedo()
{
	GetDocument()->GetVedaContext()->Redo(); 
}
void CPreviewView::OnMerge()
{
	GetDocument()->OnMerge();	
}

void CPreviewView::OnDraw(CDC* /*pDC*/)
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();

	CRect rect;
	GetClientRect(rect);

	// fill the view with the specified color
	//CBrush br(pDoc->m_colorData);
	//pDC->FillRect(rect, &br);

}

// -------------------------------------------------------- 

// -------------------------------------------------------- 

int CPreviewView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO :  Ajoutez ici votre code de création spécialisé
	//CreateGLWindow( 640,400,32);
	return 0;
}
