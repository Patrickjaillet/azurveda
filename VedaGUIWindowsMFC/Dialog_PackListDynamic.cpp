
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackListDynamic.h"

#include "Dialog_PackSerializable.h"
#include "PackList.h"
#include "PackList_TimeTrack.h"
#include "PackList_TimeBlockTrack.h"
#include ".\dialog_PackListDynamic.h"

#include "VedaDemoOGLMfcGuiDoc.h"
#include "BaseContext.h"

void CDialog_PackListDynamic::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PackListDynamic,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackListDynamic::NewInstance(CWnd* pParent)
{
	return new CDialog_PackListDynamic(pParent);
}

IMPLEMENT_DYNAMIC(CDialog_PackListDynamic, CDialog)
CDialog_PackListDynamic::CDialog_PackListDynamic(CWnd* pParent )
	: CDialog_AbstractPackSerializable(CDialog_PackListDynamic::IDD, pParent)
	,m_ShapeInited(false)
	,m_pMemberEditor(NULL)
{
}

CDialog_PackListDynamic::~CDialog_PackListDynamic()
{
	if( m_pMemberEditor)
	{
		m_pMemberEditor->DestroyWindow();
		delete m_pMemberEditor ;
		m_pMemberEditor = NULL ;
	}
}

void CDialog_PackListDynamic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listctrl);

	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_BUTTON_UP, m_BtUp);
	DDX_Control(pDX, IDC_BUTTON_DOWN, m_BtDown);
	DDX_Control(pDX, IDC_BUTTON_Clone, m_BtClone);
	DDX_Control(pDX, IDC_BUTTON_SELECTATPTIME, m_BtSelectAtPreviewTime);
	DDX_Control(pDX, IDC_BUTTON_ADDATPTIME, m_BtAddAtPreviewTime);
}

BEGIN_MESSAGE_MAP(CDialog_PackListDynamic, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PLUS, OnBnClickedButtonPlus)
	ON_BN_CLICKED(IDC_BUTTON_MINUS, OnBnClickedButtonMinus)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST1, OnLvnItemActivateList1)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_Clone, OnBnClickedButtonClone)
	ON_BN_CLICKED(IDC_BUTTON_SELECTATPTIME, OnBnClickedButtonSelectatptime)
	ON_BN_CLICKED(IDC_BUTTON_ADDATPTIME, OnBnClickedButtonAddatptime)
END_MESSAGE_MAP()

void CDialog_PackListDynamic::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
														   )
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);

	m_listctrl.SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

	CString classID = _objToManage->GetClassID();

	if(classID.Compare("PackList::PackList_TimeTrack")==0 ||
		classID.Compare("PackList::PackList_TimeBlockTrack")==0
		)
	{

	} else
	{

		m_BtSelectAtPreviewTime.ShowWindow( SW_HIDE);
		m_BtAddAtPreviewTime.ShowWindow( SW_HIDE);
	}

	Update();
}

void CDialog_PackListDynamic::OnOK()
{

}

void CDialog_PackListDynamic::OnCancel()
{

}

BOOL CDialog_PackListDynamic::OnInitDialog()
{
	CDialog_AbstractPackSerializable::OnInitDialog();

	m_listctrl.ModifyStyle(0, LVS_SHOWSELALWAYS);
	m_listctrl.ModifyStyleEx(0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	return TRUE;

}
void	CDialog_PackListDynamic::SetElementEditor( BaseType *_pObjectToShape)
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
		m_pMemberEditor->Update();

	}
	Update();
}
void	CDialog_PackListDynamic::InitListShape( BaseType *_pObjectToShapeList  )
{

	LV_COLUMN	lvc;
	lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT ;
	lvc.cx = 60 ;
	lvc.fmt = LVCFMT_CENTER ;
	lvc.iSubItem = 0;

	CRect editorrec;
	if( strcmp(_pObjectToShapeList->GetClassID(),"PackStruct") ==0 )
	{
			lvc.cx = 24 ;
			lvc.mask = LVCF_TEXT | LVCF_WIDTH ;
			lvc.pszText = (LPTSTR)_T("");
			lvc.iSubItem = 0;
			m_listctrl.InsertColumn( 0, &lvc );
			lvc.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_FMT ;
			lvc.cx = 80 ;

		PackStruct *plist = (PackStruct *)_pObjectToShapeList;
		PackStruct::Cell *pcell = plist->GetFirstCell();
		int nbc = 1;
		while( pcell )
		{

			BaseType *pob = pcell->GetManagedObject();
			if( pob->GetInfoString() )
			{
				lvc.iSubItem = nbc;
				lvc.pszText = _T( (LPSTR)(pob->GetInfoString()) );
				m_listctrl.InsertColumn( nbc, &lvc );
				nbc++;
			}
			pcell = pcell->GetNextBrotherCell();
		}
	} else
	{
			lvc.cx = 24 ;
			lvc.mask = LVCF_TEXT | LVCF_WIDTH ;
			lvc.pszText = (LPTSTR)_T("");
			lvc.iSubItem = 0;
			m_listctrl.InsertColumn( 0, &lvc );
			lvc.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_FMT ;
			lvc.cx = 160 ;
			if( _pObjectToShapeList->GetInfoString() )
			{
				lvc.iSubItem = 1;
				lvc.pszText = _T( (LPSTR)(_pObjectToShapeList->GetInfoString()) );
				m_listctrl.InsertColumn( 1, &lvc );
			}

	}
	m_ShapeInited = true;
}
void	CDialog_PackListDynamic::Update(void)
{
	if( !m_pObjectToManage ) return;
	PackList *pVedaList =(PackList *)m_pObjectToManage;
	if(!m_ShapeInited && pVedaList->GetFirstCell() != NULL )
	{
		InitListShape( pVedaList->GetFirstCell()->GetManagedObject() );
	}

	int lastselected = m_listctrl.GetNextItem(-1, LVNI_SELECTED );

	BaseType *pLastSelectedPS=NULL;
	if(m_pMemberEditor)
		pLastSelectedPS= m_pMemberEditor->GetManagedObject();
	unsigned int stillpresent=0;

	m_listctrl.DeleteAllItems();

	LVITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_STATE ;
	lvi.state = 0;
	lvi.stateMask = lvi.state ;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	int index = 0;
	PackStruct::Cell *plistcell = pVedaList->GetFirstCell();
	while(plistcell)
	{

		lvi.iItem = index;
		lvi.iSubItem = 0;
		CString str;
		str.Format("%d",index );
		lvi.pszText = (LPSTR)str.GetString();
		lvi.iItem = m_listctrl.InsertItem( &lvi);
 		BaseType *peltobj = (BaseType *) plistcell->GetManagedObject() ;
		if(pLastSelectedPS == peltobj) stillpresent=1;
		m_listctrl.SetItemData(lvi.iItem, (DWORD_PTR) peltobj );

		if( strcmp(peltobj->GetClassID(),"PackStruct") == 0 )
		{
			PackStruct *psublist = (PackStruct *)peltobj;
			PackStruct::Cell *psubcell = psublist->GetFirstCell();
			int nbc = 1;
			while(psubcell)
			{
				BaseType *pob = psubcell->GetManagedObject();
				if( pob->GetInfoString() )
				{
					lvi.iSubItem	= nbc;
					lvi.pszText		= _T( (LPSTR)pob->ValueToString() );
					m_listctrl.SetItem( &lvi);
					nbc++;
				}
				psubcell = psubcell->GetNextBrotherCell();
			}
		}
		else
		{
			if( peltobj->GetInfoString() )
			{
				lvi.iSubItem	= 1;
				lvi.pszText		= _T( (LPSTR)peltobj->ValueToString() );
				m_listctrl.SetItem( &lvi);
			}

		}
		index++;
		plistcell = plistcell->GetNextBrotherCell();
	}

	if( stillpresent == 0 )
	{
		SetElementEditor( NULL );
	}

	if(m_pMemberEditor) m_pMemberEditor->Update();

	int		thiswidth;
	CRect listrec;
	m_listctrl.GetClientRect(&listrec);
	m_listctrl.ClientToScreen(&listrec);
	ScreenToClient(&listrec);
	CRect editorrec(0,0,1,1);
	thiswidth = listrec.Width();
	if(m_pMemberEditor)
	{
		m_pMemberEditor->GetClientRect(&editorrec);
		m_pMemberEditor->GetWindowRect( &editorrec);
		if( editorrec.Width()+12>thiswidth ) thiswidth = editorrec.Width()+8;
		m_pMemberEditor->MoveWindow( listrec.left, listrec.bottom + 10 ,thiswidth ,editorrec.Height());

	}

	Resize(thiswidth+8,listrec.bottom +10 + editorrec.Height());

	if( lastselected != -1 )
	{

		m_listctrl.EnsureVisible( lastselected, FALSE ) ;
		m_listctrl.SetItemState( lastselected, LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);

	}
}
void CDialog_PackListDynamic::OnBnClickedButtonPlus()
{
	if( !m_pObjectToManage ) return;
	PackList *pVedaList =(PackList *)m_pObjectToManage;

	int nItem = -1;
	POSITION pos = m_listctrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		nItem = m_listctrl.GetNextSelectedItem(pos);
	}
	if( nItem == -1 )
	{

		pVedaList->AddElement();
	} else
	{

		pVedaList->AddElement(nItem);
	}
}

void CDialog_PackListDynamic::OnBnClickedButtonMinus()
{
	if( !m_pObjectToManage ) return;
	PackList *pVedaList =(PackList *)m_pObjectToManage;

	POSITION pos = m_listctrl.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
    int nItem = m_listctrl.GetNextSelectedItem(pos);

	BaseType *peltobj = (BaseType *)
										m_listctrl.GetItemData(nItem );
	if( peltobj == NULL ) return;

	if( m_pMemberEditor && m_pMemberEditor->GetManagedObject() == peltobj )
	{
		SetElementEditor( NULL );
	}

	pVedaList->DeleteElement(peltobj);

	Update();

}

void CDialog_PackListDynamic::OnDestroy()
{
	if( m_pMemberEditor)
	{
		m_pMemberEditor->DestroyWindow();
		delete m_pMemberEditor ;
		m_pMemberEditor = NULL ;
	}
	CDialog_AbstractPackSerializable::OnDestroy();

}

void CDialog_PackListDynamic::OnLvnItemActivateList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	BaseType *peltobj = (BaseType *)
										m_listctrl.GetItemData( pNMIA->iItem );
	if( peltobj == NULL ) return;
	SetElementEditor( peltobj );

	m_pDoc->SetUpdateType(BaseContext::eVedaUpdate_MemberChanged);

	m_pDoc->SetObjectUpdated(peltobj->GetObjectThatManagesThis() );
	m_pDoc->SetMemberUpdated(peltobj);
	m_pDoc->UpdateAllViews(NULL);

	m_pDoc->SetUpdateType(BaseContext::eVedaUpdate_MemberChanged);

	m_pDoc->SetObjectUpdated(peltobj->GetObjectThatManagesThis() );
	m_pDoc->SetMemberUpdated(peltobj);
	m_pDoc->UpdateAllViews(NULL);

	m_pDoc->SetUpdateType(BaseContext::eVedaUpdate_MemberChanged);

	m_pDoc->SetObjectUpdated(peltobj->GetObjectThatManagesThis() );
	m_pDoc->SetMemberUpdated(peltobj);
	m_pDoc->UpdateAllViews(NULL);
	m_pDoc->SetUpdateType(BaseContext::eVedaUpdate_MemberChanged);

	m_pDoc->SetObjectUpdated(peltobj->GetObjectThatManagesThis() );
	m_pDoc->SetMemberUpdated(peltobj);
	m_pDoc->UpdateAllViews(NULL);

}
void CDialog_PackListDynamic::SetMemberName(	const char *_pMemberName )
{
	m_MemberName.SetWindowText( _pMemberName );
}
void CDialog_PackListDynamic::OnBnClickedButtonUp()
{
	if( !m_pObjectToManage ) return;
	PackList *pVedaList =(PackList *)m_pObjectToManage;

	POSITION pos = m_listctrl.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
    int nItem = m_listctrl.GetNextSelectedItem(pos);

	BaseType *peltobj = (BaseType *)
										m_listctrl.GetItemData(nItem );
	PackList::Cell *pCell = pVedaList->GetFirstCell();
	int ii=0;
	while(pCell)
	{
		if(pCell->GetManagedObject() == peltobj) break;
		pCell = pCell->GetNextBrotherCell();
		ii++;
	}
	if(ii==0) return;
	ii--;
	pVedaList->SwapElements(ii);

}

void CDialog_PackListDynamic::OnBnClickedButtonDown()
{
	if( !m_pObjectToManage ) return;
	PackList *pVedaList =(PackList *)m_pObjectToManage;

	POSITION pos = m_listctrl.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
    int nItem = m_listctrl.GetNextSelectedItem(pos);

	BaseType *peltobj = (BaseType *)
										m_listctrl.GetItemData(nItem );
	PackList::Cell *pCell = pVedaList->GetFirstCell();
	int ii=0;
	while(pCell)
	{
		if(pCell->GetManagedObject() == peltobj) break;
		pCell = pCell->GetNextBrotherCell();
		ii++;
	}
	pVedaList->SwapElements(ii);
}

void CDialog_PackListDynamic::OnBnClickedButtonClone()
{
	if( !m_pObjectToManage ) return;
	PackList *pVedaList =(PackList *)m_pObjectToManage;

	POSITION pos = m_listctrl.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
    int nItem = m_listctrl.GetNextSelectedItem(pos);

	BaseType *peltobj = (BaseType *)
										m_listctrl.GetItemData(nItem );
	PackList::Cell *pCell = pVedaList->GetFirstCell();
	int ii=0;
	while(pCell)
	{
		if(pCell->GetManagedObject() == peltobj) break;
		pCell = pCell->GetNextBrotherCell();
		ii++;
	}
	pVedaList->CloneElement(ii);
}
void CDialog_PackListDynamic::OnBnClickedButtonSelectatptime()
{
	if( !m_pObjectToManage ) return;
	double prevtime = m_pDoc->GetPreviewCurrentTime();
	CString classID = m_pObjectToManage->GetClassID();
	if(classID.Compare("PackList::PackList_TimeTrack")==0)
	{
		PackList_TimeTrack *pVedaList =(PackList_TimeTrack *)m_pObjectToManage;
		PackList::Cell *pCell= pVedaList->GetCellByDate(prevtime);
		if(pCell)
		{
			int ii;
			for(ii=0;ii<m_listctrl.GetItemCount() ; ii++)
			{
				PackList_TimeTrack::TimeTrackElement *peltobj = (PackList_TimeTrack::TimeTrackElement *)
											m_listctrl.GetItemData(ii );
				if(peltobj==pCell->GetManagedObject())
				{
					m_listctrl.SetItemState(ii,LVNI_SELECTED|LVNI_FOCUSED,LVNI_SELECTED|LVNI_FOCUSED);
					SetElementEditor( peltobj );

					Update();
					return;
				}

			}
		}
	}
	else if(classID.Compare("PackList::PackList_TimeBlockTrack")==0)
	{
		PackList_TimeBlockTrack *pVedaList =(PackList_TimeBlockTrack *)m_pObjectToManage;
		PackList::Cell *pCell= pVedaList->GetCellByDate(prevtime);
		if(pCell)
		{
			int ii;
			for(ii=0;ii<m_listctrl.GetItemCount() ; ii++)
			{
				PackList_TimeBlockTrack::TimeBlockTrackElement *peltobj = (PackList_TimeBlockTrack::TimeBlockTrackElement *)
											m_listctrl.GetItemData(ii );
				if(peltobj==pCell->GetManagedObject())
				{
					m_listctrl.SetItemState(ii,LVNI_SELECTED|LVNI_FOCUSED,LVNI_SELECTED|LVNI_FOCUSED);
					SetElementEditor( peltobj );
					Update();
					return;
				}

			}
		}

	}

}
void CDialog_PackListDynamic::OnBnClickedButtonAddatptime()
{
	if( !m_pObjectToManage ) return;
	double prevtime = m_pDoc->GetPreviewCurrentTime();
	CString classID = m_pObjectToManage->GetClassID();
	if(classID.Compare("PackList::PackList_TimeTrack")==0)
	{
		PackList_TimeTrack *pVedaList =(PackList_TimeTrack *)m_pObjectToManage;
		PackList::Cell *pCell= pVedaList->GetCellByDate(prevtime);
		if(!pCell) return ;
		int ii;
		for(ii=0;ii<m_listctrl.GetItemCount() ; ii++)
		{
			PackList_TimeTrack::TimeTrackElement *peltobj = (PackList_TimeTrack::TimeTrackElement *)
										m_listctrl.GetItemData(ii );
			if(peltobj==pCell->GetManagedObject())
			{
				PackList::Cell *pnCell = pVedaList->AddElement(ii+1);
				if(pnCell)
				{
					((PackList_TimeTrack::TimeTrackElement *) pnCell->GetManagedObject())
						->SetTimeInSecond(prevtime);
					SetElementEditor( pnCell->GetManagedObject() );
					Update();
				}
				return;
			}
		}
	}
	else if(classID.Compare("PackList::PackList_TimeBlockTrack")==0)
	{
		PackList_TimeTrack *pVedaList =(PackList_TimeTrack *)m_pObjectToManage;
		PackList::Cell *pCell= pVedaList->GetCellByDate(prevtime);
		if(!pCell) return ;
		int ii;
		for(ii=0;ii<m_listctrl.GetItemCount() ; ii++)
		{
			PackList_TimeTrack::TimeTrackElement *peltobj = (PackList_TimeTrack::TimeTrackElement *)
										m_listctrl.GetItemData(ii );
			if(peltobj==pCell->GetManagedObject())
			{
				PackList::Cell *pnCell = pVedaList->AddElement(ii);

				if(pnCell)
				{
					SetElementEditor( pnCell->GetManagedObject() );
					Update();
				}
				return;
			}
		}
	}

}
