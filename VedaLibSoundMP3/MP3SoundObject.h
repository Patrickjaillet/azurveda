#ifndef	COM_M4NKIND_MP3SoundObject_H
#define COM_M4NKIND_MP3SoundObject_H
/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "VirtualMedia.h"
#include "PackResource.h"

struct mad_decoder;
struct mad_stream;
struct mad_header;
struct mad_frame;
struct mad_synth;

/*!
	\class	MP3SoundObject
	\ingroup BaseObjectInherited_VedaLibSoundMP3
	\brief	Class defining a MP3SoundObject to play. 
*/
class MP3SoundObject : public VirtualMedia
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	MP3SoundObject(void);

	/*!
		\brief	Class Inheritance Descriptors.
	*/
	BASEOBJECT_DEFINE_CLASS(MP3SoundObject);

	/*!
		\brief	 Process a media at a given date, 
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

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the mp3 file:
	PackResource		mSer_MP3File;
	//! the mad input stream.
    struct mad_stream *m_pstream;
	//! the mad valid frame.
    struct mad_frame *m_pframe;
	//! the mad frame synthetiser.
    struct mad_synth *m_psynth;

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
				a GUI could need to play, draw, print, or output from any way, a preview of a 
				created object.sub classes can implement it (or not) in
				any way, to explicit current shape of an object.
				_pPreviewConfiguration can be read to follow  <br>			

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration Preview Configuration object. Never 0L. Do not keep a pointer to it.
	*/
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
	//! total number of frames in the whole MP3 File, and size of ;
	unsigned	int	m_NumberOfFrame;
	/*
		\struct	FrameReference
		\brief	element of the frame reference table.
	*/
	typedef struct {
		//! pointer to the begining chunk:
		unsigned char const *m_pFrameStart;
		//! time that correspond to the start of the frame, in milliseconds.
		unsigned int		m_TimeStart;
	} FrameReference;
	//! table that reference the frames in the MP3 binary. Created at init. Size is m_NumberOfFrame +1
	FrameReference	*m_pFrameReferenceTable;

	//! input
	static const unsigned int	m_InputStreamLength=1024*16;
	//! the whole input file.
	unsigned char const *m_pInputStart;
	//! the whole input length.
	unsigned long		m_InputLength;
	//! the current input stream position. 
	//unsigned char const *m_pCurrentInputStart;
	//! the rest of the file to stream.
	//unsigned long		m_CurrentInputLength;
	//! last frame index in m_pFrameReferenceTable, (which is current played.)
	//! if m_LastFrameIndex>=m_NumberOfFrame, we are after the song (silence.)
	//! if m_LastFrameIndex=0 we are at start.
	unsigned int		m_LastFrameIndex;

	//! output
	unsigned int m_nchannels;
	unsigned int m_nsamples;
	unsigned int m_samplerate;
	signed int const *m_pleft_ch;
	signed int const *m_pright_ch;
	// the synth left to run:
	int m_nsamplesRunner;
	signed int const *m_pleft_chRunner;
	signed int const *m_pright_chRunner;
	//! <<16
	unsigned int			m_RunnerVector;
	//! <<16
	unsigned int			m_RunnerIndex;
		
	//int	m_debugZeroedCase;
	/*
		\brief	Continue stream feed if stream not finished.
		\return	true if feed OK, else if no more feed.
	*/
	bool	FeedStream();
	/*
		\brief	If stream time is not synchro, reset the stream:
				it will set it to the closest frame, which means
				it has more or less a time error under 1/4 second.
				it allows time-free random access.
		\param	_milliseconds
	*/
	void	SeekStream(int _milliseconds);

	/*
		\brief tool used privately to determine full time length quickly:
	*/
	static bool head_check(unsigned long head);
	/*
		\struct RawFrame
		\brief	used privately to determine full time length quickly:
	*/
	struct RawFrame
	{
		int lsf;
		int mpeg25;
		int lay;
		int bitrate_index;
		int sampling_frequency;
		int padding;
		int framesize;
	};
	/*
		\brief	tool used privately to determine full time length quickly:
	*/
	static int decode_header(struct RawFrame *fr, unsigned long newhead);
};

#endif
