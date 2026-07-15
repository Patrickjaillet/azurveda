#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "VedaDemoOGLMfcGuiDoc.h"
class InterfacePrefObject;

class CPreviewManagerView : public CFormView
{
	DECLARE_DYNCREATE(CPreviewManagerView)

protected:
	CPreviewManagerView();
	virtual ~CPreviewManagerView();

public:
	enum { IDD = IDD_FORMVIEWPREVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
inline CVedaDemoOGLMfcGuiDoc* GetDocument()
   { return reinterpret_cast<CVedaDemoOGLMfcGuiDoc*>(m_pDocument); }

	class CActionFocusable_Main : public CVedaDemoOGLMfcGuiDoc::CActionFocusable
	{
	public:

		CActionFocusable_Main(){ m_pLastViewPortClicked=0L; m_LastXClick=m_LastYClick=0.0f; };

		virtual void	NotifySetActionFocusable(bool _isActionFocusable=true);

		CPreviewManagerView	*m_pManager;

		VirtualMachine::InternalViewPort *m_pLastViewPortClicked;

		float	m_LastXClick;

		float	m_LastYClick;

	};
	class CActionFocusable_Move : public CActionFocusable_Main
	{
	public: virtual void	ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
												float _xClic,float _yClic,double _previewtimesec,
												unsigned int _mouseButton,
												unsigned int _mouseButtonEvent
													);
	};
	class CActionFocusable_Rotate : public CActionFocusable_Main
	{
	public: virtual void	ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
												float _xClic,float _yClic,double _previewtimesec,
												unsigned int _mouseButton,
												unsigned int _mouseButtonEvent
													);
	};
	class CActionFocusable_VPMove : public CActionFocusable_Main
	{
	public: virtual void	ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
												float _xClic,float _yClic,double _previewtimesec,
												unsigned int _mouseButton,
												unsigned int _mouseButtonEvent
													);
	};
	class CActionFocusable_VPReset : public CActionFocusable_Main
	{
	public: virtual void	ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
												float _xClic,float _yClic,double _previewtimesec,
												unsigned int _mouseButton,
												unsigned int _mouseButtonEvent
													);
	};
	CStatic m_ObjectName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* , LPARAM , CObject* );
	bool	m_isInited;

	void	UpdateFromPreferenceObject(InterfacePrefObject *ppref);

	InterfacePrefObject *m_pPref;

	CBitmap	m_Bm_Play;
	CBitmap	m_Bm_Pause;

	CBitmap	m_Bm_Move;
	CBitmap	m_Bm_MoveFocus;
	CBitmap	m_Bm_Rotate;
	CBitmap	m_Bm_RotateFocus;
	CBitmap	m_Bm_Zoom;
	CBitmap	m_Bm_ZoomFocus;
	CBitmap	m_Bm_VPReset;
	CBitmap	m_Bm_VPResetFocus;

public:
	virtual void OnInitialUpdate();
	CEdit m_EditStartTime;
	CEdit m_EditEndTime;
	CStatic m_ButtonPlay;
	CStatic m_ButtonRewind;
	CStatic m_EditDate;
	CStatic m_SpeedStatic;
	CSliderCtrl m_SliderTime;
	CSliderCtrl m_SliderSpeed;
	CSliderCtrl m_SliderVolume;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnKillfocusMinsec();
	afx_msg void OnEnKillfocusMaxsec();
	afx_msg void OnStnClickedSpeed();
	CProgressCtrl m_Progressbar;
	afx_msg void OnStnClickedBplaypause();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnStnClickedBreturnstop();

	afx_msg void OnStnClickedBMove();
	afx_msg void OnStnClickedBRotate();
	afx_msg void OnStnClickedBVPMove();
	afx_msg void OnStnClickedBVPReset();

	CStatic m_butonMove;
	CStatic m_butonRotate;
	CStatic m_butonVPMove;
	CStatic m_butonVPReset;

	CActionFocusable_Move		m_focusMove;
	CActionFocusable_Rotate		m_focusRotate;
	CActionFocusable_VPMove		m_focusVPMove;
	CActionFocusable_VPReset	m_focusVPReset;

	virtual BOOL DestroyWindow();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnSysDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
