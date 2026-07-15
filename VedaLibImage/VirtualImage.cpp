/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "VirtualImage.h"
#include "VirtualMachine.h"
#include "PackFloat.h"
BASEOBJECT_DECLARE_VIRTUALCLASS( "Img", VirtualImage, BaseObject );

VirtualImage::VirtualImage() : BaseObject()
		,m_pBuffer(0L)
		,m_IsTimeDynamic(false)
#ifdef _ENGINE_EDITABLE_
		,m_pPeviewRGBABuffer(0L)
#endif
{
	
}

/*!
	\brief	Method that really build the object using the serializable parameters.
			For the virtual VirtualImage class, it allocs the bitmap memory.
			Close() should close everything opened by CreateInternal().
*/
bool VirtualImage::CreateInternal(void)
{
	m_pBuffer = new unsigned char[	GetPixelWidth() *
									GetPixelHeight() *
									GetByteDepth()
									];
	// error after alloc ?
	if(m_pBuffer == 0L ) return(false);
	// set dynamique update date to something impossible:
	m_CurrentFrameDate = PackFloat::m_Max ;


	return(true);
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void VirtualImage::CloseInternal(void)
{
	if( m_pBuffer )
	{
		delete [] m_pBuffer;
		m_pBuffer = 0L;
	}
	if(m_pPeviewRGBABuffer)
	{
		delete [] m_pPeviewRGBABuffer;
		m_pPeviewRGBABuffer = 0L;
	}
}
#endif
/*
	\brief	If IsTimeDynamic() is true, this image can change with time.
			You have to use UpdateToFrame()to make the image change to a given date.
			by default, UpdateToFrame() does nothing.
	\param	_framedate time in seconds.
*/
void	VirtualImage::UpdateToFrame(float _framedate)
{
	// does nothing....
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. This is done with this method. sub classes can implement it (or not) in
			any way, to explicit current shape of an object.<br>

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
	\param	_pPreviewConfiguration
*/
void VirtualImage::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	// the image can be time dynamic:
	if( IsTimeDynamic() )  UpdateToFrame((float)_frameDate);

	float opx,opy,opz;
	_pPreviewConfiguration->GetPreviewPosition( opx,opy,opz);
	const int nbx =  GetPixelWidth() ;
	const int nby =  GetPixelHeight() ;
	const float v65536=-65536.0f;
	int decalx = (((int)(opx*v65536))*nbx)>>16; // in pixels
	int decaly = (((int)(opy*v65536))*nby)>>16;
	decalx %= nbx;
	decaly %= nby;
	if(decalx<0)decalx+=nbx;
	if(decaly<0)decaly+=nby;
	if(!m_pPeviewRGBABuffer)
	{	// always RGBA (size 4) for preview buffer:
		// it is being closed by CloseInternal()
		m_pPeviewRGBABuffer = new unsigned char[ nbx * nby * 4 ];
		if(!m_pPeviewRGBABuffer) return; // alloc failed.
	}
	unsigned char *pp = m_pPeviewRGBABuffer ;
	unsigned char *pread = m_pBuffer;
	if( !pread ) return;


	// draw the exact image in grey or RGB.
	switch( GetByteDepth() )
	{
		case 1:
		{
			for(int yy=0 ; yy<nby ; yy++)
			{
				unsigned char *preadx = &pread[decaly*nbx];
				unsigned int decalx2=decalx;
				for(int xx=0 ; xx<nbx ; xx++)
				{
					unsigned char cc;
					cc = preadx[decalx2];
					*(pp++) = cc;
					*(pp++) = cc;
					*(pp++) = cc;
					*(pp++) = 255;					
					decalx2++;
					if(decalx2>=nbx)decalx2=0;
				}	
				decaly++;
				if(decaly>=nby)decaly=0;
			}
		}
		break;	
		case 4:
		{
			for(int yy=0 ; yy<nby ; yy++)
			{
				unsigned char *preadx = &pread[decaly*nbx<<2];
				unsigned int decalx2=decalx;
				for(int xx=0 ; xx<nbx ; xx++)
				{
					// mix alpha is needed.
					unsigned char *px = &(preadx[decalx2<<2]);
					unsigned int rr,gg,bb,aa;
					rr =*px++;
					gg =*px++;
					bb =*px++;
					aa =*px++;	
					if(aa==255)
					{
						*(pp++) = (unsigned char)rr;
						*(pp++) = (unsigned char)gg;
						*(pp++) = (unsigned char)bb;
					} else
					{
						int greyv;
						if(((xx>>4)&1)^((yy>>4)&1))
							greyv=128-32;
						else
							greyv=128+32;
						greyv *= (255L-aa);
						*(pp++) = (unsigned char)((rr*aa+greyv)>>8);
						*(pp++) = (unsigned char)((gg*aa+greyv)>>8);
						*(pp++) = (unsigned char)((bb*aa+greyv)>>8);
						/**(pp++) = aa;
						*(pp++) = aa;
						*(pp++) = aa;*/
					}
					// alpha
					*(pp++) = 255;
					decalx2++;
					if(decalx2>=nbx)decalx2=0;
				}	
				decaly++;
				if(decaly>=nby)decaly=0;
			}
		}
		break;
		default:
			return;
			//break;
	} // end switch
	_pPreviewViewPort->DrawRGBARectangle( m_pPeviewRGBABuffer, 
									nbx,
									nby												
									);
}
#endif
