// Dialog_PackTreeCell.cpp : fichier d'implémentation
//

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

// Boîte de dialogue CDialog_PackTreeCell

IMPLEMENT_DYNAMIC(CDialog_PackTreeCell, CDialog)
CDialog_PackTreeCell::CDialog_PackTreeCell(CWnd* pParent /*=NULL*/)
	: CDialog_AbstractPackSerializable(CDialog_PackTreeCell::IDD, pParent)
	,m_pMemberEditor(NULL)
	,m_pCurrentSelection(NULL)
//	,m_CurrentSelectionItem(0)
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


// Gestionnaires de messages CDialog_PackTreeCell

void CDialog_PackTreeCell::OnOK()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base
	//CDialog::OnOK();
}

void CDialog_PackTreeCell::OnCancel()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base
	//CDialog::OnCancel();
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

	//re fill tree:
	m_treectrl.DeleteAllItems();
	m_MapTreeCellToItem.RemoveAll();

	PackTreeCell *pCell =(PackTreeCell *)m_pObjectToManage;
	FillTreeWithCell(pCell, TVI_ROOT);
	if(m_pCurrentSelection == 0L) m_pCurrentSelection = pCell;
	// reselect last selected:
	if(m_pCurrentSelection)
	{ // CTreeCtrl
		HTREEITEM cellitem = (HTREEITEM) m_MapTreeCellToItem.GetValueAt((void *)m_pCurrentSelection );
		m_treectrl.SelectItem( cellitem );
		m_treectrl.EnsureVisible(cellitem);
	}
		//m_treectrl.SelectItem(m_CurrentSelectionItem);
		//m_treectrl.SetItem(m_CurrentSelectionItem,TVIF_STATE,NULL,0,0,TVIS_SELECTED,TVIS_SELECTED,NULL );


/*
	// Select the item that is at the point myPoint.
	GetTreeCtrl().ScreenToClient( &point );
	UINT uFlags;
	HTREEITEM hItem = GetTreeCtrl().HitTest(point, &uFlags);
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
	   GetTreeCtrl().Select(hItem, TVGN_CARET);
	}
	if(hItem== NULL) return; 
	GetTreeCtrl().ClientToScreen( &point );
	HTREEITEM nItem = hItem;
*/
	if(!m_pMemberEditor)
	{
		SetElementEditor( m_pCurrentSelection->GetObject() );
	}
	// send update to editor window:
	if(m_pMemberEditor) m_pMemberEditor->Update();

	// set editor window size 
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
		//m_pMemberEditor->ShowWindow( SW_HIDE );
	}
	int twidth =listrec.Width();
	if(editorrec.Width()>twidth) twidth = editorrec.Width();
	// reset size of this component:
	Resize(twidth,listrec.bottom + editorrec.Height()+9);


}
void	CDialog_PackTreeCell::FillTreeWithCell(PackTreeCell *_pCell,HTREEITEM treeitem)
{	
	//item =GetTreeCtrl().InsertItem(  _pClassNode->GetDisplayClassName() ,2,2,_fatheritem,TVI_LAST);
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
	//cs.style |= TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES | TVS_SHOWSELALWAYS  ;


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialog_PackTreeCell::OnBnClickedButtonPlus()
{
	// chope item selectionné:
	//HTREEITEM nItem = m_treectrl.GetNextItem(TVI_ROOT, TVGN_CARET );
	//if(nItem == NULL)	return;
	if( !m_pCurrentSelection ) return;
	/*
	PackTreeCell *pcell = (PackTreeCell *) m_treectrl.GetItemData(nItem);
	if(!pcell) return;
*/
	m_pCurrentSelection->AddSubCellToThisCell();
	
	//Update(); // veda shoud send update -> remove.
}

void CDialog_PackTreeCell::OnBnClickedButtonMinus()
{
	if( !m_pCurrentSelection ) return;
	// we can't delete root cell:
	if( m_pCurrentSelection == m_pObjectToManage) return;
	PackTreeCell *pToDestroy =  m_pCurrentSelection;
	// it is not selected anymore.
	m_pCurrentSelection = 0L;
	SetElementEditor(0L);
	PackTreeCell *pCell =(PackTreeCell *)m_pObjectToManage;
	pCell->DeleteSubCells(pToDestroy);


}

void CDialog_PackTreeCell::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TVN_SELCHANGED
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	//pNMTreeView->itemNew
	*pResult = 0;
	UINT action = pNMTreeView->action ;
	if( !(action == TVC_BYMOUSE ||action == TVC_BYKEYBOARD)  )
	{
		// CTreeCtrl
		//m_treectrl.SelectItem(m_CurrentSelectionItem);
		//m_treectrl.SetItem(m_CurrentSelectionItem,TVIF_STATE,NULL,0,0,TVIS_SELECTED,TVIS_SELECTED,NULL );
		return;
	}
	/*
	TVC_BYKEYBOARD  By a keystroke. 
TVC_BYMOUSE  By a mouse click. 
TVC_UNKNOWN  Unknown 
*/
	
	HTREEITEM nItem = pNMTreeView->itemNew.hItem ;   //m_treectrl.GetNextItem(TVI_ROOT, TVGN_CARET );

	PackTreeCell *pCell = (PackTreeCell *)	m_treectrl.GetItemData( nItem );
	if( !pCell ) return;
	BaseType *peltobj = pCell->GetObject();
	if( !peltobj) return;

	m_pCurrentSelection = pCell;
	//m_CurrentSelectionItem = nItem;
	SetElementEditor( peltobj );
	
	//Update();
	// need an update of all the view to resize:
	m_pDoc->SetUpdateType(BaseContext::eVedaUpdate_MemberChanged);
	//m_pDoc->SetContextUpdated();
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
	// set editor:
		
	m_pMemberEditor = new CDialog_PackSerializable( this);
	if( m_pMemberEditor)
	{
		m_pMemberEditor->Create( /*&m_ListMemberEditorWindow*/ this);
		m_pMemberEditor->ModifyStyle(0, WS_BORDER);
		m_pMemberEditor->SetPackSerializable(_pObjectToShape,m_pDoc,m_pView);
		//m_pMemberEditor->GetClientRect( &editorrec);

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

	// TODO : ajoutez ici le code de votre gestionnaire de messages
}
