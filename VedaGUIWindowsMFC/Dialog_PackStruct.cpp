
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

IMPLEMENT_DYNAMIC(CDialog_PackStruct, CDialog)
CDialog_PackStruct::CDialog_PackStruct(CWnd* pParent )
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
{
	DestroyAllDialogs();
}

void CDialog_PackStruct::DoDataExchange(CDataExchange* pDX)
{
	CDialog_AbstractPackSerializable::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDialog_PackStruct, CDialog_AbstractPackSerializable)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CDialog_PackStruct::OnOK()
{

}

void CDialog_PackStruct::OnCancel()
{

}
void	CDialog_PackStruct::Update(void)
{
	int		ww=1;
	int		hh=1;
	CRect	recBeforeUpdate;
	CRect	rec;
	POSITION pos = m_CurrentDialogList.GetHeadPosition();

	CDialog_AbstractPackSerializable *pDialog;

	for (int i=0;i < m_CurrentDialogList.GetCount();i++)
	{
		pDialog = (CDialog_AbstractPackSerializable *) m_CurrentDialogList.GetNext( pos);

		pDialog->ShowWindow(SW_SHOW);
		pDialog->Update();
			pDialog->GetClientRect( &rec);
			pDialog->ClientToScreen( &rec);
			ScreenToClient(&rec);

			int oldh = rec.top;
			pDialog->MoveWindow( 8, hh,rec.Width() ,rec.Height() );
			if(oldh != hh ) pDialog->RedrawWindow();

		hh +=rec.Height();
		if( rec.Width()+8 > ww) ww =  rec.Width()+8;

	}

	Resize(ww+4,hh);

}
void CDialog_PackStruct::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
														   )
{

	if( m_pObjectToManage ) return;
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);

	PackStruct *pPackStruct =(PackStruct *)_objToManage;

	DestroyAllDialogs();

	int		verticalline=0;
	PackStruct::Cell *pcell = pPackStruct->GetFirstCell();
	while( pcell )
	{
		CRect rec;

		if( pcell->GetManagedObject()->GetInfoString() != NULL )
		{

			CString labeltext = pcell->GetManagedObject()->GetInfoString();
			labeltext += ":";

			int lineHeight = 18;

			CDialog_AbstractPackSerializable *pnewDialog = new CDialog_PackSerializable(this);
			if(pnewDialog )
			{
				pnewDialog->Create(this);

				pnewDialog->SetPackSerializable( pcell->GetManagedObject(), m_pDoc,m_pView );

				m_CurrentDialogList.AddTail( pnewDialog );

			}
			verticalline +=lineHeight;
		}

		pcell = pcell->GetNextBrotherCell();
	}

	Update();

}
void	CDialog_PackStruct::DestroyAllDialogs()
{

	POSITION pos = m_CurrentDialogList.GetHeadPosition();
	for (int i=0;i < m_CurrentDialogList.GetCount();i++)
	{
		CDialog *pDialog = (CDialog *) m_CurrentDialogList.GetNext( pos);
		pDialog->DestroyWindow();
		delete pDialog;
	}
	m_CurrentDialogList.RemoveAll();

}
void CDialog_PackStruct::OnDestroy()
{
	DestroyAllDialogs();
	CDialog_AbstractPackSerializable::OnDestroy();

}

void	CDialog_PackStruct::RegisterPackDialog( const char *_pSuportedClass,CDialog_AbstractPackSerializable *(*_CreatorFunc)(CWnd *) )
{

	m_PackDialogMap.SetAt(_pSuportedClass,(void *)_CreatorFunc);

}
BOOL CDialog_PackStruct::OnInitDialog()
{
	CDialog_AbstractPackSerializable::OnInitDialog();

	return TRUE;

}
void CDialog_PackStruct::SetMemberName(	const char * )
{

}
