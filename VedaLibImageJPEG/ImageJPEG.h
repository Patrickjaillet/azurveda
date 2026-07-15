// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_ImageJPEG_H
#define COM_M4NKIND_ImageJPEG_H

#include "VirtualImage.h"
#include "PackResource.h"
#include "PackULong_Flags.h"

class ImageJPEG : public VirtualImage
{

public:

	ImageJPEG(void);

	BASEOBJECT_DEFINE_CLASS(ImageJPEG);

	virtual unsigned int	GetByteDepth(){ return(4); };

	virtual unsigned int	GetPixelWidth();

	virtual unsigned int	GetPixelHeight();

protected:

	PackResource	mSer_JPEGFile;

	enum {
		b_GreyToAlpha=1
	};

	PackULong_Flags	mSer_Flags;

	unsigned int		m_PixelWidth;

	unsigned int		m_PixelHeight;

	virtual bool CreateInternal(void);

};

#endif
