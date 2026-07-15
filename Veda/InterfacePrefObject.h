/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_InterfacePrefObject_H
#define COM_M4NKIND_InterfacePrefObject_H

#include "BaseObject.h"
#include "PackULong_Flags.h"
#include "PackLong.h"
#include "PackULong_Enums.h"
#include "PackList.h"
#include "PackObjectReference.h"
#include "PackFloat_FixedPoint.h"
#include "PackFloatByte_Limits.h"
class BaseContext;

/*!
	\class	InterfacePrefObject
	\ingroup BaseObjectInheritedDocGroup BaseObjectInherited_Veda
	\brief	This class provide a unified way to stock preferences and states informations about
			a GUI that would edit a AzurVeda Context, so it is saved within a context. Basically,
			an editor should use Get()/Set() and serialization, to keep the current objects
			edited and previewed. 
			It is not registered to BaseContext by default. you got to register it or not, 
			according to your needs. Thus, it has to be registered first if you use inherited 
			classes. ( through BaseContext::RegisterClassList() of course ).
			As editor preferences may be only useful during edition, it is
			of course possible to flush these objects from the context, like any other objects.
			This class has no sense in non editable mode and should just not be used in such compilation mode.
*/

class InterfacePrefObject : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:

	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	InterfacePrefObject(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(InterfacePrefObject);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
				Special process: purge all null pointer links in the object before serialization.
		\return	byte size of the serialisation to do.
	*/
	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Only one preference object can have the active flag at the same time,
				in the context. This one is returned if it exists.
				If there is no preference object or no active one,
				a new peference object is created, set active, and returned.
		\param	_pContext the context where to search the active preference..
		\return a preference object or NULL.
	*/
	static	InterfacePrefObject *GetContextActivePref(BaseContext *_pContext,bool _createOneIfNone=false);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Set the edited object in interface. It affects the current active 
				Preference object in the context. (it creates one if there wasnt one).
				The _ObjectToEdit must belong to _pContext.
				if _ObjectToEdit is NULL, the edited object is set to NULL 
				(no edit.)
		\param	_ObjectToEdit the object to preview.
		\param _EditViewIndex if the editor support multiple object edition, an abstract index of the view affected.
	*/
	void	SetContextEditedObject(BaseObject *_ObjectToEdit,unsigned int _EditViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Get a current edited object for a context.
					If there are none, return 0L.
		\param _EditViewIndex if there is support for multiple editors, the index of the editor affected.
		\return the current edtited object or 0L.
	*/
	BaseObject *GetContextEditedObject(unsigned int _EditViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Edit Previous: scroll down the history of edited object. May Change result of GetContextPreviewObject().
		\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
	*/
	void EditPrevious(unsigned int _RenderViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Edit Next: scroll up the history of edited object. May Change result of GetContextPreviewObject().
		\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
	*/
	void EditNext(unsigned int _RenderViewIndex=0);
#endif
	class PackObjectReferenceSpecial;
	friend class PackObjectReferenceSpecial;
	/*!
		\brief	 nested class to patch PackObjectReference so that
					If set to null pointer, it deletes 
	*/
	class	PackObjectReferenceSpecial	: public PackObjectReference
	{	public:

		/*! \brief constructor */
		PackObjectReferenceSpecial() : PackObjectReference(){};
#ifdef _ENGINE_EDITABLE_
		/*!
			\brief	see PackObjectReference::Set()
		*/
		virtual	enum_SetReferenceResult	SetObjectPointer( BaseObject * _Object );	
#endif
	};

	class PackULong_Flags_Activate;
	friend class PackULong_Flags_Activate;
	/*!
		\brief	 nested class to patch the activate flag edition
	*/
	class PackULong_Flags_Activate : public	PackULong_Flags
	{ public:
#ifdef _ENGINE_EDITABLE_
		/*!
			\brief	change the value:
			\param	_value 28b unsigned int value.
		*/
		virtual void	Set(unsigned int _value); 
#endif
	};
	/*!
		\brief	 Nested class that describes a reference to an object preview.
	*/
	class PreviewEdited : public PackStruct
	{
	public:
		//! the reference to an object:
		PackObjectReferenceSpecial	mSer_Reference;
		//! for previous/next
		static const unsigned int m_PF_HistoryFlag=1;
		//! if true, the preview is in play mode.
		static const unsigned int m_PF_PlayState=2;
		//! flag for history browsing:
		PackULong_Flags		mSer_Flag;
		//! rectangle of the preview viewport. def is(0,0,1,1)
		PackFloat			mSer_Corner1;
		//! rectangle of the preview viewport. def is(0,0,1,1)
		PackFloat			mSer_Corner2;
		//! Position
		PackFloat			mSer_PreviewPosition;
		//!  Rotation
		PackFloat			mSer_PreviewRotation;
		//!  preview time lapse start
		PackFloat			mSer_PreviewStartTime;
		//!  preview time lapse end
		PackFloat			mSer_PreviewEndTime;
		//! preview time current time:
		PackFloat			mSer_PreviewCurrentTime;
		//! TimePreviewer: playspeed, default 1.0
		PackFloat			mSer_PlaySpeed;

		PreviewEdited(void) : PackStruct()
			,mSer_Corner1(PackFloat::vd_XY)
			,mSer_Corner2(PackFloat::vd_XY)
			,mSer_PreviewPosition(PackFloat::vd_XYZ)
			,mSer_PreviewRotation(PackFloat::vd_XYZ)
			,mSer_PreviewStartTime(PackFloat::vd_X)
			,mSer_PreviewEndTime(PackFloat::vd_X)
			,mSer_PreviewCurrentTime(PackFloat::vd_X)
			,mSer_PlaySpeed(PackFloat::vd_X)
		{
			// Note the "InfoString" of the members are null (0L), so that 
			// modifying them is not managed by undo and does not produce update events.
			REGISTER_MEMBER_REFERENCE(mSer_Reference ,/*"Object"*/0L, BaseObject::m_Description );
			REGISTER_MEMBER_PACKULONG(mSer_Flag ,0L,0 );
			REGISTER_MEMBER_PACKFLOAT_XY(mSer_Corner1 ,0L,0.0f,0.0f );
			REGISTER_MEMBER_PACKFLOAT_XY(mSer_Corner2 ,0L,1.0f,1.0f );
			REGISTER_MEMBER_PACKFLOAT_XYZ(mSer_PreviewPosition ,0L,0.0f,0.0f,0.0f );
			REGISTER_MEMBER_PACKFLOAT_XYZ(mSer_PreviewRotation ,0L,0.0f,0.0f,0.0f );
			REGISTER_MEMBER_PACKFLOAT(mSer_PreviewStartTime ,0L/*"Start"*/,0.0f );
			REGISTER_MEMBER_PACKFLOAT(mSer_PreviewEndTime ,0L/*"End"*/,360.0f );
			REGISTER_MEMBER_PACKFLOAT(mSer_PreviewCurrentTime ,0L/*"Time"*/,0.0f );
			REGISTER_MEMBER_PACKFLOAT(mSer_PlaySpeed ,0L/*"Speed"*/,1.0f );
#ifdef _ENGINE_EDITABLE_
			m_PreviewConfiguration.m_pManager = this;
#endif
		};
#ifdef _ENGINE_EDITABLE_
		/*!
			\brief	 fill a PreviewConfiguration class, to return the object contract for preview.
		*/
		class InterfacePreviewConfiguration  : public PreviewConfiguration
		{ public:
			PreviewEdited	*m_pManager;

			//! \brief	Set a viewport scale in 0.0,0.0,1.0,1.0 units. useful to zoom 2D previews..
			virtual	void	SetViewportScale(float _x1,float _y1,float _x2,float _y2) const
			{  
				m_pManager->mSer_Corner1.Set(_x1);
				m_pManager->mSer_Corner1.Set(1,_y1);
				m_pManager->mSer_Corner2.Set(_x2);
				m_pManager->mSer_Corner2.Set(1,_y2);  } ;
			//! \brief	return a viewport scale in 0.0,0.0,1.0,1.0 units. useful to zoom 2D previews..
			virtual	void	GetViewportScale(float &_x1,float &_y1,float &_x2,float &_y2) const
			{  
				_x1 = m_pManager->mSer_Corner1.Get(0);
				_y1 = m_pManager->mSer_Corner1.Get(1);
				_x2 = m_pManager->mSer_Corner2.Get(0);
				_y2 = m_pManager->mSer_Corner2.Get(1);  } ;
			//! \brief	set an edited object position in a preview.
			virtual	void	SetPreviewPosition(float _PositionX,float _PositionY,float _PositionZ ) const
			{	 m_pManager->mSer_PreviewPosition.Set(_PositionX);
				 m_pManager->mSer_PreviewPosition.Set(1,_PositionY);
				 m_pManager->mSer_PreviewPosition.Set(2,_PositionZ);				
			};
			//! \brief	return an edited object position in a preview.
			virtual	void	GetPreviewPosition(float &_PositionX,float &_PositionY,float &_PositionZ ) const
			{	_PositionX = m_pManager->mSer_PreviewPosition.Get(0);
				_PositionY = m_pManager->mSer_PreviewPosition.Get(1);
				_PositionZ = m_pManager->mSer_PreviewPosition.Get(2);				
			};
			//! \brief	set a rotation in a preview.
			void	SetPreviewRotation(float _Xaxis,float _YAxis,float _ZAxis)
			{	m_pManager->mSer_PreviewRotation.Set( _Xaxis);
				m_pManager->mSer_PreviewRotation.Set(1,_YAxis);
				m_pManager->mSer_PreviewRotation.Set(2,_ZAxis);				
			};
			//! \brief	return a rotation in a preview.
			virtual	void	GetPreviewRotation(float &_Xaxis,float &_YAxis,float &_ZAxis) const 
			{	_Xaxis = m_pManager->mSer_PreviewRotation.Get(0);
				_YAxis = m_pManager->mSer_PreviewRotation.Get(1);
				_ZAxis = m_pManager->mSer_PreviewRotation.Get(2);				
			};
			//! \brief	return 0L or a pointer on a focused member, or sub-member, or member of another object that may be linked to this one.
			virtual	const BaseType	*GetFocusedObjectPointer() const
			{	return 0L; //TODO
			};
			//!	\brief	return a draw preference flag.(draw grid, etc...)
			virtual	unsigned int	GetDrawPreferenceFlag() const 
			{
				return 0L;// TODO
			};
			//! \brief preview time lapse start
			virtual double	GetPreviewStartTime() const
			{
				return m_pManager->mSer_PreviewStartTime.Get();
			};
			//! \brief preview time end start
			virtual double	GetPreviewEndTime() const 
			{
				return m_pManager->mSer_PreviewEndTime.Get();			
			};

			/*!
				\brief	Set the start time of the time range displayed in seconds.
				\param _timeSec	Start time of the time range displayed in seconds.
			*/
			inline void	SetPreviewStartTime(float _timeSec){	m_pManager->mSer_PreviewStartTime.Set( _timeSec );};		
			/*!
				\brief	Start time of the time range displayed in seconds.
				\return Start time of the time range displayed in seconds.
			*/
			inline float	GetPreviewStartTime(){ return m_pManager->mSer_PreviewStartTime.Get(); };
			/*!
				\brief	Set End time of the time range displayed in seconds.
				\param  _timeSec End time of the time range displayed in seconds.
			*/
			inline void	SetPreviewEndTime(float _timeSec){		m_pManager->mSer_PreviewEndTime.Set(_timeSec); };
			/*!
				\brief	End time of the time range displayed in seconds.
				\return End time of the time range displayed in seconds.
			*/
			inline float	GetPreviewEndTime(){ return m_pManager->mSer_PreviewEndTime.Get(); };
			/*!
				\brief	A GUI preview can manage a Video Recorder-like interface.
						An interface pref object can handle a preview time, but this is just
						an interface state information, it is not necessarily used directly by the context for playing.
				\param	_timeSec the current time position in seconds.
			*/
			inline void	SetPreviewCurrentTime(float _timeSec){  m_pManager->mSer_PreviewCurrentTime.Set(_timeSec); };
			/*!
				\brief	A GUI preview can manage a Video Recorder-like interface.
						An interface pref object can handle a preview time, but this is just
						an interface state information, it is not necessarily used directly by the context for playing.
				\return the current time position in seconds.
			*/
			inline float	GetPreviewCurrentTime(){ return m_pManager->mSer_PreviewCurrentTime.Get(); };
			/*!
				\brief	Set Play Speed rate. 1.0 means normal. >1.0 means faster, 0.0 mean pause, <0.0 means reverse.
				\param	_speedRate	Play Speed rate.
			*/
			inline void SetPlaySpeed(float _speedRate){		m_pManager->mSer_PlaySpeed.Set( _speedRate ); };
			/*!
				\brief	Play Speed rate. 1.0 means normal. >1.0 means faster, 0.0 mean pause, <0.0 means reverse.
				\return Play Speed rate.
			*/
			inline float	GetPlaySpeed(){ return m_pManager->mSer_PlaySpeed.Get(); };			
			/*!
				\brief	set play state.
				\param	_isPlaying true to play, stop to pause.
			*/
			inline void	SetPlayState(bool _isPlaying){ if(_isPlaying)m_pManager->mSer_Flag.AddFlags(m_PF_PlayState);
														else m_pManager->mSer_Flag.RemoveFlags(m_PF_PlayState); };
			/*!
				\brief	set play state.
				\return	true to play, stop to pause.
			*/
			inline bool	GetPlayState(){ return m_pManager->mSer_Flag.TestFlags(m_PF_PlayState); };

		};
		InterfacePreviewConfiguration	m_PreviewConfiguration;
#endif
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new PartToPlay object.
	*/
	static BaseType *NewPreviewEdited(){ return new PreviewEdited(); };
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new PartToPlay object.
	*/
	static BaseType *NewPreviewIndexList(){ return new PackList(NewPreviewEdited); };
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Set the Preview object. It affects the current active 
				Preference object in the context. (it creates one if there wasnt one).
				The _ObjectToPreview must belong to _pContext.
				if _ObjectToPreview is NULL, the Preview object is set to NULL 
				(no preview.)
		\param	_ObjectToPreview the object to preview.
		\param _RenderViewIndex if the viewer support multiple previews, an abstract index of the preview affected.
	*/
	void	SetContextPreviewObject(BaseObject *_ObjectToPreview,unsigned int _RenderViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Get a current Preview object for a context.
					If there are none, return 0L.
		\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
		\param _pPreviewConfiguration if not nul, return a pointer to the configuration preview to it.
		\return the current Preview object or 0L.
	*/
	BaseObject *GetContextPreviewObject(unsigned int _RenderViewIndex=0,InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration **_ppPreviewConfiguration=0L);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Preview Previous: scroll down the history of previewed object. May Change result of GetContextPreviewObject().
		\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
	*/
	void PreviewPrevious(unsigned int _RenderViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Preview Next: scroll up the history of previewed object. May Change result of GetContextPreviewObject().
		\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
	*/
	void PreviewNext(unsigned int _RenderViewIndex=0);
#endif
	/*!
		\brief	 Nested class that describes a reference to an object
	*/
	class Edited : public PackStruct
	{
	public:
		//! the reference to an object:
		PackObjectReferenceSpecial	mSer_Reference;
		//! a serialized but not edited value, just to manage the case when this InterfacePrefObject is edited:(PackObjectReference can't self recurse.)
		PackULong_Flags			mSer_Flag;

		//! for previous/next
		static const unsigned int m_EF_HistoryFlag=1;
		//!
		static const unsigned int m_EF_SelfReference=2;
		Edited(void) : PackStruct()
		{
			REGISTER_MEMBER_REFERENCE(mSer_Reference ,0L/*"Object"*/, BaseObject::m_Description );
			// we serialize mSer_SelfPrefEditionFlag but don't edit it:
			REGISTER_MEMBER( mSer_Flag,0L);
		};
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new PartToPlay object.
	*/
	static BaseType *NewEdited(){ return new Edited(); };

	/*!
		\brief	static method passed to the list to create new elements.
		\return a new PartToPlay object.
	*/
	static BaseType *NewEditorIndexList(){ return new PackList(NewEdited); };
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	the sound main volume, from an interface editor point of view, not a context point of view.
		\param _volume volume [0.0,1.0]
	*/
	inline void	SetSoundMainVolume(float _volume){  mSer_SoundMachineMainVolume.Set(_volume); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	the sound main volume, from an interface editor point of view, not a context point of view.
		\return volume [0.0,1.0]
	*/
	inline float	GetSoundMainVolume(){ return mSer_SoundMachineMainVolume.Get(); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	a context can have zero, one , or more InterfacePrefObject, but only one
				must be active at the same time. Find it with GetContextActivePref()
		\return true if it is the active pref, false if not.
	*/
	inline bool	IsActive(){ return (mSer_Flags.Get() & ipof_Active); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	all reference to this object will be closed. used by deletion done by ClassNode.
				special close.
	*/
	virtual void CloseReferences();
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! mSer_Flags states.
	static const unsigned int	ipof_Active=1;
	//! mSer_Flags states.
	static const unsigned int	ipof_IsPlaying=1<<1;
	//! Draw Path As Lines.
	static const unsigned int	ipof_DrawPathAsLines=1<<2;
	//! at destruction, we lock, to escape unfinite loop:
	bool		m_isBeingDestroyed;

	//! Flags ipof_...
	PackULong_Flags_Activate	mSer_Flags;

	// ------------- preview conf ------------
	//! the way the preview are all shown on the interface.
	PackULong_Enums			mSer_PreviewShapeConfiguration;
	//! list by possible multiple preview
	PackList				mSer_PreviewIndexList;
	// ------------- object editor ------------
	//! 
	PackULong_Enums			mSer_ObjectEditorShapeConfiguration;
	//! list by possible multiple editors
	PackList				mSer_EditorIndexList;
		
	// ------------ sound -------------
	//! TimePreviewer: playspeed, default 1.0
	PackFloatByte_Limits	mSer_SoundMachineMainVolume;
	
	// ------------ Protected Tools ------
#ifdef _ENGINE_EDITABLE_
	/*
		\brief protected: if not done, create a preview history list for an indexed view.
		\param	optionnal index of the view.
		\return	the list, which itself manages PreviewEdited objects.
	*/
	PackList	*GetOrCreatePreviewIndexList( unsigned int _RenderViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_
	/*
		\brief protected: if not done, create a Editor history list for an indexed view.
		\param	optionnal index of the view.
		\return	the list, which itself manages PreviewEdited objects.
	*/
	PackList	*GetOrCreateEditorIndexList( unsigned int _RenderViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_
	/*
		\brief protected tool, delete all edition or preview preference that are linked to nothing.
	*/
	void	PurgePreferenceToDeadObjects();
#endif
};

// end of file
#endif
