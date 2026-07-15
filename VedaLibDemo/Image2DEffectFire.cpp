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
	// at the level of VirtualImage, inform that the image may change, and UpdateToFrame() used. 
	m_IsTimeDynamic = true;
}
/*!
	\brief	Return the pixel width of the image
*/
unsigned int	Image2DEffectFire::GetPixelWidth()
{
	return( 1<<mSer_2nPixelWidth.Get() );
}
/*!
	\brief	Return the pixel height of the image
*/
unsigned int	Image2DEffectFire::GetPixelHeight()
{
	return( 1<<mSer_2nPixelHeight.Get() );
}
/*!
	\brief	Method that really build the object using the serializable parameters.
*/
bool Image2DEffectFire::CreateInternal(void)
{
	// alloc buf1 with supercall:
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
	// init the double buffer swapable members:
	m_pDblBuf1 = m_pBitmap1Alloc ;
	m_pDblBuf2 = m_pBitmap1Alloc+bitmapsize ;
	m_CurrentFrameDate = 0.0f;
	m_randomval = 0x78139b7 ;
	return true;
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void	Image2DEffectFire::CloseInternal(void)
{
	// close extra buffer2 :
	if(m_pBitmap1Alloc)
	{
		delete [] m_pBitmap1Alloc;
		m_pBitmap1Alloc = 0L;
	}
	// close buf1 with supzercall:
	VirtualImage::CloseInternal();
}
#endif

/*
	\brief	If IsTimeDynamic() is true, this image can change with time.
			You have to use UpdateToFrame()to make the image change to a given date.
			by default, UpdateToFrame() does nothing.
	\param	_framedate time in seconds.
*/
void	Image2DEffectFire::UpdateToFrame(float _framedate)
{
	float timelapse =_framedate - m_CurrentFrameDate;
	//old if(timelapse <= 0.0f) return; // already OK for that pass.
	if(timelapse*20.0f <= 1.0f) return; // both check immobility, reverse and block framerate to a maximum.
	// Ok, we will compute a new frame:
	m_CurrentFrameDate = _framedate;

	int	xx,yy;
	const int width = GetPixelWidth() ;
	const int height = GetPixelHeight();
	// ---- feed fire ----
	float *pReadEnd = m_pDblBuf2+(width*((height)-2));
	for(xx=0 ; xx<width>>1 ; xx++)
	{
		float val;
/*		float val;
		 *((unsigned int *)(&val)) = 0x3f000000 |(m_randomval&0x008fffff);
		 val = (val-0.5f)*2.0f;*/
		val =  ((float)(m_randomval>>16))*(1.0f/65536.0f);
		*pReadEnd++ = val*val;
		*pReadEnd++ = val*val;
		m_randomval = 0x129e1923 + 0x722e159b * m_randomval;
	}
	// ---- apply convolution  ---
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
			// value to bitmap:
			int pixelv = (int)((sinf(value*64.0f)+1.0f)*127.5f);
			*(pBitmap++) = (unsigned char)pixelv;
			*(pBitmap++) = (unsigned char)pixelv;
			*(pBitmap++) = (unsigned char)pixelv>>1;
			*(pBitmap++) = 255;
			pWriten++;
			pRead++;
		}	

	}
	// ---- swap buffers  ---
	float *inv = m_pDblBuf1;
	m_pDblBuf1 = m_pDblBuf2;
	m_pDblBuf2 = inv;

}


