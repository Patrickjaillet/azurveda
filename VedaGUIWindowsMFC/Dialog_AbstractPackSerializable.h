#pragma once

// classe de base de tout les dialogues gérant un PackSerializable
// Boîte de dialogue CDialog_AbstractPackSerializable

class BaseType;
//class CVedaDemoOGLMfcGuiDoc;
#include "VedaDemoOGLMfcGuiDoc.h"
class CDialog_AbstractPackSerializable : public CDialog
{
public:
	CDialog_AbstractPackSerializable(UINT nIDTemplate,CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialog_AbstractPackSerializable();
	
	virtual void Create(CWnd* pParent = NULL)=0;

	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);
	virtual	void	Update(void);
	void			Resize(int width, int height);
	inline BaseType *GetManagedObject(){ return m_pObjectToManage; };
	virtual	void SetMemberName(	const char *_pMemberName );
	afx_msg void OnActionEditClicks();
	/*
		\brief	
		\return true if is ActionFocused.
	*/
	bool		IsFocused();

	//! tells if parameters are valid in ChangeValueOfManagedObjectWithCursor()
	static const unsigned int	m_cursorbit_XValid=1;
	//! tells if parameters are valid in ChangeValueOfManagedObjectWithCursor()
	static const unsigned int	m_cursorbit_YValid=2;
	//! tells if parameters are valid in ChangeValueOfManagedObjectWithCursor()
	static const unsigned int	m_cursorbit_ZValid=4;
	//! tells if parameters are valid in ChangeValueOfManagedObjectWithCursor()
	static const unsigned int	m_cursorbit_TimeValid=8;

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
	
	/*
		\class	CFocusable
		\brief	pure nested virtual class to define what object is ActionFocusable, in the application,'s
			own focus management. Each Focusable object should implement it and manage a member.
			anything, AzurVeda object, member, or something else, can be actionfocused. there is only one
			actionfocused object at a time.
			to be declared action-focusable. action focusable means mouse action on the drawing will
			affect the focused object.
	*/
	class CActionFocusableDialog : public CVedaDemoOGLMfcGuiDoc::CActionFocusable
	{
	public:
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
			\param  _mouseButton index of the mouse button. 0=left,1=center,...
			\param	_mouseButtonEvent 0=clickdown, 1=move when down,2=up.
		*/
		virtual void	ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
												float _xClic,float _yClic,double _previewtimesec,
												unsigned int _mouseButton,
												unsigned int _mouseButtonEvent
													);	
		CDialog_AbstractPackSerializable	*m_pManager;
	};
protected: 
	CVedaDemoOGLMfcGuiDoc	*m_pDoc;
	CView					*m_pView;
	BaseType	*m_pObjectToManage;

	 CBitmap m_Bm_Focused;
	 CBitmap m_Bm_UnFocused;
	
	 CActionFocusableDialog	m_ActionFocusable;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMpreview();
	afx_msg void OnBaseobjectClone();
	afx_msg void OnBaseobjectDestroy();
	afx_msg void OnBaseobjectDestroyindependant();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPreviewPrevious();
	afx_msg void OnEditPreview();
	afx_msg void OnEditPrevious();
	//! same for everyone:
	CStatic m_MemberName;
	//afx_msg void OnDestroy();
};
