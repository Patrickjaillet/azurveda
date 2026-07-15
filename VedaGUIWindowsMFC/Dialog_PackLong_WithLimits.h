#pragma once

#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"
#include "afxcmn.h"
// Boîte de dialogue CDialog_PackLong_WithLimits

class CDialog_PackLong_WithLimits : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackLong_WithLimits)

public:
	CDialog_PackLong_WithLimits(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialog_PackLong_WithLimits();

// Données de boîte de dialogue
	enum { IDD = IDD_PACKLONG_WITHLIMITS };

	static const char *GetManagedClassName(){ return("PackLong_WithLimits");   };
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
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CStatic m_mintext;
	CStatic m_maxtext;
	CSpinButtonCtrl m_Spin;
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEdit1();
	CStatic m_Focusedit;
};
