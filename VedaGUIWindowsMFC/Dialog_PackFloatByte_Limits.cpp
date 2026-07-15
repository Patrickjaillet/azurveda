
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "Dialog_PackFloatByte_Limits.h"
#include ".\dialog_PackFloatByte_Limits.h"
#include "PackFloatByte_Limits.h"
void CDialog_PackFloatByte_Limits::Create(CWnd* pParent )
{
	CDialog::Create( IDD_PackFloatByte_Limits,pParent);
}

CDialog_AbstractPackSerializable *CDialog_PackFloatByte_Limits::NewInstance(CWnd* pParent)
{
	return new CDialog_PackFloatByte_Limits(pParent);

}

IMPLEMENT_DYNAMIC(CDialog_PackFloatByte_Limits, CDialog)
CDialog_PackFloatByte_Limits::CDialog_PackFloatByte_Limits(CWnd* pParent )
	: CDialog_AbstractPackSerializable(CDialog_PackFloatByte_Limits::IDD, pParent)
{
}

CDialog_PackFloatByte_Limits::~CDialog_PackFloatByte_Limits()
{
}

void CDialog_PackFloatByte_Limits::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_EDIT1, m_editctrl);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
	DDX_Control(pDX, IDC_STATICMIN, m_mintext);
	DDX_Control(pDX, IDC_STATICMAX, m_maxtext);
	DDX_Control(pDX, IDC_MEMBERNAME, m_MemberName);
	DDX_Control(pDX, IDC_EDIT2, m_editctrlY);
	DDX_Control(pDX, IDC_EDIT3, m_editctrlZ);
	DDX_Control(pDX, IDC_EDIT4, m_editctrlD);
	DDX_Control(pDX, IDC_SLIDER2, m_sliderY);
	DDX_Control(pDX, IDC_SLIDER3, m_sliderZ);
	DDX_Control(pDX, IDC_SLIDER4, m_sliderD);
	DDX_Control(pDX, IDC_STATICMIN22, m_mintextY);
	DDX_Control(pDX, IDC_STATICMIN3, m_mintextZ);
	DDX_Control(pDX, IDC_STATICMAX2, m_maxtextY);
	DDX_Control(pDX, IDC_STATICMAX3, m_maxtextZ);
	DDX_Control(pDX, IDC_BEDIT_X, m_focusedit);
}

BEGIN_MESSAGE_MAP(CDialog_PackFloatByte_Limits, CDialog)
	ON_WM_HSCROLL()
	ON_STN_CLICKED(IDC_BEDIT_X, OnActionEditClicks)
END_MESSAGE_MAP()

void CDialog_PackFloatByte_Limits::OnOK()
{
	if( !m_pObjectToManage ) return;

	PackFloatByte_Limits *pVedaOb =(PackFloatByte_Limits *) m_pObjectToManage;
	CString ctxt;
	m_editctrl.GetWindowText(ctxt);
	float value = (float)atof(ctxt.GetString());
	pVedaOb->Set(value );

	if(pVedaOb->GetVectorDimension()>1 )
	{
		m_editctrlY.GetWindowText(ctxt);
		float valueY = (float)atof(ctxt.GetString());
		pVedaOb->Set(1,valueY);
	}
	if(pVedaOb->GetVectorDimension()>2 )
	{
		m_editctrlZ.GetWindowText(ctxt);
		float valueZ = (float)atof(ctxt.GetString());
		pVedaOb->Set(2,valueZ);
	}
	if(pVedaOb->GetVectorDimension()>3 )
	{
		m_editctrlD.GetWindowText(ctxt);
		float valueD = (float)atof(ctxt.GetString());
		pVedaOb->Set(3,valueD);
	}

}

void CDialog_PackFloatByte_Limits::OnCancel()
{
	OnOK();
}
void	CDialog_PackFloatByte_Limits::Update(void)
{
	PackFloatByte_Limits *pveda =(PackFloatByte_Limits *) m_pObjectToManage;
	if(!pveda)return;

	if( IsFocused() )
		m_focusedit.SetBitmap( (HBITMAP)m_Bm_Focused.m_hObject );
	else
		m_focusedit.SetBitmap( (HBITMAP)m_Bm_UnFocused.m_hObject );

	CString str;
	str.Format( "%f", pveda->Get() );
	m_editctrl.SetWindowText( str.GetString() );

	float min = pveda->GetMin();
	float max = pveda->GetMax();

	int sliderpos =  (int)((pveda->Get()-min)*255.0/(max-min)) ;
	if( m_slider.GetPos() != sliderpos )
		m_slider.SetPos(sliderpos );
	if(pveda->GetVectorDimension()>1 )
	{
		str.Format( "%f", pveda->Get(1) );
		m_editctrlY.SetWindowText( str.GetString() );
		sliderpos =  (int)((pveda->Get(1)-min)*255.0/(max-min)) ;
		m_sliderY.SetPos(sliderpos );
	}
	if(pveda->GetVectorDimension()>2 )
	{
		str.Format( "%f", pveda->Get(2) );
		m_editctrlZ.SetWindowText( str.GetString() );
		sliderpos =  (int)((pveda->Get(2)-min)*255.0/(max-min)) ;
		m_sliderZ.SetPos(sliderpos );
	}
	if(pveda->GetVectorDimension()>3 )
	{
		str.Format( "%f", pveda->Get(3) );
		m_editctrlD.SetWindowText( str.GetString() );
		sliderpos =  (int)((pveda->Get(3)-min)*255.0/(max-min)) ;
		m_sliderD.SetPos(sliderpos );
	}
}
void CDialog_PackFloatByte_Limits::SetPackSerializable( BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
														   )
{
	CDialog_AbstractPackSerializable::SetPackSerializable(_objToManage,_pDoc,_pView);

	PackFloatByte_Limits *pVedaLong =(PackFloatByte_Limits *)_objToManage;
	float min = pVedaLong->GetMin();
	float max = pVedaLong->GetMax();

	m_slider.SetRange(0,255,true);
	m_sliderY.SetRange(0,255,true);
	m_sliderZ.SetRange(0,255,true);
	m_sliderD.SetRange(0,255,true);

	CString text ;
	text.Format("%.03f",min);
	m_mintext.SetWindowText(text.GetString());
	m_mintextY.SetWindowText(text.GetString());
	m_mintextZ.SetWindowText(text.GetString());
	text.Format("%.03f",max);
	m_maxtext.SetWindowText(text.GetString());
	m_maxtextY.SetWindowText(text.GetString());
	m_maxtextZ.SetWindowText(text.GetString());

	m_editctrlY.ShowWindow( SW_HIDE );
	m_editctrlZ.ShowWindow( SW_HIDE );
	m_editctrlD.ShowWindow( SW_HIDE );
	m_sliderY.ShowWindow( SW_HIDE );
	m_sliderZ.ShowWindow( SW_HIDE );
	m_sliderD.ShowWindow( SW_HIDE );
	m_mintextY.ShowWindow( SW_HIDE );
	m_mintextZ.ShowWindow( SW_HIDE );
	m_maxtextY.ShowWindow( SW_HIDE );
	m_maxtextZ.ShowWindow( SW_HIDE );

	PackFloat::VectorDimension  dim= pVedaLong->GetVectorDimension();
	switch(dim)
	{
		default:
		case PackFloat::vd_X:
		{
			Resize(-1, 24);
		}
		break;
		case PackFloat::vd_XY:
		{
			m_editctrlY.ShowWindow( SW_SHOW );
			m_sliderY.ShowWindow( SW_SHOW );
			m_mintextY.ShowWindow( SW_SHOW );
			m_maxtextY.ShowWindow( SW_SHOW );
			Resize(-1, 50);
		}
		break;
		case PackFloat::vd_XYZ:
		{
			m_editctrlY.ShowWindow( SW_SHOW );
			m_sliderY.ShowWindow( SW_SHOW );
			m_mintextY.ShowWindow( SW_SHOW );
			m_maxtextY.ShowWindow( SW_SHOW );
			m_editctrlZ.ShowWindow( SW_SHOW );
			m_sliderZ.ShowWindow( SW_SHOW );
			m_mintextZ.ShowWindow( SW_SHOW );
			m_maxtextZ.ShowWindow( SW_SHOW );
			Resize(-1, 78);
		}
		break;
		case PackFloat::vd_XYZD:
		{
			m_editctrlY.ShowWindow( SW_SHOW );
			m_sliderY.ShowWindow( SW_SHOW );
			m_mintextY.ShowWindow( SW_SHOW );
			m_maxtextY.ShowWindow( SW_SHOW );
			m_editctrlZ.ShowWindow( SW_SHOW );
			m_sliderZ.ShowWindow( SW_SHOW );
			m_mintextZ.ShowWindow( SW_SHOW );
			m_maxtextZ.ShowWindow( SW_SHOW );
			m_editctrlD.ShowWindow( SW_SHOW );
			m_sliderD.ShowWindow( SW_SHOW );
			Resize(-1, 102);
		}
		break;
	}

	Update();
}

void CDialog_PackFloatByte_Limits::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog_AbstractPackSerializable::OnHScroll(nSBCode, nPos, pScrollBar);

	if(pScrollBar)
	{
		int id = pScrollBar->GetDlgCtrlID();
		PackFloatByte_Limits *pVedaOb =(PackFloatByte_Limits *) m_pObjectToManage;
		if( pVedaOb )
		{
			if( id == IDC_SLIDER1 )
			{
				int pos = m_slider.GetPos();
				float min = pVedaOb->GetMin();
				float max = pVedaOb->GetMax();
				pVedaOb->Set((float)( min + ((max-min)* (float)pos/255.0))  );
			} else
			if( id == IDC_SLIDER2 )
			{
				int pos = m_sliderY.GetPos();
				float min = pVedaOb->GetMin();
				float max = pVedaOb->GetMax();
				pVedaOb->Set(1,(float)( min + ((max-min)* (float)pos/255.0))  );

			} else
			if( id == IDC_SLIDER3 )
			{
				int pos = m_sliderZ.GetPos();
				float min = pVedaOb->GetMin();
				float max = pVedaOb->GetMax();
				pVedaOb->Set(2,(float)( min + ((max-min)* (float)pos/255.0))  );
			}
			if( id == IDC_SLIDER4 )
			{
				int pos = m_sliderD.GetPos();
				float min = pVedaOb->GetMin();
				float max = pVedaOb->GetMax();
				pVedaOb->Set(3,(float)( min + ((max-min)* (float)pos/255.0))  );
			}
		}
	}

}

BOOL CDialog_PackFloatByte_Limits::OnInitDialog()
{
	CDialog_AbstractPackSerializable::OnInitDialog();
	m_Bm_Focused.LoadBitmap( IDC_B_RESETVPFOCUS );
	m_Bm_UnFocused.LoadBitmap( IDC_B_RESETVP );

	return TRUE;

}

void CDialog_PackFloatByte_Limits::ChangeValueOfManagedObjectWithCursor(
	VirtualMachine::InternalViewPort *_pClickedViewPort,
	unsigned int _validityBits,
	float _x,float _y,float _z,double _time )
{
	PackFloatByte_Limits *pPackFloat =(PackFloatByte_Limits *) m_pObjectToManage;
	if( !pPackFloat ) return;

	_x  = (_x-_pClickedViewPort->GetPositionX1()) /
	(_pClickedViewPort->GetPositionX2() - _pClickedViewPort->GetPositionX1() );
	_y  = (_y-_pClickedViewPort->GetPositionY1()) /
	(_pClickedViewPort->GetPositionY2() - _pClickedViewPort->GetPositionY1() );
	_z  = (_z-_pClickedViewPort->GetPositionY1()) /
	(_pClickedViewPort->GetPositionY2() - _pClickedViewPort->GetPositionY1() );

	if( _validityBits & m_cursorbit_XValid ) pPackFloat->Set(
		pPackFloat->GetMin()+ _x * (pPackFloat->GetMax()-pPackFloat->GetMin())
					);
	PackFloat::VectorDimension  dim= pPackFloat->GetVectorDimension();
	if( (_validityBits & m_cursorbit_YValid) && dim>PackFloat::vd_X )
				pPackFloat->Set(1,pPackFloat->GetMin()+ _y * (pPackFloat->GetMax()-pPackFloat->GetMin()));
	if( (_validityBits & m_cursorbit_ZValid) && dim>PackFloat::vd_XY )
				pPackFloat->Set(2,pPackFloat->GetMin()+ _y * (pPackFloat->GetMax()-pPackFloat->GetMin()));
	if( (_validityBits & m_cursorbit_TimeValid) && dim>PackFloat::vd_XYZ ) pPackFloat->Set(3,(float)_time);

}
