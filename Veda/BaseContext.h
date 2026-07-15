// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_BaseContext_H
#define COM_M4NKIND_BaseContext_H

#include "BaseObject.h"
#include "ClassNode.h"
#include "PackList.h"

class PackObjectReference;
class VirtualMachine;

class BaseContext : public ClassNode
{

public:
	friend class BaseObject;

	BaseContext(void);
#ifdef _ENGINE_EDITABLE_

	virtual ~BaseContext(void);
#endif
#ifdef _ENGINE_EDITABLE_

	BaseObject *NewObject(  const BaseObject::ClassDescription & _classDescription ,char *  _ObjectName=0L );
#endif
#ifdef _ENGINE_EDITABLE_

	void	DestroyObject( BaseObject * _objectToDelete );
#endif
#ifdef _ENGINE_EDITABLE_

	void	DestroyAllObjects( );
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);
#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif

	virtual void SetMachine( VirtualMachine *_pMachine ){ m_pMachine =_pMachine;  };

	inline VirtualMachine *GetMachine(){ return m_pMachine; };

#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "BaseContext"; };
#endif
#ifdef _ENGINE_EDITABLE_

	void NotifyObjectChange( eVedaUpdate _UpDateType, BaseObject *_pObjectEdited, BaseType *_ObjectMember,
								const unsigned char *_PreviousShapeChunk=0L,
								unsigned int		 _PreviousShapeChunkLength=0L,
								const unsigned char *_NewShapeChunk=0L,
								unsigned int		 _NewShapeChunkLength=0L
								);
#endif

	inline PackObjectReference		*GetFirstObjectReferenceInContextList(){ return m_pFirstObjectReferenceInContextList; };

#ifdef _ENGINE_EDITABLE_

	virtual bool CanUndo();
#endif
#ifdef _ENGINE_EDITABLE_

	virtual bool Undo();
#endif
#ifdef _ENGINE_EDITABLE_

	virtual bool CanRedo();
#endif
#ifdef _ENGINE_EDITABLE_

	virtual bool Redo();
#endif
#ifdef _ENGINE_EDITABLE_

	void FlushUndoStack();
#endif
#ifdef _ENGINE_EDITABLE_

	void SetMaximumNumberOfUndoableEvents( unsigned int _maxNumber );
#endif
#ifdef _ENGINE_EDITABLE_

	void AddGUIUpdateFunction(	void (*_func)(void *_GUIObjectToUpdateReturned,eVedaUpdate _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated) ,
								void *_GUIObjectToUpdate
								);
#endif
#ifdef _ENGINE_EDITABLE_

	bool RemoveGUIUpdateFunction(	void (*_func)(void *_GUIObjectToUpdateReturned,eVedaUpdate _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated)) ;
#endif
#ifdef _ENGINE_EDITABLE_

	void UpdateAllGUI( eVedaUpdate _updateType,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated);
#endif

	void	LinkObjectReference( PackObjectReference *_pObjectReferenceToLink );
#ifdef _ENGINE_EDITABLE_

	void UnLinkObjectReference( PackObjectReference *_pReferenceToUnlink );
#endif
#ifdef _ENGINE_EDITABLE_

	void DestroyAllIndependantObjects( BaseObject *_pObjectWhichAllOtherShouldDependOn );
#endif
#ifdef _ENGINE_EDITABLE_

	virtual  const unsigned char * MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_pendName=0L);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual  bool AddOneLockForThreadUse();
#endif
#ifdef _ENGINE_EDITABLE_

	virtual  void RemoveOneLockForThreadUse();
#endif

#ifdef _ENGINE_EDITABLE_

	void CloneClassesAndObjects(BaseContext *_pContextToModify);
#endif

#ifdef _ENGINE_EDITABLE_

	void ExportAsCPPTableFile(const char *_pfileToExport,const char *_pGobalTableName,bool _optimize=true);
#endif

protected:

	PackObjectReference		*m_pFirstObjectReferenceInContextList;

	VirtualMachine			*m_pMachine;

#ifdef _ENGINE_EDITABLE_

	class UpdateFunctionListCell : public PackStruct
	{ public:
		 void (*m_func)(void *_GUIObjectToUpdateReturned,eVedaUpdate _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated);
		 void *m_UserObject;
		 static BaseType *NewCell(){ return new UpdateFunctionListCell(); };
	};

	PackList			m_GUIUpdateFunctionList;
#endif

#ifdef _ENGINE_EDITABLE_

	class UndoCell : public PackStruct
	{
	public:
			UndoCell();
			void Init(unsigned int _editionType,BaseType *_pObjectMemberEdited,const unsigned char *_PreviousValueChunk,unsigned int _PreviousValueChunkLength,const unsigned char *_AfterValueChunk,unsigned int _AfterValueChunkLength);
			virtual ~UndoCell(void);
			inline BaseType	*GetMemberToChange(){ return m_pObjectMemberEdited;};
			inline unsigned char *GetPreviousValueChunk(){ return m_pPreviousValueChunk;};
			inline unsigned char *GetAfterValueChunk(){ return m_pAfterValueChunk;};
			static BaseType *NewCell(){ return new UndoCell(); };
	protected:
		unsigned int			m_EditionType;
		BaseType	*m_pObjectMemberEdited;
		unsigned char			*m_pPreviousValueChunk;
		unsigned char			*m_pAfterValueChunk;
	};

	PackList			m_UndoStack;

	PackStruct::Cell	*m_pCurrentUndoCell;

	unsigned int			m_CurrentNumberOfUndoableEvents;

	unsigned int			m_MaximumNumberOfUndoableEvents;
#endif
#ifdef _ENGINE_EDITABLE_

	void UpdateAllReferencesSerializationID();
#endif
};

#endif
