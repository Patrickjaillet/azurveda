#ifndef	COM_M4NKIND_MP3SoundObject_H
#define COM_M4NKIND_MP3SoundObject_H

#include "VirtualMedia.h"
#include "PackResource.h"

struct mad_decoder;
struct mad_stream;
struct mad_header;
struct mad_frame;
struct mad_synth;

class MP3SoundObject : public VirtualMedia
{

public:

	MP3SoundObject(void);

	BASEOBJECT_DEFINE_CLASS(MP3SoundObject);

	virtual void ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort );

	virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal );

protected:

	PackResource		mSer_MP3File;

    struct mad_stream *m_pstream;

    struct mad_frame *m_pframe;

    struct mad_synth *m_psynth;

	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif

#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

	unsigned	int	m_NumberOfFrame;

	typedef struct {

		unsigned char const *m_pFrameStart;

		unsigned int		m_TimeStart;
	} FrameReference;

	FrameReference	*m_pFrameReferenceTable;

	static const unsigned int	m_InputStreamLength=1024*16;

	unsigned char const *m_pInputStart;

	unsigned long		m_InputLength;

	unsigned int		m_LastFrameIndex;

	unsigned int m_nchannels;
	unsigned int m_nsamples;
	unsigned int m_samplerate;
	signed int const *m_pleft_ch;
	signed int const *m_pright_ch;

	int m_nsamplesRunner;
	signed int const *m_pleft_chRunner;
	signed int const *m_pright_chRunner;

	unsigned int			m_RunnerVector;

	unsigned int			m_RunnerIndex;

	bool	FeedStream();

	void	SeekStream(int _milliseconds);

	static bool head_check(unsigned long head);

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

	static int decode_header(struct RawFrame *fr, unsigned long newhead);
};

#endif
