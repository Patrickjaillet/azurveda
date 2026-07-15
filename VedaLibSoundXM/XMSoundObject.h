/*! \file 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_XMSoundObject_H
#define COM_M4NKIND_XMSoundObject_H

#include "VirtualMedia.h"
#include "PackResource.h"
#include "PackULong.h"
#include "PackFloat.h"
#include "PackList.h"
#include "PackObjectReference.h"

typedef struct _sUniMiniXM sUniMiniXM;

/*!
	\class	XMSoundObject
	\brief	Class defining a XMSoundObject to play. 

*/
class XMSoundObject : public VirtualMedia
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	XMSoundObject(void);

	/*!
		\brief	Class Inheritance Descriptors.
	*/
	BASEOBJECT_DEFINE_CLASS(XMSoundObject);

	/*!
		\brief	 Process a media at a given date, 
				or do anything) using a machine. You must process the media on machine this->GetMachine().
				Note there are no play/pause or speed concept here, by default, a date is passed.
		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pViewPort the viewport to render. Can't be 0L.
	*/
	//virtual void ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort );
	/*!
		\brief	 If the media had to generate real time sounds, 
				ProcessSoundInterupt will be called by a machine. There is a default 
				behaviour for this method, which does nothing. the float buffer 
				accumulate the whole machine sound.
		\param	_SoundBufferToAddYourSignal description of the buffer to fill.
	*/
	virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal );

	/*
		\brief when playing, return the current pattern number which is played.
		\return pattern index.
	*/
	int		GetCurrentPatternNumber();
	/*
		\brief when playing, return the current row index in the pattern.
			Watch out, the maximum row by pattern vary.
		\return the row.
	*/
	int		GetCurrentRowIndex();
	/*
		\brief return the current volume mixed for a given sample.
		\param	_instrument index.
		\param	_sample index.
		\param	_volume current volume is added there. 
	*/
	void	AddCurrentMixedVolume(unsigned int _instrument,
									unsigned int _sample,
									float &_volume);

	/*!
		\class	SampleOverrideReference
		\brief	 Nested class that describes a reference to a media used as sample.
	*/
	class SampleOverrideReference : public PackStruct // : public PackList_TimeTrack::TimeTrackElement
	{public:
		//!
		PackObjectReference		mSer_SampleReference;
		//!
		PackULong				mSer_InstrumentIndex;
		//!
		PackULong				mSer_SampleIndex;
		//!
		PackULong				mSer_Note;
		//! 
		PackFloat				mSer_SecLength;

		SampleOverrideReference() ;
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new element.
	*/
	static  BaseType *NewSampleOverrideReference(); 

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the XM file:
	PackResource	mSer_XMFile;
	//! the time length of the media is precomputed in erditable mode, and just read in non-editable.
	PackFloat		mSer_TimeLength;
	//! the list of sample to be overriden.
	PackList		mSer_SampleOverrideList;

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	virtual void	CloseInternal(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Extend this method to draw your preview !
		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration Preview Configuration object. Never 0L. Do not keep a pointer to it.
	*/
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif	
	//! the music:
	sUniMiniXM	*m_pXM;

};

#endif
