
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackULongEnums.h"
#include ".\dialog_packulongenums.h"

#include "PackUlong_Enums.h"

void CDialog_PackULongEnums::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PACKULONG_ENUMS,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackULongEnums::NewInstance(CWnd* pParent)
{
	return new CDialog_PackULongEnums(pParent);

}

IMPLEMENT_DYNAMIC(CDialog_PackULongEnums, CDialog)
CDialog_PackULongEnums::CDialog_PackULongEnums(CWnd* pParent )
	: CDialog_AbstractPackSerializable(CDialog_PackULongEnums::IDD, pParent)
{
}

CDialog_PackULongEnums::~CDialog_PackULongEnums()
{
}

void CDialog_PackULongEnums::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_COMBOENUMS, m_ComboBox);
}

BEGIN_MESSAGE_MAP(CDialog_PackULongEnums, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBOENUMS, OnCbnSelchangeComboenums)
END_MESSAGE_MAP()

void CDialog_PackULongEnums::OnOK()
{
}
void CDialog_PackULongEnums::OnCancel()
{
}
void	CDialog_PackULongEnums::Update(void)
{
	PackULong_Enums *pVedaEnum =(PackULong_Enums *) m_pObjectToManage;
	if(!pVedaEnum)return;
	unsigned int value = pVedaEnum->Get();

	m_ComboBox.SetCurSel(value);

}
void CDialog_PackULongEnums::SetPackSerializable(	BaseType *_objToManage,
							CVedaDemoOGLMfcGuiDoc *_pDoc,
							CView				  *_pView
							)
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);

	PackULong_Enums *pVedaEnum =(PackULong_Enums *) m_pObjectToManage;
	if(!pVedaEnum)return;

	const char *pVedaEnumList = pVedaEnum->GetEnumsInfo();
	if(pVedaEnumList )
	{
		CString cstr;
		const char *pLastStart = pVedaEnumList;
		int wordLength=0;
		do
		{
			wordLength++;
			pVedaEnumList++;
			if( *pVedaEnumList == '.' ||  *pVedaEnumList == 0 )
			{
				cstr.SetString(pLastStart, wordLength);
				m_ComboBox.AddString(cstr.GetString() );
				if( *pVedaEnumList == '.' ) pVedaEnumList++;
				pLastStart = pVedaEnumList;
				wordLength =0;
			}

		} while( *pVedaEnumList != 0 );
	}

}

void CDialog_PackULongEnums::OnCbnSelchangeComboenums()
{
	PackULong_Enums *pVedaEnum =(PackULong_Enums *) m_pObjectToManage;
	if(!pVedaEnum)return;

	int sel = m_ComboBox.GetCurSel();
	pVedaEnum->Set(sel);
}
