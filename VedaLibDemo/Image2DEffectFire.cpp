#include "Image2DEffectFire.h"

#include "PackFloat.h"
#include "GreyImage.h"
#include "VirtualEquation.h"
#include <math.h>
BASEOBJECT_DECLARE_CLASS("fxf", Image2DEffectFire, VirtualImage,"Image Effect Fire","ImFxFire","Dynamic Image:Classic Fire effect" );

Image2DEffectFire::Image2DEffectFire() : VirtualImage()
	,m_pBitmap1Alloc(0L)
{
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_2nPixelWidth, "2PowerN Width",8,3,9 );
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_2nPixelHeight, "2PowerN Height",8,3,9 );

	m_IsTimeDynamic = true;
}

unsigned int	Image2DEffectFire::GetPixelWidth()
{
	return( 1<<mSer_2nPixelWidth.Get() );
}

unsigned int	Image2DEffectFire::GetPixelHeight()
{
	return( 1<<mSer_2nPixelHeight.Get() );
}

bool Image2DEffectFire::CreateInternal(void)
{

	bool res = VirtualImage::CreateInternal();
	if(!res) return false;
	const unsigned int bitmapsize = GetPixelWidth()*GetPixelHeight();
	m_pBitmap1Alloc = new float[bitmapsize<<1];
	if(!m_pBitmap1Alloc) return false;
	float *pf=m_pBitmap1Alloc;
	for(int ii=0; ii<bitmapsize ; ii++)
	{
		*pf++ = 0.0f;
		*pf++ = 0.0f;
	}

	m_pDblBuf1 = m_pBitmap1Alloc ;
	m_pDblBuf2 = m_pBitmap1Alloc+bitmapsize ;
	m_CurrentFrameDate = 0.0f;
	m_randomval = 0x78139b7 ;
	return true;
}
#ifdef _ENGINE_EDITABLE_

void	Image2DEffectFire::CloseInternal(void)
{

	if(m_pBitmap1Alloc)
	{
		delete [] m_pBitmap1Alloc;
		m_pBitmap1Alloc = 0L;
	}

	VirtualImage::CloseInternal();
}
#endif

void	Image2DEffectFire::UpdateToFrame(float _framedate)
{
	float timelapse =_framedate - m_CurrentFrameDate;

	if(timelapse*20.0f <= 1.0f) return;

	m_CurrentFrameDate = _framedate;

	int	xx,yy;
	const int width = GetPixelWidth() ;
	const int height = GetPixelHeight();

	float *pReadEnd = m_pDblBuf2+(width*((height)-2));
	for(xx=0 ; xx<width>>1 ; xx++)
	{
		float val;

		val =  ((float)(m_randomval>>16))*(1.0f/65536.0f);
		*pReadEnd++ = val*val;
		*pReadEnd++ = val*val;
		m_randomval = 0x129e1923 + 0x722e159b * m_randomval;
	}

	float *pWriten = m_pDblBuf1+width;
	float *pRead = m_pDblBuf2+width;
	unsigned char *pBitmap = m_pBuffer;
	const float fdiv = 1.0f/(6.08f);
	const float v255 = 255.0f;
	for(yy=1 ; yy<(height-2) ; yy++)
	{
		for(xx=0 ; xx<width ; xx++)
		{
			float value = fdiv*(
					*(pRead-1)+pRead[0]+pRead[1]+
						*(pRead+(width-1))+*(pRead+width)+*(pRead+(width+1)));
			*pWriten = value ;

			int pixelv = (int)((sinf(value*64.0f)+1.0f)*127.5f);
			*(pBitmap++) = (unsigned char)pixelv;
			*(pBitmap++) = (unsigned char)pixelv;
			*(pBitmap++) = (unsigned char)pixelv>>1;
			*(pBitmap++) = 255;
			pWriten++;
			pRead++;
		}

	}

	float *inv = m_pDblBuf1;
	m_pDblBuf1 = m_pDblBuf2;
	m_pDblBuf2 = inv;

}
