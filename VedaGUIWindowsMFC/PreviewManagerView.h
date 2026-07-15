#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "VedaDemoOGLMfcGuiDoc.h"
class InterfacePrefObject;
// Mode formulaire CPreviewManagerView

class CPreviewManagerView : public CFormView
{
	DECLARE_DYNCREATE(CPreviewManagerView)

protected:
	CPreviewManagerView();           // constructeur protégé utilisé par la création dynamique
	virtual ~CPreviewManagerView();

public:
	enum { IDD = IDD_FORMVIEWPREVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
inline CVedaDemoOGLMfcGuiDoc* GetDocument()
   { return reinterpret_cast<CVedaDemoOGLMfcGuiDoc*>(m_pDocument); }
	
	/*
		\class	CFocusable
		\brief	pure nested virtual class to define what object is ActionFocusable, in the application,'s
			own focus management. Each Focusable object should implement it and manage a member.
			anything, AzurVeda object, member, or something else, can be actionfocused. there is only one
			actionfocused object at a time.
			to be declared action-focusable. action focusable means mouse action on the drawing will
			affect the focused object.
	*/
	class CActionFocusable_Main : public CVedaDemoOGLMfcGuiDoc::CActionFocusable
	{
	public:
		/*
			\brief	sent when the object become the actionfocused one.	
		*/
		CActionFocusable_Main(){ m_pLastViewPortClicked=0L; m_LastXClick=m_LastYClick=0.0f; };

		/*
			\brief	sent when the object become the actionfocused one.	
		*/
		virtual void	NotifySetActionFocusable(bool _isActionFocusable=true);

		/*
			\brief	a viewport click will affect something in a way or another
						according to the active CActionFocusable. It is a 
						virtual method to be overriden.
			\param	_pClickedViewPort the viewport that has been clicked.
			\param	_xClic the position clicked in _pClickedViewPort's ratio
			\param	_yClic the position clicked in _pClickedViewPort's ratio
			\param	_previewtimesec preview time position.
			\param  _mouseButton index of the mouse button. 1=left,2=center,4=right...
			\param	_mouseButtonEvent 0=clickdown, 1=move when down,2=up.
		*/
		/*virtual void	ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												float _xClic,float _yClic,double _previewtimesec,
												unsigned int _mouseButton,
												unsigned int _mouseButtonEvent
													);	*/
		//! the object that has it as a member.
		CPreviewManagerView	*m_pManager;
		//! 
		VirtualMachine::InternalViewPort *m_pLastViewPortClicked;
		//! first click coord, before any move.
		float	m_LastXClick;
		//! 
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
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	bool	m_isInited;

	void	UpdateFromPreferenceObject(InterfacePrefObject *ppref);
	//! currently displayed prefset.
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


