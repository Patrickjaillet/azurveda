// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_BubbleGreyImage_H
#define COM_M4NKIND_BubbleGreyImage_H

#include "ProceduralImage.h"
#include "PackULong_Flags.h"
#include "PackObjectReference.h"

class BubbleGreyImage : public ProceduralImage
{

public:

	BubbleGreyImage(void);

	BASEOBJECT_DEFINE_CLASS(BubbleGreyImage);

protected:
	enum {
		BUBFLAG_OneColorByRegion=1,
		BUBFLAG_Multiply=2
	};

	PackULong_Flags		mSer_Flags;

	PackObjectReference	mSer_SubImage;

	virtual bool CreateInternal(void);

	int					m_RandImageSeed;
	int					FindDistance( int distx, int disty );

	unsigned int		FindShiftX( int distx, int disty );
	unsigned int		FindShiftY( int distx, int disty );

};

#endif
