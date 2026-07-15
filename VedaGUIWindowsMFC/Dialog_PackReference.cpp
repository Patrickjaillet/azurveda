// Dialog_PackReference.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "PackObjectReference.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackReference.h"
#include ".\dialog_packreference.h"
#include "BaseObject.h"
#include "ClassNode.h"
#include "BaseContext.h"

#include "VedaDemoOGLMfcGuiDoc.h"

void CDialog_PackReference::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PACKREFERENCE,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackReference::NewInstance(CWnd* pParent)
{
	return new CDialog_PackReference(pParent);

}
// Boîte de dialogue CDialog_PackReference

IMPLEMENT_DYNAMIC(CDialog_PackReference, CDialog)
CDialog_PackReference::CDialog_PackReference(CWnd* pParent /*=NULL*/)
	: CDialog_AbstractPackSerializable(CDialog_PackReference::IDD, pParent)
	,m_isBeingUpdated(true)
{
}

CDialog_PackReference::~CDialog_PackReference()
{
}

void CDialog_PackReference::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_SelectedName, m_SelectedName);
	DDX_Control(pDX, IDC_OBJECTCOMBO, m_ObjectCombo);
	DDX_Control(pDX, IDC_PREVIEW, m_PreviewButton);
//	DDX_Control(pDX, IDC_NEWCOMBO, m_newClassCombo);
}


BEGIN_MESSAGE_MAP(CDialog_PackReference, CDialog)
	ON_CBN_SELCHANGE(IDC_OBJECTCOMBO, OnCbnSelchangeObjectcombo)
	ON_BN_CLICKED(IDC_SelectedName, OnBnClickedSelectedname)
	ON_BN_CLICKED(IDC_PREVIEW, OnBnClickedPreview)
END_MESSAGE_MAP()


// Gestionnaires de messages CDialog_PackReference

void CDialog_PackReference::OnCancel()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

//	CDialog_AbstractPackSerializable::OnCancel();
}

void CDialog_PackReference::OnOK()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

//	CDialog_AbstractPackSerializable::OnOK();
}
void CDialog_PackReference::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView														   
														   )
{
	m_isBeingUpdated = true;
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);
	// cast veda type:
	PackObjectReference *pVeda = (PackObjectReference *)_objToManage;

	// update liste objet dispo
	// destroy list:
	while( m_ObjectCombo.GetCount() >0 )
		m_ObjectCombo.DeleteString(0);
	// add all objects corresponding to the referable class:

	UpdateComboList();
	Update();

}


void	CDialog_PackReference::InsertNewClassNode( const ClassNode *_pClassNode )
{
	if( !_pClassNode ) return;
	if( ! _pClassNode->IsAbstract() ) 
	{
		CString strn="New ";
		strn += _pClassNode->GetDisplayClassName();
		// CComboBox
		int item = m_ObjectCombo.AddString( strn.GetString() );
		m_ObjectCombo.SetItemData(item,(DWORD_PTR)_pClassNode );
	}
	// autres classes sous la classe:
	const ClassNode *pSonClassNode = _pClassNode->GetFirstSonClassNode();
	while( pSonClassNode )
	{
		InsertNewClassNode( pSonClassNode );
		pSonClassNode = pSonClassNode->GetNextBrotherClassNode();
	}
}

void	CDialog_PackReference::UpdateComboList()
{
	m_isBeingUpdated = true;
	// cast veda type:
	PackObjectReference *pVeda = (PackObjectReference *) m_pObjectToManage;
	if( !pVeda ) return;
	const BaseObject::ClassDescription	*pClassDescription = &(pVeda->GetReferableBaseClass());
	BaseContext *pContext = pVeda->GetObjectThatManagesThis()->GetContext();

	// empty combo:
	// CComboBox
	m_ObjectCombo.ResetContent();
	m_IndexMap.RemoveAll();
	// add combo item for NULL:
	int item = m_ObjectCombo.AddString("-");
	m_ObjectCombo.SetItemData(item,(DWORD_PTR)NULL );
	// get the corresponding context classnode:	
	ClassNode *pRootClassNode = pContext->GetSubClassNodeByDescription( pClassDescription );
	if( pRootClassNode )
	{
		AddClassObjectsInCombo(pRootClassNode);
	}
	// ---- add new object at the end:
	InsertNewClassNode( pRootClassNode );
	

}
void	CDialog_PackReference::AddClassObjectsInCombo( const ClassNode *_pFatherClassNode)
{
	// add all object from this class:
	// objets instancié:
	const PackList	*pObjectList = _pFatherClassNode->GetObjectList(); // can't be NULL.
	PackStruct::Cell *pcell = pObjectList->GetFirstCell();
	while(pcell)
	{			
		BaseObject *pObj = ((BaseObject *) pcell->GetManagedObject());
		CString obname = pObj->GetName();
		int item = m_ObjectCombo.AddString(obname.GetString() );
			m_ObjectCombo.SetItemData(item,(DWORD_PTR)pObj );
		m_IndexMap.SetAt( (void *)pObj,(WORD)item);
		pcell = pcell->GetNextBrotherCell();
	}
	// sub classes:
	const ClassNode *pSonClassNode = _pFatherClassNode->GetFirstSonClassNode();
	while( pSonClassNode )
	{
		AddClassObjectsInCombo(pSonClassNode);
		pSonClassNode = pSonClassNode->GetNextBrotherClassNode();
	}

}

void CDialog_PackReference::OnCbnSelchangeObjectcombo()
{
	if(m_isBeingUpdated) return;
	// selection change:
	int index = m_ObjectCombo.GetCurSel();
	if( index != LB_ERR )
	{
		BaseType *pSer = (BaseObject *)m_ObjectCombo.GetItemData(index);
		BaseObject *pObj;
		if( !pSer )pObj= NULL; // "-" selected.
		else
		if( strcmp( pSer->GetClassID(),"ClassNode")==0  )
		{
			ClassNode *pClassNode = (ClassNode *)pSer;
			//CString newname= m_pObjectToManage->GetObjectThatManagesThis()->GetName();
			//newname+=m_pObjectToManage->GetInfoString();
			//  create new object with default name:
			pObj = pClassNode->NewObject(); //newname.GetString() );
			CString newname= pObj->GetName();
			newname +=",for:";
			newname += m_pObjectToManage->GetObjectThatManagesThis()->GetName();
			newname +=":";
			newname +=m_pObjectToManage->GetInfoString();	
			pObj->SetName(newname.GetString());
		} else
		{
			pObj =  (BaseObject *)pSer;
		}
		//if( pObj == NULL ) return; CAN BE NULL
		// cast veda type:
		PackObjectReference *pVedaReference = (PackObjectReference *)m_pObjectToManage;
		PackObjectReference::enum_SetReferenceResult result = pVedaReference->SetObjectPointer( pObj);
		// if failed, reset combo selection:
		if( result != PackObjectReference::eSetRef_SetOK )
		{
			switch(result)
			{
				case PackObjectReference::eSetRef_CantReferObjectThatReferThisOne:
					MessageBox("This object already refer the selected one directly or indirectly.","", MB_OK | MB_ICONERROR );
				break;
				/* shouldnt happen.
				case PackObjectReference::eSetRef_ObjectClassDoesntMatch:
					MessageBox( "", "This object has a wrong class: it should be in the list.", MB_OK | MB_ICONERROR );
					break:
					case PackObjectReference::eSetRef_ObjectFromOtherContext
				*/
			}	
			m_ObjectCombo.SelectString(0,"-");
		}
		
	}
}
void	CDialog_PackReference::Update(void)
{	
	m_isBeingUpdated = true;
	if(!m_pObjectToManage ) return;
	// cast veda type:
	PackObjectReference *pVeda = (PackObjectReference *) m_pObjectToManage;
	// redraw combo:
	UpdateComboList();
	// set the value to the obj selected:
	BaseObject *pObjRefered  = pVeda->GetObjectPointer();
	if( pObjRefered )
	{
		// CComboBox
		WORD value;
		m_IndexMap.Lookup((void*)pObjRefered,value );
		m_ObjectCombo.SetCurSel((int)value);// lectString(
		//m_ObjectCombo.GetItemData( SetItemData(
		/*int sitem = m_ObjectCombo.SelectString(0,pObjRefered->GetName() );
		while((BaseObject*)m_ObjectCombo.GetItemData(sitem)!=pObjRefered)
		{
			sitem = m_ObjectCombo.SelectString(sitem,pObjRefered->GetName() );
		}*/
	
		//old? m_ObjectCombo.SetWindowText(pObjRefered->GetName());
		//  set select button
		CString selname= "Go";
		//selname += pObjRefered->GetName();
		m_SelectedName.SetWindowText( selname.GetString() );
		m_SelectedName.ShowWindow(SW_SHOW);
		m_PreviewButton.ShowWindow( SW_SHOW );
	} else //none selected:
	{
		 m_ObjectCombo.SelectString(0,"-");
		//m_SelectedName.SetWindowText(" Selected: -");	
		m_SelectedName.ShowWindow( SW_HIDE );
		m_PreviewButton.ShowWindow( SW_HIDE );
	}
	m_isBeingUpdated = false;
}

void CDialog_PackReference::OnBnClickedSelectedname()
{
	if(!m_pObjectToManage ) return;
	// cast veda type:
	PackObjectReference *pVeda = (PackObjectReference *) m_pObjectToManage;
	// value to the obj selected:
	BaseObject *pObjRefered  = pVeda->GetObjectPointer();
	if( pObjRefered ) m_pDoc->SetEditionSelectedBaseObject(pObjRefered);

}

void CDialog_PackReference::OnBnClickedPreview()
{
	if(!m_pObjectToManage ) return;
	// cast veda type:
	PackObjectReference *pVeda = (PackObjectReference *) m_pObjectToManage;
	// value to the obj selected:
	BaseObject *pObjRefered  = pVeda->GetObjectPointer();
	if( pObjRefered ) m_pDoc->SetPreviewSelectedBaseObject(pObjRefered);
}
