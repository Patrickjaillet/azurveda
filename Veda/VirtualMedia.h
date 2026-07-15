// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_VirtualMedia_H
#define COM_M4NKIND_VirtualMedia_H

#include "BaseObject.h"
#include "VirtualMachine.h"

class VirtualMedia : public BaseObject
{

public:

	VirtualMedia(void);

	BASEOBJECT_DEFINE_VIRTUALCLASS(VirtualMedia);

	void ProcessMediaOnDefaultViewport( double _frameDate );

	virtual void ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort );

	inline double	GetTimeLength(){ return m_MediaTimeLength; };

	virtual void SetSound( bool	_Enable );

	virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal );
#ifdef _ENGINE_EDITABLE_

	virtual		void	StartPreview();
#endif
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual		void	EndPreview();
#endif

protected:

	bool	m_SoundIsMixed;

	double	m_MediaTimeLength;

#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void ProcessPreview_DrawSoundBuffer(
								VirtualMachine::InternalViewPort *_pViewport,
								float	*_pSoundBuffer,
								unsigned int	_bufferlength );
#endif

private:

	class MediaSoundInterface : public VirtualMachine::SoundInterface
	{
		public:

		virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal );

		VirtualMedia	*m_pVirtualMedia;

	};

	MediaSoundInterface	m_MediaSoundInterface;
};

#endif
