#pragma once

#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"
#include "afxcmn.h"

// Boîte de dialogue CDialog_PackFloatByte_Limits

class CDialog_PackFloatByte_Limits : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackFloatByte_Limits)

public:
	CDialog_PackFloatByte_Limits(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialog_PackFloatByte_Limits();

// Données de boîte de dialogue
	enum { IDD = IDD_PackFloatByte_Limits };

	static const char *GetManagedClassName(){ return("PackFloat::PackFloatByte_Limits");   };
	static CDialog_AbstractPackSerializable *NewInstance(CWnd* pParent);

	virtual void Create(CWnd* pParent = NULL);
	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);
	virtual	void	Update(void);
	/*
		\brief	if an object of any type is managed, it could be changed by a cursor state.
				by default, it does nothing. It has to be extended.
		\param	_validityBits tels if _x,_y,_z and _time are  valid
		\param	_x x position.
		\param	_y y position.
		\param	_z z position.
		\param	_time the time of the cursor in seconds.
	*/
	virtual	void ChangeValueOfManagedObjectWithCursor(
						VirtualMachine::InternalViewPort *_pClickedViewPort,
						unsigned int _validityBits,
						float _x,float _y,float _z,double _time );
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	CEdit m_editctrl;
	CSliderCtrl m_slider;
	CStatic m_mintext;
	CStatic m_maxtext;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	CEdit m_editctrlY;
	CEdit m_editctrlZ;
	CEdit m_editctrlD;
	CSliderCtrl m_sliderY;
	CSliderCtrl m_sliderZ;
	CSliderCtrl m_sliderD;
	CStatic m_mintextY;
	CStatic m_mintextZ;
	CStatic m_maxtextY;
	CStatic m_maxtextZ;
	virtual BOOL OnInitDialog();
	CStatic m_focusedit;
};



