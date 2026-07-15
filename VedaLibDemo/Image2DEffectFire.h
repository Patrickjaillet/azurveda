#ifndef	COM_M4NKIND_Image2DEffectFire_H
#define COM_M4NKIND_Image2DEffectFire_H

#include "VirtualImage.h"
#include "PackObjectReference.h"
#include "PackULong_Flags.h"
#include "PackLong_WithLimits.h"
#include "PackULong_Enums.h"
#include "PackList.h"

class Image2DEffectFire : public VirtualImage
{

public:

	Image2DEffectFire(void);

	BASEOBJECT_DEFINE_CLASS(Image2DEffectFire);

	virtual unsigned int	GetByteDepth(){ return(4); };

	virtual unsigned int	GetPixelWidth();

	virtual unsigned int	GetPixelHeight();

	virtual	void	UpdateToFrame(float _framedate);

protected:

	PackLong_WithLimits	mSer_2nPixelWidth;

	PackLong_WithLimits	mSer_2nPixelHeight;

	float *m_pBitmap1Alloc;

	float *m_pDblBuf1;

	float *m_pDblBuf2;

	unsigned int m_randomval;

	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif
};

#endif
