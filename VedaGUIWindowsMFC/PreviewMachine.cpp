#include "StdAfx.h"
#include ".\previewmachine.h"
#include "OpenGLWnd.h"
#include <gl/gl.h>
#include <gl/glu.h>

CPreviewMachine *CPreviewMachine::m_pCurrentGLContextMachine=0L;

CPreviewMachine::CPreviewMachine(void) : OGLMachineWinDxSound()
	,m_pGLMFCView(0L)
{

}

CPreviewMachine::~CPreviewMachine(void)
{
	m_pGLMFCView = NULL;
	unsigned int ii;
	for(ii=0 ; ii<m_MaxNumberOfChildView ; ii++)
	{
		if( m_ChildViewportTable[ii].m_ChildViewPort != NULL )
			DeleteViewPort(&(m_ChildViewportTable[ii].m_ChildViewPort));
		if( m_ChildViewportTable[ii].m_pPositionInScreenViewPort != NULL )
			DeleteViewPort(&(m_ChildViewportTable[ii].m_pPositionInScreenViewPort));
	}

}

VirtualMachine::eVMResult	CPreviewMachine::InitViewPort(  InternalViewPort **  )
{
	SetContext();

	return vmr_OK;
}

void	CPreviewMachine::InitFromMFCView(  COpenGLWnd *_pMFCOpenGLView )
{	SetContext();
	m_pGLMFCView = _pMFCOpenGLView ;
	InitMachine();
}

void	CPreviewMachine::PreviewOGLInternalViewPort::SetGLToViewPort( )
{
	if( ! m_pManager->SetContext() ) return;
	OGLMachine::OGLInternalViewPort::SetGLToViewPort();

}

bool	CPreviewMachine::SetContext(void)
{
	if( !m_pGLMFCView ) return false;
	if(m_pCurrentGLContextMachine == this) return true;
	m_pCurrentGLContextMachine = this;
	m_pGLMFCView->SetContext();
	return true;
}

VirtualMachine::eVMResult	CPreviewMachine::InitMachine()
{

	SoundInit( AfxGetApp()->m_pMainWnd->GetSafeHwnd() );

	InitLocalization();

	SetContext() ;
	if( m_pDefaultViewPort ) return vmr_OK;

	OGLInternalViewPort *pglDefVP = new PreviewOGLInternalViewPort(0L,this);
	m_pDefaultViewPort = pglDefVP ;
	if( !m_pDefaultViewPort ) return vmr_FAILED;

	unsigned int ii;
	for(ii=0 ; ii<m_MaxNumberOfChildView ; ii++)
	{
		m_ChildViewportTable[ii].m_ChildViewPort =NULL;
		m_ChildViewportTable[ii].m_pPositionInScreenViewPort =NULL;
	}
	for(ii=0 ; ii<m_MaxNumberOfChildView ; ii++)
	{
		NewViewPortChild( &(m_ChildViewportTable[ii].m_pPositionInScreenViewPort),m_pDefaultViewPort);
		if( m_ChildViewportTable[ii].m_pPositionInScreenViewPort == NULL ) return vmr_FAILED;
		NewViewPortChild( &(m_ChildViewportTable[ii].m_ChildViewPort),m_ChildViewportTable[ii].m_pPositionInScreenViewPort);
		if( m_ChildViewportTable[ii].m_ChildViewPort == NULL ) return vmr_FAILED;
	}

	if(!glActiveTextureARB)
		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
			wglGetProcAddress("glActiveTextureARB");
	if(!glClientActiveTextureARB)
		glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)
		wglGetProcAddress("glClientActiveTextureARB");

	return OGLMachine::InitMachine() ;
}

void	CPreviewMachine::NewObject3DBuffer(  InternalObject3DBuffer **_ppVertexAndPolygonBufferOut,
						unsigned int _maxNbVertex,
						unsigned int _maxNbTriangle,
						unsigned int _extraMemberFlags )
{
	SetContext() ;
	OGLMachine::NewObject3DBuffer(_ppVertexAndPolygonBufferOut,_maxNbVertex,_maxNbTriangle,
		_extraMemberFlags ) ;
}

CPreviewMachine::PreviewOGLInternalTexture::PreviewOGLInternalTexture() :
	OGLInternalTexture()
{

}
void	CPreviewMachine::NewTexture(  InternalTexture **_ppTextureOut  )
{
	SetContext() ;

	PreviewOGLInternalTexture *pTexture = new PreviewOGLInternalTexture;
	if(pTexture) pTexture->m_pPreviewMachine = this;
	*_ppTextureOut = (InternalTexture *)pTexture ;
}

void	CPreviewMachine::NewViewPortChild(  InternalViewPort **_ppViewPortOut,InternalViewPort *_pFatherViewPort)
{
	SetContext() ;

	*_ppViewPortOut = 0L;
	if(!_pFatherViewPort) _pFatherViewPort = GetDefaultViewPort();
	PreviewOGLInternalViewPort *pVP;
	*_ppViewPortOut = pVP = new PreviewOGLInternalViewPort((OGLInternalViewPort *)_pFatherViewPort,this) ;

}

void	CPreviewMachine::NewViewPortOnTexture(  InternalViewPort **_ppViewPortOut,InternalTexture *_pRootTexture )
{
	SetContext() ;

	*_ppViewPortOut = 0L;

	if(!_pRootTexture) return;
	OGLInternalViewPort *pNewVP;
	*_ppViewPortOut = pNewVP = new PreviewOGLInternalViewPort(0L,this);
	pNewVP->m_pRenderTexture = (OGLInternalTexture *)_pRootTexture;
}
#ifdef _ENGINE_EDITABLE_

void	CPreviewMachine::DeleteViewPort(  InternalViewPort **_ppViewPortToDestroy )
{
	SetContext() ;
	OGLMachine::DeleteViewPort(_ppViewPortToDestroy);
}
#endif
void	CPreviewMachine::DeleteObject3DBuffer(  InternalObject3DBuffer **_ppBufferToDestroy )
{
	SetContext() ;
	OGLMachine::DeleteObject3DBuffer(_ppBufferToDestroy);
}
void	CPreviewMachine::DeleteTexture(  InternalTexture **_ppTextureToDestroy )
{
	SetContext() ;
	OGLMachine::DeleteTexture(_ppTextureToDestroy);
}

void	CPreviewMachine::PreviewOGLInternalViewPort::Clear( )
{
	if( ! m_pManager->SetContext() ) return;
	OGLMachine::OGLInternalViewPort::Clear();
}

void	CPreviewMachine::PreviewOGLInternalViewPort::RenderMesh(
						InternalObject3DBuffer *_pMeshBuffer,
						InternalTexture *_pTexture
						)
{
	if( ! m_pManager->SetContext() ) return;
	OGLMachine::OGLInternalViewPort::RenderMesh(_pMeshBuffer,_pTexture);
}

void	CPreviewMachine::PreviewOGLInternalViewPort::SwapBuffer()
{
	if( ! m_pManager->SetContext() ) return;
	OGLMachine::OGLInternalViewPort::SwapBuffer();
}

void	CPreviewMachine::SwapMainScreenBuffer()
{
	SetContext();
	OGLMachineWinDxSound::SwapMainScreenBuffer();
}

void	CPreviewMachine::PreviewOGLInternalViewPort::DrawRGBARectangle(
									unsigned char	*_prgbaTable,
									unsigned int	_PixelWidth,
									unsigned int	_PixelHeight
									)
{
	if( ! m_pManager->SetContext() ) return;

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_BLEND );
	glDisable( GL_DITHER );
	OGLMachine::OGLInternalViewPort::DrawRGBARectangle(_prgbaTable,_PixelWidth,_PixelHeight);
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glEnable( GL_DITHER );

}
void	CPreviewMachine::PreviewOGLInternalViewPort::Matrix_LoadID()
{
	m_pManager->SetContext();
	OGLInternalViewPort::Matrix_LoadID();
}
void	CPreviewMachine::PreviewOGLInternalViewPort::Matrix_Translate(float _x,float _y,float _z)
{
	m_pManager->SetContext();
	OGLInternalViewPort::Matrix_Translate(_x,_y,_z);
}
void    CPreviewMachine::PreviewOGLInternalViewPort::Matrix_Rotate( float _angle,float _x,float _y,float _z)
{
	m_pManager->SetContext();
	OGLInternalViewPort::Matrix_Rotate(_angle,_x,_y,_z);
}
void	CPreviewMachine::PreviewOGLInternalViewPort::Matrix_Scale(float _sx,float _sy,float _sz)
{
	m_pManager->SetContext();
	OGLInternalViewPort::Matrix_Scale(_sx,_sy,_sz );
}
void	CPreviewMachine::PreviewOGLInternalViewPort::Matrix_Pop()
{
	m_pManager->SetContext();
	OGLInternalViewPort::Matrix_Pop();
}
void	CPreviewMachine::PreviewOGLInternalViewPort::Matrix_Push()
{
	m_pManager->SetContext();
	OGLInternalViewPort::Matrix_Push();
}

void CPreviewMachine::PreviewOGLInternalTexture::SetRenderFlags( unsigned int _flags )
{
	m_pPreviewMachine->SetContext();
	OGLInternalTexture::SetRenderFlags(_flags);
}
void	CPreviewMachine::PreviewOGLInternalTexture::SetImageBitmap(	TextureImageLayerEnum _layerToSet,
					unsigned char *_pByteChunk,
					unsigned int _pixelByteDepth,
					unsigned int _pixelWidth,
					unsigned int _pixelHeight
					)
{
	m_pPreviewMachine->SetContext();
	OGLInternalTexture::SetImageBitmap(_layerToSet,_pByteChunk,_pixelByteDepth,_pixelWidth,_pixelHeight);
}

void	CPreviewMachine::OnSize( int _width, int _height)
{
	if( m_pDefaultViewPort )
	{
		((OGLInternalViewPort *)m_pDefaultViewPort)->ResetRootPixelSize(_width,_height);
	}
}

CPreviewMachine::PreviewOGLInternalViewPort::PreviewOGLInternalViewPort(
									OGLInternalViewPort *_pFather,
									CPreviewMachine *_pManager)
  : OGLInternalViewPort(_pFather,_pManager)
{

	m_pManager = _pManager;
}
#ifdef _ENGINE_EDITABLE_

CPreviewMachine::PreviewOGLInternalViewPort::~PreviewOGLInternalViewPort()
{

}
#endif

CPreviewMachine::PreviewOGLInternalViewPort *
	CPreviewMachine::PreviewOGLInternalViewPort::GetViewportAtPixel(
						int _xPosition,int _yPosition,
						float	&_returnXValue,float	&_returnYValue )
{
	if( _xPosition<m_pixel_x1 ||
		_xPosition>=m_pixel_x2 ||
		_yPosition<m_pixel_y1 ||
		_yPosition>=m_pixel_y2 ) return 0L;

	PreviewOGLInternalViewPort *pBetterChildMatch=0L;
	PreviewOGLInternalViewPort *pChild = (PreviewOGLInternalViewPort *)m_pClippingFirstSonViewPort ;
	while(pChild)
	{

		pBetterChildMatch = pChild->GetViewportAtPixel(_xPosition,_yPosition,
			_returnXValue,_returnYValue);
		if(pBetterChildMatch) return pBetterChildMatch;

		pChild = (PreviewOGLInternalViewPort *)
							pChild->m_pClippingNextBrotherViewPort ;
	}
	float mul = 1.0f/65536.0f;
	_returnXValue = m_ScaleX1 +
						((float)(((_xPosition-m_pixel_x1)<<16)/(m_pixel_x2-m_pixel_x1))) *
							(m_ScaleX2-m_ScaleX1) * mul;
	_returnYValue =  m_ScaleY1 +
						((float)(((_yPosition-m_pixel_y1)<<16)/(m_pixel_y2-m_pixel_y1))) *
							(m_ScaleY2-m_ScaleY1) * mul;
	return this;
}

void	CPreviewMachine::GetChildViewport(
		InternalViewPort *&_pchildViewport,
		InternalViewPort *&_ppositionViewPort,
		unsigned int _index)
{
	_pchildViewport = 0L;
	_ppositionViewPort = 0L;
	if( _index>=m_MaxNumberOfChildView) return;
	_pchildViewport = m_ChildViewportTable[_index].m_ChildViewPort ;
	_ppositionViewPort = m_ChildViewportTable[_index].m_pPositionInScreenViewPort ;

}
