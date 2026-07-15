/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "OGLMachine.h"

#include <GL/gl.h>
//
#ifndef NULL
#define	NULL	0L
#endif

PFNGLACTIVETEXTUREARBPROC		OGLMachine::glActiveTextureARB=0L;
PFNGLCLIENTACTIVETEXTUREARBPROC	OGLMachine::glClientActiveTextureARB=0L;

bool	OGLMachine::m_RenderObjectExtension=false;
PFNGLGENRENDERBUFFERSEXTPROC OGLMachine::glGenRenderbuffersEXT=NULL;// init
PFNGLGENFRAMEBUFFERSEXTPROC OGLMachine::glGenFramebuffersEXT=NULL;// init
PFNGLBINDRENDERBUFFEREXTPROC OGLMachine::glBindRenderbufferEXT =NULL; // init
PFNGLRENDERBUFFERSTORAGEEXTPROC OGLMachine::glRenderbufferStorageEXT=NULL; // init
PFNGLDELETEFRAMEBUFFERSEXTPROC OGLMachine::glDeleteFramebuffersEXT=NULL; // end
PFNGLDELETERENDERBUFFERSEXTPROC OGLMachine::glDeleteRenderbuffersEXT=NULL; //end
PFNGLBINDFRAMEBUFFEREXTPROC OGLMachine::glBindFramebufferEXT=NULL; // draw
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC OGLMachine::glFramebufferTexture2DEXT=NULL; //draw
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC OGLMachine::glFramebufferRenderbufferEXT=NULL ; //draw

OGLMachine::OGLMachine() : VirtualMachine()
{
	// OGL extension functions must be inited in inherited classes:
	// this is still a virtual OpenGL Machine class.
}

OGLMachine::~OGLMachine(void)
{
	if( m_pDefaultViewPort )
	{	
		delete m_pDefaultViewPort;
		m_pDefaultViewPort = 0L;
	}
}
/*!
	\brief	Init the machine. It would open a screen and do the init for each theme (3D,sound,...)
			the machine closing should be done at deletion.
	\return vmr_OK or vmr_FAILED
*/
VirtualMachine::eVMResult	OGLMachine::InitMachine()
{
	// default vp is created before by final class init:
	if( m_pDefaultViewPort )
	{
		m_pDefaultViewPort->SetFOVLength( 0.75f );
		m_pDefaultViewPort->SetNearCutPlane( 0.25f );
		m_pDefaultViewPort->SetFarCutPlane( 1024.0f );
	}
// Usual OpenGL default stuff

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glShadeModel( GL_SMOOTH ); // GL_FLAT
	glBlendFunc( GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA );
	glClearDepth(1.0f);
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_TEXTURE_2D );
	//glEnable( GL_ALPHA ); // test
	//glEnable( GL_RGBA ); // test
	glEnable( GL_BLEND );
	glEnable( GL_DITHER ); // so that it is not ugly on 16b screens.
	glEnable( GL_CULL_FACE );
	glEnable( GL_NORMALIZE ); // glScale renormals
	//glEnable( GL_SCISSOR_TEST );
    glCullFace( GL_FRONT );
	//glLineWidth( 2.0f );
	//glPointSize( 3.0f );
	// --- defaults:
	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
	// always:
    glEnableClientState(    GL_VERTEX_ARRAY );
    glDisableClientState(   GL_INDEX_ARRAY );
    glDisableClientState(   GL_EDGE_FLAG_ARRAY );

	return vmr_OK;
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
void	OGLMachine::NewObject3DBuffer(  InternalObject3DBuffer **_ppVertexAndPolygonBufferOut, 
						unsigned int _maxNbVertex,
						unsigned int _maxNbTriangle, 
						unsigned int _extraMemberFlags )
{
	OGLInternalObject3DBuffer *pObj; 

	*_ppVertexAndPolygonBufferOut = pObj =  new OGLInternalObject3DBuffer();
	if( !pObj ) return;
	
	pObj->m_pVertexBase		= new InternalVertex[ _maxNbVertex ];
	if( !(pObj->m_pVertexBase) )
	{
		delete pObj;
		*_ppVertexAndPolygonBufferOut = 0L;
		return ;
	}

	pObj->m_pTriangleBase	= new InternalTriangle[ _maxNbTriangle ];
	if( !(pObj->m_pTriangleBase) )
	{
		delete pObj->m_pVertexBase;
		delete pObj;
		*_ppVertexAndPolygonBufferOut = 0L;
		return ;
	}
	pObj->m_MaximumNumberOfTriangles = _maxNbTriangle;
	pObj->m_MaximumNumberOfVertexes = _maxNbVertex;
	pObj->m_NumberOfActiveTriangles = 0;
	pObj->m_FirstActiveTriangleIndex = 0;
	// at start, object is dynamic, and  not compiled:
	pObj->m_CompiledObjectHandler = 0;
	// set vertex to white untransparent by default.
	InternalVertex *pVert = pObj->m_pVertexBase;
	unsigned int ii;
	const float v1=1.0f;
	for(ii=0 ; ii<_maxNbVertex ; ii++)
	{
		float *pRGBA = &(pVert->m_ColorRGBA[0]);
		*(pRGBA++) = v1;
		*(pRGBA++) = v1;
		*(pRGBA++) = v1;
		*(pRGBA++) = v1;
		pVert++;
	}
	pObj->m_ExtraMemberFlags = _extraMemberFlags;
	pObj->SetColor(1.0f,1.0f,1.0f,1.0f);
}
/*!
	\brief	A call to CompileAsStatic() will fix the current geometry of an object.
			It will try to use the 3D buffer compilation of a given machine, which can fasten the drawing.
			From that moment on, further modification to its shape will be useless.
*/
void	OGLMachine::OGLInternalObject3DBuffer::CompileAsStatic( )
{
	//TODO
	m_CompiledObjectHandler = 1;
}
/*!
	\brief	return true is CompileAsStatic() were used.
	\return	 true is CompileAsStatic() were used, false if still dynamic.
*/
bool	OGLMachine::OGLInternalObject3DBuffer::IsCompiledAsStatic()
{
	return (m_CompiledObjectHandler!=0L);
}
/*
	\brief		Destroy objects previously created with NewObject3DBuffer()
	\param		_ppBufferToDestroy object to destroy, and set to 0L.
*/
void	OGLMachine::DeleteObject3DBuffer(  InternalObject3DBuffer **_ppBufferToDestroy )
{
	OGLInternalObject3DBuffer *pObj = (OGLInternalObject3DBuffer *) *_ppBufferToDestroy; 
	if( !pObj ) return;
		delete [] (pObj->m_pTriangleBase) ;
		delete [] (pObj->m_pVertexBase) ;
	delete pObj;

	*_ppBufferToDestroy = 0L;
}

void	OGLMachine::NewTexture(  InternalTexture **_ppTextureOut  )
{
	OGLInternalTexture *pTexture = new OGLInternalTexture;
	*_ppTextureOut = pTexture ;
	//if( !pTexture ) return; // failed if NULL.
}

#ifdef _ENGINE_EDITABLE_
void	OGLMachine::DeleteTexture(  InternalTexture **_ppTextureToDestroy )
{
	OGLInternalTexture *pTexture = (OGLInternalTexture *) *_ppTextureToDestroy;
	if( !pTexture ) return;

	if( pTexture->m_Layer[0].m_OGLTextureHandler ) glDeleteTextures(2, &(pTexture->m_Layer[0].m_OGLTextureHandler) );

	delete pTexture;
	*_ppTextureToDestroy = 0L;

}
#endif
/*!
	\brief	Constructor. 
	\param	_pFather the father, or NULL if new root.
	\param	_pCreator the machine upon that manage the VP.
*/
OGLMachine::OGLInternalViewPort::OGLInternalViewPort(OGLInternalViewPort *_pFather, OGLMachine *_pCreator)
: m_pOGLMachine(_pCreator)
//OGLMachine::OGLInternalViewPort::OGLInternalViewPort(OGLMachine::OGLInternalViewPort *_pFather)
{
	m_ppClippingHierarchyAttached = 0L; // we are not attached.
	m_pClippingNextBrotherViewPort = 0L; // default value.
	m_pClippingFirstSonViewPort = 0L;
	m_pClippingFatherViewPort = _pFather; // can be 0L.
	const float v0 = 0.0f;
	const float v1 = 1.0f;
	if(_pFather)
	{
		m_pRootViewPort = _pFather->GetRootViewPort(); 

		m_pClippingNextBrotherViewPort = _pFather->m_pClippingFirstSonViewPort;
		//tell brother he is attached to us:
		if(m_pClippingNextBrotherViewPort) 
			((OGLInternalViewPort *)m_pClippingNextBrotherViewPort)->m_ppClippingHierarchyAttached = &m_pClippingNextBrotherViewPort;

		m_ppClippingHierarchyAttached = &(_pFather->m_pClippingFirstSonViewPort);
		_pFather->m_pClippingFirstSonViewPort = this;

		SetClippingCoordinates(v0,v0,v1,v1);
		SetScaleCoordinates(v0,v0,v1,v1);
	}
	else // root case:
	{
		m_ClippingRootX1 =
		m_ClippingRootY1 =
		m_ScaleRootX1 =
		m_ScaleRootY1 = 
		m_ScaleX1 =
		m_ScaleY1 =
		m_ClippingX1 = 
		m_ClippingY1 = v0;
		m_ClippingRootX2 =
		m_ClippingRootY2 =
		m_ScaleRootX2 =
		m_ScaleRootY2 = 
		m_ScaleX2 =
		m_ScaleY2 =
		m_ClippingX2 = 
		m_ClippingY2 = v1;
		m_pRootViewPort = this; // declared as root itself.
	}
	m_NearCutPlane = 0.25f;
	m_FarCutPlane = 1024.0f;
	m_fov = 0.5f;
	m_pRenderTexture = 0L;
	m_Valid = true;
	ResetPixelCoordinates();
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Destructor. See DeleteViewport() 
*/
OGLMachine::OGLInternalViewPort::~OGLInternalViewPort()
{
	// detach sons, leaving invalid viewports due to this destruction:
	ClippingDetach();

}
#endif
#ifdef _ENGINE_EDITABLE_
/*
	\brief	tool: invalidate down the viewport hierachy when a viewport is destroyed.
*/
void	OGLMachine::OGLInternalViewPort::ClippingDetach()
{
	// detach from father or upper bother, but keep the brother chain.
	if(m_ppClippingHierarchyAttached)
	{
		*m_ppClippingHierarchyAttached = m_pClippingNextBrotherViewPort;
		if(m_pClippingNextBrotherViewPort)
			((OGLInternalViewPort*)m_pClippingNextBrotherViewPort)->m_ppClippingHierarchyAttached = m_ppClippingHierarchyAttached;
	}
	m_ppClippingHierarchyAttached = 0L ;
	m_pClippingNextBrotherViewPort = 0L; // we are no more in a brother chain.
	m_pRootViewPort = 0L;
	// all sons are detached.
	OGLInternalViewPort *pSon = (OGLInternalViewPort *)m_pClippingFirstSonViewPort ;
	OGLInternalViewPort *pNext ;
	while(pSon)
	{	
		pNext =(OGLInternalViewPort *)( pSon->m_pClippingNextBrotherViewPort);
		pSon->ClippingDetach();
		pSon = pNext;
	}
}
#endif
/*
	\brief	tool: attach to a son or brother , or detach if _ppPointerWhereToAttach null.
	\param	_ppPointerWhereToAttach pointer to a pointer that point this or 0L to detach.
*//*
void	ClippingHierarchy_AttachToChain( InternalViewPort	**_ppPointerWhereToAttach=0L )
{
	// if previous attach, we detach from this bro chain:
	if( m_ppClippingHierarchyAttached)
	{
		*m_ppClippingHierarchyAttached = 0L ; // can be null.
	}
	// if we detach or attach to new:
	m_ppClippingHierarchyAttached = _ppPointerWhereToAttach; // can be null.
	// if we attach to new:
	if(_ppPointerWhereToAttach)
	{
		*_ppPointerWhereToAttach = this;
	}
}*/
/*
	\brief	Change the coodinates of the clipping borders of a ViewPort,
			in the origin of its father.default is 0.0,0.0,1.0,1.0
			If the ViewPort is a Root ViewPort, like the main screen,
			or a texture rendering, it is inactive.
	\param	_ClippingX1 upperleft corner in [0,1] scale for visible bounds.
	\param	_ClippingY1 upperleft corner in [0,1] scale for visible bounds.
	\param	_ClippingX2 downright corner in [0,1] scale for visible bounds.
	\param	_ClippingY2 downright corner in [0,1] scale for visible bounds.
*/
void OGLMachine::OGLInternalViewPort::SetClippingCoordinates(float _ClippingX1,float _ClippingY1,float _ClippingX2,float _ClippingY2 )
{
	if( !m_pClippingFatherViewPort ) return;
	// no action if same coords.
	//OPTIM !!! do it with some or.
	if( m_ClippingX1 == _ClippingX1 && 
		m_ClippingY1 == _ClippingY1 &&
		m_ClippingX2 == _ClippingX2 &&
		m_ClippingY2 == _ClippingY2				
		) return;
	m_ClippingX1 = _ClippingX1;
	m_ClippingY1 = _ClippingY1;
	m_ClippingX2 = _ClippingX2;
	m_ClippingY2 = _ClippingY2;

	ResetPixelCoordinates();
}
/*
	\brief	Change the coodinates of the draw scale of a ViewPort, in the origin 
			of its father. default is 0.0,0.0,1.0,1.0
			If the ViewPort is a Root ViewPort, like the main screen,
			or a texture rendering, it is inactive.
	\param	_ScaleX1 upperleft corner in [0,1] scale for aspect ratio .
	\param	_ScaleY1 upperleft corner in [0,1] scale for aspect ratio .
	\param	_ScaleX2 downright corner in [0,1] scale for aspect ratio .
	\param	_ScaleY2 downright corner in [0,1] scale for aspect ratio .
*/
void OGLMachine::OGLInternalViewPort::SetScaleCoordinates(float _ScaleX1,float _ScaleY1,float _ScaleX2,float _ScaleY2 )
{
	// inactive method for all root screen.
	if( !m_pClippingFatherViewPort) return;
	// no action if same coords.
	//OPTIM !!! do it with some or.
	if( m_ScaleX1 == _ScaleX1 && 
		m_ScaleY1 == _ScaleY1 &&
		m_ScaleX2 == _ScaleX2 &&
		m_ScaleY2 == _ScaleY2
		) return;

	m_ScaleX1 = _ScaleX1;
	m_ScaleY1 = _ScaleY1;
	m_ScaleX2 = _ScaleX2;
	m_ScaleY2 = _ScaleY2;

	ResetPixelCoordinates();
}

/*!
	\brief	Change the root viewport pixel resolution.This is a low level call in case of resize or screen init.		
	\param	_pixelWidth 
	\param	_pixelHeight
*/	
void	OGLMachine::OGLInternalViewPort::ResetRootPixelSize( unsigned int _pixelWidth,unsigned int _pixelHeight )
{
	OGLInternalViewPort *pRoot = (OGLInternalViewPort *)m_pRootViewPort;
	pRoot->m_pixel_x1 = 0;
	pRoot->m_pixel_y1 = 0;
	pRoot->m_pixel_x2 = _pixelWidth;
	pRoot->m_pixel_y2 = _pixelHeight;
	pRoot->ResetPixelCoordinates();
}

/*
	\brief	Change the coodinates of the ViewPort pixels, in its root origin.
			Used by SetCoordinates(). non-virtual.
*/
void	OGLMachine::OGLInternalViewPort::ResetPixelCoordinates()
{
	// in term of ratio in the root rectangle scale.
	// m_pFatherViewPort's m_ClippingRoot members must be updated.
	register OGLInternalViewPort *pOGLFather = (OGLInternalViewPort *)m_pClippingFatherViewPort;
	register OGLInternalViewPort *pOGLRoot = (OGLInternalViewPort *)m_pRootViewPort;
	m_Valid = true; // default.

	// if father invalid, all sons invalid:
	if(pOGLFather && !pOGLFather->m_Valid) m_Valid = false;
	
	if(pOGLFather && pOGLFather->m_Valid)
	{			
		// the Scale is given in the scale of the father's POSITION:
		m_ScaleRootX1 = pOGLFather->m_ScaleRootX1 + 
			(pOGLFather->m_ScaleRootX2-pOGLFather->m_ScaleRootX1) * m_ScaleX1;
		m_ScaleRootY1 = pOGLFather->m_ScaleRootY1 + 
			(pOGLFather->m_ScaleRootY2-pOGLFather->m_ScaleRootY1) * m_ScaleY1;
		m_ScaleRootX2 = pOGLFather->m_ScaleRootX1 + 
			(pOGLFather->m_ScaleRootX2-pOGLFather->m_ScaleRootX1) * m_ScaleX2;
		m_ScaleRootY2 = pOGLFather->m_ScaleRootY1 + 
			(pOGLFather->m_ScaleRootY2-pOGLFather->m_ScaleRootY1) * m_ScaleY2;

		// the clipping is given in the scale of the father's POSITION:
		m_ClippingRootX1 = pOGLFather->m_ScaleRootX1 + 
			(pOGLFather->m_ScaleRootX2-pOGLFather->m_ScaleRootX1) * m_ClippingX1;
		m_ClippingRootY1 = pOGLFather->m_ScaleRootY1 + 
			(pOGLFather->m_ScaleRootY2-pOGLFather->m_ScaleRootY1) * m_ClippingY1;
		m_ClippingRootX2 = pOGLFather->m_ScaleRootX1 + 
			(pOGLFather->m_ScaleRootX2-pOGLFather->m_ScaleRootX1) * m_ClippingX2;
		m_ClippingRootY2 = pOGLFather->m_ScaleRootY1 + 
			(pOGLFather->m_ScaleRootY2-pOGLFather->m_ScaleRootY1) * m_ClippingY2;
		// clipping rectangle can be invalid.
		
		if( m_ClippingRootX1>pOGLFather->m_ClippingRootX2 ||
			m_ClippingRootX2<pOGLFather->m_ClippingRootX1 ||
			m_ClippingRootY1>pOGLFather->m_ClippingRootY2 ||
			m_ClippingRootY2<pOGLFather->m_ClippingRootY1 
			) m_Valid = false;
		else
		{
			
			//... and the clipping is itself clipped by the father's clipping
			if(m_ClippingRootX1<pOGLFather->m_ClippingRootX1)
				m_ClippingRootX1 = pOGLFather->m_ClippingRootX1;
			if(m_ClippingRootY1<pOGLFather->m_ClippingRootY1)
				m_ClippingRootY1 = pOGLFather->m_ClippingRootY1;
			if(m_ClippingRootX2>pOGLFather->m_ClippingRootX2)
				m_ClippingRootX2 = pOGLFather->m_ClippingRootX2;
			if(m_ClippingRootY2>pOGLFather->m_ClippingRootY2)
				m_ClippingRootY2 = pOGLFather->m_ClippingRootY2;

			// in term of pixel in the root rectangle bitmap:
			float	rootpixelwidth =(float)(pOGLRoot->m_pixel_x2-pOGLRoot->m_pixel_x1) ;
			float	rootpixelheight =(float)(pOGLRoot->m_pixel_y2-pOGLRoot->m_pixel_y1) ;
			m_pixel_x1 = pOGLRoot->m_pixel_x1 + (int)(rootpixelwidth*m_ClippingRootX1);
			m_pixel_y1 = pOGLRoot->m_pixel_y1 + (int)(rootpixelheight*m_ClippingRootY1);
			m_pixel_x2 = pOGLRoot->m_pixel_x1 + (int)(rootpixelwidth*m_ClippingRootX2);
			m_pixel_y2 = pOGLRoot->m_pixel_y1 + (int)(rootpixelheight*m_ClippingRootY2);
		}
	}
	// recursive calls:
	register OGLInternalViewPort *pOGLSon = (OGLInternalViewPort *) m_pClippingFirstSonViewPort;
	while(pOGLSon)
	{
		((OGLInternalViewPort*)pOGLSon)->ResetPixelCoordinates();
		pOGLSon = (OGLInternalViewPort *)(pOGLSon->m_pClippingNextBrotherViewPort) ;
	}
	
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
void	OGLMachine::NewViewPortChild(  InternalViewPort **_ppViewPortOut,InternalViewPort *_pFatherViewPort)
{
	*_ppViewPortOut = 0L; 
	if(!_pFatherViewPort) _pFatherViewPort = GetDefaultViewPort();
	*_ppViewPortOut = new OGLInternalViewPort((OGLInternalViewPort *)_pFatherViewPort,this) ;
}

/*!
	\brief	Ask the Creation of a new Root InternalViewPort, that will render on a texture image.
			specified by _pRootTexture. You got to create a texture first for parameter _pRootTexture.
			In edition mode, each InternalViewPort has to be destroyed with DeleteViewPort().
	\param	_ppViewPortOut	the object returned, or 0L if failed.(no mem ?)
	\param	_pRootTexture an InternalTexture. Can't be null.
	\param	_np2PixelWidth	integer which can be 1,2,4,8,16,32,64,128,256,512 that stand for the square of the pixel width.
	\param	_np2PixelHeight	integer which can be 1,2,4,8,16,32,64,128,256,512 that stand for the square of the pixel height.
	\param	_layerToAffect	the texture layer (simple mapping, environment) where  to render.
*/
void	OGLMachine::NewViewPortOnTexture(  InternalViewPort **_ppViewPortOut,
									InternalTexture *_pRootTexture,
									unsigned int _np2PixelWidth,
									unsigned int _np2PixelHeight,
									InternalTexture::TextureImageLayerEnum _layerToAffect)
{
	*_ppViewPortOut = 0L; 
	// null if no texture:
	if(!_pRootTexture) return;
	OGLInternalViewPort *pNewVP;
	*_ppViewPortOut = pNewVP = new OGLInternalViewPort(0L,this); // create new root.
	pNewVP->m_pRenderTexture = (OGLInternalTexture *)_pRootTexture;
	int renderBuffer_Width  = 1<<_np2PixelWidth;
	int renderBuffer_Height = 1<<_np2PixelHeight;
	int	glTextureHandle = ((OGLInternalTexture *)_pRootTexture)->m_Layer[_layerToAffect].m_OGLTextureHandler ;
									
	pNewVP->m_RenderTextureLayer = (int)_layerToAffect;
	// ------ in case of render object extension:
	if(m_RenderObjectExtension)
	{
		GLuint frameBuffer,depthRenderBuffer;
		glGenFramebuffersEXT( 1, &frameBuffer );
		((OGLInternalTexture *)_pRootTexture)->m_frameBuffer = frameBuffer;
		glGenRenderbuffersEXT( 1, &depthRenderBuffer  );
		((OGLInternalTexture *)_pRootTexture)->m_depthRenderBuffer = depthRenderBuffer;

		// Initialize the render-buffer for usage as a depth buffer.
		// We don't really need this to render things into the frame-buffer object,
		// but without it the geometry will not be sorted properly.
		glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, depthRenderBuffer );
		glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, renderBuffer_Width, renderBuffer_Height );
		// Check for errors ?
		//GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
		// GL_FRAMEBUFFER_UNSUPPORTED_EXT
	} 
	// for both cases:
	// the gl texture to be bind is the mapping one:
	// it is possible to reset its bitmap size with 0L as a bitmap:
	glBindTexture(GL_TEXTURE_2D, glTextureHandle);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 
		        renderBuffer_Width, renderBuffer_Height, 
		        0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
	pNewVP->m_pRenderTexture->m_ActiveLayerFlag |= (1<<(int)_layerToAffect);

	// ---- AND THEN, classic root rectangle init.
	// we set the size as the texture size.
	pNewVP->ResetRootPixelSize(renderBuffer_Width,renderBuffer_Height);

}

/*!
	\brief	Destroy any objects previously created with NewViewPortChild() or NewViewPortOnTexture().
	\param	_ppViewPortToDestroy	 pointer to object to destroy, and set to 0L.
*/
void	OGLMachine::DeleteViewPort(  InternalViewPort **_ppViewPortToDestroy )
{
	if( !(*_ppViewPortToDestroy) ) return;	
	OGLInternalViewPort	*pViewport = (OGLInternalViewPort *) *_ppViewPortToDestroy ;
	/*
 	m_pClippingNextBrotherViewPort = 0L; // default value.
	m_pClippingFirstSonViewPort = 0L;
	m_pClippingFatherViewPort = _pFather; // can be 0L.
 */
	// important: if it was a render to texture viewport, destroy framebuffers:
	if( pViewport->m_pRenderTexture
		)
	{
		if(pViewport->m_pRenderTexture->m_frameBuffer )
			glDeleteFramebuffersEXT( 1, &(pViewport->m_pRenderTexture->m_frameBuffer) );
		if( pViewport->m_pRenderTexture->m_depthRenderBuffer )
			glDeleteRenderbuffersEXT(1,&(pViewport->m_pRenderTexture->m_depthRenderBuffer) );

	}
	if( pViewport )
	{
		//InternalViewPort	*pfather = pViewport->m_pClippingFatherViewPort;
		delete pViewport ;
	}
	*_ppViewPortToDestroy = 0L;
}
/*!
	\brief	Swap buffer so that what was drawn on the viewport is shown.
			this is needed to active all Clear(), RenderMesh() ,...
			It has to be done with main screen viewport like texture-rendering
			viewports.
*/
void	OGLMachine::OGLInternalViewPort::SwapBuffer()
{
	// in all cases, only root viewport should be switched:
	OGLInternalViewPort *pRoot=(OGLInternalViewPort *)m_pRootViewPort;
	if(pRoot->m_pRenderTexture)
	{	// render to texture case:
		OGLMachine::OGLInternalTexture *pTexture = pRoot->m_pRenderTexture;
		int safeY=0;
		glBindTexture( GL_TEXTURE_2D, 
			pTexture->m_Layer[pRoot->m_RenderTextureLayer].m_OGLTextureHandler );
		glTexEnvi(  GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE,
						GL_MODULATE
						);
		glPixelZoom(1.0f,1.0f);
		glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 0, safeY, 
							pRoot->m_pixel_x2, pRoot->m_pixel_y2,
						0); // border
		
	}else
	{	// for the main screen viewport:
		m_pOGLMachine->SwapMainScreenBuffer();
	}

}
/*!
	\brief	Reset color to black (or given color),and depth buffer, of this ViewPort.
	\param	_red [0.0,1.0]
	\param	_green [0.0,1.0]
	\param	_blue [0.0,1.0]
*/
void	OGLMachine::OGLInternalViewPort::Clear( float _red,float _green,float _blue)
{
	if(!m_Valid) return; // undrawable viewport.
	SetGLToViewPort();
	glDepthMask( GL_TRUE ); // needed to flush depth buffer.
	glEnable(GL_SCISSOR_TEST ); // scissor cut is needed for clearing

	glScissor( GetPixelX1(),
				((OGLInternalViewPort*)GetRootViewPort())->GetPixelY2()-GetPixelY2(),
				GetPixelX2() - GetPixelX1(),
				GetPixelY2() - GetPixelY1());
	glClearColor( _red, _green, _blue,1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	
	glDisable(GL_SCISSOR_TEST );
}
/*!
	\brief	Render a 3D Object. The vertexes are transformed with matrix _pMatrix,
			the object shape is defined by _pMeshBuffer, the texture of the object is
			defined by _pTexture, the ViewPort that define the rectangle where to draw,
			and the camera parameters is _pViewPort, or the default viewport if not specified.	
			Note drawings on the main screen needs a SwapBuffer() to be shown at each frames.
	\param	_pMatrix Internal3DMatrixStack
	\param	_pMeshBuffer InternalObject3DBuffer
	\param	_pTexture InternalTexture
*/
void	OGLMachine::OGLInternalViewPort::RenderMesh(
							InternalObject3DBuffer *_pMeshBuffer,
							InternalTexture *_pTexture
							)
{
	if(!m_Valid) return; // undrawable viewport.
	if(!_pMeshBuffer) return; // this test is better here than everywhere.
	// ----- PRE: in case of render-to-texture: set render screen ---- (pre, and post at end)
	OGLMachine::OGLInternalTexture *renderTexture = ((OGLInternalViewPort *)GetRootViewPort())->m_pRenderTexture;
	if( renderTexture )
	{
		if( renderTexture->m_frameBuffer )  // means render extension is active and texture inited.
		{	
			glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_pRenderTexture->m_frameBuffer );
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
											GL_TEXTURE_2D, 
renderTexture->m_Layer[((OGLInternalViewPort *)GetRootViewPort())->m_RenderTextureLayer].m_OGLTextureHandler
											, 0 );
			glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
												GL_RENDERBUFFER_EXT, 
												m_pRenderTexture->m_depthRenderBuffer );
			// end then , a glViewport must be done...->SetGLToViewPort()
		} else
		{	// GL1.1 texture copy method.
			// without framebuffer extensions, we must
			// render on a screen part and use glCopy...
			// note: does nothing, SetGLToViewPort() already affect
			// a sub screen with correct dimension on the main screen.
		}
	}
	// -----
	// set viewport rectangle IN the render screen, and FOV.
	SetGLToViewPort();
	// clientelise vertex lists:
	unsigned int objectFlags =  ((OGLInternalObject3DBuffer *)_pMeshBuffer)->m_ExtraMemberFlags ;
	InternalVertex *pFirstVertex = ((OGLInternalObject3DBuffer *)_pMeshBuffer)->GetFirstVertex();
	glVertexPointer(    3, GL_FLOAT , sizeof( InternalVertex ),	&(pFirstVertex->m_x) );
	if(objectFlags & bOb3D_VertexNormal )
	{
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer(    GL_FLOAT , sizeof( InternalVertex ),	&(pFirstVertex->m_nx) );
	} else
	{
		glDisableClientState( GL_NORMAL_ARRAY );
	}
	if(objectFlags & bOb3D_VertexUVMapping )
	{
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(  2, GL_FLOAT , sizeof( InternalVertex  ), &(pFirstVertex->m_u) );
	} else
	{
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}
	if(objectFlags & bOb3D_VertexRGBA )
	{
		glEnableClientState( GL_COLOR_ARRAY );
		glColorPointer(  4,/* GL_UNSIGNED_BYTE*/GL_FLOAT , sizeof( InternalVertex ), &(pFirstVertex->m_ColorRGBA[0]) );	
	}
	else
	{
		glDisableClientState( GL_COLOR_ARRAY );
		float	*pObjectColor=&(((OGLInternalObject3DBuffer *)_pMeshBuffer)->m_Color[0]);
		float	rgba[4];
		rgba[0] = *pObjectColor++;
		rgba[1] = *pObjectColor++;
		rgba[2] = *pObjectColor++;
		rgba[3] = *pObjectColor;
		OGLInternalTexture *pOGLTexture =(OGLInternalTexture *)_pTexture;
		if(pOGLTexture)
		{	// multiply texture base color:
			pObjectColor = &(pOGLTexture->m_BaseColorRGBA[0]);
			rgba[0] *= *pObjectColor++;
			rgba[1] *= *pObjectColor++;
			rgba[2] *= *pObjectColor++;
			rgba[3] *= *pObjectColor;
		}
		glColor4fv( &(rgba[0]) );
	}
	unsigned int txt=0,txtenv=0,activeLayerflag=0,renderflag=
		InternalTexture::itf_Filtered|
		InternalTexture::itf_depthwrite|
		InternalTexture::itf_depthtest; // default flags if no texture.
	if(_pTexture)
	{
		OGLInternalTexture *pOGLTexture =(OGLInternalTexture *)_pTexture;
		activeLayerflag =	pOGLTexture->m_ActiveLayerFlag;
		txt =				pOGLTexture->m_Layer[0].m_OGLTextureHandler ;
		txtenv =			pOGLTexture->m_Layer[1].m_OGLTextureHandler ;
		renderflag = pOGLTexture->m_flags ;
	}
	// ---------- Set common texture states: (before glBind...)
	int ii,jj;
	if(renderflag & InternalTexture::itf_doublesided)
		glDisable( GL_CULL_FACE );
	else glEnable( GL_CULL_FACE );
	//  itf_nodepthwrite itf_nodepthtest
	if( renderflag & InternalTexture::itf_depthwrite  ) // with sorting, bring to perfect transparence:
			glDepthMask( GL_TRUE  );
		else
			glDepthMask( GL_FALSE  );

	ii = GL_LEQUAL ;
	if( (renderflag & InternalTexture::itf_depthtest )==0 ) // with sorting, bring to perfect transparence:
			ii = GL_ALWAYS ;
	glDepthFunc(ii);	
	// itf_Additive background
	jj = GL_SRC_ALPHA ;
	if( (renderflag & InternalTexture::itf_Additive) == 0 )
	{	ii= GL_ONE_MINUS_SRC_ALPHA;	//multiplicative
		if( (renderflag & InternalTexture::itf_substractive) != 0 )
		{	jj =  GL_ZERO;
			ii =  GL_ONE_MINUS_SRC_ALPHA;}
	} else
	{	ii= GL_ONE; // additive
	}
	glBlendFunc( jj, ii );
	// -------------- process by case  ------------------
	if( (activeLayerflag & 1) != 0  ) 
	{
		if( (activeLayerflag & 2) ==0)
		{
			//---------------------------------- simple mapping:
			glBindTexture( GL_TEXTURE_2D ,txtenv );
			glDisable( GL_TEXTURE_GEN_S ); // -> needed because of other cases.
			glDisable( GL_TEXTURE_GEN_T );	// on radeon8500,  need 2 disable in some cases!
			glBindTexture( GL_TEXTURE_2D ,txt );
			glDisable( GL_TEXTURE_GEN_S ); // -> needed because of other cases.
			glDisable( GL_TEXTURE_GEN_T );
	
			//  itf_Filtered
			if( (renderflag & InternalTexture::itf_Filtered ) )
				jj = GL_LINEAR ;    // scale linearly
			else	jj = GL_NEAREST ;
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, jj );
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, jj );
			//  mapping coordinates clamp pref:
			if( (renderflag & InternalTexture::itf_ClampTxtU ) != 0 )
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP );
			else
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT );
			if( (renderflag & InternalTexture::itf_ClampTxtV ) != 0 )
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP );
			else
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT );
			//---- draw dynamic object for mapping/color pass:
			glDrawElements( GL_TRIANGLES , ((OGLInternalObject3DBuffer *)_pMeshBuffer)->m_NumberOfActiveTriangles * 3 ,
				GL_UNSIGNED_INT, ((OGLInternalObject3DBuffer *)_pMeshBuffer)->GetFirstTriangle()
							);
		} else
		{
			if( ! OGLMachine::glActiveTextureARB ) return; // if extension not inited !!!
			//-------------------------- simple mapping + environment:
			// txt -> Texture Unit 0
			//orig pOGLTexture->SetRenderFlags(pOGLTexture->m_flags );
			glActiveTextureARB( GL_TEXTURE0_ARB );
			glClientActiveTextureARB( GL_TEXTURE0_ARB );
			glBindTexture( GL_TEXTURE_2D , txt );
			glEnable( GL_TEXTURE_2D );
			glDisable( GL_TEXTURE_GEN_S );
			glDisable( GL_TEXTURE_GEN_T );
				
			if( (renderflag & InternalTexture::itf_Filtered ) )
				jj = GL_LINEAR ;    // scale linearly
			else	jj = GL_NEAREST ;
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, jj );
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, jj );
			//  mapping coordinates clamp pref:
			if( (renderflag & InternalTexture::itf_ClampTxtU ) != 0 )
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP );
			else
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT );
			if( (renderflag & InternalTexture::itf_ClampTxtV ) != 0 )
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP );
			else
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT );

			//Active la Texture Unit 1, active le placage de texture pour cette TU
			glActiveTextureARB( GL_TEXTURE1_ARB );
			glClientActiveTextureARB( GL_TEXTURE1_ARB );
	
				glBindTexture( GL_TEXTURE_2D, txtenv );
				glEnable( GL_TEXTURE_2D );					
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, jj );
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, jj );
				glEnable( GL_BLEND ); //optim ?
				//glNormalPointer(    GL_FLOAT , sizeof( InternalVertex ),	&(pFirstVertex->m_nx) );
				glEnable( GL_TEXTURE_GEN_S );
				glEnable( GL_TEXTURE_GEN_T );
				glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
				glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
				//Configure le blending des 2 textures
				if( renderflag & InternalTexture::itf_envAdd  )
				{
					glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_ADD);
				} else
				{
					glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
				/*	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE_EXT);
					glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_EXT,GL_INTERPOLATE_EXT);
					glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB_EXT,GL_PREVIOUS_EXT);
					glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_EXT,GL_SRC_COLOR);
					glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB_EXT,GL_TEXTURE);
					glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_EXT,GL_SRC_COLOR);
					glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB_EXT,GL_TEXTURE);
					glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND2_RGB_EXT,GL_SRC_ALPHA);*/

				}
		/*	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE_EXT);
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_EXT,GL_INTERPOLATE_EXT);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB_EXT,GL_PREVIOUS_EXT);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_EXT,GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB_EXT,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_EXT,GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB_EXT,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND2_RGB_EXT,GL_SRC_ALPHA);
*/			/*glTexEnvi(  GL_TEXTURE_ENV ,
                    GL_TEXTURE_ENV_MODE,
                    //GL_REPLACE    // only texture  DEFAULT.
                    //GL_DECAL      // image -alphaimage sur couleur
                    GL_ADD   // as blend ? -> WORK ON LINUX/WIN/AMIGASTORMMESA
                    //GL_BLEND
                );*/
	/* glTexEnvi(  GL_TEXTURE_ENV ,
                    GL_TEXTURE_ENV_MODE,
                    //GL_REPLACE    // only texture  DEFAULT.
                    //GL_DECAL      // image -alphaimage sur couleur
                    GL_ADD   // as blend ? -> WORK ON LINUX/WIN/AMIGASTORMMESA
                    //GL_BLEND
                );	*/
			//Affichage
			glDrawElements( GL_TRIANGLES , ((OGLInternalObject3DBuffer *)_pMeshBuffer)->m_NumberOfActiveTriangles * 3 ,
				GL_UNSIGNED_INT, ((OGLInternalObject3DBuffer *)_pMeshBuffer)->GetFirstTriangle()
							);
			//Desactivate TU 1
			glDisable( GL_TEXTURE_2D );
			//glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			glActiveTextureARB( GL_TEXTURE0_ARB );
			glClientActiveTextureARB( GL_TEXTURE0_ARB );
		}
	} else
	{
		if( activeLayerflag & 2)
		{	// environnement:
			glBindTexture( GL_TEXTURE_2D ,txtenv );
			if( (renderflag & InternalTexture::itf_Filtered ) )
				jj = GL_LINEAR ;    // scale linearly
			else	jj = GL_NEAREST ;
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, jj );
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, jj );
			glEnable( GL_TEXTURE_GEN_S );
			glEnable( GL_TEXTURE_GEN_T );
			//---- draw dynamic object for mapping/color pass:
			glDrawElements( GL_TRIANGLES , ((OGLInternalObject3DBuffer *)_pMeshBuffer)->m_NumberOfActiveTriangles * 3 ,
				GL_UNSIGNED_INT, ((OGLInternalObject3DBuffer *)_pMeshBuffer)->GetFirstTriangle()
							);
		} else
		{ // no bitmap case:
			//re? glBindTexture( GL_TEXTURE_2D , 0 );
			glBindTexture( GL_TEXTURE_2D ,txt );
			glDisable( GL_TEXTURE_2D );
			//---- draw dynamic object for mapping/color pass:
			glDrawElements( GL_TRIANGLES , ((OGLInternalObject3DBuffer *)_pMeshBuffer)->m_NumberOfActiveTriangles * 3 ,
				GL_UNSIGNED_INT, ((OGLInternalObject3DBuffer *)_pMeshBuffer)->GetFirstTriangle()
							);
			glEnable( GL_TEXTURE_2D );
		}
	}
	// ---------- POST: in case of render-to-texture, set normal rendering back:
	if( ((OGLInternalViewPort *)GetRootViewPort())->m_pRenderTexture )
	{
		if(((OGLInternalViewPort *)GetRootViewPort())->m_pRenderTexture->m_frameBuffer) // means render extension is active and texture inited.
		{	
			// Unbind the frame-buffer and render-buffer objects.
			glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

		} else
		{	// GL1.1 texture copy method for render-to-texture.
			// does nothing, 
		}
	}
	// -----

}
/*!
	\brief	Render directly a 32bit pixel bitmap image from memory on a viewport.
	\param	_prgbaTable	pointer to the beginning of a 32bit RGBA table.
	\param	_PixelWidth number of pixel of a line
	\param	_PixelHeight number of pixel lines.
*/
void	OGLMachine::OGLInternalViewPort::DrawRGBARectangle( 
							unsigned char	*_prgbaTable,
							unsigned int	_PixelWidth,
							unsigned int	_PixelHeight
							)
{
	if(!m_Valid) return; // undrawable viewport.
	int	px1 = GetPixelX1() ;
	int	py1 = GetPixelY1() ;
	int	px2 = GetPixelX2() ;
	int	py2 = GetPixelY2() ;
	// set viewport and projection to to scale the rectangle.
	glViewport( px1,
				((OGLInternalViewPort *)GetRootViewPort())->GetPixelY2() - py2,
				px2 - px1,
				py2 - py1 );
	//re? glDisable( GL_SCISSOR_TEST );
	
/*	glScissor(  px1,
				((OGLInternalViewPort*)GetRootViewPort())->GetPixelY2()-py2,
				px2 - px1,
				py2 - py1 );
*/
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();	
	/*glOrtho( (GetPositionX1()-0.5f ),(GetPositionX2()-0.5f),
			(GetPositionY1()-0.5f) ,(GetPositionY2()-0.5f),
			-1.0f, 1.0f);*/
/*	glOrtho( (GetPositionX1()-0.5f ),(GetPositionX2()-0.5f),
			(GetPositionY1()-0.5f) ,(GetPositionY2()-0.5f),
			-1.0f, 1.0f);*/
	glOrtho( -0.5f,0.5f,
			-0.5f ,0.5f,
			-1.0f, 1.0f);
		
	/*const float	fnear = 0.5f;
	const float	fov = 0.5f ;
	
	glFrustum(
                (GetPositionX1()-0.5f )*fnear ,(GetPositionX2()-0.5f)*fnear,		// left , right
                (GetPositionY1()-0.5f) *fnear,(GetPositionY2()-0.5f)*fnear,		// botton, top
               fov * fnear ,1024.0f	//ffov * fnear, ffar  // near, far.
                );*/
/*	glFrustum(
				-0.5f*fnear ,
				0.5f*fnear,		// left , right
                -0.5f*fnear,
				0.5f*fnear,		// botton, top
               fov * fnear ,1024.0f	//ffov * fnear, ffar  // near, far.
                );*/
	glMatrixMode( GL_MODELVIEW ); // back to classic default state.
	glLoadIdentity();

/* old 

	glRasterPos3f(	-1.0f+ _pViewPort->GetClippingX1()*2.0f,
					1.0f- _pViewPort->GetClippingY1()*2.0f,
					-1.0f);	// is moved by GL_PROJECTION.

	float zx = (((float)( px2 -  px1)) * (_pViewPort->GetClippingX2()-_pViewPort->GetClippingX1() ) )
					/(float)_PixelWidth ;
	float zy =	-(((float)( py2 -  py1)) * (_pViewPort->GetClippingY2()-_pViewPort->GetClippingY1() ) )
				/(float)_PixelHeight ;
	glPixelZoom(zx,zy);
	*/
	/*glRasterPos3f(	-1.0f,
					1.0f,
					-1.0f);	// is moved by GL_PROJECTION.
					*/
//	float cenx= (GetPositionX1()+GetPositionX2())*0.5f;
//	float ceny= (GetPositionY1()+GetPositionY2())*0.5f;
	glRasterPos2f(-0.5f,0.5f);
	float zx = (((float)( px2 -  px1))  )
					/(float)_PixelWidth ;
	float zy =	-(((float)( py2 -  py1)) )
				/(float)_PixelHeight ;
	glPixelZoom(zx,zy);

	// this one seems to be needed to make glDrawPixels work:
	glBindTexture( GL_TEXTURE_2D , 0 );
	
//  glPixelStore, glPixelTransfer, glPixelMap, 

	// glDisable( GL_DEPTH_TEST );

	glDrawPixels(
		_PixelWidth,
		_PixelHeight,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			_prgbaTable
			);
	//glEnable( GL_SCISSOR_TEST );
}
/*!
	\brief	Tool for other methods: Set the current GL viewport matrix to an InternalViewPort.
*/
void	OGLMachine::OGLInternalViewPort::SetGLToViewPort( )
{
	// this method could apply for rendered textures or rendered screens.
	glViewport( m_pixel_x1,
				((OGLInternalViewPort*)GetRootViewPort())->m_pixel_y2-m_pixel_y2,
				m_pixel_x2 - m_pixel_x1,
				m_pixel_y2 -m_pixel_y1 );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	float	fnear = m_NearCutPlane;

	float	_ScaleClipX1 = 	(m_ClippingRootX1-m_ScaleRootX1)
							/(m_ScaleRootX2-m_ScaleRootX1);
	float	_ScaleClipX2 = 	(m_ClippingRootX2-m_ScaleRootX1)
							/(m_ScaleRootX2-m_ScaleRootX1);

	float	_ScaleClipY1 = 	(m_ClippingRootY1-m_ScaleRootY1)
							/(m_ScaleRootY2-m_ScaleRootY1);
	float	_ScaleClipY2 = 	(m_ClippingRootY2-m_ScaleRootY1)
							/(m_ScaleRootY2-m_ScaleRootY1);

/*
	float	_ScaleClipX1 = 	(m_ScaleRootX1-m_ClippingRootX1)
							/(m_ClippingRootX2-m_ClippingRootX1);
	float	_ScaleClipX2 = 	(m_ScaleRootX2-m_ClippingRootX1)
							/(m_ClippingRootX2-m_ClippingRootX1);
	float	_ScaleClipY1 = 	(m_ScaleRootY1-m_ClippingRootY1)
							/(m_ClippingRootY2-m_ClippingRootY1);
	float	_ScaleClipY2 = 	(m_ScaleRootY2-m_ClippingRootY1)
							/(m_ClippingRootY2-m_ClippingRootY1);*/
	glFrustum(
                (_ScaleClipX1-0.5f )*fnear,(_ScaleClipX2-0.5f)*fnear,		// left , right
                (_ScaleClipY2-0.5f) *fnear,(_ScaleClipY1-0.5f)*fnear,		// botton, top
               m_fov * fnear ,m_FarCutPlane	//ffov * fnear, ffar  // near, far.
                );
	glMatrixMode( GL_MODELVIEW );	


}
OGLMachine::InternalVertex	 *OGLMachine::OGLInternalObject3DBuffer::GetFirstVertex(){ return m_pVertexBase; }
OGLMachine::InternalTriangle *OGLMachine::OGLInternalObject3DBuffer::GetFirstTriangle(){ return m_pTriangleBase+ m_FirstActiveTriangleIndex; }
/*!
	\brief	Set the number of active triangle defined in the triangle list.
			note the active number of vertex is actually given by the highest vertex
			number used by one of the active triangle. If _CurrentNumberOfTriangle is
			greater than GetMaximumNumberOfTriangles(), it is set to zero.
			_FirstActiveTriangleIndex is an optional offset in the triangle list.
			it allows the rendering of a mesh with multiple textures, by 
			setting a windowing in the triangle list and using
			multiple calls to RenderMesh(). Default is zero, start of the list.
	\param _CurrentNumberOfTriangle the new number of triangle in the object.
	\param _FirstActiveTriangleIndex optional active triangle offset in list.
*/
void OGLMachine::OGLInternalObject3DBuffer::SetNumberOfActiveTriangle( unsigned int _CurrentNumberOfTriangle,unsigned int _FirstActiveTriangleIndex )
{ 	
	if(_CurrentNumberOfTriangle>m_MaximumNumberOfTriangles ) m_NumberOfActiveTriangles=0;
	else m_NumberOfActiveTriangles =  _CurrentNumberOfTriangle ; 
	m_FirstActiveTriangleIndex = _FirstActiveTriangleIndex ;
}
/*!
	\brief	Get the number of active triangle defined by SetNumberOfActiveTriangle().
	\return the number of triangle in the object.
*/
unsigned int OGLMachine::OGLInternalObject3DBuffer::GetNumberOfActiveTriangle()
{
	return m_NumberOfActiveTriangles;
}
/*!
	\brief	the maximum number of elements in table GetFirstVertex()
	\return  the maximum number of elements in table GetFirstVertex() 
*/
unsigned int	OGLMachine::OGLInternalObject3DBuffer::GetMaximumNumberOfVertexes()
{
	return  m_MaximumNumberOfVertexes;
}
/*!
	\brief	the maximum number of elements in table GetFirstTriangle()
	\return  the maximum number of elements in table GetFirstTriangle() 
*/
unsigned int	OGLMachine::OGLInternalObject3DBuffer::GetMaximumNumberOfTriangles()
{
	return m_MaximumNumberOfTriangles;
}
/*!
	\brief	 Set the color for the whole object. Active if object was not created with bOb3D_EnableRGB or bOb3D_EnableAlpha.
	\param	_red	color component
	\param	_green	color component
	\param	_blue	color component
	\param	_alpha	color component
*/
void	OGLMachine::OGLInternalObject3DBuffer::SetColor(float _red,float _green,float _blue,float _alpha)
{
	m_Color[0] = _red;
	m_Color[1] = _green;
	m_Color[2] = _blue;
	m_Color[3] = _alpha;
}
OGLMachine::OGLInternalTexture::OGLInternalTexture(void)
: m_flags(-1) // to test for change.
,m_ActiveLayerFlag(0L)
,m_frameBuffer(0L)
,m_depthRenderBuffer(0L)
{
	m_Layer[0].m_OGLTextureHandler =
	m_Layer[1].m_OGLTextureHandler = 0L;
	m_BaseColorRGBA[0] =
	m_BaseColorRGBA[1] =
	m_BaseColorRGBA[2] = 
	m_BaseColorRGBA[3] = 1.0f;

	// generate the simple mapping and environnement texture, to be defined later
	// with SetRenderFlags() and SetImageBitmap()
	glGenTextures( 1, &(m_Layer[0].m_OGLTextureHandler) );	
	glGenTextures( 1, &(m_Layer[1].m_OGLTextureHandler) );	
}
void OGLMachine::OGLInternalTexture::SetRenderFlags( unsigned int _flags )
{
/*
#define	itf_Filtered	1 X
#define	itf_Additive	2
#define	itf_EnvAdd		4 X
#define	itf_ClampTxtU	8
#define	itf_ClampTxtV	16
#define	itf_nodepthwrite	32 X
#define	itf_nodepthtest		64 X
#define	itf_substractive	128 X
	*/
	// no change, no process:
//	if(  _flags == m_flags) return;
	m_flags = _flags;

}
/*!
	\brief	Set base color, default to 1.0f,1.0f,1.0f,1.0f
	\param	_rgba Red Green Blue and Alpha transparency components. 
*/
void	OGLMachine::OGLInternalTexture::SetBaseColor( float _rgba[4] )
{
	m_BaseColorRGBA[0] = _rgba[0];
	m_BaseColorRGBA[1] = _rgba[1];
	m_BaseColorRGBA[2] = _rgba[2];
	m_BaseColorRGBA[3] = _rgba[3];
}
/*!
	\brief	Get the render flags for a texture.
	\return	 or'ed bit flags (InternalTextureFlagBitsEnum)
*/
unsigned int OGLMachine::OGLInternalTexture::GetRenderFlags( )
{
	return m_flags;
}
void	OGLMachine::OGLInternalTexture::SetImageBitmap(	TextureImageLayerEnum _layerToSet,
					unsigned char *_pByteChunk,
					unsigned int _pixelByteDepth,
					unsigned int _pixelWidth,
					unsigned int _pixelHeight
					)
{
	/*old
	if( _layerToSet == itile_TextureMapping )
	{
		m_ActiveLayerFlag |= 1;
		glBindTexture( GL_TEXTURE_2D , m_Layer[0].m_OGLTextureHandler );
	} else
	{	// == itile_Environment
		m_ActiveLayerFlag |= 2;
		glBindTexture( GL_TEXTURE_2D , m_Layer[1].m_OGLTextureHandler );
	}*/
	m_ActiveLayerFlag |= 1<<((int)_layerToSet);
	glBindTexture( GL_TEXTURE_2D , m_Layer[_layerToSet].m_OGLTextureHandler );

	glTexEnvi(  GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE,
                    //GL_REPLACE    // only texture  DEFAULT.
                    //GL_DECAL      // image -alphaimage sur couleur
					//GL_REPLACE   // as blend ? -> WORK ON LINUX/WIN/AMIGASTORMMESA
					GL_MODULATE
					//GL_BLEND
					);

	// set bitmap image to given layer:
	switch( _pixelByteDepth)
	{
		case 1:
			glTexImage2D( GL_TEXTURE_2D, 0,
							GL_LUMINANCE,
							_pixelWidth ,//width
							_pixelHeight ,//height
							0,
							GL_LUMINANCE,
							GL_UNSIGNED_BYTE,
							_pByteChunk
							);
			break;
		case 4:
			glTexImage2D( GL_TEXTURE_2D, 0,
							GL_RGBA/* GL_LUMINANCE*/,
							_pixelWidth /*width*/,
							_pixelHeight /*height*/,
							0,
							GL_RGBA,
							GL_UNSIGNED_BYTE,
							_pByteChunk
							);
			break;
	}
	

}
void	OGLMachine::OGLInternalViewPort::Matrix_LoadID(eLoadId _enum )
{
	glMatrixMode( GL_MODELVIEW );
	if( _enum == eLoadId_All )
	{
		glLoadIdentity();
	} else if(_enum == eLoadId_Rotation)
	{

		float mat[4][4];
		glGetFloatv(GL_MODELVIEW_MATRIX,&(mat[0][0]));
		mat[0][0] = 
		mat[1][1] = 
		mat[2][2] = 1.0f ;
		mat[0][1] = 
		mat[0][2] = 
		mat[1][0] = 
		mat[1][2] = 
		mat[2][1] = 
		mat[2][0] = 0.0f ;
		glLoadMatrixf( &(mat[0][0]) );
	}
}
void	OGLMachine::OGLInternalViewPort::Matrix_Translate(float _x,float _y,float _z)
{

	glTranslatef(_x,_y,_z);
}
void    OGLMachine::OGLInternalViewPort::Matrix_Rotate( float _angle,float _x,float _y,float _z)
{
	glRotatef( _angle * (180.0f/3.14159265358979323846f) ,_x,_y,_z);
}
void	OGLMachine::OGLInternalViewPort::Matrix_Scale(float _sx,float _sy,float _sz)
{
	glScalef(_sx,_sy,_sz);
}
void	OGLMachine::OGLInternalViewPort::Matrix_Push()
{
	glPushMatrix();
}
void	OGLMachine::OGLInternalViewPort::Matrix_Pop()
{
	glPopMatrix();
}

/* 
ATI Radeon 8500 
  driver: version 6.14.10.6451, date: 15/05/2004
-		ext	0x14ba6b2c 
"GL_ARB_multitexture GL_EXT_texture_env_add GL_EXT_compiled_vertex_array GL_S3_s3tc 
GL_ARB_depth_texture GL_ARB_fragment_program GL_ARB_fragment_shader GL_ARB_multisample 
GL_ARB_occlusion_query GL_ARB_point_parameters GL_ARB_shader_objects GL_ARB_shading_language_100 
GL_ARB_shadow GL_ARB_shadow_ambient GL_ARB_texture_border_clamp GL_ARB_texture_compression 
GL_ARB_texture_cube_map GL_ARB_texture_env_add GL_ARB_texture_env_combine GL_ARB_texture_env_crossbar 
GL_ARB_texture_env_dot3 GL_ARB_texture_mirrored_repeat GL_ARB_transpose_matrix GL_ARB_vertex_blend 
GL_ARB_vertex_buffer_object GL_ARB_vertex_program GL_ARB_vertex_shader GL_ARB_window_pos GL_ATI_draw_buffers 
GL_ATI_element_array GL_ATI_envmap_bumpmap GL_ATI_fragment_shader GL_ATI_map_object_buffer 
GL_ATI_separate_stencil 
GL_ATI_texture_env_combine3 GL_ATI_texture_float GL_ATI_texture_mirror_once GL_ATI_vertex_array_object 
GL_ATI_vertex_attrib_array_object GL_ATI_vertex_streams GL_ATIX_texture_env_combine3 
GL_ATIX_texture_env_route GL_ATIX_verte"	char *



*/
