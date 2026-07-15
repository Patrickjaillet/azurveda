// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_VirtualImage_H
#define COM_M4NKIND_VirtualImage_H

#include "BaseObject.h"

class VirtualImage : public BaseObject
{

public:

	VirtualImage(void);

	BASEOBJECT_DEFINE_VIRTUALCLASS(VirtualImage);

	virtual unsigned int	GetPixelWidth()=0;

	virtual unsigned int	GetPixelHeight()=0;

	virtual unsigned int	GetByteDepth()=0;

	inline unsigned char	*GetBuffer(){ return(m_pBuffer); };

	inline	bool	IsTimeDynamic() const { return m_IsTimeDynamic; };

	virtual	void	UpdateToFrame(float _framedate);

#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

protected:

	unsigned char	*m_pBuffer;

	bool		m_IsTimeDynamic;

	float	m_CurrentFrameDate;
#ifdef _ENGINE_EDITABLE_

	unsigned char	*m_pPeviewRGBABuffer;
#endif

	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif
};

#endif
