// SPDX-License-Identifier: LGPL-2.1-only

#include "VirtualMedia.h"
#include "VirtualMachine.h"
#include "BaseContext.h"
BASEOBJECT_DECLARE_VIRTUALCLASS( "VMe", VirtualMedia, BaseObject );

VirtualMedia::VirtualMedia() : BaseObject()
	,m_SoundIsMixed(false)
	,m_MediaTimeLength(120.0)
{

	m_MediaSoundInterface.m_pVirtualMedia = this ;
}

void VirtualMedia::ProcessMediaOnDefaultViewport( double _frameDate )
{
	VirtualMachine *pMachine = GetMachine();
	if(!pMachine) return;
	VirtualMachine::InternalViewPort *pViewPort =pMachine->GetDefaultViewPort();
	if(!pViewPort) return;

	ProcessMedia(_frameDate,pViewPort);
}

void VirtualMedia::ProcessMedia( double ,VirtualMachine::InternalViewPort * )
{

}
#ifdef _ENGINE_EDITABLE_

void	VirtualMedia::StartPreview()
{
	SetSound(true);
}
#endif
#ifdef _ENGINE_EDITABLE_

void VirtualMedia::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *)
{
	ProcessMedia(_frameDate,_pPreviewViewPort);
}
#endif
#ifdef _ENGINE_EDITABLE_

void	VirtualMedia::EndPreview()
{
	SetSound(false);
}
#endif

void VirtualMedia::ProcessSoundInterupt(  VirtualMachine::SoundBufferToAddYourSignal & )
{

}

void	VirtualMedia::SetSound( bool	_Enable )
{
	if( m_SoundIsMixed == _Enable ) return;
	GetMachine()->EnableMediaSound( &m_MediaSoundInterface,_Enable);
	m_SoundIsMixed = _Enable ;
}
#ifdef _ENGINE_EDITABLE_

void	VirtualMedia::CloseInternal(void)
{

	SetSound(false);
}
#endif

void	VirtualMedia::MediaSoundInterface::ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )
{

#ifdef _ENGINE_EDITABLE_

		if( m_pVirtualMedia->GetContext() &&
		    m_pVirtualMedia->GetContext()->AddOneLockForThreadUse() )
		{

			if( m_pVirtualMedia->isCreated() )
				m_pVirtualMedia->ProcessSoundInterupt( _SoundBufferToAddYourSignal );
			m_pVirtualMedia->GetContext()->RemoveOneLockForThreadUse() ;
		}
#endif

#ifndef _ENGINE_EDITABLE_

	if( m_pVirtualMedia->isCreated() )
		m_pVirtualMedia->ProcessSoundInterupt( _SoundBufferToAddYourSignal );

#endif
}

#ifdef _ENGINE_EDITABLE_

void VirtualMedia::ProcessPreview_DrawSoundBuffer(
								VirtualMachine::InternalViewPort *_pViewport,
								float	*_pSoundBuffer,
								unsigned int	_bufferlength )
{
	if( !_pViewport ) return;

	int bufferIndex =0;
	 int	width = 320;
	 int	height = 96;
	unsigned char	*pDrawRec= new unsigned char[ (width * height)<<2 ];
	if(!pDrawRec) return;

	unsigned char	*pDrawRecD=  pDrawRec;
	float flhh = (float)(height>>1);
	int	yy,xx,nl,nr,lyl,lyr;
	lyr = lyl = height>>1;
	for(xx=0; xx<width ; xx++)
	{
		unsigned char	*pDrawRecE= pDrawRecD ;

		for(yy=0 ; yy<height ; yy++)
		{
			pDrawRecE[0]=0;
			pDrawRecE[1]=0;
			pDrawRecE[2]=64;
			pDrawRecE[3]=255;
			pDrawRecE += (width<<2) ;

		}
		if(bufferIndex>=(int)(_bufferlength<<1))
		{
			bufferIndex =0;
		}

			nl = (int)(( _pSoundBuffer[bufferIndex] )* flhh )+(height>>1);
			bufferIndex++;
			int aa= lyl;
			int bb= nl;
			if(aa>bb)
			{	aa = nl;
				bb = lyl;
			}
			if(aa == bb) bb++;
			if(aa<0) aa=0;
			else if (aa>=height) aa = height-1;
			if(bb<0) bb=0;
			else if (bb>=height) bb = height-1;

			pDrawRecE = pDrawRecD + (aa*width<<2) ;
			for( ;aa<bb ; aa++)
			{
				pDrawRecE[0]=255;
				pDrawRecE += (width<<2) ;
			}
			lyl = nl;

				nr = (int)(( _pSoundBuffer[bufferIndex] )* flhh )+(height>>1);
				bufferIndex++;
				aa= lyr;
				bb= nr;
				if(aa>bb)
				{	aa = nr;
					bb = lyr;
				}
				if(aa == bb) bb++;
				if(aa<0) aa=0;
				else if (aa>=height) aa = height-1;
				if(bb<0) bb=0;
				else if (bb>=height) bb = height-1;

				pDrawRecE = pDrawRecD + (aa*width<<2) ;
				for( ;aa<bb ; aa++)
				{
					pDrawRecE[1]=255;
					pDrawRecE += (width<<2) ;
				}
				lyr = nr;

		pDrawRecD+=4;
	}

	_pViewport->DrawRGBARectangle( pDrawRec,
							width,
							height
							);

	delete pDrawRec;
}
#endif
