// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_RGBAImage_H
#define COM_M4NKIND_RGBAImage_H

#include "VirtualImage.h"
#include "PackObjectReference.h"
#include "PackULong_Flags.h"
#include "PackLong_WithLimits.h"
#include "PackULong_Enums.h"
#include "PackList.h"

class RGBAImage : public VirtualImage
{

public:

	RGBAImage(void);

	BASEOBJECT_DEFINE_CLASS(RGBAImage);

	virtual unsigned int	GetByteDepth(){ return(4); };

	virtual unsigned int	GetPixelWidth();

	virtual unsigned int	GetPixelHeight();

	static inline void TslToRgb(  unsigned int t,unsigned int  s,unsigned int  l, unsigned char *R );

	class ImageLayer : public PackStruct
	{
	public:

#define LayerFlag_Red		1
#define LayerFlag_Green		2
#define LayerFlag_Blue		4
#define LayerFlag_Alpha		8
#define LayerFlag_HSV		16
#define LayerFlag_AddMul	32
#define LayerFlag_MulAlpha	64

		PackULong_Flags		mSer_LayerFlag;

		PackObjectReference	mSerRef_ComponentImage;

		PackObjectReference mSerRef_Curve;

		ImageLayer(void);
	};

	static BaseType *NewImageLayer(){ return new ImageLayer(); };

protected:

	PackLong_WithLimits		mSer_PixelWidth;

	PackLong_WithLimits		mSer_PixelHeight;

	PackList		mSer_LayerList;

	virtual bool CreateInternal(void);

};

#endif
