/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_MediaTimePattern_H
#define COM_M4NKIND_MediaTimePattern_H

#include "VirtualMedia.h"
#include "VirtualMachine.h"

#include "PackFloat.h"
#include "PackList_TimeBlockTrack.h"
#include "PackLong.h"
#include "PackObjectReference.h"
/*!
	\class	MediaTimePattern
	\ingroup BaseObjectInheritedDocGroup BaseObjectInherited_Veda
	\brief	The class Defining a media pattern as a list of tracks of other media to mix.
			Each track is a suit of VirtualMedia reference to play. 
			a MediaTimePattern can call other ones recursively.
*/
class MediaTimePattern : public VirtualMedia
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	MediaTimePattern(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(MediaTimePattern);

	/*!
		\brief	 Process a media at a given date, (compute,draw, 
				or do anything) using a machine. You must process the media on machine this->GetMachine().
				Note there are no play/pause or speed concept here, by default, a date is passed.

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pViewPort the viewport to render. Can't be 0L.
	*/
	virtual void ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort );

	/*!
		\brief	 If the media had to generate real time sounds, 
				ProcessSoundInterupt will be called by a machine. There is a default 
				behaviour for this method, which does nothing. the float buffer 
				accumulate the whole machine sound.
		\param	_SoundBufferToAddYourSignal description of the buffer to fill.
	*/
	virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal );


	/*!
		\brief	 Nested class that describes something to play in a MediaTimePattern.
				It is used as the element of the script list, each reference another script: 
	*/
	class PartToPlay : public PackList_TimeBlockTrack::TimeBlockTrackElement
	{
	public:
		//! the reference to the subscript to play:
		PackObjectReference	mSer_ReferenceToPlay;
		//! an offset applyed to the part time, in 100Hz units:
		PackFloat_FixedPoint mSer_TimeOffset;
		//! the speed<<4
		PackFloat_FixedPoint mSer_TimeSpeed;
		//! \brief constructor . It registers the
		PartToPlay(void) : PackList_TimeBlockTrack::TimeBlockTrackElement()
			,mSer_TimeSpeed(PackFloat::m_0p0625) // 1/16
		{
			REGISTER_MEMBER_REFERENCE(mSer_ReferenceToPlay ,"MediaToPlay", VirtualMedia::m_Description );
			REGISTER_MEMBER( mSer_TimeOffset,"Offset");
			REGISTER_MEMBER_PACKFLOAT( mSer_TimeSpeed,"Speed",1.0f);
		};
	};

	/*!
		\brief	static method passed to the list to create new elements.
		\return a new PartToPlay object.
	*/
	static PackList_TimeBlockTrack::TimeBlockTrackElement *NewPartToPlay(){ return new PartToPlay(); };

	/*!
		\brief	static method passed to mSer_TrackList to create new elements.
		\return a new Track object.
	*/
	static BaseType *NewTrack();

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Draw the preview.

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration Preview Configuration object. Never 0L. Do not keep a pointer to it.
	*/
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif	
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Draw a shceme of the tracks and parts in a special rectangle for preview.

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration Preview Configuration object. Never 0L. Do not keep a pointer to it.
	*/
	virtual	void ProcessPreview_DrawTracks(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif	
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Preview Tool: Create a preview Square 3D Object.
	*/
	virtual	void ProcessPreview_CreateSquare(VirtualMachine::InternalObject3DBuffer **_ppVertexAndPolygonBufferOut);
#endif	
	/*
		\class	ReferenceMember
		\brief	Note: THIS WAS EXPERIMENTAL !!!
				Inner base class that stands for special data members to a BaseObject,
				cf:  BaseObject::ReferenceMembers
	*/
	/*
	class MediaTimePattern_ReferenceMembers	: public ReferenceMembers
	{ public:
	
		MediaTimePattern_ReferenceMembers(){};
		ReferenceMembers	**m_pTrackList_ReferenceMembers;
	};*/
	/*
		\brief	THIS WAS EXPERIMENTAL !!! cf: BaseObject::CreateNewReferenceMembers()	
		\return new members by reference, or empty adress.
	*//*
	virtual	ReferenceMembers	*CreateNewReferenceMembers();
	*/

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the track list on the pattern. 
	PackList		mSer_TrackList;
	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by Create().
		\return false if creation failed.
	*/
	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	virtual void CloseInternal(void);
#endif

#ifdef _ENGINE_EDITABLE_
	//! object created for preview.
	VirtualMachine::InternalObject3DBuffer	*m_pPreviewSquareObject;
#endif


};
#endif
