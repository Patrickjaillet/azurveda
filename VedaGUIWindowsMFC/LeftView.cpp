
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"

#include "VedaDemoOGLMfcGuiDoc.h"
#include "LeftView.h"

#include "BaseContext.h"
#include "ClassNode.h"
#include "BaseObject.h"
#include ".\leftview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnNMRclick)
	ON_COMMAND(ID_NEWOBJECT, OnNewobject)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)

    ON_NOTIFY_REFLECT (TVN_GETINFOTIP, OnTvnGetInfoTip)

	ON_COMMAND(ID_BASEOBJECT_CLONE, OnBaseobjectClone)
	ON_COMMAND(ID_MPREVIEW, OnMpreview)
	ON_COMMAND(ID_BASEOBJECT_DESTROY, OnBaseobjectDestroy)
	ON_COMMAND(ID_BASEOBJECT_DESTROYINDEPENDANT, OnDestroyIndependants)

	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify )

ON_COMMAND(ID_MPREVIEWB1, OnPreviewPrevious)
ON_COMMAND(ID_MEDITPreview, OnEditPreview)
ON_COMMAND(ID_MEDITB1, OnEditPrevious)

END_MESSAGE_MAP()

CLeftView::CLeftView()
{

}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{

	cs.style |= TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES | TVS_SHOWSELALWAYS| TVS_INFOTIP ;

	return CTreeView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	EnableToolTips(TRUE);
	GetTreeCtrl().EnableToolTips(TRUE);

	m_ImgList.Create( IDB_TREEIMAGELIST,16,4,0);

	GetTreeCtrl().SetImageList( &m_ImgList, TVSIL_NORMAL);

	GetTreeCtrl().DeleteAllItems();

	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if(!pdoc) return;
	BaseContext *pVedaDoc = pdoc->GetVedaContext();
	CString classname = pVedaDoc->GetName();
	InsertClassNode(pVedaDoc,TVI_ROOT,classname);

}

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CVedaDemoOGLMfcGuiDoc* CLeftView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVedaDemoOGLMfcGuiDoc)));
	return (CVedaDemoOGLMfcGuiDoc*)m_pDocument;
}
#endif

void	CLeftView::InsertClassNode( const ClassNode *_pClassNode, HTREEITEM _fatheritem, CString &_classlevel )
{
	HTREEITEM item = NULL;
	if( ! _pClassNode->IsAbstract() )
	{

		HTREEITEM nItem = (HTREEITEM) m_ClassNodeMap.GetValueAt( (void*)_pClassNode );
		if( nItem == 0 )
		{

			item =GetTreeCtrl().InsertItem(  _pClassNode->GetDisplayClassName() ,2,2,_fatheritem, TVI_SORT);
			GetTreeCtrl().SetItemData(item ,(DWORD_PTR)_pClassNode );

			GetTreeCtrl().SetItem(item, TVIF_STATE, NULL, 2, 2, TVIS_BOLD, TVIS_BOLD, 0);
			m_ClassNodeMap.SetAt( (void *)_pClassNode , (void*)item );
		} else
			item = nItem;
	}

	const PackList	*pObjectList = _pClassNode->GetObjectList();
	PackStruct::Cell *pcell = pObjectList->GetFirstCell();
	while(pcell && (item != NULL))
	{
		BaseObject *pObj = (BaseObject *) pcell->GetManagedObject();
		int image;
		if( pObj->isCreated() ) image=0; else image=1;

		HTREEITEM nItem = (HTREEITEM) m_BaseObjectMap.GetValueAt( (void*)pObj );
		if( nItem == 0 )
		{
			HTREEITEM itemobj =GetTreeCtrl().InsertItem( pObj->GetName() ,image,image,item,TVI_LAST);
			GetTreeCtrl().SetItemData(itemobj ,(DWORD_PTR) pObj );
			m_BaseObjectMap.SetAt( (void *)pObj,(void *)itemobj);
		}
		pcell = pcell->GetNextBrotherCell();
	}

	const ClassNode *pSonClassNode = _pClassNode->GetFirstSonClassNode();
	while( pSonClassNode )
	{
		CString nextlevel = _classlevel;
		nextlevel += "::";
		nextlevel += pSonClassNode->GetName();
		InsertClassNode( pSonClassNode, _fatheritem,nextlevel );
		pSonClassNode = pSonClassNode->GetNextBrotherClassNode();
	}
	GetTreeCtrl().Expand( item, TVE_EXPAND   );
}

void CLeftView::OnUpdate(CView* , LPARAM lHint, CObject* pHint)
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if(!pdoc) return;
	switch( pdoc->GetUpdateType() )
	{
		case BaseContext::eVedaUpdate_ObjectNew:
		{
			BaseObject *pObj = pdoc->GetObjectUpdated();

			const ClassNode *pClassnode =
				pObj->GetContext()->GetSubClassNodeByDescription(
										&(pObj->GetClassDescription()) );

			HTREEITEM nItem = (HTREEITEM) m_ClassNodeMap.GetValueAt( (void*)pClassnode );
			if( nItem )
			{
				int image;
				if( pObj->isCreated() ) image=0; else image=1;
				HTREEITEM itemobj = GetTreeCtrl().InsertItem( pObj->GetName() ,image,image,nItem,TVI_LAST);
				GetTreeCtrl().SetItemData(itemobj ,(DWORD_PTR) pObj );
				m_BaseObjectMap.SetAt( (void *)pObj,(void *)itemobj);
				GetTreeCtrl().Expand( nItem, TVE_EXPAND   );
			}

		}
		break;
		case BaseContext::eVedaUpdate_ObjectDestroyed:
		{
			BaseObject *pObj = pdoc->GetObjectUpdated();
			HTREEITEM nItem = (HTREEITEM) m_BaseObjectMap.GetValueAt( (void*)pObj );
			if(nItem) GetTreeCtrl().DeleteItem( nItem );
			m_BaseObjectMap.RemoveKey( (void *)pObj );
		}
		break;
		case BaseContext::eVedaUpdate_ObjectReCreated:
		case BaseContext::eVedaUpdate_ObjectClosed:
		{
			int image;
			BaseObject *pObj = pdoc->GetObjectUpdated();
			if( pObj->isCreated() )
				image=0;
			else if(pObj->DidLastCreationFailed() )
				image = 3;
			else image=1;

			HTREEITEM nItem = (HTREEITEM) m_BaseObjectMap.GetValueAt( (void*)pObj );
			if( nItem )
				GetTreeCtrl().SetItemImage(nItem,image,image);
		}
		break;
		case BaseContext::eVedaUpdate_MemberChanged:
		case BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo:
		{
			BaseObject *pObj = pdoc->GetObjectUpdated();
			HTREEITEM nItem = (HTREEITEM) m_BaseObjectMap.GetValueAt( (void*)pObj );
			BaseType *pMember = pdoc->GetMemberUpdated();
			if(nItem &&  pMember->GetInfoString() && (strcmp( pMember->GetInfoString(),"Name") ==0) )
			{
				GetTreeCtrl().SetItemText( nItem,pObj->GetName() );
			}
		}
		break;
	}

	HTREEITEM nCurrentItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );

	BaseObject *pSelected = pdoc->GetEditionSelectedBaseObject();
	if ( pSelected != NULL )
	{
		HTREEITEM nItem = (HTREEITEM) m_BaseObjectMap.GetValueAt( (void*)pSelected );
		if( nItem != 0 && nCurrentItem != nItem )
		{

			GetTreeCtrl().SelectItem(nItem);
			CString itemtxt =
				GetTreeCtrl().GetItemText( nItem );
				GetTreeCtrl().SetItemText(nItem,"");
			GetTreeCtrl().EnsureVisible(nItem);
			GetTreeCtrl().SetItemText(nItem,itemtxt.GetString());

		}
	}
}

void CLeftView::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{

	CPoint pt;
	GetCursorPos( &pt);
	CPoint point = pt;

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

	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	CMenu menu;
	CMenu *pPopUp;
	ClassNode *pClassNodeOfTheItem=NULL;
	if( image == 2 )
	{

		menu.LoadMenu( IDR_MENU_CCLASSNODE );
		pClassNodeOfTheItem =  (ClassNode *)GetTreeCtrl().GetItemData(nItem);
		if( pClassNodeOfTheItem == NULL ) return;
	} else if( image == 0 || image ==1 ||image ==3)
	{
		menu.LoadMenu( IDR_MENU_BASEOBJECT );
	}
	pPopUp = menu.GetSubMenu(0);

	SetForegroundWindow();
	pPopUp->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y,this);

	*pResult = 0;
}

void CLeftView::OnNewobject()
{

	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
	if( nItem == NULL ) return;

	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	ClassNode *pClassNodeOfTheItem=NULL;
	if( image == 2 )
	{
		pClassNodeOfTheItem =  (ClassNode *)GetTreeCtrl().GetItemData(nItem);
	} else if( image == 0 || image ==1 ||image ==3 )
	{
		return;
	}
	if( pClassNodeOfTheItem == NULL ) return;

	BaseObject *pnew = pClassNodeOfTheItem->NewObject();
	if(pnew)
	{
		CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
		if( pdoc )
			pdoc->SetEditionSelectedBaseObject(pnew);
	}

}

void CLeftView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );

	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	if( image == 2 )
	{

		*pResult = 0;
		return;
	}
	BaseObject *pObToEdit = (BaseObject *) (GetTreeCtrl().GetItemData(nItem));
	if( !pObToEdit )
	{

		*pResult = 0;
		return;
	}

	GetDocument()->SetEditionSelectedBaseObject(pObToEdit);

	*pResult = 0;
}

void CLeftView::OnBaseobjectDestroy()
{

	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
	if( nItem == NULL ) return;

	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	if( image == 2 )
	{
		return;
	}

	BaseObject *pObToEdit = (BaseObject *) (GetTreeCtrl().GetItemData(nItem));
	if( !pObToEdit ) return;

	if( GetDocument()->GetEditionSelectedBaseObject() == pObToEdit)  GetDocument()->SetEditionSelectedBaseObject(NULL);
	if( GetDocument()->GetPreviewSelectedBaseObject() == pObToEdit ) GetDocument()->SetPreviewSelectedBaseObject(NULL);
	pObToEdit->GetContext()->DestroyObject(pObToEdit);

}
void CLeftView::OnDestroyIndependants()
{
	if (AfxGetMainWnd()->MessageBox("This Function Destroy all objects that are not used by this object. Do we do that ?" ,"Watch out!" ,MB_YESNO|MB_ICONQUESTION  )==IDNO)
     return;

	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
	if( nItem == NULL ) return;

	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	if( image == 2 )
	{
		return;
	}

	BaseObject *pObToEdit = (BaseObject *) (GetTreeCtrl().GetItemData(nItem));
	if( !pObToEdit ) return;
	pObToEdit->GetContext()->DestroyAllIndependantObjects( pObToEdit );

}

void CLeftView::OnBaseobjectClone()
{

	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
	if( nItem == NULL ) return;

	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	if( image == 2 )
	{
		return;
	}

	BaseObject *pObToEdit = (BaseObject *) (GetTreeCtrl().GetItemData(nItem));
	if( !pObToEdit ) return;
	pObToEdit->Clone();

}

void CLeftView::OnMpreview()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;

	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
	if( nItem == NULL ) return;

	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	if( image == 2 )
	{
		return;
	}

	BaseObject *pObToEdit = (BaseObject *) (GetTreeCtrl().GetItemData(nItem));
	if( !pObToEdit ) return;

	pdoc->SetPreviewSelectedBaseObject( pObToEdit );

}

void CLeftView::OnPreviewPrevious()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;
	pdoc->OnPreviewPrevious();
}

void CLeftView::OnEditPreview()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;
	BaseObject *pPreview = pdoc->GetPreviewSelectedBaseObject();
	if(pPreview) pdoc->SetEditionSelectedBaseObject(pPreview);
}

void CLeftView::OnEditPrevious()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;
	pdoc->OnEditPrevious();
}
BOOL CLeftView::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID =pNMHDR->idFrom;
    if (pTTT->uFlags & TTF_IDISHWND)
    {

        nID = ::GetDlgCtrlID((HWND)nID);
        if(nID)
        {
            pTTT->lpszText = MAKEINTRESOURCE(nID);
            pTTT->hinst = AfxGetResourceHandle();
            return(TRUE);
        }
    }
    return(FALSE);
}

INT_PTR CLeftView::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{

	return CTreeView::OnToolHitTest(point, pTI);
}

BOOL CLeftView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{

	if(wParam == TVN_GETINFOTIP)
	{
		 LPNMTVGETINFOTIP lpGetInfoTip = (LPNMTVGETINFOTIP)lParam;
		 lpGetInfoTip->pszText=(LPTSTR)_T("azaz");
		 return TRUE;

	}

	return CTreeView::OnNotify(wParam, lParam, pResult);
}
void CLeftView::OnTvnGetInfoTip(NMHDR *pNMHDR,
                                    LRESULT *pResult)

{
  LPNMTVGETINFOTIP pGetInfoTip = (LPNMTVGETINFOTIP)pNMHDR;

  	int image,selimage;
	GetTreeCtrl().GetItemImage( pGetInfoTip->hItem, image, selimage );
	CMenu menu;

	ClassNode *pClassNodeOfTheItem=NULL;
	if( image == 2 )
	{
		ClassNode *pClassNodeOfTheItem =  (ClassNode *)GetTreeCtrl().GetItemData(pGetInfoTip->hItem);
		if( pClassNodeOfTheItem == NULL ) return;
		strncpy(pGetInfoTip->pszText, pClassNodeOfTheItem->GetClassHelpDescription(),pGetInfoTip->cchTextMax-1);
	} else if( image == 0 || image ==1 )
	{

		BaseObject *pobj =  (BaseObject *)GetTreeCtrl().GetItemData(pGetInfoTip->hItem);
		if(pobj==NULL) return;

		strncpy(pGetInfoTip->pszText, pobj->ValueToString(),pGetInfoTip->cchTextMax-1);
	} else if(image ==3)
	{
		BaseObject *pobj =  (BaseObject *)GetTreeCtrl().GetItemData(pGetInfoTip->hItem);
		strncpy(pGetInfoTip->pszText, pobj->GetLastCreationErrorString() ,pGetInfoTip->cchTextMax-1);
	}

  *pResult = 0;

}
