/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_VirtualMedia_H
#define COM_M4NKIND_VirtualMedia_H

#include "BaseObject.h"
#include "VirtualMachine.h"
/*!
	\class	VirtualMedia
	\ingroup BaseObjectInheritedDocGroup BaseObjectInherited_Veda
	\brief	Base model for all class that plays a suit of animations and/or sounds.
			You should extend ProcessMedia() for animations, and ProcessSoundInterupt() for sounds,
			in any way. This \ref Example_MinimalMedia "example" extends a VirtualMedia and plays it.
			Note this class is not registered to BaseContext by default. you got to register it or not, 
			according to your needs. Thus, it got to be registered first if you use inherited 
			classes. ( through BaseContext::RegisterClassList() )
			 Also note that VirtualMedia::ProcessSoundInterupt() manages the sound
			 registration to the machine itself.
*/
class VirtualMedia : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	VirtualMedia(void);
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(VirtualMedia);

	/*!
		\brief	 Process a media at a given date, 
				or do anything) using a machine. 
				Note there are no play/pause or speed concept here, by default, a date is passed.
				This version redirect the drawing to the main screen viewport.
				It should the main entry to play a media.
				Developpers should extend 
		\param	_frameDate a date, in second, which defines the effect cinematic.
	*/
	void ProcessMediaOnDefaultViewport( double _frameDate );
	
	/*!
		\brief	 Process a media at a given date, 
				or do anything) using a machine. You must process the media on machine this->GetMachine().
				Note there are no play/pause or speed concept here, by default, a date is passed.
				By default, does nothing.
		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pViewPort the viewport to render. Can't be 0L.
	*/
	virtual void ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort );

	/*!
		\brief	 returns the Total Time length of this media, in seconds, when played at 
				normal speed. This is just an information about the time length of this media,
				and you can pass any date to ProcessMedia().
		\return	Total Time length of this media, in seconds, when played at normal speed. Can be used or not.
	*/
	inline double	GetTimeLength(){ return m_MediaTimeLength; };

	/*!
		\brief	 Use SetSound() to enable  sound capabilities for a media object, 
				and the machine should then send ProcessSoundInterupt() to mix it.
				SetSound(true) at start and SetSound(false) at closing, have to be 
				performed by external code, and should not be used by AzurVeda objects 
				in their methods.
				Note: SetSound(true) enable One object, but this object
				can point other Media objects in the context with SetSound(false),
				and use ProcessSoundInterupt() recursively to mix all of them. So an 
				object with SetSound(false) can still be noisy. 

				In its default behaviour, SetSound() uses the default AzurVeda machine,
				GetMachine()

		\param	_Enable true start sound, false stop it.
	*/
	virtual void SetSound( bool	_Enable );

	/*!
		\brief	 If the media had to generate real time sounds, 
				ProcessSoundInterupt will be called by a machine. There is a default 
				behaviour for this method, which does nothing. the float buffer 
				accumulate the whole machine sound.
		\param	_SoundBufferToAddYourSignal description of the buffer to fill.
	*/
	virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal );
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	any suit of call to ProcessPreview() should be done between
				StartPreview() and EndPreview() .
				EndPreview(). This is needed to stop sound mixing from the object
				when the preview change.
	*/
	virtual		void	StartPreview();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
				created object. This is done with this method. sub classes can implement it (or not) in
				any way, to explicit current shape of an object.<br>

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration
	*/
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	any suit of call to ProcessPreview() should be done between
				StartPreview() and EndPreview() .
				EndPreview(). This is needed to stop sound mixing from the object
				when the preview change.
	*/
	virtual		void	EndPreview();
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:

	//! true if m_MediaSoundInterface is currently registered to VirtualMachine as playing sound.
	bool	m_SoundIsMixed;
	//! Total Time length of this media, in seconds, when played at normal speed. Can be used or not.
	double	m_MediaTimeLength;

	// no CreateInternal or CloseInternal for this one.
	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	//virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	virtual void	CloseInternal(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Tool to draw a preview of the current sound signal.
		\param	_pViewport  where to draw
		\param	_pSoundBuffer float left/right table
		\param	_bufferlength number of right/left on the table.
	*/
	virtual void ProcessPreview_DrawSoundBuffer(	
								VirtualMachine::InternalViewPort *_pViewport,
								float	*_pSoundBuffer,
								unsigned int	_bufferlength );
#endif
/*==================================================================
								PRIVATE
==================================================================*/
private:
	/*!
		\class	MediaSoundInterface
		\brief	Implements VirtualMachine::SoundInterface as a protected member.
				This mecanism just redirect the machine's sound mixing to VirtualMedia::ProcessSoundInterupt()
	*/
	class MediaSoundInterface : public VirtualMachine::SoundInterface
	{
		public:
		/*!
			\brief	Method thrown by the sound creation thread to add a new signal in a float buffer.
					The whole result is played As soon as possible.
			\param	_SoundBufferToAddYourSignal the sound to write description.
		*/
		virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal );

		//! when inited, MediaSoundInterface receive the object that implement it. 
		VirtualMedia	*m_pVirtualMedia;

	};
	//!	this aggregate member is the object touch by the Virtual Machine
	MediaSoundInterface	m_MediaSoundInterface;
};

#endif
