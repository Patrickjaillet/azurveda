
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackDynamicType.h"

#include "PackDynamicType.h"
#include ".\dialog_packdynamictype.h"

void CDialog_PackDynamicType::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PACKDYNAMICTYPE,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackDynamicType::NewInstance(CWnd* pParent)
{
	return new CDialog_PackDynamicType(pParent);

}

IMPLEMENT_DYNAMIC(CDialog_PackDynamicType, CDialog)
CDialog_PackDynamicType::CDialog_PackDynamicType(CWnd* pParent )
	: CDialog_AbstractPackSerializable(CDialog_PackDynamicType::IDD, pParent)
	,m_pCurrentShapeObject(0L)
	,m_pCurrentShapeDialog(0L)
{
}

CDialog_PackDynamicType::~CDialog_PackDynamicType()
{
	if( m_pCurrentShapeDialog)
	{
		m_pCurrentShapeDialog->DestroyWindow();
		delete m_pCurrentShapeDialog ;
		m_pCurrentShapeDialog = NULL ;
	}
}

void CDialog_PackDynamicType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_COMBOENUMS, m_ComboBox);
}

BEGIN_MESSAGE_MAP(CDialog_PackDynamicType, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBOENUMS, OnCbnSelchangeComboenums)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CDialog_PackDynamicType::OnOK()
{
}
void CDialog_PackDynamicType::OnCancel()
{
}
void	CDialog_PackDynamicType::Update(void)
{
	PackDynamicType *pDynamicType =(PackDynamicType *) m_pObjectToManage;
	if(!pDynamicType)return;

	m_ComboBox.SetCurSel( pDynamicType->GetSelectedIndex() );

	SetCurrentShape( pDynamicType->Get() );

	int		thiswidth,thisheight;
	CRect listrec;
	m_ComboBox.GetClientRect(&listrec);
	m_ComboBox.ClientToScreen(&listrec);
	ScreenToClient(&listrec);
	CRect editorrec(0,0,1,1);
	thiswidth = listrec.right+2;
	thisheight = listrec.Height()+2;
	if(m_pCurrentShapeDialog)
	{
		m_pCurrentShapeDialog->Update();
		m_pCurrentShapeDialog->GetWindowRect( &editorrec);
		thiswidth += editorrec.Width();

		m_pCurrentShapeDialog->MoveWindow( listrec.right+2, listrec.top-2 ,editorrec.Width() ,editorrec.Height());
		if( thisheight<editorrec.Height() )thisheight = editorrec.Height();

	}

	Resize(thiswidth+2,thisheight);

}
void	CDialog_PackDynamicType::SetCurrentShape(BaseType *_pCurrentShape)
{
	if(m_pCurrentShapeObject == _pCurrentShape) return;

	if( m_pCurrentShapeDialog)
	{
		m_pCurrentShapeDialog->DestroyWindow();
		delete m_pCurrentShapeDialog ;
		m_pCurrentShapeDialog = NULL ;
	}
	m_pCurrentShapeObject = _pCurrentShape ;
	if(_pCurrentShape == NULL) return;

	m_pCurrentShapeDialog = new CDialog_PackSerializable( this);
	if( m_pCurrentShapeDialog)
	{
		m_pCurrentShapeDialog->Create(  this);

		m_pCurrentShapeDialog->SetPackSerializable(_pCurrentShape,m_pDoc,m_pView);

		m_pCurrentShapeDialog->ShowWindow(SW_SHOW);
	}

}

void CDialog_PackDynamicType::SetPackSerializable(	BaseType *_objToManage,
							CVedaDemoOGLMfcGuiDoc *_pDoc,
							CView				  *_pView
							)
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);

	PackDynamicType *pDynamicType =(PackDynamicType *) m_pObjectToManage;
	if(!pDynamicType)return;

	m_ComboBox.AddString("-");
	PackStruct::Cell *pcell = pDynamicType->GetFirstCell();
	while( pcell )
	{
		BaseType *pShape = pcell->GetManagedObject();
		if(pShape->GetInfoString())
		{
			m_ComboBox.AddString(pShape->GetInfoString() );
		}
		pcell = pcell->GetNextBrotherCell();
	}
	Update();

}

void CDialog_PackDynamicType::OnCbnSelchangeComboenums()
{
	PackDynamicType *pDynamicType =(PackDynamicType *) m_pObjectToManage;
	if(!pDynamicType)return;

	int sel = m_ComboBox.GetCurSel();
	pDynamicType->Set(sel);
}

void CDialog_PackDynamicType::OnDestroy()
{
	if( m_pCurrentShapeDialog)
	{
		m_pCurrentShapeDialog->DestroyWindow();
		delete m_pCurrentShapeDialog ;
		m_pCurrentShapeDialog = NULL ;
	}
	CDialog_AbstractPackSerializable::OnDestroy();

}
