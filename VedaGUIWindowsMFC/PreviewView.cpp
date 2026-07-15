
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

#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

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

	DDX_Control(pDX, IDC_OBJECTNAME, m_ObjectName);
}

BEGIN_MESSAGE_MAP(CPreviewView, CFormView)
	ON_COMMAND(ID_UNDO, CPreviewView::OnUndo)
	ON_COMMAND(ID_REDO, CPreviewView::OnRedo)
	ON_COMMAND(ID_MERGE, CPreviewView::OnMerge)
	ON_WM_CREATE()

END_MESSAGE_MAP()

#ifdef _DEBUG
void CPreviewView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPreviewView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif

void CPreviewView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

}
void CPreviewView::OnUpdate(CView* , LPARAM lHint, CObject* pHint)
{
	CVedaDemoOGLMfcGuiDoc *pdoc = (CVedaDemoOGLMfcGuiDoc *)GetDocument();
	if( !pdoc) return;

	BaseObject *pobj = pdoc->GetPreviewSelectedBaseObject();

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

void CPreviewView::OnDraw(CDC* )
{

	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();

	CRect rect;
	GetClientRect(rect);

}

int CPreviewView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
