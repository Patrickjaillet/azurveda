// Dialog_PackSerializable.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "BaseType.h"
#include "Dialog_PackSerializable.h"
#include ".\dialog_packserializable.h"

#include "Dialog_PackString.h"
#include "Dialog_PackStringMultiple.h"
#include "Dialog_PackULong.h"
#include "Dialog_PackULongFlags.h"
#include "Dialog_PackReference.h"
#include "Dialog_PackLong_WithLimits.h"
#include "Dialog_PackListDynamic.h"
#include "Dialog_PackStruct.h"
#include "Dialog_PackTreeCell.h"
#include "Dialog_PackULongEnums.h"
#include "Dialog_PackFloat.h"
#include "Dialog_PackFloatByte_Limits.h"
#include "Dialog_PackDynamicType.h"
#include "Dialog_PackResource.h"
//#include "Dialog_PackFloatXY.h"
//#include "Dialog_PackFloatXYZ.h"
void CDialog_PackSerializable::Create(CWnd* pParent )
{
	CDialog::Create( CDialog_PackSerializable::IDD,pParent);
}

// Boîte de dialogue CDialog_PackSerializable

IMPLEMENT_DYNAMIC(CDialog_PackSerializable, CDialog)
CDialog_PackSerializable::CDialog_PackSerializable(CWnd* pParent /*=NULL*/)
	: CDialog_AbstractPackSerializable(CDialog_PackSerializable::IDD, pParent)
{
}

CDialog_PackSerializable::~CDialog_PackSerializable()
{
	DestroyAllDialogs();
}

void CDialog_PackSerializable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialog_PackSerializable, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// Gestionnaires de messages CDialog_PackSerializable

void CDialog_PackSerializable::OnOK()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	//CDialog::OnOK();
}

void CDialog_PackSerializable::OnCancel()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	//CDialog::OnCancel();
}
void	CDialog_PackSerializable::Update(void)
{
	BaseType *pVeda =(BaseType *) m_pObjectToManage;
	if(!pVeda)return;
	// set the value to the edit ctrl:
	CDialog_AbstractPackSerializable *pDialog;
	CRect rec(0,0,1,1);
	POSITION pos = m_CurrentDialogList.GetHeadPosition();
	for (int i=0;i < m_CurrentDialogList.GetCount();i++)
	{
		pDialog = (CDialog_AbstractPackSerializable *) m_CurrentDialogList.GetNext( pos);
		pDialog->Update();
		pDialog->GetClientRect( &rec);
	}
	// reset the size if needed:
	//CRect rc;
	//GetClientRect(&rc);
	// reset size of this component:
	//MoveWindow( rc.left, rc.top,rec.Width() ,rec.Height() );
	Resize(rec.Width(),rec.Height() );

}

void CDialog_PackSerializable::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView														   
														   )
{
	if( _objToManage == m_pObjectToManage ) return;
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc, _pView);

	DestroyAllDialogs();

	if( _objToManage == NULL ) return;
	// search proper class:
	CDialog_AbstractPackSerializable *(*_CreatorFunc)(CWnd *)=NULL;
	CString classID = _objToManage->GetClassID();
	while( !_CreatorFunc && classID.GetLength()>0 )
	{
		_CreatorFunc = (CDialog_AbstractPackSerializable *(*)(CWnd *) )m_PackDialogMap[ classID.GetString() ] ;
		//if not found, try the upper class:
		int ii;
		if( (ii = classID.ReverseFind(':')) != -1 )
			classID = classID.Mid(0,ii-1);
		else 
			classID = "";
	}

	CRect rec;
	rec.SetRect( 0,0,100, 24);
	int maxright=1,verticalline=1;
	if( _CreatorFunc != NULL)
	{
		CDialog_AbstractPackSerializable *pnewDialog = _CreatorFunc(this);
		if(pnewDialog )
		{	// 
			pnewDialog->Create(this);
			// set Veda object to edit, and resize:
			pnewDialog->SetPackSerializable(_objToManage, m_pDoc,m_pView );
			CString labeltext = _objToManage->GetInfoString();
			labeltext += ":";
			
			pnewDialog->SetMemberName( labeltext.GetString() );

			pnewDialog->GetClientRect( &rec);
			verticalline =  rec.Height();
			maxright = rec.Width();
			pnewDialog->ShowWindow(SW_SHOW);
			m_CurrentDialogList.AddHead( pnewDialog );
		}
	}
	// set size 
	CRect rc;
	GetClientRect(&rc);
	// reset size of this component:
	MoveWindow( rc.left, rc.top,maxright ,verticalline );


}
BOOL CDialog_PackSerializable::OnInitDialog()
{
	CDialog_AbstractPackSerializable::OnInitDialog();

	RegisterPackDialog( CDialog_PackString::GetManagedClassName(),CDialog_PackString::NewInstance );
	RegisterPackDialog( CDialog_PackULong::GetManagedClassName(),CDialog_PackULong::NewInstance );
	RegisterPackDialog( CDialog_PackULongFlags::GetManagedClassName(),CDialog_PackULongFlags::NewInstance );
	RegisterPackDialog( CDialog_PackReference::GetManagedClassName(),CDialog_PackReference::NewInstance );
	RegisterPackDialog( CDialog_PackLong_WithLimits::GetManagedClassName(),CDialog_PackLong_WithLimits::NewInstance );
	RegisterPackDialog( CDialog_PackListDynamic::GetManagedClassName(),CDialog_PackListDynamic::NewInstance );
	RegisterPackDialog( CDialog_PackStruct::GetManagedClassName(),CDialog_PackStruct::NewInstance );
	RegisterPackDialog( CDialog_PackTreeCell::GetManagedClassName(),CDialog_PackTreeCell::NewInstance );
	RegisterPackDialog( CDialog_PackULongEnums::GetManagedClassName(),CDialog_PackULongEnums::NewInstance );
	RegisterPackDialog( CDialog_PackFloat::GetManagedClassName(),CDialog_PackFloat::NewInstance );
	RegisterPackDialog( CDialog_PackFloatByte_Limits::GetManagedClassName(),CDialog_PackFloatByte_Limits::NewInstance );
	RegisterPackDialog( CDialog_PackDynamicType::GetManagedClassName(),CDialog_PackDynamicType::NewInstance );
	RegisterPackDialog( CDialog_PackResource::GetManagedClassName(),CDialog_PackResource::NewInstance );
	RegisterPackDialog( CDialog_PackStringMultiple::GetManagedClassName(),CDialog_PackStringMultiple::NewInstance );


//	RegisterPackDialog( CDialog_PackFloatXY::GetManagedClassName(),CDialog_PackFloatXY::NewInstance );
//	RegisterPackDialog( CDialog_PackFloatXYZ::GetManagedClassName(),CDialog_PackFloatXYZ::NewInstance );

// Dialog_PackFloatXYZ

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}
//! enregistre les gadgets capable d'editer les membres: 
void	CDialog_PackSerializable::RegisterPackDialog( const char *_pSuportedClass,CDialog_AbstractPackSerializable *(*_CreatorFunc)(CWnd *) )
{
	//CMapStringToPtr
	m_PackDialogMap.SetAt(_pSuportedClass,(void *)_CreatorFunc);

}
void	CDialog_PackSerializable::DestroyAllDialogs()
{
	//CPtrList

	POSITION pos = m_CurrentDialogList.GetHeadPosition();
	for (int i=0;i < m_CurrentDialogList.GetCount();i++)
	{
		CDialog *pDialog = (CDialog *) m_CurrentDialogList.GetNext( pos);
		pDialog->DestroyWindow();
		delete pDialog;
	}
	m_CurrentDialogList.RemoveAll();


}
void CDialog_PackSerializable::OnDestroy()
{
	DestroyAllDialogs();
	CDialog_AbstractPackSerializable::OnDestroy();
}
void CDialog_PackSerializable::SetMemberName(	const char *_pMemberName )
{
	
}