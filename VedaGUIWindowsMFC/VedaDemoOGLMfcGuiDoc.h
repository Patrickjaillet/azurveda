// VedaDemoOGLMfcGuiDoc.h : interface de la classe CVedaDemoOGLMfcGuiDoc
//
#pragma once

class BaseContext;
//class BaseObject; because of nested class:
#include "InterfacePrefObject.h"
class BaseType;
class CPreviewMachine;
// for VirtualMachine::InternalViewport
#include "VirtualMachine.h"

/*
	\class	CVedaDemoOGLMfcGuiDoc
	\brief	the CDocument from the MFC point of view. It manages a veda BaseContext ( GetVedaContext() ),
			of any shape, it manages itself an abstract set of class. 
			It redirects the BaseContext messages to the MFC View's updates, redirect the context's
			serialization to the CDocument serialization.
			It also manages some GUI states for previews.
*/
class CVedaDemoOGLMfcGuiDoc : public CDocument
{
protected: // création à partir de la sérialisation uniquement
	CVedaDemoOGLMfcGuiDoc();
	DECLARE_DYNCREATE(CVedaDemoOGLMfcGuiDoc)

public:
	/*
		\class	CFocusable
		\brief	nested class to define what object is ActionFocusable, in the application,'s
			own focus management. Each Focusable object should implement it and manage a member.
			anything, veda object, member, or something else, can be actionfocused. there is only one
			actionfocused object at a time.
			to be declared action-focusable. action focusable means mouse action on the drawing will
			affect the focused object.
	*/
	class CActionFocusable 
	{
	public:
		/*
			\brief constr.
		*/
		CActionFocusable();
		/*
			\brief	sent when the object become the actionfocused one.	
		*/
		virtual void	NotifySetActionFocusable(bool _isActionFocusable=true);
		/*
			\brief	return if this object stands for the action focusable one.
		*/
		inline bool	IsActionFocusable(){ return m_isFocused; };
		/*
			\brief	a viewport click will affect something in a way or another
						according to the active CActionFocusable. It is a 
						virtual method to be overriden.
			\param	_pClickedViewPort the viewport that has been clicked.
			\param	_pPreviewConf	the preview configuration object to change.
			\param	_xClic the position clicked in _pClickedViewPort's ratio
			\param	_yClic the position clicked in _pClickedViewPort's ratio
			\param	_previewtimesec preview time position.
			\param  _mouseButton index of the mouse button. 1=left,2=center,4=right...
			\param	_mouseButtonEvent 0=clickdown, 1=move when down,2=up.
		*/
		virtual void	ManageViewPortClickAction(  VirtualMachine::InternalViewPort *_pClickedViewPort,
												InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
												float _xClic,float _yClic,double _previewtimesec,
												unsigned int _mouseButton,
												unsigned int _mouseButtonEvent
													);
	protected:
		//! true if currently selected.
		bool	m_isFocused;
		//! previous possible focusable or null.
		CActionFocusable	*m_pPrevious;
		//! next possible focusable or null.
		CActionFocusable	*m_pNext;
	};


// Substitutions
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implémentation
public:
	virtual ~CVedaDemoOGLMfcGuiDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	inline BaseContext *GetVedaContext(){ return(m_pContextManaged); };

//	void	SetEditionSelectedBaseObject(BaseObject	*pObject);
//	BaseObject	*GetEditionSelectedBaseObject();

	inline unsigned int GetUpdateType(){ return m_updateType; };
	inline BaseContext *GetContextUpdated(){ return m_pContextUpdated ;  };
	inline BaseObject  *GetObjectUpdated(){  return m_pObjectUpdated;   };
	inline BaseType *GetMemberUpdated(){ return m_pMemberUpdated ; };

	inline void SetUpdateType(unsigned int _type){  m_updateType= _type; };
	inline void SetContextUpdated(BaseContext *_pcontext){ m_pContextUpdated=_pcontext ;  };
	inline void SetObjectUpdated(BaseObject  *_pobj){  m_pObjectUpdated=_pobj;   };
	inline void SetMemberUpdated(BaseType *_pMember){ m_pMemberUpdated = _pMember ; };

	//! \brief need to know if we are currently serializing: no use to redraw updates within a context load or merge:
	inline bool IsCurrentlySerializing(){ return m_IsCurrentlySerializing;  };
	static void		VedaStaticUpdate(void *_GUIObjectToUpdateReturned,unsigned int _updateType, BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated);
	virtual void	VedaUpdate( unsigned int _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated);
	virtual void	OnMerge();
	virtual void	OnFlushNames();
	// preview methods:
	/*oldtypedef enum {
		eps_Stop=0,
		eps_Play	
	} ePlayState ;
	*/
	inline CPreviewMachine	*GetPreviewMachine(){ return m_pPreviewMachine ; };
	void					SetEditionSelectedBaseObject(BaseObject	*pObject);
	BaseObject				*GetEditionSelectedBaseObject(unsigned int EditorIndex=0);
	/*!
		\brief	Set a current Preview object for a context.
		\param _pObject BaseObject to set must belong to the context.
		\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
	*/
	void					SetPreviewSelectedBaseObject(BaseObject	*_pObject,unsigned int _RenderViewIndex=0);
	/*!
		\brief	Get a current Preview object for a context.
					If there are none, return 0L.
		\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
		\param _pPreviewConfiguration if not nul, return a pointer to the configuration preview to it.
		\return the current Preview object or 0L.
	*/
	BaseObject *GetPreviewSelectedBaseObject(unsigned int _RenderViewIndex=0,InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration **_ppPreviewConfiguration=0L);
	/*
		\brief	Get play/pause state for a Preview.
		\return	 if true, play, if false, pause.
	*/
	bool					GetPreviewPlayState(unsigned int _RenderViewIndex=0);
	/*
		\brief	set play/pause state for a Preview.
		\param	_play if true, play, if false, pause.
	*/
	void					SetPreviewPlayState(bool _play,unsigned int _RenderViewIndex=0);
	double					GetPreviewCurrentTime(unsigned int _RenderViewIndex=0); // in seconds.
	void					SetPreviewCurrentTime(double _sec,unsigned int _RenderViewIndex=0); // in seconds.

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

	/*!
		\brief	set an object from the context (object or object members) that is to be edited
				by actions on the preview.
		\param _pObjectToFocus the object, or 0L or nothing, when there is no more object to edit.
	*/
	void	SetActionFocusObject( CActionFocusable *_pObjectToFocus=0L );
	/*!
		\brief	set the default focus. when focus is killed, it become
			automatically the default.
		\param _pObjectToFocus the object, or 0L or nothing, when there is no more object to edit.
	*/
	void	SetDefaultActionFocusObject( CActionFocusable *_pObjectToFocus=0L );

	/*!
		\brief	Get an object from the context (object or object members) that is to be edited
				by actions on the preview. 0L if no object to edit.
		\return a CActionFocusable object, or 0L or nothing, when there is no more object to edit.
	*/
	inline 	CActionFocusable *GetActionFocusObject(){ return m_pCurrentActionFocused; };

	/*
		\brief	Set the object actually previewed, to enable/disable sound 
		\param	_pNewSoundObject
	*/
	void	SetCurrentSoundObject(BaseObject *_pNewSoundObject);
protected:

	//! the context that stands for the document:
	BaseContext	*m_pContextManaged;
	//! the current selected object from the context, or NULL:
	BaseObject	*m_pCurrentSoundObject;
	//old BaseObject	*m_pSelectedBaseObject;
	//! update info:
	unsigned int m_updateType;
	BaseContext *m_pContextUpdated;
	BaseObject  *m_pObjectUpdated;
	BaseType *m_pMemberUpdated;

	//! the preview machine object linked to this context:
	CPreviewMachine			*m_pPreviewMachine;
	//! the current play state
	//old ePlayState	m_ePlayState;
	//! the time date
	clock_t		m_LastStartPlaySystemDate;
	double		m_TotalTimeBeforeLastStop;
		
	bool		m_IsCurrentlySerializing;

	// ------------ Focused Object or member to edit: ------------
	//! a current focused member to edit graphically. as it can be an object member, it can't be link and serialized directly.
	//CDialog_AbstractPackSerializable	* m_pCurrentActionFocused;
	CActionFocusable					*m_pCurrentActionFocused;
	CActionFocusable					*m_pDefaultActionFocused;

	CMDIChildWnd	*m_pDocumentChildWindow;
	// Fonctions générées de la table des messages
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCloseDocument();
};


