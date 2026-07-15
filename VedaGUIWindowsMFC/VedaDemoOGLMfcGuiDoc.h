
#pragma once

class BaseContext;

#include "InterfacePrefObject.h"
class BaseType;
class CPreviewMachine;

#include "VirtualMachine.h"

class CVedaDemoOGLMfcGuiDoc : public CDocument
{
protected:
	CVedaDemoOGLMfcGuiDoc();
	DECLARE_DYNCREATE(CVedaDemoOGLMfcGuiDoc)

public:

	class CActionFocusable
	{
	public:

		CActionFocusable();

		virtual void	NotifySetActionFocusable(bool _isActionFocusable=true);

		inline bool	IsActionFocusable(){ return m_isFocused; };

		virtual void	ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
												float _xClic,float _yClic,double _previewtimesec,
												unsigned int _mouseButton,
												unsigned int _mouseButtonEvent
													);
	protected:

		bool	m_isFocused;

		CActionFocusable	*m_pPrevious;

		CActionFocusable	*m_pNext;
	};

	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

public:
	virtual ~CVedaDemoOGLMfcGuiDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	inline BaseContext *GetVedaContext(){ return(m_pContextManaged); };

	inline unsigned int GetUpdateType(){ return m_updateType; };
	inline BaseContext *GetContextUpdated(){ return m_pContextUpdated ;  };
	inline BaseObject  *GetObjectUpdated(){  return m_pObjectUpdated;   };
	inline BaseType *GetMemberUpdated(){ return m_pMemberUpdated ; };

	inline void SetUpdateType(unsigned int _type){  m_updateType= _type; };
	inline void SetContextUpdated(BaseContext *_pcontext){ m_pContextUpdated=_pcontext ;  };
	inline void SetObjectUpdated(BaseObject  *_pobj){  m_pObjectUpdated=_pobj;   };
	inline void SetMemberUpdated(BaseType *_pMember){ m_pMemberUpdated = _pMember ; };

	inline bool IsCurrentlySerializing(){ return m_IsCurrentlySerializing;  };
	static void		VedaStaticUpdate(void *_GUIObjectToUpdateReturned,unsigned int _updateType, BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated);
	virtual void	VedaUpdate( unsigned int _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated);
	virtual void	OnMerge();
	virtual void	OnFlushNames();

	inline CPreviewMachine	*GetPreviewMachine(){ return m_pPreviewMachine ; };
	void					SetEditionSelectedBaseObject(BaseObject	*pObject);
	BaseObject				*GetEditionSelectedBaseObject(unsigned int EditorIndex=0);

	void					SetPreviewSelectedBaseObject(BaseObject	*_pObject,unsigned int _RenderViewIndex=0);

	BaseObject *GetPreviewSelectedBaseObject(unsigned int _RenderViewIndex=0,InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration **_ppPreviewConfiguration=0L);

	bool					GetPreviewPlayState(unsigned int _RenderViewIndex=0);

	void					SetPreviewPlayState(bool _play,unsigned int _RenderViewIndex=0);
	double					GetPreviewCurrentTime(unsigned int _RenderViewIndex=0);
	void					SetPreviewCurrentTime(double _sec,unsigned int _RenderViewIndex=0);

	void					OnPreviewPrevious();
	void					OnEditPrevious();

	bool					IsActiveDoc();

	void				OnExportcfile();
	void				OnExportExecutable();

	void				OnUndo();
	void				OnRedo();

	void				OnAddLibImage();
	void				OnAddLibBAOOM();

	void OnForceLang( unsigned int _languageCode);

	void	SetActionFocusObject( CActionFocusable *_pObjectToFocus=0L );

	void	SetDefaultActionFocusObject( CActionFocusable *_pObjectToFocus=0L );

	inline 	CActionFocusable *GetActionFocusObject(){ return m_pCurrentActionFocused; };

	void	SetCurrentSoundObject(BaseObject *_pNewSoundObject);
protected:

	BaseContext	*m_pContextManaged;

	BaseObject	*m_pCurrentSoundObject;

	unsigned int m_updateType;
	BaseContext *m_pContextUpdated;
	BaseObject  *m_pObjectUpdated;
	BaseType *m_pMemberUpdated;

	CPreviewMachine			*m_pPreviewMachine;

	clock_t		m_LastStartPlaySystemDate;
	double		m_TotalTimeBeforeLastStop;

	bool		m_IsCurrentlySerializing;

	CActionFocusable					*m_pCurrentActionFocused;
	CActionFocusable					*m_pDefaultActionFocused;

	CMDIChildWnd	*m_pDocumentChildWindow;

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCloseDocument();
};
