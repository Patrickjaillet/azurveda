#include "StdAfx.h"
#include ".\previewmachine.h"
#include "OpenGLWnd.h"
#include <gl/gl.h>
#include <gl/glu.h>

//! the machine currently set for a wgl context:
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
/*!
	\brief	Init
*/
VirtualMachine::eVMResult	CPreviewMachine::InitViewPort(  InternalViewPort **_ppViewPortOut  )
{
	SetContext();

	return vmr_OK;
}

/*!
	\brief	Init
*/
void	CPreviewMachine::InitFromMFCView(  COpenGLWnd *_pMFCOpenGLView )
{	SetContext();
	m_pGLMFCView = _pMFCOpenGLView ;
	InitMachine();
}

/*!
	\brief	Tool for other methods: Set the current GL viewport matrix to an InternalViewPort.
*/
void	CPreviewMachine::PreviewOGLInternalViewPort::SetGLToViewPort( )
{
	if( ! m_pManager->SetContext() ) return;
	OGLMachine::OGLInternalViewPort::SetGLToViewPort();

}
/*!
	\brief	Tool for other methods: Set the opengl context for this preview windows
*/
bool	CPreviewMachine::SetContext(void)
{
	if( !m_pGLMFCView ) return false;
	if(m_pCurrentGLContextMachine == this) return true; // already OK.
	m_pCurrentGLContextMachine = this;
	m_pGLMFCView->SetContext();
	return true;
}

VirtualMachine::eVMResult	CPreviewMachine::InitMachine()
{
	// sound init:
	SoundInit( AfxGetApp()->m_pMainWnd->GetSafeHwnd() );

	InitLocalization();

	SetContext() ;
	if( m_pDefaultViewPort ) return vmr_OK; // already inited??? shouldn't happen.

	OGLInternalViewPort *pglDefVP = new PreviewOGLInternalViewPort(0L,this);
	m_pDefaultViewPort = pglDefVP ;
	if( !m_pDefaultViewPort ) return vmr_FAILED;
	// init some child of m_pDefaultViewPort, used for preview rendering
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
	// init openGL extension:
	if(!glActiveTextureARB)
		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
			wglGetProcAddress("glActiveTextureARB");
	if(!glClientActiveTextureARB)
		glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)
		wglGetProcAddress("glClientActiveTextureARB");
	// process supermethod to OGLMachine, not OGLMachineWinDxSound, which is made
	// for a single screen fast rendering.
	return OGLMachine::InitMachine() ;
}

/*!
	\brief	Ask the Creation of a new InternalObject3DBuffer you can then fill with 
				InternalObject3DBuffer::GetFirstVertex() , InternalObject3DBuffer::GetFirstTriangle()
				and InternalObject3DBuffer::SetNumberOfActiveTriangle() .
				Then the object can be positionned with the matrix methods, and drawn with RenderMesh()
				The object shape can be dynamic and rebuilt, until a call to CompileAsStatic().
				In edition mode, object has to be destroyed with DeleteObject3DBuffer().
	\param	_ppVertexAndPolygonBufferOut	the object returned, or 0L if failed.(no mem ?)
	\param	_maxNbVertex					maximum number of vertex to fill.
	\param	_maxNbTriangle					maximum number of Triangle to fill.
	\param	_extraMemberFlags				or'ed bits (bOb3D_...) to activate extra vertex members.
*/
void	CPreviewMachine::NewObject3DBuffer(  InternalObject3DBuffer **_ppVertexAndPolygonBufferOut, 
						unsigned int _maxNbVertex,
						unsigned int _maxNbTriangle, 
						unsigned int _extraMemberFlags )
{
	SetContext() ;
	OGLMachine::NewObject3DBuffer(_ppVertexAndPolygonBufferOut,_maxNbVertex,_maxNbTriangle,
		_extraMemberFlags ) ;
}
// constructor;
CPreviewMachine::PreviewOGLInternalTexture::PreviewOGLInternalTexture() : 
	OGLInternalTexture()
{

}
void	CPreviewMachine::NewTexture(  InternalTexture **_ppTextureOut  )
{
	SetContext() ;
	//NO! OGLMachine::NewTexture(_ppTextureOut);
	PreviewOGLInternalTexture *pTexture = new PreviewOGLInternalTexture;
	if(pTexture) pTexture->m_pPreviewMachine = this;
	*_ppTextureOut = (InternalTexture *)pTexture ;
}

/*!
	\brief	Ask the Creation of a new InternalViewPort, that must be the child of another ViewPort,
			specified by _pFatherViewPort.if no father is specified, the new ViewPort will be
			the child of the default root ViewPort. To create another Root ViewPort, create a texture
			and use VirtualMedia::NewViewPortOnTexture().
			In edition mode, each InternalViewPort has to be destroyed with DeleteViewPort().
	\param	_ppViewPortOut	the object returned, or 0L if failed.(no mem ?)
	\param	_pFatherViewPort the father viewport. If 0L, it uses the default root.
*/
void	CPreviewMachine::NewViewPortChild(  InternalViewPort **_ppViewPortOut,InternalViewPort *_pFatherViewPort)
{
	SetContext() ;
	//? *_ppViewPortOut = new PreviewOGLInternalViewPort();
	*_ppViewPortOut = 0L; 
	if(!_pFatherViewPort) _pFatherViewPort = GetDefaultViewPort();
	PreviewOGLInternalViewPort *pVP;
	*_ppViewPortOut = pVP = new PreviewOGLInternalViewPort((OGLInternalViewPort *)_pFatherViewPort,this) ;
	/*if(pVP)
	{
		pVP->
	}*/
}
/*!
	\brief	Ask the Creation of a new Root InternalViewPort, that will render on a texture image.
			specified by _pRootTexture. You got to create a texture first for parameter _pRootTexture.
			In edition mode, each InternalViewPort has to be destroyed with DeleteViewPort().
	\param	_ppViewPortOut	the object returned, or 0L if failed.(no mem ?)
	\param	_pRootTexture an InternalTexture. Can't be null.
*/
void	CPreviewMachine::NewViewPortOnTexture(  InternalViewPort **_ppViewPortOut,InternalTexture *_pRootTexture )
{
	SetContext() ;
	//OGLMachine::NewViewPortOnTexture(_ppViewPortOut,_pRootTexture);

	*_ppViewPortOut = 0L; 
	// null if no texture:
	if(!_pRootTexture) return;
	OGLInternalViewPort *pNewVP;
	*_ppViewPortOut = pNewVP = new PreviewOGLInternalViewPort(0L,this); // create new root.
	pNewVP->m_pRenderTexture = (OGLInternalTexture *)_pRootTexture;
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Destroy any objects previously created with NewViewPortChild() or NewViewPortOnTexture().
	\param	_ppViewPortToDestroy	 pointer to object to destroy, and set to 0L.
*/
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

/*!
	\brief	Reset color to black and depth buffer.
*/
void	CPreviewMachine::PreviewOGLInternalViewPort::Clear( )
{
	if( ! m_pManager->SetContext() ) return;		
	OGLMachine::OGLInternalViewPort::Clear();
}
/*!
	\brief	Render a 3D Object. The vertexes are transformed with matrix ,
			the object shape is defined by _pMeshBuffer, the texture of the object is
			defined by _pTexture, the ViewPort that define the rectangle where to draw,
			and the camera parameters is _pViewPort, or the default viewport if not specified.	
			Note drawings on the main screen needs a SwapScreenBuffer() to be shown at each frames.
	\param	_pMeshBuffer InternalObject3DBuffer
	\param	_pTexture InternalTexture
	\param	_pViewPort InternalViewPort
*/
void	CPreviewMachine::PreviewOGLInternalViewPort::RenderMesh(
						InternalObject3DBuffer *_pMeshBuffer,
						InternalTexture *_pTexture
						) 
{
	if( ! m_pManager->SetContext() ) return;	
	OGLMachine::OGLInternalViewPort::RenderMesh(_pMeshBuffer,_pTexture);
}
/*!
	\brief	Swap buffer so that what was drawn on the viewport is shown.
			this is needed to active all Clear(), RenderMesh() ,...
			It has to be done with main screen viewport like texture-rendering
			viewports.
*/
void	CPreviewMachine::PreviewOGLInternalViewPort::SwapBuffer()
{
	if( ! m_pManager->SetContext() ) return;	
	OGLMachine::OGLInternalViewPort::SwapBuffer();
}
/*!
	\brief	Swap buffer so that what was drawn is shown.
*/
void	CPreviewMachine::SwapMainScreenBuffer()
{
	SetContext();
	OGLMachineWinDxSound::SwapMainScreenBuffer();
}
/*!
	\brief	Render directly a 32bit pixel bitmap image from memory on a viewport.
			Note drawings on the main screen needs a SwapScreenBuffer() to be shown at each frames.
	\param	_prgbaTable	pointer to the beginning of a 32bit RGBA table.
	\param	_PixelWidth number of pixel of a line
	\param	_PixelHeight number of pixel lines.
	\param	_pViewPort the viewport to affect, or the default screen if not specified.
*/
void	CPreviewMachine::PreviewOGLInternalViewPort::DrawRGBARectangle( 
									unsigned char	*_prgbaTable,
									unsigned int	_PixelWidth,
									unsigned int	_PixelHeight
									)
{
	if( ! m_pManager->SetContext() ) return;
	// optimisation.
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
/*
	\brief notification of a window resize:
*/
void	CPreviewMachine::OnSize( int _width, int _height)
{
	if( m_pDefaultViewPort )
	{
		((OGLInternalViewPort *)m_pDefaultViewPort)->ResetRootPixelSize(_width,_height);
	}
}
/*!
	\brief	Constructor. 
	\param	_pFather the father, or NULL if new root.
*/
CPreviewMachine::PreviewOGLInternalViewPort::PreviewOGLInternalViewPort(
									OGLInternalViewPort *_pFather,
									CPreviewMachine *_pManager)
  : OGLInternalViewPort(_pFather,_pManager)
{
	/* DONT OVERRIDE m_pixel_x1 AFTER CORRECT SUPER INIT !!!
	m_pixel_x1 =
	m_pixel_y1 =
	m_pixel_x2 =
	m_pixel_y2 = 0;*/
	m_pManager = _pManager;
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Destructor. See DeleteViewport() 
*/
CPreviewMachine::PreviewOGLInternalViewPort::~PreviewOGLInternalViewPort()
{

}
#endif
/*!
	\brief	return the viewport in which a pixel coordinate is.
			if a child viewport is also surrounding, the child is returned.
			It also return the value of this pixel position in the viewport's ratio.
	\param	_xPosition pixel position in the root viewport.
	\param	_yPosition pixel position in the root viewport.
	\param	_returnXValue	if return is not null, the pixel's value in the VP ratio.
	\param	_returnYValue	if return is not null, the pixel's value in the VP ratio.

*/	
CPreviewMachine::PreviewOGLInternalViewPort *
	CPreviewMachine::PreviewOGLInternalViewPort::GetViewportAtPixel( 
						int _xPosition,int _yPosition,
						float	&_returnXValue,float	&_returnYValue ) 
{
	if( _xPosition<m_pixel_x1 ||
		_xPosition>=m_pixel_x2 ||
		_yPosition<m_pixel_y1 ||
		_yPosition>=m_pixel_y2 ) return 0L;
	// recurse to test if child match better
	PreviewOGLInternalViewPort *pBetterChildMatch=0L;
	PreviewOGLInternalViewPort *pChild = (PreviewOGLInternalViewPort *)m_pClippingFirstSonViewPort ;
	while(pChild)
	{
		// test 
		pBetterChildMatch = pChild->GetViewportAtPixel(_xPosition,_yPosition,
			_returnXValue,_returnYValue);
		if(pBetterChildMatch) return pBetterChildMatch;
		// next:
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


/*!
	\brief	return a preview viewport, child of the main viewport.
	\param	_index the index since zero, or nothing.
	\return an InternalViewPort ready to draw, or NULL if index too high.
*/
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
