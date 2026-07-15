
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

IMPLEMENT_DYNAMIC(CDialog_PackReference, CDialog)
CDialog_PackReference::CDialog_PackReference(CWnd* pParent )
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

}

BEGIN_MESSAGE_MAP(CDialog_PackReference, CDialog)
	ON_CBN_SELCHANGE(IDC_OBJECTCOMBO, OnCbnSelchangeObjectcombo)
	ON_BN_CLICKED(IDC_SelectedName, OnBnClickedSelectedname)
	ON_BN_CLICKED(IDC_PREVIEW, OnBnClickedPreview)
END_MESSAGE_MAP()

void CDialog_PackReference::OnCancel()
{

}

void CDialog_PackReference::OnOK()
{

}
void CDialog_PackReference::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
														   )
{
	m_isBeingUpdated = true;
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);

	PackObjectReference *pVeda = (PackObjectReference *)_objToManage;

	while( m_ObjectCombo.GetCount() >0 )
		m_ObjectCombo.DeleteString(0);

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

		int item = m_ObjectCombo.AddString( strn.GetString() );
		m_ObjectCombo.SetItemData(item,(DWORD_PTR)_pClassNode );
	}

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

	PackObjectReference *pVeda = (PackObjectReference *) m_pObjectToManage;
	if( !pVeda ) return;
	const BaseObject::ClassDescription	*pClassDescription = &(pVeda->GetReferableBaseClass());
	BaseContext *pContext = pVeda->GetObjectThatManagesThis()->GetContext();

	m_ObjectCombo.ResetContent();
	m_IndexMap.RemoveAll();

	int item = m_ObjectCombo.AddString("-");
	m_ObjectCombo.SetItemData(item,(DWORD_PTR)NULL );

	ClassNode *pRootClassNode = pContext->GetSubClassNodeByDescription( pClassDescription );
	if( pRootClassNode )
	{
		AddClassObjectsInCombo(pRootClassNode);
	}

	InsertNewClassNode( pRootClassNode );

}
void	CDialog_PackReference::AddClassObjectsInCombo( const ClassNode *_pFatherClassNode)
{

	const PackList	*pObjectList = _pFatherClassNode->GetObjectList();
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

	int index = m_ObjectCombo.GetCurSel();
	if( index != LB_ERR )
	{
		BaseType *pSer = (BaseObject *)m_ObjectCombo.GetItemData(index);
		BaseObject *pObj;
		if( !pSer )pObj= NULL;
		else
		if( strcmp( pSer->GetClassID(),"ClassNode")==0  )
		{
			ClassNode *pClassNode = (ClassNode *)pSer;

			pObj = pClassNode->NewObject();
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

		PackObjectReference *pVedaReference = (PackObjectReference *)m_pObjectToManage;
		PackObjectReference::enum_SetReferenceResult result = pVedaReference->SetObjectPointer( pObj);

		if( result != PackObjectReference::eSetRef_SetOK )
		{
			switch(result)
			{
				case PackObjectReference::eSetRef_CantReferObjectThatReferThisOne:
					MessageBox("This object already refer the selected one directly or indirectly.","", MB_OK | MB_ICONERROR );
				break;

			}
			m_ObjectCombo.SelectString(0,"-");
		}

	}
}
void	CDialog_PackReference::Update(void)
{
	m_isBeingUpdated = true;
	if(!m_pObjectToManage ) return;

	PackObjectReference *pVeda = (PackObjectReference *) m_pObjectToManage;

	UpdateComboList();

	BaseObject *pObjRefered  = pVeda->GetObjectPointer();
	if( pObjRefered )
	{

		WORD value;
		m_IndexMap.Lookup((void*)pObjRefered,value );
		m_ObjectCombo.SetCurSel((int)value);

		CString selname= "Go";

		m_SelectedName.SetWindowText( selname.GetString() );
		m_SelectedName.ShowWindow(SW_SHOW);
		m_PreviewButton.ShowWindow( SW_SHOW );
	} else
	{
		 m_ObjectCombo.SelectString(0,"-");

		m_SelectedName.ShowWindow( SW_HIDE );
		m_PreviewButton.ShowWindow( SW_HIDE );
	}
	m_isBeingUpdated = false;
}

void CDialog_PackReference::OnBnClickedSelectedname()
{
	if(!m_pObjectToManage ) return;

	PackObjectReference *pVeda = (PackObjectReference *) m_pObjectToManage;

	BaseObject *pObjRefered  = pVeda->GetObjectPointer();
	if( pObjRefered ) m_pDoc->SetEditionSelectedBaseObject(pObjRefered);

}

void CDialog_PackReference::OnBnClickedPreview()
{
	if(!m_pObjectToManage ) return;

	PackObjectReference *pVeda = (PackObjectReference *) m_pObjectToManage;

	BaseObject *pObjRefered  = pVeda->GetObjectPointer();
	if( pObjRefered ) m_pDoc->SetPreviewSelectedBaseObject(pObjRefered);
}
