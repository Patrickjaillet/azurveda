
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackULongFlags.h"
#include ".\dialog_packulongflags.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "PackULong_Flags.h"

void CDialog_PackULongFlags::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PACKULONG_FLAGS,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackULongFlags::NewInstance(CWnd* pParent)
{
	return new CDialog_PackULongFlags(pParent);

}
IMPLEMENT_DYNAMIC(CDialog_PackULongFlags, CDialog)
CDialog_PackULongFlags::CDialog_PackULongFlags(CWnd* pParent )
	: CDialog_AbstractPackSerializable(CDialog_PackULongFlags::IDD, pParent)
	,m_NumberOfBitManaged(0)
{
}

CDialog_PackULongFlags::~CDialog_PackULongFlags()
{
}

void CDialog_PackULongFlags::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_CHECK1, m_c[0]);
	DDX_Control(pDX, IDC_CHECK2, m_c[1]);
	DDX_Control(pDX, IDC_CHECK3, m_c[2]);
	DDX_Control(pDX, IDC_CHECK4, m_c[3]);
	DDX_Control(pDX, IDC_CHECK5, m_c[4]);
	DDX_Control(pDX, IDC_CHECK6, m_c[5]);
	DDX_Control(pDX, IDC_CHECK7, m_c[6]);
	DDX_Control(pDX, IDC_CHECK8, m_c[7]);
	DDX_Control(pDX, IDC_CHECK9, m_c[8]);
	DDX_Control(pDX, IDC_CHECK10, m_c[9]);
}

BEGIN_MESSAGE_MAP(CDialog_PackULongFlags, CDialog)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck0)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK3, OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK4, OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK5, OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_CHECK6, OnBnClickedCheck5)
	ON_BN_CLICKED(IDC_CHECK7, OnBnClickedCheck6)
	ON_BN_CLICKED(IDC_CHECK8, OnBnClickedCheck7)
	ON_BN_CLICKED(IDC_CHECK9, OnBnClickedCheck8)
	ON_BN_CLICKED(IDC_CHECK10, OnBnClickedCheck9)
END_MESSAGE_MAP()

void CDialog_PackULongFlags::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
														   )
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);

	PackULong_Flags *pVedaFlags = (PackULong_Flags *)m_pObjectToManage;

	CString bitfieldmeaning =	pVedaFlags->GetBitFieldInfo();
	int numberOfBitUsed=0;
	int index;
	for( index=0 ; index<NBCHECKBOX ; index++) m_c[index].ShowWindow(false);
	index=0;

	while( index<bitfieldmeaning.GetLength() )
	{
		if(numberOfBitUsed<NBCHECKBOX) m_c[numberOfBitUsed].ShowWindow(true);
		int next = bitfieldmeaning.Find('.',index);
		if(next == -1)
		{
			CString substring = bitfieldmeaning.Mid( index, bitfieldmeaning.GetLength()-index );
			if(numberOfBitUsed<NBCHECKBOX) m_c[numberOfBitUsed].SetWindowText( substring.GetString() );
			numberOfBitUsed++;

			break;
		}
		CString substring = bitfieldmeaning.Mid( index,next-index );
		if(numberOfBitUsed<NBCHECKBOX) m_c[numberOfBitUsed].SetWindowText( substring.GetString() );
		index = next+1;
		numberOfBitUsed++;
	}
	if( m_NumberOfBitManaged>NBCHECKBOX ) m_NumberOfBitManaged = NBCHECKBOX;
	m_NumberOfBitManaged = numberOfBitUsed;

	if(m_NumberOfBitManaged<=5) Resize(-1, 20);

	Update();

}

void CDialog_PackULongFlags::OnOK()
{

}

void CDialog_PackULongFlags::OnCancel()
{

}
void	CDialog_PackULongFlags::Update(void)
{
	PackULong_Flags *pVedaFlag =(PackULong_Flags *) m_pObjectToManage;
	if(!pVedaFlag)return;
	unsigned int value = pVedaFlag->Get();
	unsigned int ii;

	for(ii=0 ; ii<m_NumberOfBitManaged && ii<NBCHECKBOX ; ii++)
	{
		if( (value & 1)!= 0 )
			m_c[ii].SetCheck( BST_CHECKED );
		else
			m_c[ii].SetCheck( BST_UNCHECKED );
		value>>=1;
	}

}
void CDialog_PackULongFlags::OnBnClickedCheck0()
{
	PackULong_Flags *pVedaFlag =(PackULong_Flags *) m_pObjectToManage;
	if(!pVedaFlag)return;
	unsigned int value = pVedaFlag->Get();

	if( m_c[0].GetCheck() == BST_CHECKED )
		value |=1;
	else
		value &=(~1);

	pVedaFlag->Set( value );
}

void CDialog_PackULongFlags::OnBnClickedCheck1()
{
	PackULong_Flags *pVedaFlag =(PackULong_Flags *) m_pObjectToManage;
	if(!pVedaFlag)return;
	unsigned int value = pVedaFlag->Get();

	if( m_c[1].GetCheck() == BST_CHECKED )
		value |=2;
	else
		value &=(~2);

	pVedaFlag->Set( value );
}

void CDialog_PackULongFlags::OnBnClickedCheck2()
{
	PackULong_Flags *pVedaFlag =(PackULong_Flags *) m_pObjectToManage;
	if(!pVedaFlag)return;
	unsigned int value = pVedaFlag->Get();

	if( m_c[2].GetCheck() == BST_CHECKED )
		value |=4;
	else
		value &=(~4);

	pVedaFlag->Set( value );
}

void CDialog_PackULongFlags::OnBnClickedCheck3()
{
	PackULong_Flags *pVedaFlag =(PackULong_Flags *) m_pObjectToManage;
	if(!pVedaFlag)return;
	unsigned int value = pVedaFlag->Get();

	if( m_c[3].GetCheck() == BST_CHECKED )
		value |=8;
	else
		value &=(~8);

	pVedaFlag->Set( value );
}

void CDialog_PackULongFlags::OnBnClickedCheck4()
{
	PackULong_Flags *pVedaFlag =(PackULong_Flags *) m_pObjectToManage;
	if(!pVedaFlag)return;
	unsigned int value = pVedaFlag->Get();

	if( m_c[4].GetCheck() == BST_CHECKED )
		value |=16;
	else
		value &=(~16);

	pVedaFlag->Set( value );
}

void CDialog_PackULongFlags::OnBnClickedCheck5()
{
	PackULong_Flags *pVedaFlag =(PackULong_Flags *) m_pObjectToManage;
	if(!pVedaFlag)return;
	unsigned int value = pVedaFlag->Get();

	if( m_c[5].GetCheck() == BST_CHECKED )
		value |=32;
	else
		value &=(~32);

	pVedaFlag->Set( value );
}

void CDialog_PackULongFlags::OnBnClickedCheck6()
{
	PackULong_Flags *pVedaFlag =(PackULong_Flags *) m_pObjectToManage;
	if(!pVedaFlag)return;
	unsigned int value = pVedaFlag->Get();

	if( m_c[6].GetCheck() == BST_CHECKED )
		value |=64;
	else
		value &=(~64);

	pVedaFlag->Set( value );
}

void CDialog_PackULongFlags::OnBnClickedCheck7()
{
	PackULong_Flags *pVedaFlag =(PackULong_Flags *) m_pObjectToManage;
	if(!pVedaFlag)return;
	unsigned int value = pVedaFlag->Get();

	if( m_c[7].GetCheck() == BST_CHECKED )
		value |=128;
	else
		value &=(~128);

	pVedaFlag->Set( value );
}

void CDialog_PackULongFlags::OnBnClickedCheck8()
{
	PackULong_Flags *pVedaFlag =(PackULong_Flags *) m_pObjectToManage;
	if(!pVedaFlag)return;
	unsigned int value = pVedaFlag->Get();

	if( m_c[8].GetCheck() == BST_CHECKED )
		value |=256;
	else
		value &=(~256);

	pVedaFlag->Set( value );
}

void CDialog_PackULongFlags::OnBnClickedCheck9()
{
	PackULong_Flags *pVedaFlag =(PackULong_Flags *) m_pObjectToManage;
	if(!pVedaFlag)return;
	unsigned int value = pVedaFlag->Get();

	if( m_c[9].GetCheck() == BST_CHECKED )
		value |=512;
	else
		value &=(~512);

	pVedaFlag->Set( value );
}
