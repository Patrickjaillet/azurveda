// SPDX-License-Identifier: LGPL-2.1-only

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

class InterfacePrefObject : public BaseObject
{

public:

	InterfacePrefObject(void);

	BASEOBJECT_DEFINE_CLASS(InterfacePrefObject);
#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	static	InterfacePrefObject *GetContextActivePref(BaseContext *_pContext,bool _createOneIfNone=false);
#endif
#ifdef _ENGINE_EDITABLE_

	void	SetContextEditedObject(BaseObject *_ObjectToEdit,unsigned int _EditViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_

	BaseObject *GetContextEditedObject(unsigned int _EditViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_

	void EditPrevious(unsigned int _RenderViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_

	void EditNext(unsigned int _RenderViewIndex=0);
#endif
	class PackObjectReferenceSpecial;
	friend class PackObjectReferenceSpecial;

	class	PackObjectReferenceSpecial	: public PackObjectReference
	{	public:

		PackObjectReferenceSpecial() : PackObjectReference(){};
#ifdef _ENGINE_EDITABLE_

		virtual	enum_SetReferenceResult	SetObjectPointer( BaseObject * _Object );
#endif
	};

	class PackULong_Flags_Activate;
	friend class PackULong_Flags_Activate;

	class PackULong_Flags_Activate : public	PackULong_Flags
	{ public:
#ifdef _ENGINE_EDITABLE_

		virtual void	Set(unsigned int _value);
#endif
	};

	class PreviewEdited : public PackStruct
	{
	public:

		PackObjectReferenceSpecial	mSer_Reference;

		static const unsigned int m_PF_HistoryFlag=1;

		static const unsigned int m_PF_PlayState=2;

		PackULong_Flags		mSer_Flag;

		PackFloat			mSer_Corner1;

		PackFloat			mSer_Corner2;

		PackFloat			mSer_PreviewPosition;

		PackFloat			mSer_PreviewRotation;

		PackFloat			mSer_PreviewStartTime;

		PackFloat			mSer_PreviewEndTime;

		PackFloat			mSer_PreviewCurrentTime;

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

			REGISTER_MEMBER_REFERENCE(mSer_Reference ,0L, BaseObject::m_Description );
			REGISTER_MEMBER_PACKULONG(mSer_Flag ,0L,0 );
			REGISTER_MEMBER_PACKFLOAT_XY(mSer_Corner1 ,0L,0.0f,0.0f );
			REGISTER_MEMBER_PACKFLOAT_XY(mSer_Corner2 ,0L,1.0f,1.0f );
			REGISTER_MEMBER_PACKFLOAT_XYZ(mSer_PreviewPosition ,0L,0.0f,0.0f,0.0f );
			REGISTER_MEMBER_PACKFLOAT_XYZ(mSer_PreviewRotation ,0L,0.0f,0.0f,0.0f );
			REGISTER_MEMBER_PACKFLOAT(mSer_PreviewStartTime ,0L,0.0f );
			REGISTER_MEMBER_PACKFLOAT(mSer_PreviewEndTime ,0L,360.0f );
			REGISTER_MEMBER_PACKFLOAT(mSer_PreviewCurrentTime ,0L,0.0f );
			REGISTER_MEMBER_PACKFLOAT(mSer_PlaySpeed ,0L,1.0f );
#ifdef _ENGINE_EDITABLE_
			m_PreviewConfiguration.m_pManager = this;
#endif
		};
#ifdef _ENGINE_EDITABLE_

		class InterfacePreviewConfiguration  : public PreviewConfiguration
		{ public:
			PreviewEdited	*m_pManager;

			virtual	void	SetViewportScale(float _x1,float _y1,float _x2,float _y2) const
			{
				m_pManager->mSer_Corner1.Set(_x1);
				m_pManager->mSer_Corner1.Set(1,_y1);
				m_pManager->mSer_Corner2.Set(_x2);
				m_pManager->mSer_Corner2.Set(1,_y2);  } ;

			virtual	void	GetViewportScale(float &_x1,float &_y1,float &_x2,float &_y2) const
			{
				_x1 = m_pManager->mSer_Corner1.Get(0);
				_y1 = m_pManager->mSer_Corner1.Get(1);
				_x2 = m_pManager->mSer_Corner2.Get(0);
				_y2 = m_pManager->mSer_Corner2.Get(1);  } ;

			virtual	void	SetPreviewPosition(float _PositionX,float _PositionY,float _PositionZ ) const
			{	 m_pManager->mSer_PreviewPosition.Set(_PositionX);
				 m_pManager->mSer_PreviewPosition.Set(1,_PositionY);
				 m_pManager->mSer_PreviewPosition.Set(2,_PositionZ);
			};

			virtual	void	GetPreviewPosition(float &_PositionX,float &_PositionY,float &_PositionZ ) const
			{	_PositionX = m_pManager->mSer_PreviewPosition.Get(0);
				_PositionY = m_pManager->mSer_PreviewPosition.Get(1);
				_PositionZ = m_pManager->mSer_PreviewPosition.Get(2);
			};

			void	SetPreviewRotation(float _Xaxis,float _YAxis,float _ZAxis)
			{	m_pManager->mSer_PreviewRotation.Set( _Xaxis);
				m_pManager->mSer_PreviewRotation.Set(1,_YAxis);
				m_pManager->mSer_PreviewRotation.Set(2,_ZAxis);
			};

			virtual	void	GetPreviewRotation(float &_Xaxis,float &_YAxis,float &_ZAxis) const
			{	_Xaxis = m_pManager->mSer_PreviewRotation.Get(0);
				_YAxis = m_pManager->mSer_PreviewRotation.Get(1);
				_ZAxis = m_pManager->mSer_PreviewRotation.Get(2);
			};

			virtual	const BaseType	*GetFocusedObjectPointer() const
			{	return 0L;
			};

			virtual	unsigned int	GetDrawPreferenceFlag() const
			{
				return 0L;
			};

			virtual double	GetPreviewStartTime() const
			{
				return m_pManager->mSer_PreviewStartTime.Get();
			};

			virtual double	GetPreviewEndTime() const
			{
				return m_pManager->mSer_PreviewEndTime.Get();
			};

			inline void	SetPreviewStartTime(float _timeSec){	m_pManager->mSer_PreviewStartTime.Set( _timeSec );};

			inline float	GetPreviewStartTime(){ return m_pManager->mSer_PreviewStartTime.Get(); };

			inline void	SetPreviewEndTime(float _timeSec){		m_pManager->mSer_PreviewEndTime.Set(_timeSec); };

			inline float	GetPreviewEndTime(){ return m_pManager->mSer_PreviewEndTime.Get(); };

			inline void	SetPreviewCurrentTime(float _timeSec){  m_pManager->mSer_PreviewCurrentTime.Set(_timeSec); };

			inline float	GetPreviewCurrentTime(){ return m_pManager->mSer_PreviewCurrentTime.Get(); };

			inline void SetPlaySpeed(float _speedRate){		m_pManager->mSer_PlaySpeed.Set( _speedRate ); };

			inline float	GetPlaySpeed(){ return m_pManager->mSer_PlaySpeed.Get(); };

			inline void	SetPlayState(bool _isPlaying){ if(_isPlaying)m_pManager->mSer_Flag.AddFlags(m_PF_PlayState);
														else m_pManager->mSer_Flag.RemoveFlags(m_PF_PlayState); };

			inline bool	GetPlayState(){ return m_pManager->mSer_Flag.TestFlags(m_PF_PlayState); };

		};
		InterfacePreviewConfiguration	m_PreviewConfiguration;
#endif
	};

	static BaseType *NewPreviewEdited(){ return new PreviewEdited(); };

	static BaseType *NewPreviewIndexList(){ return new PackList(NewPreviewEdited); };
#ifdef _ENGINE_EDITABLE_

	void	SetContextPreviewObject(BaseObject *_ObjectToPreview,unsigned int _RenderViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_

	BaseObject *GetContextPreviewObject(unsigned int _RenderViewIndex=0,InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration **_ppPreviewConfiguration=0L);
#endif
#ifdef _ENGINE_EDITABLE_

	void PreviewPrevious(unsigned int _RenderViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_

	void PreviewNext(unsigned int _RenderViewIndex=0);
#endif

	class Edited : public PackStruct
	{
	public:

		PackObjectReferenceSpecial	mSer_Reference;

		PackULong_Flags			mSer_Flag;

		static const unsigned int m_EF_HistoryFlag=1;

		static const unsigned int m_EF_SelfReference=2;
		Edited(void) : PackStruct()
		{
			REGISTER_MEMBER_REFERENCE(mSer_Reference ,0L, BaseObject::m_Description );

			REGISTER_MEMBER( mSer_Flag,0L);
		};
	};

	static BaseType *NewEdited(){ return new Edited(); };

	static BaseType *NewEditorIndexList(){ return new PackList(NewEdited); };
#ifdef _ENGINE_EDITABLE_

	inline void	SetSoundMainVolume(float _volume){  mSer_SoundMachineMainVolume.Set(_volume); };
#endif
#ifdef _ENGINE_EDITABLE_

	inline float	GetSoundMainVolume(){ return mSer_SoundMachineMainVolume.Get(); };
#endif
#ifdef _ENGINE_EDITABLE_

	inline bool	IsActive(){ return (mSer_Flags.Get() & ipof_Active); };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void CloseReferences();
#endif

protected:

	static const unsigned int	ipof_Active=1;

	static const unsigned int	ipof_IsPlaying=1<<1;

	static const unsigned int	ipof_DrawPathAsLines=1<<2;

	bool		m_isBeingDestroyed;

	PackULong_Flags_Activate	mSer_Flags;

	PackULong_Enums			mSer_PreviewShapeConfiguration;

	PackList				mSer_PreviewIndexList;

	PackULong_Enums			mSer_ObjectEditorShapeConfiguration;

	PackList				mSer_EditorIndexList;

	PackFloatByte_Limits	mSer_SoundMachineMainVolume;

#ifdef _ENGINE_EDITABLE_

	PackList	*GetOrCreatePreviewIndexList( unsigned int _RenderViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_

	PackList	*GetOrCreateEditorIndexList( unsigned int _RenderViewIndex=0);
#endif
#ifdef _ENGINE_EDITABLE_

	void	PurgePreferenceToDeadObjects();
#endif
};

#endif
