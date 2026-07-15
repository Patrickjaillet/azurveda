
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackTreeCell.h"
#include ".\dialog_packtreecell.h"
#include "PackTreeCell.h"
#include "Dialog_AbstractPackSerializable.h"
#include "Dialog_PackSerializable.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "BaseContext.h"

void CDialog_PackTreeCell::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PACKTREECELL,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackTreeCell::NewInstance(CWnd* pParent)
{
	return new CDialog_PackTreeCell(pParent);

}

IMPLEMENT_DYNAMIC(CDialog_PackTreeCell, CDialog)
CDialog_PackTreeCell::CDialog_PackTreeCell(CWnd* pParent )
	: CDialog_AbstractPackSerializable(CDialog_PackTreeCell::IDD, pParent)
	,m_pMemberEditor(NULL)
	,m_pCurrentSelection(NULL)

{
}

CDialog_PackTreeCell::~CDialog_PackTreeCell()
{
}

void CDialog_PackTreeCell::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_TREE1, m_treectrl);
}

BEGIN_MESSAGE_MAP(CDialog_PackTreeCell, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PLUS, OnBnClickedButtonPlus)
	ON_BN_CLICKED(IDC_BUTTON_MINUS, OnBnClickedButtonMinus)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CDialog_PackTreeCell::OnOK()
{

}

void CDialog_PackTreeCell::OnCancel()
{

}
void CDialog_PackTreeCell::SetPackSerializable(	BaseType *_objToManage,
									CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
									)
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);

	Update();

}
void	CDialog_PackTreeCell::Update(void)
{

	m_treectrl.DeleteAllItems();
	m_MapTreeCellToItem.RemoveAll();

	PackTreeCell *pCell =(PackTreeCell *)m_pObjectToManage;
	FillTreeWithCell(pCell, TVI_ROOT);
	if(m_pCurrentSelection == 0L) m_pCurrentSelection = pCell;

	if(m_pCurrentSelection)
	{
		HTREEITEM cellitem = (HTREEITEM) m_MapTreeCellToItem.GetValueAt((void *)m_pCurrentSelection );
		m_treectrl.SelectItem( cellitem );
		m_treectrl.EnsureVisible(cellitem);
	}

	if(!m_pMemberEditor)
	{
		SetElementEditor( m_pCurrentSelection->GetObject() );
	}

	if(m_pMemberEditor) m_pMemberEditor->Update();

	CRect listrec;
	m_treectrl.GetClientRect(&listrec);
	m_treectrl.ClientToScreen(&listrec);
	ScreenToClient(&listrec);
	CRect editorrec(0,0,1,1);

	if(m_pMemberEditor)
	{
		m_pMemberEditor->GetClientRect(&editorrec);
		m_pMemberEditor->MoveWindow( listrec.left, listrec.bottom+5 ,editorrec.Width() ,editorrec.Height());
		m_pMemberEditor->GetWindowRect( &editorrec);

	}
	int twidth =listrec.Width();
	if(editorrec.Width()>twidth) twidth = editorrec.Width();

	Resize(twidth,listrec.bottom + editorrec.Height()+9);

}
void	CDialog_PackTreeCell::FillTreeWithCell(PackTreeCell *_pCell,HTREEITEM treeitem)
{

	BaseType *pCellObject = _pCell->GetObject();

	HTREEITEM cellitem =
		m_treectrl.InsertItem(pCellObject->ValueToString(),0,0,treeitem);
	m_treectrl.SetItemData( cellitem, (DWORD_PTR)_pCell);
	m_MapTreeCellToItem.SetAt((void*)_pCell,(void *)cellitem);

	PackTreeCell *pSubcell = _pCell->GetFirstSubCell();
	while(pSubcell)
	{
		FillTreeWithCell(pSubcell,cellitem);
		pSubcell = pSubcell->GetNextBrotherCell();
	}
	m_treectrl.Expand( cellitem, TVE_EXPAND   );

}
BOOL CDialog_PackTreeCell::OnInitDialog()
{
	CDialog_AbstractPackSerializable::OnInitDialog();

	m_treectrl.ModifyStyle(0,TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES | TVS_SHOWSELALWAYS );

	return TRUE;

}

void CDialog_PackTreeCell::OnBnClickedButtonPlus()
{

	if( !m_pCurrentSelection ) return;

	m_pCurrentSelection->AddSubCellToThisCell();

}

void CDialog_PackTreeCell::OnBnClickedButtonMinus()
{
	if( !m_pCurrentSelection ) return;

	if( m_pCurrentSelection == m_pObjectToManage) return;
	PackTreeCell *pToDestroy =  m_pCurrentSelection;

	m_pCurrentSelection = 0L;
	SetElementEditor(0L);
	PackTreeCell *pCell =(PackTreeCell *)m_pObjectToManage;
	pCell->DeleteSubCells(pToDestroy);

}

void CDialog_PackTreeCell::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	*pResult = 0;
	UINT action = pNMTreeView->action ;
	if( !(action == TVC_BYMOUSE ||action == TVC_BYKEYBOARD)  )
	{

		return;
	}

	HTREEITEM nItem = pNMTreeView->itemNew.hItem ;

	PackTreeCell *pCell = (PackTreeCell *)	m_treectrl.GetItemData( nItem );
	if( !pCell ) return;
	BaseType *peltobj = pCell->GetObject();
	if( !peltobj) return;

	m_pCurrentSelection = pCell;

	SetElementEditor( peltobj );

	m_pDoc->SetUpdateType(BaseContext::eVedaUpdate_MemberChanged);

	m_pDoc->SetObjectUpdated(peltobj->GetObjectThatManagesThis() );
	m_pDoc->SetMemberUpdated(peltobj);
	m_pDoc->UpdateAllViews(NULL);

}
void	CDialog_PackTreeCell::SetElementEditor( BaseType *_pObjectToShape)
{
	if( m_pMemberEditor)
	{
		m_pMemberEditor->DestroyWindow();
		delete m_pMemberEditor ;
		m_pMemberEditor = NULL ;
	}
	if(_pObjectToShape == NULL) return;

	m_pMemberEditor = new CDialog_PackSerializable( this);
	if( m_pMemberEditor)
	{
		m_pMemberEditor->Create(  this);
		m_pMemberEditor->ModifyStyle(0, WS_BORDER);
		m_pMemberEditor->SetPackSerializable(_pObjectToShape,m_pDoc,m_pView);

		m_pMemberEditor->ShowWindow(SW_SHOW);
	}
	Update();

}
void CDialog_PackTreeCell::OnDestroy()
{
	if( m_pMemberEditor)
	{
		m_pMemberEditor->DestroyWindow();
		delete m_pMemberEditor ;
		m_pMemberEditor = NULL ;
	}
	CDialog_AbstractPackSerializable::OnDestroy();

}
