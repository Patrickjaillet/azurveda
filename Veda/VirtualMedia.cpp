/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "VirtualMedia.h"
#include "VirtualMachine.h"
#include "BaseContext.h"
BASEOBJECT_DECLARE_VIRTUALCLASS( "VMe", VirtualMedia, BaseObject );

VirtualMedia::VirtualMedia() : BaseObject()
	,m_SoundIsMixed(false)
	,m_MediaTimeLength(120.0) // default value.
{
	// all medias are potentially noisy objects:
	m_MediaSoundInterface.m_pVirtualMedia = this ;
}
/*!
	\brief	 Process a media at a given date,
			or do anything) using a machine. 
			Note there are no play/pause or speed concept here, by default, a date is passed.
			This version redirect the drawing to the main screen viewport.
			It should the main entry to play a media.
	\param	_frameDate a date, in second, which defines the effect cinematic.
*/
void VirtualMedia::ProcessMediaOnDefaultViewport( double _frameDate )
{
	VirtualMachine *pMachine = GetMachine();
	if(!pMachine) return;
	VirtualMachine::InternalViewPort *pViewPort =pMachine->GetDefaultViewPort();
	if(!pViewPort) return;
	// send to virtual version:
	ProcessMedia(_frameDate,pViewPort);
}
/*!
	\brief	 Process a media at a given date, 
			or do anything) using a machine. You must process the media on machine this->GetMachine().
			Note there are no play/pause or speed concept here, by default, a date is passed.

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pViewPort the viewport to render. Can't be 0L.
*/
void VirtualMedia::ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort )
{

}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	any suit of call to ProcessPreview() should be done between
			StartPreview() and EndPreview() .
			EndPreview(). This is needed to stop sound mixing from the object
			when the preview change.
*/
void	VirtualMedia::StartPreview()
{
	SetSound(true);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. This is done with this method. sub classes can implement it (or not) in
			any way, to explicit current shape of an object.<br>
			Important: 
	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
*/
void VirtualMedia::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	ProcessMedia(_frameDate,_pPreviewViewPort);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	any suit of call to ProcessPreview() should be done between
			StartPreview() and EndPreview() .
			EndPreview(). This is needed to stop sound mixing from the object
			when the preview change.
*/
void	VirtualMedia::EndPreview()
{
	SetSound(false);
}
#endif
/*!
	\brief	 If the media had to generate real time sounds, ProcessSoundInterupt
			will be called by a machine. There is a default behaviour for this method,
			which does nothing. the float buffer accumulate the whole machine sound,
			and in the better case, it will stand in cache.

*/
void VirtualMedia::ProcessSoundInterupt(  VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )
{

}
/*!
	\brief	

		\param	_Enable true start sound, false stop it.
*/
void	VirtualMedia::SetSound( bool	_Enable )
{
	if( m_SoundIsMixed == _Enable ) return; // important, or it could link twice the same object.
	GetMachine()->EnableMediaSound( &m_MediaSoundInterface,_Enable); // link or unlink this objet in the list of object to mix:
	m_SoundIsMixed = _Enable ;				// change.
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void	VirtualMedia::CloseInternal(void)
{
	// stop the sound.
	SetSound(false);
}
#endif

/*!
	\brief	Method thrown by the sound creation thread to add a new signal in a float buffer.
			The whole result is played As soon as possible.
	\param	_SoundBufferToAddYourSignal the sound to write description.
*/
void	VirtualMedia::MediaSoundInterface::ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )
{
	// this MediaSoundInterface is only used to redirect the Virtual Machine method,
	// to the BaseObject and VirtualMedia method. This way, the VM is independant from
	// the azurveda mecanism.

#ifdef _ENGINE_EDITABLE_
		// in edition mode, it can be thrown by another thread, so we lock the context 
		// from any modification/deletion from a native thread.
		if( m_pVirtualMedia->GetContext() && 
		    m_pVirtualMedia->GetContext()->AddOneLockForThreadUse() )
		{
			// all BaseObjects (and so VirtualMedia objects) can only be used when created:
			if( m_pVirtualMedia->isCreated() )
				m_pVirtualMedia->ProcessSoundInterupt( _SoundBufferToAddYourSignal );
			m_pVirtualMedia->GetContext()->RemoveOneLockForThreadUse() ;
		}
#endif

#ifndef _ENGINE_EDITABLE_
	// non-editable mode: no thread lock.
	// all BaseObjects (and so VirtualMedia objects) can only be used when created:
	if( m_pVirtualMedia->isCreated() )
		m_pVirtualMedia->ProcessSoundInterupt( _SoundBufferToAddYourSignal );

#endif
}

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Tool to draw a preview of the current sound signal.
	\param	_pViewport  where to draw
	\param	_pSoundBuffer float left/right table
	\param	_bufferlength number of right/left on the table.
*/
void VirtualMedia::ProcessPreview_DrawSoundBuffer(	
								VirtualMachine::InternalViewPort *_pViewport,
								float	*_pSoundBuffer,
								unsigned int	_bufferlength )
{
	if( !_pViewport ) return;
	// alloc a bitmap:
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
		// clear
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

			// draw left:
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

	// draw:
	_pViewport->DrawRGBARectangle( pDrawRec,
							width,
							height
							);
	// free bitmap:
	delete pDrawRec;
}
#endif