#pragma once

class BaseType;

#include "VedaDemoOGLMfcGuiDoc.h"
class CDialog_AbstractPackSerializable : public CDialog
{
public:
	CDialog_AbstractPackSerializable(UINT nIDTemplate,CWnd* pParent = NULL);
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

	bool		IsFocused();

	static const unsigned int	m_cursorbit_XValid=1;

	static const unsigned int	m_cursorbit_YValid=2;

	static const unsigned int	m_cursorbit_ZValid=4;

	static const unsigned int	m_cursorbit_TimeValid=8;

	virtual	void ChangeValueOfManagedObjectWithCursor(
						VirtualMachine::InternalViewPort *_pClickedViewPort,
						unsigned int _validityBits,
						float _x,float _y,float _z,double _time );

	class CActionFocusableDialog : public CVedaDemoOGLMfcGuiDoc::CActionFocusable
	{
	public:

		virtual void	NotifySetActionFocusable(bool _isActionFocusable=true);

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

	CStatic m_MemberName;

};
