// Dialog_PackStruct.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackStruct.h"
#include ".\dialog_PackStruct.h"
#include "PackStruct.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "BaseContext.h"

#include "Dialog_PackString.h"
#include "Dialog_PackULong.h"
#include "Dialog_PackULongFlags.h"
#include "Dialog_PackReference.h"
#include "Dialog_PackLong_WithLimits.h"
#include "Dialog_PackListDynamic.h"
#include "Dialog_PackTreeCell.h"
#include "Dialog_PackSerializable.h"
#include "Dialog_PackFloat.h"
#include "Dialog_PackResource.h"
// Boîte de dialogue CDialog_PackStruct

IMPLEMENT_DYNAMIC(CDialog_PackStruct, CDialog)
CDialog_PackStruct::CDialog_PackStruct(CWnd* pParent /*=NULL*/)
	: CDialog_AbstractPackSerializable(CDialog_PackStruct::IDD, pParent)
{
}


void CDialog_PackStruct::Create(CWnd* pParent )
{
	CDialog::Create( CDialog_PackStruct::IDD,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackStruct::NewInstance(CWnd* pParent)
{
	return new CDialog_PackStruct(pParent);
}

CDialog_PackStruct::~CDialog_PackStruct()
{	// if not done:
	DestroyAllDialogs();
}

void CDialog_PackStruct::DoDataExchange(CDataExchange* pDX)
{
	CDialog_AbstractPackSerializable::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialog_PackStruct, CDialog_AbstractPackSerializable)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// Gestionnaires de messages CDialog_PackStruct

void CDialog_PackStruct::OnOK()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	//CDialog::OnOK();
}

void CDialog_PackStruct::OnCancel()
{
	// TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	//CDialog::OnCancel();
}
void	CDialog_PackStruct::Update(void)
{
	CVedaDemoOGLMfcGuiDoc	*pdoc =m_pDoc;

	// send update to members and recalc size
	int		ww=1;
	int		hh=1;
	CRect	recBeforeUpdate;
	CRect	rec;
	POSITION pos = m_CurrentDialogList.GetHeadPosition();
//	POSITION posl =	m_CurrentLabelMemberList.GetHeadPosition();
	CDialog_AbstractPackSerializable *pDialog;
//	CDialog	*pLabel;
	for (int i=0;i < m_CurrentDialogList.GetCount();i++)
	{
		pDialog = (CDialog_AbstractPackSerializable *) m_CurrentDialogList.GetNext( pos);
	//	pLabel = (CDialog *) m_CurrentLabelMemberList.GetNext(posl);
		pDialog->ShowWindow(SW_SHOW);
		pDialog->Update();
			pDialog->GetClientRect( &rec);
			pDialog->ClientToScreen( &rec);
			ScreenToClient(&rec);
			// move/resize dialog:
			int oldh = rec.top;
			pDialog->MoveWindow( 8, hh,rec.Width() ,rec.Height() );
			if(oldh != hh ) pDialog->RedrawWindow();
			//CRect labelrect;
			/*if(pLabel)
			{
				pLabel->GetClientRect(&labelrect);
				pLabel->MoveWindow(0,hh,labelrect.Width(),labelrect.Height() );
			}*/
		
		hh +=rec.Height();
		if( rec.Width()+8 > ww) ww =  rec.Width()+8;
		//if( rec.bottom > hh ) hh = rec.bottom;
	}		
	// reset the size if needed:
	Resize(ww+4,hh);


}
void CDialog_PackStruct::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView														   
														   )
{
	// already inited:
	if( m_pObjectToManage ) return;
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);
	// cast veda type:
	PackStruct *pPackStruct =(PackStruct *)_objToManage;

	// erase all previous dialogs: (shouldn't be useful)
	DestroyAllDialogs();

	// construct all new dialogs:
	int		verticalline=0;
	int		maxright=40;
	PackStruct::Cell *pcell = pPackStruct->GetFirstCell();
	while( pcell )
	{	
		CRect rec;
		// on n'édite quer les éléments avec un infostring:
		if( pcell->GetManagedObject()->GetInfoString() != NULL )
		{
			// add and display label InfoString from the member
			CString labeltext = pcell->GetManagedObject()->GetInfoString();
			labeltext += ":";
			
			//rec.SetRect( 0,verticalline+8,100,verticalline + 24);
			int lineHeight = 18; 
			//CStatic *pLineLabel = new CStatic();
			//pLineLabel->Create( labeltext.GetString(), SS_RIGHT ,rec,this);
			//pLineLabel->ShowWindow(SW_SHOW);
			//m_CurrentLabelMemberList.AddTail( pLineLabel );
			// add a CDialog_PackSerializable that match the type:
			CDialog_AbstractPackSerializable *pnewDialog = new CDialog_PackSerializable(this);
			if(pnewDialog )
			{	// create mfc dialog:
				pnewDialog->Create(this);
				// set Veda object to edit, and resize:
				pnewDialog->SetPackSerializable( pcell->GetManagedObject(), m_pDoc,m_pView );
				// set member name label:

				//
				//pnewDialog->SetMemberName( labeltext.GetString() );

				// place dialog in view:
				//pnewDialog->GetClientRect( &rec);
				//rec.MoveToX( 16+96);
				//rec.MoveToY(verticalline);

				//pnewDialog->MoveWindow( &rec);// 16+130,verticalline,200,30);
				//if( rec.Height()> lineHeight) lineHeight =  rec.Height();
				//if( (rec.Width()+16+96) > maxright ) maxright = rec.Width()+16+96;
				//pnewDialog->ShowWindow(SW_SHOW);
				m_CurrentDialogList.AddTail( pnewDialog );
				
			}
			verticalline +=lineHeight;
		}
		
		//TRACE("ShapeGUIForBaseObject");
		/*if( pcell->GetManagedObject()->GetInfoString() != NULL )
			TRACE(pcell->GetManagedObject()->GetInfoString());
			*/
		//TRACE("\n");
		pcell = pcell->GetNextBrotherCell();
	}
	// resize all components:
	Update();
	// set size 
/*	CRect rc;
	GetClientRect(&rc);
	// reset size of this component:
	MoveWindow( rc.left, rc.top,maxright ,verticalline );
*/
	// CWnd

}
void	CDialog_PackStruct::DestroyAllDialogs()
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
/*
	pos = m_CurrentLabelMemberList.GetHeadPosition();
	for (int i=0;i < m_CurrentLabelMemberList.GetCount();i++)
	{
		CDialog *pDialog = (CDialog *) m_CurrentLabelMemberList.GetNext( pos);
		pDialog->DestroyWindow();
		delete pDialog;
	}
	m_CurrentLabelMemberList.RemoveAll();
*/


}
void CDialog_PackStruct::OnDestroy()
{
	DestroyAllDialogs();
	CDialog_AbstractPackSerializable::OnDestroy();

	// TODO : ajoutez ici le code de votre gestionnaire de messages
}
//! enregistre les gadgets capable d'editer les membres: 
void	CDialog_PackStruct::RegisterPackDialog( const char *_pSuportedClass,CDialog_AbstractPackSerializable *(*_CreatorFunc)(CWnd *) )
{
	//CMapStringToPtr
	m_PackDialogMap.SetAt(_pSuportedClass,(void *)_CreatorFunc);

}
BOOL CDialog_PackStruct::OnInitDialog()
{
	CDialog_AbstractPackSerializable::OnInitDialog();
/*
	RegisterPackDialog( CDialog_PackString::GetManagedClassName(),CDialog_PackString::NewInstance );
	RegisterPackDialog( CDialog_PackULong::GetManagedClassName(),CDialog_PackULong::NewInstance );
	RegisterPackDialog( CDialog_PackULongFlags::GetManagedClassName(),CDialog_PackULongFlags::NewInstance );
	RegisterPackDialog( CDialog_PackReference::GetManagedClassName(),CDialog_PackReference::NewInstance );
	RegisterPackDialog( CDialog_PackLong_WithLimits::GetManagedClassName(),CDialog_PackLong_WithLimits::NewInstance );
	RegisterPackDialog( CDialog_PackListDynamic::GetManagedClassName(),CDialog_PackListDynamic::NewInstance );
	RegisterPackDialog( CDialog_PackTreeCell::GetManagedClassName(),CDialog_PackTreeCell::NewInstance );
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}
void CDialog_PackStruct::SetMemberName(	const char *_pMemberName )
{
	// do nothing.	
}