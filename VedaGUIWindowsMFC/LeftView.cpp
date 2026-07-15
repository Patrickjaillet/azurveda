// LeftView.cpp : implémentation de la classe CLeftView
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"

#include "VedaDemoOGLMfcGuiDoc.h"
#include "LeftView.h"
// veda:
#include "BaseContext.h"
#include "ClassNode.h"
#include "BaseObject.h"
#include ".\leftview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnNMRclick)
	ON_COMMAND(ID_NEWOBJECT, OnNewobject)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
//	ON_NOTIFY_REFLECT(TVN_GETINFOTIP, OnTvnInfoTip)
    ON_NOTIFY_REFLECT (TVN_GETINFOTIP, OnTvnGetInfoTip)
/*	ON_COMMAND(ID_UNDO, CLeftView::OnUndo)
	ON_COMMAND(ID_REDO, CLeftView::OnRedo)
	ON_COMMAND(ID_MERGE, CLeftView::OnMerge)
	ON_COMMAND(ID_FLUSHNAMES, CLeftView::OnFlushNames)
*/
	ON_COMMAND(ID_BASEOBJECT_CLONE, OnBaseobjectClone)
	ON_COMMAND(ID_MPREVIEW, OnMpreview)
	ON_COMMAND(ID_BASEOBJECT_DESTROY, OnBaseobjectDestroy)
	ON_COMMAND(ID_BASEOBJECT_DESTROYINDEPENDANT, OnDestroyIndependants)

//	ON_COMMAND(ID_FILE_LIBIMAGE, OnAddLibImage)
//	ON_COMMAND(ID_FILE_LIBBAOOM, OnAddLibBAOOM)
//	ON_COMMAND(ID_FILE_LIBDEMO, OnAddLibDemo)

	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify )

ON_COMMAND(ID_MPREVIEWB1, OnPreviewPrevious)
ON_COMMAND(ID_MEDITPreview, OnEditPreview)
ON_COMMAND(ID_MEDITB1, OnEditPrevious)
//ON_COMMAND(ID_EXPORTEXECUTABLE, OnExportExecutable)
END_MESSAGE_MAP()


// construction ou destruction de CLeftView

CLeftView::CLeftView()
{
	// TODO : ajoutez ici le code d'une construction
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO : Changez ici la classe ou les styles Window en modifiant CREATESTRUCT cs
	cs.style |= TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES | TVS_SHOWSELALWAYS| TVS_INFOTIP ;

	return CTreeView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

/*	CToolTipCtrl
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.set
*/
	EnableToolTips(TRUE);   // enable tool tips for view
	GetTreeCtrl().EnableToolTips(TRUE);
 
	// TODO : Vous pouvez remplir votre contrôle TreeView avec des éléments en accédant directement
	//  au contrôle Tree par un appel à GetTreeCtrl().
	m_ImgList.Create( IDB_TREEIMAGELIST,16,4,0);

	GetTreeCtrl().SetImageList( &m_ImgList, TVSIL_NORMAL);

	GetTreeCtrl().DeleteAllItems();

	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if(!pdoc) return;
	BaseContext *pVedaDoc = pdoc->GetVedaContext();
	CString classname = pVedaDoc->GetName();
	InsertClassNode(pVedaDoc,TVI_ROOT,classname);


}


// diagnostics pour CLeftView

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CVedaDemoOGLMfcGuiDoc* CLeftView::GetDocument() // la version non déboguée est en ligne
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVedaDemoOGLMfcGuiDoc)));
	return (CVedaDemoOGLMfcGuiDoc*)m_pDocument;
}
#endif //_DEBUG


// gestionnaires de messages pour CLeftView


void	CLeftView::InsertClassNode( const ClassNode *_pClassNode, HTREEITEM _fatheritem, CString &_classlevel )
{
	HTREEITEM item = NULL;
	if( ! _pClassNode->IsAbstract() ) 
	{
		// CTreeCtrl
		//item =GetTreeCtrl().InsertItem(  _classlevel.GetString() ,2,2,_fatheritem,TVI_LAST);
		// don't insert if already there:
		HTREEITEM nItem = (HTREEITEM) m_ClassNodeMap.GetValueAt( (void*)_pClassNode );
		if( nItem == 0 )
		{
//			CToolTipCtrl
			item =GetTreeCtrl().InsertItem(  _pClassNode->GetDisplayClassName() ,2,2,_fatheritem,/*TVI_LAST*/ TVI_SORT);
			GetTreeCtrl().SetItemData(item ,(DWORD_PTR)_pClassNode );
			// bold:
			GetTreeCtrl().SetItem(item, TVIF_STATE, NULL, 2, 2, TVIS_BOLD, TVIS_BOLD, 0);
			m_ClassNodeMap.SetAt( (void *)_pClassNode , (void*)item );
		} else
			item = nItem;
	}

// bold:
	/*old
	// bold if not abstract:
	if( ! _pClassNode->IsAbstract() )
	{
		GetTreeCtrl().SetItem(item, TVIF_STATE, NULL, 2, 2, TVIS_BOLD, TVIS_BOLD, 0);

	}else
	{
		// if abstract, no menu:
		GetTreeCtrl().SetItemData(item ,(DWORD_PTR) NULL );
	}
*/
	// objets *DEJA* instancié:
// les objets sont attaché par l'update de leurs creation maintenant.
	const PackList	*pObjectList = _pClassNode->GetObjectList(); // can't be NULL.
	PackStruct::Cell *pcell = pObjectList->GetFirstCell();
	while(pcell && (item != NULL))
	{			
		BaseObject *pObj = (BaseObject *) pcell->GetManagedObject();
		int image;
		if( pObj->isCreated() ) image=0; else image=1;
		// don't insert if already there:
	
		HTREEITEM nItem = (HTREEITEM) m_BaseObjectMap.GetValueAt( (void*)pObj );
		if( nItem == 0 )
		{
			HTREEITEM itemobj =GetTreeCtrl().InsertItem( pObj->GetName() ,image,image,item,TVI_LAST);
			GetTreeCtrl().SetItemData(itemobj ,(DWORD_PTR) pObj );
			m_BaseObjectMap.SetAt( (void *)pObj,(void *)itemobj);
		} 
		pcell = pcell->GetNextBrotherCell();
	}

	// autres classes sous la classe:
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

void CLeftView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if(!pdoc) return;
	switch( pdoc->GetUpdateType() )
	{
		case BaseContext::eVedaUpdate_ObjectNew:
		{
			BaseObject *pObj = pdoc->GetObjectUpdated();
			// find the classnode of the new object:
			const ClassNode *pClassnode =  
				pObj->GetContext()->GetSubClassNodeByDescription(
										&(pObj->GetClassDescription()) );
			// add object item:
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
		{	// change image:
			int image;
			BaseObject *pObj = pdoc->GetObjectUpdated();
			if( pObj->isCreated() ) 
				image=0; 
			else if(pObj->DidLastCreationFailed() )
				image = 3; // creation failed.
			else image=1; // closed.
	
			HTREEITEM nItem = (HTREEITEM) m_BaseObjectMap.GetValueAt( (void*)pObj );
			if( nItem )
				GetTreeCtrl().SetItemImage(nItem,image,image);
		}
		break;
		case BaseContext::eVedaUpdate_MemberChanged:
		case BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo:
		{	// update item only if "name" change
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
	// ------ assure that the selected item is the same on doc, if not, reselect the tree.
	HTREEITEM nCurrentItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET ); // current selected.
	// tODO
	BaseObject *pSelected = pdoc->GetEditionSelectedBaseObject();
	if ( pSelected != NULL )
	{
		HTREEITEM nItem = (HTREEITEM) m_BaseObjectMap.GetValueAt( (void*)pSelected );
		if( nItem != 0 && nCurrentItem != nItem )
		{
			//CTreeCtrl
			GetTreeCtrl().SelectItem(nItem);
			CString itemtxt = 
				GetTreeCtrl().GetItemText( nItem );
				GetTreeCtrl().SetItemText(nItem,"");
			GetTreeCtrl().EnsureVisible(nItem);
			GetTreeCtrl().SetItemText(nItem,itemtxt.GetString());
			// bef = GetScrollPosition( );
//	SetScrollPos(SB_VERT,bef.y - zDelta,FALSE);
			//int spos = GetTreeCtrl().GetScrollPos(0);
			//GetTreeCtrl().sc
			//int spos = this->GetScrollPos(0);
			//SetScrollPos(0,0);
			//GetTreeCtrl().Invalidate();
			//GetTreeCtrl().RedrawWindow();

			//GetTreeCtrl().Invalidate();
			//GetTreeCtrl().SetScrollPos(0, 0);
			/*ef = GetTreeCtrl().GetScrollPos( );
			GetTreeCtrl().SetScrollPos(
			GetTreeCtrl().Invalidate();*/
		}
	}
}
// menu droit sur arbre objet:
void CLeftView::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// get point :
	CPoint pt;
	GetCursorPos( &pt);
	CPoint point = pt;

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
/* old bugged	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
	if(nItem == NULL)
	{
		*pResult = 0;
		return;
	}*/
	//---- find which object type...
	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	CMenu menu;
	CMenu *pPopUp;
	ClassNode *pClassNodeOfTheItem=NULL;
	if( image == 2 )
	{
		// class item:
		menu.LoadMenu( IDR_MENU_CCLASSNODE );
		pClassNodeOfTheItem =  (ClassNode *)GetTreeCtrl().GetItemData(nItem);
		if( pClassNodeOfTheItem == NULL ) return; // abstract class.
	} else if( image == 0 || image ==1 ||image ==3)
	{	// object item:
		menu.LoadMenu( IDR_MENU_BASEOBJECT );	
	}
	pPopUp = menu.GetSubMenu(0);
	//
	//


	// diasble./enable

	// track:

	SetForegroundWindow();
	pPopUp->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y,this);

	*pResult = 0;
}

void CLeftView::OnNewobject()
{
	// action on a classNode not abstract:
	// chope item selectionné:
	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
	if( nItem == NULL ) return;
	// ---- find which object type...
	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	ClassNode *pClassNodeOfTheItem=NULL;
	if( image == 2 )
	{
		pClassNodeOfTheItem =  (ClassNode *)GetTreeCtrl().GetItemData(nItem);
	} else if( image == 0 || image ==1 ||image ==3 )
	{	// object item:
		return;	
	}
	if( pClassNodeOfTheItem == NULL ) return; // abstract class.
	// ----  ----
	BaseObject *pnew = pClassNodeOfTheItem->NewObject();
	if(pnew) // select new.
	{
		CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
		if( pdoc )
			pdoc->SetEditionSelectedBaseObject(pnew);
	}

}
/*
void CLeftView::OnTvnInfoTip(NMHDR *pNMHDR, LRESULT *pResult)
{
	//lParam
   lpGetInfoTip = (LPNMTVGETINFOTIP)pResult;
	*pResult = 0;
}*/
void CLeftView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	//TRACE("CLeftView::OnTvnSelchanged\n");

//non	HTREEITEM item = (HTREEITEM) pNMTreeView->itemNew ;
 // CTreeCtrl
	// chope item selectionné:
	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
//	HTREEITEM nItem = GetTreeCtrl().GetSelectedItem();
	//---- find which object type...
	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	if( image == 2 )
	{
		// class item:
		*pResult = 0;
		return;
	} 
	BaseObject *pObToEdit = (BaseObject *) (GetTreeCtrl().GetItemData(nItem));
	if( !pObToEdit ) 
	{
		// class item:
		*pResult = 0;
		return;	
	}
	//
	GetDocument()->SetEditionSelectedBaseObject(pObToEdit);


	*pResult = 0;
}

void CLeftView::OnBaseobjectDestroy()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
	// action on a classNode not abstract:
	// chope item selectionné:
	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
	if( nItem == NULL ) return;
	// ---- find which object is selected:
	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	if( image == 2 )
	{	// class item:
		return;
	}
	// object item:
	BaseObject *pObToEdit = (BaseObject *) (GetTreeCtrl().GetItemData(nItem));
	if( !pObToEdit ) return;
	// we really got to do that:
	if( GetDocument()->GetEditionSelectedBaseObject() == pObToEdit)  GetDocument()->SetEditionSelectedBaseObject(NULL);
	if( GetDocument()->GetPreviewSelectedBaseObject() == pObToEdit ) GetDocument()->SetPreviewSelectedBaseObject(NULL);
	pObToEdit->GetContext()->DestroyObject(pObToEdit);

}
void CLeftView::OnDestroyIndependants()
{
	if (AfxGetMainWnd()->MessageBox("This Function Destroy all objects that are not used by this object. Do we do that ?" ,"Watch out!" ,MB_YESNO|MB_ICONQUESTION  /*QUESTION*/)==IDNO)
     return;

// TODO : ajoutez ici le code de votre gestionnaire de commande
	// action on a classNode not abstract:
	// chope item selectionné:
	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
	if( nItem == NULL ) return;
	// ---- find which object is selected:
	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	if( image == 2 )
	{	// class item:
		return;
	}
	// object item:
	BaseObject *pObToEdit = (BaseObject *) (GetTreeCtrl().GetItemData(nItem));
	if( !pObToEdit ) return;
	pObToEdit->GetContext()->DestroyAllIndependantObjects( pObToEdit );

}


void CLeftView::OnBaseobjectClone()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
	// action on a classNode not abstract:
	// chope item selectionné:
	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
	if( nItem == NULL ) return;
	// ---- find which object is selected:
	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	if( image == 2 )
	{	// class item:
		return;
	}
	// object item:
	BaseObject *pObToEdit = (BaseObject *) (GetTreeCtrl().GetItemData(nItem));
	if( !pObToEdit ) return;
	pObToEdit->Clone();

}

void CLeftView::OnMpreview()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if( !pdoc ) return;
	
	// TODO : ajoutez ici le code de votre gestionnaire de commande
	// action on a classNode not abstract:
	// chope item selectionné:
	HTREEITEM nItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CARET );
	if( nItem == NULL ) return;
	// ---- find which object is selected:
	int image,selimage;
	GetTreeCtrl().GetItemImage( nItem, image, selimage );
	if( image == 2 )
	{	// class item:
		return;
	}
	// object item:
	BaseObject *pObToEdit = (BaseObject *) (GetTreeCtrl().GetItemData(nItem));
	if( !pObToEdit ) return;
	//pObToEdit->Clone();	
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
        // idFrom is actually the HWND of the tool
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
{/*
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base
   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
   TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
   CString strTipText;
   UINT nID = pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
      pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = ::GetDlgCtrlID((HWND)nID);
   }

   if (nID != 0) // will be zero on a separator
      strTipText.Format("Control ID = %d", nID);

   if (pNMHDR->code == TTN_NEEDTEXTA)
      lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
   else
      ::MultiByteToWideChar( CP_ACP , 0, strTipText, -1, pTTTW->szText, sizeof(pTTTW->szText) );
   *pResult = 0;

   return TRUE;    // message was handled
*/

	return CTreeView::OnToolHitTest(point, pTI);
}

BOOL CLeftView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TVN_GETINFOTIP
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base
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
/*
  CString csItemTxt =
	  GetTreeCtrl().GetItemText(pGetInfoTip->hItem);
*/
  	int image,selimage;
	GetTreeCtrl().GetItemImage( pGetInfoTip->hItem, image, selimage );
	CMenu menu;
	//CMenu *pPopUp;
	ClassNode *pClassNodeOfTheItem=NULL;
	if( image == 2 )
	{
		ClassNode *pClassNodeOfTheItem =  (ClassNode *)GetTreeCtrl().GetItemData(pGetInfoTip->hItem);
		if( pClassNodeOfTheItem == NULL ) return; 
		strncpy(pGetInfoTip->pszText, pClassNodeOfTheItem->GetClassHelpDescription(),pGetInfoTip->cchTextMax-1);
	} else if( image == 0 || image ==1 )
	{	// object item:
		//menu.LoadMenu( IDR_MENU_BASEOBJECT );	
		BaseObject *pobj =  (BaseObject *)GetTreeCtrl().GetItemData(pGetInfoTip->hItem);
		if(pobj==NULL) return;
		//*(pGetInfoTip->pszText)='.';
		strncpy(pGetInfoTip->pszText, pobj->ValueToString(),pGetInfoTip->cchTextMax-1);
	} else if(image ==3)
	{	// if object construction failed:
		BaseObject *pobj =  (BaseObject *)GetTreeCtrl().GetItemData(pGetInfoTip->hItem);
		strncpy(pGetInfoTip->pszText, pobj->GetLastCreationErrorString() ,pGetInfoTip->cchTextMax-1);
	}
 // *(pGetInfoTip->pszText)='.';
 

  *pResult = 0;

}/*
void CLeftView::OnExportcfile()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if(!pdoc) return;
	BaseContext *pveda = pdoc->GetVedaContext();
	if(!pveda) return;

	CFileDialog	cdiag(FALSE,"cpp","context.cpp",OFN_OVERWRITEPROMPT,0,0,0);
	if( cdiag.DoModal ()!=IDOK ) return;
  
	CString filename = cdiag.GetPathName();
	pveda->ExportAsCTableFile( filename.GetString(),"Context");


}
*/
/*
void CLeftView::OnExportExecutable()
{
	CVedaDemoOGLMfcGuiDoc *pdoc = GetDocument();
	if(!pdoc) return;
	BaseContext *pveda = pdoc->GetVedaContext();
	if(!pveda) return;

	// CWinApp
	CFileDialog	cdiag(FALSE,"","demo",OFN_OVERWRITEPROMPT,0,0,0);
	if( cdiag.DoModal ()!=IDOK ) return;
	// write cpp file:
	CString fileName = cdiag.GetFileName();
	CString filepath = cdiag.GetPathName();

	CString contextCFilename =".\\";//"BinCreator\\";
	contextCFilename += fileName.GetString() ;
	contextCFilename += ".cpp";
	pveda->ExportAsCTableFile( contextCFilename.GetString(),"Context");

	// compile cpp file
	CString compileCommand = ".\\cl.exe -c "; // BinCreator
	compileCommand += fileName.GetString();
	compileCommand += ".cpp";
	system( compileCommand.GetString() );

	// link data .obj to exe:
	CString linkCommand = ".\\link.exe ";
	linkCommand += fileName.GetString();
	linkCommand += ".obj DemoRunBase.obj ";
	linkCommand += "/OUT:\"./"; // add exe output name.
	linkCommand += fileName.GetString();
	linkCommand += ".exe\" ";
// NOTOK: oleaut32.lib ole32.lib advapi32.lib comdlg32.lib shell32.lib winspool.lib
// OK: uuid.lib gdi32.lib kernel32.lib user32.lib
	CString optionSet="\
/INCREMENTAL:NO /NOLOGO /NODEFAULTLIB:\"LIBC\" \
/OPT:REF /OPT:ICF /LTCG opengl32.lib dsound.lib dxguid.lib kernel32.lib user32.lib \
 gdi32.lib \
uuid.lib odbc32.lib odbccp32.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaRelease\\Veda.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaLibBAOOMRelease\\VedaBAOOM.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaLibDemoRelease\\VedaDemo.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaMachineOGLRelease\\VedaDemoOGL.lib \
..\\..\\VedaMachineOGLWinDxSound\\vedalibimagerelease\\VedaImage.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaLibMathRelease\\VedaMath.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaMachineOGLWinDxSoundRelease\\VedaMachineOGLWinDxSound.lib \
";
	linkCommand+= optionSet.GetString();
	system( linkCommand.GetString() );
	
	// use upx to shorten executable size:
	CString CommandPack = "upx -9 -f ";
	CommandPack += fileName.GetString();
	CommandPack += ".exe";
	system( CommandPack.GetString() );

	// execute...
	CString CommandExecute = fileName.GetString();
	CommandExecute +=".exe";
	system( CommandExecute.GetString() );

}
*/