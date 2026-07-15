#pragma once

#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"
#include "afxcmn.h"

class CDialog_PackLong_WithLimits : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackLong_WithLimits)

public:
	CDialog_PackLong_WithLimits(CWnd* pParent = NULL);
	virtual ~CDialog_PackLong_WithLimits();

	enum { IDD = IDD_PACKLONG_WITHLIMITS };

	static const char *GetManagedClassName(){ return("PackLong_WithLimits");   };
	static CDialog_AbstractPackSerializable *NewInstance(CWnd* pParent);

	virtual void Create(CWnd* pParent = NULL);
	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);
	virtual	void	Update(void);

	virtual	void ChangeValueOfManagedObjectWithCursor(
						VirtualMachine::InternalViewPort *_pClickedViewPort,
						unsigned int _validityBits,
						float _x,float _y,float _z,double _time );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

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
