// SPDX-License-Identifier: LGPL-2.1-only

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

bool VirtualImage::CreateInternal(void)
{
	m_pBuffer = new unsigned char[	GetPixelWidth() *
									GetPixelHeight() *
									GetByteDepth()
									];

	if(m_pBuffer == 0L ) return(false);

	m_CurrentFrameDate = PackFloat::m_Max ;

	return(true);
}
#ifdef _ENGINE_EDITABLE_

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

void	VirtualImage::UpdateToFrame(float )
{

}
#ifdef _ENGINE_EDITABLE_

void VirtualImage::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

	if( IsTimeDynamic() )  UpdateToFrame((float)_frameDate);

	float opx,opy,opz;
	_pPreviewConfiguration->GetPreviewPosition( opx,opy,opz);
	const int nbx =  GetPixelWidth() ;
	const int nby =  GetPixelHeight() ;
	const float v65536=-65536.0f;
	int decalx = (((int)(opx*v65536))*nbx)>>16;
	int decaly = (((int)(opy*v65536))*nby)>>16;
	decalx %= nbx;
	decaly %= nby;
	if(decalx<0)decalx+=nbx;
	if(decaly<0)decaly+=nby;
	if(!m_pPeviewRGBABuffer)
	{

		m_pPeviewRGBABuffer = new unsigned char[ nbx * nby * 4 ];
		if(!m_pPeviewRGBABuffer) return;
	}
	unsigned char *pp = m_pPeviewRGBABuffer ;
	unsigned char *pread = m_pBuffer;
	if( !pread ) return;

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
					if(decalx2>=(unsigned int)nbx)decalx2=0;
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

					}

					*(pp++) = 255;
					decalx2++;
					if(decalx2>=(unsigned int)nbx)decalx2=0;
				}
				decaly++;
				if(decaly>=nby)decaly=0;
			}
		}
		break;
		default:
			return;

	}
	_pPreviewViewPort->DrawRGBARectangle( m_pPeviewRGBABuffer,
									nbx,
									nby
									);
}
#endif
