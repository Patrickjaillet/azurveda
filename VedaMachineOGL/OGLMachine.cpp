// SPDX-License-Identifier: LGPL-2.1-only

#include "OGLMachine.h"

#include <GL/gl.h>

#ifndef NULL
#define	NULL	0L
#endif

PFNGLACTIVETEXTUREARBPROC		OGLMachine::glActiveTextureARB=0L;
PFNGLCLIENTACTIVETEXTUREARBPROC	OGLMachine::glClientActiveTextureARB=0L;

bool	OGLMachine::m_RenderObjectExtension=false;
PFNGLGENRENDERBUFFERSEXTPROC OGLMachine::glGenRenderbuffersEXT=NULL;
PFNGLGENFRAMEBUFFERSEXTPROC OGLMachine::glGenFramebuffersEXT=NULL;
PFNGLBINDRENDERBUFFEREXTPROC OGLMachine::glBindRenderbufferEXT =NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC OGLMachine::glRenderbufferStorageEXT=NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC OGLMachine::glDeleteFramebuffersEXT=NULL;
PFNGLDELETERENDERBUFFERSEXTPROC OGLMachine::glDeleteRenderbuffersEXT=NULL;
PFNGLBINDFRAMEBUFFEREXTPROC OGLMachine::glBindFramebufferEXT=NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC OGLMachine::glFramebufferTexture2DEXT=NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC OGLMachine::glFramebufferRenderbufferEXT=NULL ;

OGLMachine::OGLMachine() : VirtualMachine()
{

}

OGLMachine::~OGLMachine(void)
{
	if( m_pDefaultViewPort )
	{
		delete m_pDefaultViewPort;
		m_pDefaultViewPort = 0L;
	}
}

VirtualMachine::eVMResult	OGLMachine::InitMachine()
{

	if( m_pDefaultViewPort )
	{
		m_pDefaultViewPort->SetFOVLength( 0.75f );
		m_pDefaultViewPort->SetNearCutPlane( 0.25f );
		m_pDefaultViewPort->SetFarCutPlane( 1024.0f );
	}

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glShadeModel( GL_SMOOTH );
	glBlendFunc( GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA );
	glClearDepth(1.0f);
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_TEXTURE_2D );

	glEnable( GL_BLEND );
	glEnable( GL_DITHER );
	glEnable( GL_CULL_FACE );
	glEnable( GL_NORMALIZE );

    glCullFace( GL_FRONT );

	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );

    glEnableClientState(    GL_VERTEX_ARRAY );
    glDisableClientState(   GL_INDEX_ARRAY );
    glDisableClientState(   GL_EDGE_FLAG_ARRAY );

	return vmr_OK;
}

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

	pObj->m_CompiledObjectHandler = 0;

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

void	OGLMachine::OGLInternalObject3DBuffer::CompileAsStatic( )
{

	m_CompiledObjectHandler = 1;
}

bool	OGLMachine::OGLInternalObject3DBuffer::IsCompiledAsStatic()
{
	return (m_CompiledObjectHandler!=0L);
}

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

OGLMachine::OGLInternalViewPort::OGLInternalViewPort(OGLInternalViewPort *_pFather, OGLMachine *_pCreator)
: m_pOGLMachine(_pCreator)

{
	m_ppClippingHierarchyAttached = 0L;
	m_pClippingNextBrotherViewPort = 0L;
	m_pClippingFirstSonViewPort = 0L;
	m_pClippingFatherViewPort = _pFather;
	const float v0 = 0.0f;
	const float v1 = 1.0f;
	if(_pFather)
	{
		m_pRootViewPort = _pFather->GetRootViewPort();

		m_pClippingNextBrotherViewPort = _pFather->m_pClippingFirstSonViewPort;

		if(m_pClippingNextBrotherViewPort)
			((OGLInternalViewPort *)m_pClippingNextBrotherViewPort)->m_ppClippingHierarchyAttached = &m_pClippingNextBrotherViewPort;

		m_ppClippingHierarchyAttached = &(_pFather->m_pClippingFirstSonViewPort);
		_pFather->m_pClippingFirstSonViewPort = this;

		SetClippingCoordinates(v0,v0,v1,v1);
		SetScaleCoordinates(v0,v0,v1,v1);
	}
	else
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
		m_pRootViewPort = this;
	}
	m_NearCutPlane = 0.25f;
	m_FarCutPlane = 1024.0f;
	m_fov = 0.5f;
	m_pRenderTexture = 0L;
	m_Valid = true;
	ResetPixelCoordinates();
}
#ifdef _ENGINE_EDITABLE_

OGLMachine::OGLInternalViewPort::~OGLInternalViewPort()
{

	ClippingDetach();

}
#endif
#ifdef _ENGINE_EDITABLE_

void	OGLMachine::OGLInternalViewPort::ClippingDetach()
{

	if(m_ppClippingHierarchyAttached)
	{
		*m_ppClippingHierarchyAttached = m_pClippingNextBrotherViewPort;
		if(m_pClippingNextBrotherViewPort)
			((OGLInternalViewPort*)m_pClippingNextBrotherViewPort)->m_ppClippingHierarchyAttached = m_ppClippingHierarchyAttached;
	}
	m_ppClippingHierarchyAttached = 0L ;
	m_pClippingNextBrotherViewPort = 0L;
	m_pRootViewPort = 0L;

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

void OGLMachine::OGLInternalViewPort::SetClippingCoordinates(float _ClippingX1,float _ClippingY1,float _ClippingX2,float _ClippingY2 )
{
	if( !m_pClippingFatherViewPort ) return;

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

void OGLMachine::OGLInternalViewPort::SetScaleCoordinates(float _ScaleX1,float _ScaleY1,float _ScaleX2,float _ScaleY2 )
{

	if( !m_pClippingFatherViewPort) return;

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

void	OGLMachine::OGLInternalViewPort::ResetRootPixelSize( unsigned int _pixelWidth,unsigned int _pixelHeight )
{
	OGLInternalViewPort *pRoot = (OGLInternalViewPort *)m_pRootViewPort;
	pRoot->m_pixel_x1 = 0;
	pRoot->m_pixel_y1 = 0;
	pRoot->m_pixel_x2 = _pixelWidth;
	pRoot->m_pixel_y2 = _pixelHeight;
	pRoot->ResetPixelCoordinates();
}

void	OGLMachine::OGLInternalViewPort::ResetPixelCoordinates()
{

	register OGLInternalViewPort *pOGLFather = (OGLInternalViewPort *)m_pClippingFatherViewPort;
	register OGLInternalViewPort *pOGLRoot = (OGLInternalViewPort *)m_pRootViewPort;
	m_Valid = true;

	if(pOGLFather && !pOGLFather->m_Valid) m_Valid = false;

	if(pOGLFather && pOGLFather->m_Valid)
	{

		m_ScaleRootX1 = pOGLFather->m_ScaleRootX1 +
			(pOGLFather->m_ScaleRootX2-pOGLFather->m_ScaleRootX1) * m_ScaleX1;
		m_ScaleRootY1 = pOGLFather->m_ScaleRootY1 +
			(pOGLFather->m_ScaleRootY2-pOGLFather->m_ScaleRootY1) * m_ScaleY1;
		m_ScaleRootX2 = pOGLFather->m_ScaleRootX1 +
			(pOGLFather->m_ScaleRootX2-pOGLFather->m_ScaleRootX1) * m_ScaleX2;
		m_ScaleRootY2 = pOGLFather->m_ScaleRootY1 +
			(pOGLFather->m_ScaleRootY2-pOGLFather->m_ScaleRootY1) * m_ScaleY2;

		m_ClippingRootX1 = pOGLFather->m_ScaleRootX1 +
			(pOGLFather->m_ScaleRootX2-pOGLFather->m_ScaleRootX1) * m_ClippingX1;
		m_ClippingRootY1 = pOGLFather->m_ScaleRootY1 +
			(pOGLFather->m_ScaleRootY2-pOGLFather->m_ScaleRootY1) * m_ClippingY1;
		m_ClippingRootX2 = pOGLFather->m_ScaleRootX1 +
			(pOGLFather->m_ScaleRootX2-pOGLFather->m_ScaleRootX1) * m_ClippingX2;
		m_ClippingRootY2 = pOGLFather->m_ScaleRootY1 +
			(pOGLFather->m_ScaleRootY2-pOGLFather->m_ScaleRootY1) * m_ClippingY2;

		if( m_ClippingRootX1>pOGLFather->m_ClippingRootX2 ||
			m_ClippingRootX2<pOGLFather->m_ClippingRootX1 ||
			m_ClippingRootY1>pOGLFather->m_ClippingRootY2 ||
			m_ClippingRootY2<pOGLFather->m_ClippingRootY1
			) m_Valid = false;
		else
		{

			if(m_ClippingRootX1<pOGLFather->m_ClippingRootX1)
				m_ClippingRootX1 = pOGLFather->m_ClippingRootX1;
			if(m_ClippingRootY1<pOGLFather->m_ClippingRootY1)
				m_ClippingRootY1 = pOGLFather->m_ClippingRootY1;
			if(m_ClippingRootX2>pOGLFather->m_ClippingRootX2)
				m_ClippingRootX2 = pOGLFather->m_ClippingRootX2;
			if(m_ClippingRootY2>pOGLFather->m_ClippingRootY2)
				m_ClippingRootY2 = pOGLFather->m_ClippingRootY2;

			float	rootpixelwidth =(float)(pOGLRoot->m_pixel_x2-pOGLRoot->m_pixel_x1) ;
			float	rootpixelheight =(float)(pOGLRoot->m_pixel_y2-pOGLRoot->m_pixel_y1) ;
			m_pixel_x1 = pOGLRoot->m_pixel_x1 + (int)(rootpixelwidth*m_ClippingRootX1);
			m_pixel_y1 = pOGLRoot->m_pixel_y1 + (int)(rootpixelheight*m_ClippingRootY1);
			m_pixel_x2 = pOGLRoot->m_pixel_x1 + (int)(rootpixelwidth*m_ClippingRootX2);
			m_pixel_y2 = pOGLRoot->m_pixel_y1 + (int)(rootpixelheight*m_ClippingRootY2);
		}
	}

	register OGLInternalViewPort *pOGLSon = (OGLInternalViewPort *) m_pClippingFirstSonViewPort;
	while(pOGLSon)
	{
		((OGLInternalViewPort*)pOGLSon)->ResetPixelCoordinates();
		pOGLSon = (OGLInternalViewPort *)(pOGLSon->m_pClippingNextBrotherViewPort) ;
	}

}

void	OGLMachine::NewViewPortChild(  InternalViewPort **_ppViewPortOut,InternalViewPort *_pFatherViewPort)
{
	*_ppViewPortOut = 0L;
	if(!_pFatherViewPort) _pFatherViewPort = GetDefaultViewPort();
	*_ppViewPortOut = new OGLInternalViewPort((OGLInternalViewPort *)_pFatherViewPort,this) ;
}

void	OGLMachine::NewViewPortOnTexture(  InternalViewPort **_ppViewPortOut,
									InternalTexture *_pRootTexture,
									unsigned int _np2PixelWidth,
									unsigned int _np2PixelHeight,
									InternalTexture::TextureImageLayerEnum _layerToAffect)
{
	*_ppViewPortOut = 0L;

	if(!_pRootTexture) return;
	OGLInternalViewPort *pNewVP;
	*_ppViewPortOut = pNewVP = new OGLInternalViewPort(0L,this);
	pNewVP->m_pRenderTexture = (OGLInternalTexture *)_pRootTexture;
	int renderBuffer_Width  = 1<<_np2PixelWidth;
	int renderBuffer_Height = 1<<_np2PixelHeight;
	int	glTextureHandle = ((OGLInternalTexture *)_pRootTexture)->m_Layer[_layerToAffect].m_OGLTextureHandler ;

	pNewVP->m_RenderTextureLayer = (int)_layerToAffect;

	if(m_RenderObjectExtension)
	{
		GLuint frameBuffer,depthRenderBuffer;
		glGenFramebuffersEXT( 1, &frameBuffer );
		((OGLInternalTexture *)_pRootTexture)->m_frameBuffer = frameBuffer;
		glGenRenderbuffersEXT( 1, &depthRenderBuffer  );
		((OGLInternalTexture *)_pRootTexture)->m_depthRenderBuffer = depthRenderBuffer;

		glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, depthRenderBuffer );
		glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, renderBuffer_Width, renderBuffer_Height );

	}

	glBindTexture(GL_TEXTURE_2D, glTextureHandle);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
		        renderBuffer_Width, renderBuffer_Height,
		        0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
	pNewVP->m_pRenderTexture->m_ActiveLayerFlag |= (1<<(int)_layerToAffect);

	pNewVP->ResetRootPixelSize(renderBuffer_Width,renderBuffer_Height);

}

void	OGLMachine::DeleteViewPort(  InternalViewPort **_ppViewPortToDestroy )
{
	if( !(*_ppViewPortToDestroy) ) return;
	OGLInternalViewPort	*pViewport = (OGLInternalViewPort *) *_ppViewPortToDestroy ;

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

		delete pViewport ;
	}
	*_ppViewPortToDestroy = 0L;
}

void	OGLMachine::OGLInternalViewPort::SwapBuffer()
{

	OGLInternalViewPort *pRoot=(OGLInternalViewPort *)m_pRootViewPort;
	if(pRoot->m_pRenderTexture)
	{
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
						0);

	}else
	{
		m_pOGLMachine->SwapMainScreenBuffer();
	}

}

void	OGLMachine::OGLInternalViewPort::Clear( float _red,float _green,float _blue)
{
	if(!m_Valid) return;
	SetGLToViewPort();
	glDepthMask( GL_TRUE );
	glEnable(GL_SCISSOR_TEST );

	glScissor( GetPixelX1(),
				((OGLInternalViewPort*)GetRootViewPort())->GetPixelY2()-GetPixelY2(),
				GetPixelX2() - GetPixelX1(),
				GetPixelY2() - GetPixelY1());
	glClearColor( _red, _green, _blue,1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDisable(GL_SCISSOR_TEST );
}

void	OGLMachine::OGLInternalViewPort::RenderMesh(
							InternalObject3DBuffer *_pMeshBuffer,
							InternalTexture *_pTexture
							)
{
	if(!m_Valid) return;
	if(!_pMeshBuffer) return;

	OGLMachine::OGLInternalTexture *renderTexture = ((OGLInternalViewPort *)GetRootViewPort())->m_pRenderTexture;
	if( renderTexture )
	{
		if( renderTexture->m_frameBuffer )
		{
			glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_pRenderTexture->m_frameBuffer );
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
											GL_TEXTURE_2D,
renderTexture->m_Layer[((OGLInternalViewPort *)GetRootViewPort())->m_RenderTextureLayer].m_OGLTextureHandler
											, 0 );
			glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
												GL_RENDERBUFFER_EXT,
												m_pRenderTexture->m_depthRenderBuffer );

		} else
		{

		}
	}

	SetGLToViewPort();

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
		glColorPointer(  4,GL_FLOAT , sizeof( InternalVertex ), &(pFirstVertex->m_ColorRGBA[0]) );
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
		{
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
		InternalTexture::itf_depthtest;
	if(_pTexture)
	{
		OGLInternalTexture *pOGLTexture =(OGLInternalTexture *)_pTexture;
		activeLayerflag =	pOGLTexture->m_ActiveLayerFlag;
		txt =				pOGLTexture->m_Layer[0].m_OGLTextureHandler ;
		txtenv =			pOGLTexture->m_Layer[1].m_OGLTextureHandler ;
		renderflag = pOGLTexture->m_flags ;
	}

	int ii,jj;
	if(renderflag & InternalTexture::itf_doublesided)
		glDisable( GL_CULL_FACE );
	else glEnable( GL_CULL_FACE );

	if( renderflag & InternalTexture::itf_depthwrite  )
			glDepthMask( GL_TRUE  );
		else
			glDepthMask( GL_FALSE  );

	ii = GL_LEQUAL ;
	if( (renderflag & InternalTexture::itf_depthtest )==0 )
			ii = GL_ALWAYS ;
	glDepthFunc(ii);

	jj = GL_SRC_ALPHA ;
	if( (renderflag & InternalTexture::itf_Additive) == 0 )
	{	ii= GL_ONE_MINUS_SRC_ALPHA;
		if( (renderflag & InternalTexture::itf_substractive) != 0 )
		{	jj =  GL_ZERO;
			ii =  GL_ONE_MINUS_SRC_ALPHA;}
	} else
	{	ii= GL_ONE;
	}
	glBlendFunc( jj, ii );

	if( (activeLayerflag & 1) != 0  )
	{
		if( (activeLayerflag & 2) ==0)
		{

			glBindTexture( GL_TEXTURE_2D ,txtenv );
			glDisable( GL_TEXTURE_GEN_S );
			glDisable( GL_TEXTURE_GEN_T );
			glBindTexture( GL_TEXTURE_2D ,txt );
			glDisable( GL_TEXTURE_GEN_S );
			glDisable( GL_TEXTURE_GEN_T );

			if( (renderflag & InternalTexture::itf_Filtered ) )
				jj = GL_LINEAR ;
			else	jj = GL_NEAREST ;
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, jj );
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, jj );

			if( (renderflag & InternalTexture::itf_ClampTxtU ) != 0 )
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP );
			else
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT );
			if( (renderflag & InternalTexture::itf_ClampTxtV ) != 0 )
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP );
			else
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT );

			glDrawElements( GL_TRIANGLES , ((OGLInternalObject3DBuffer *)_pMeshBuffer)->m_NumberOfActiveTriangles * 3 ,
				GL_UNSIGNED_INT, ((OGLInternalObject3DBuffer *)_pMeshBuffer)->GetFirstTriangle()
							);
		} else
		{
			if( ! OGLMachine::glActiveTextureARB ) return;

			glActiveTextureARB( GL_TEXTURE0_ARB );
			glClientActiveTextureARB( GL_TEXTURE0_ARB );
			glBindTexture( GL_TEXTURE_2D , txt );
			glEnable( GL_TEXTURE_2D );
			glDisable( GL_TEXTURE_GEN_S );
			glDisable( GL_TEXTURE_GEN_T );

			if( (renderflag & InternalTexture::itf_Filtered ) )
				jj = GL_LINEAR ;
			else	jj = GL_NEAREST ;
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, jj );
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, jj );

			if( (renderflag & InternalTexture::itf_ClampTxtU ) != 0 )
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP );
			else
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT );
			if( (renderflag & InternalTexture::itf_ClampTxtV ) != 0 )
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP );
			else
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT );

			glActiveTextureARB( GL_TEXTURE1_ARB );
			glClientActiveTextureARB( GL_TEXTURE1_ARB );

				glBindTexture( GL_TEXTURE_2D, txtenv );
				glEnable( GL_TEXTURE_2D );
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, jj );
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, jj );
				glEnable( GL_BLEND );

				glEnable( GL_TEXTURE_GEN_S );
				glEnable( GL_TEXTURE_GEN_T );
				glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
				glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );

				if( renderflag & InternalTexture::itf_envAdd  )
				{
					glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_ADD);
				} else
				{
					glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

				}

			glDrawElements( GL_TRIANGLES , ((OGLInternalObject3DBuffer *)_pMeshBuffer)->m_NumberOfActiveTriangles * 3 ,
				GL_UNSIGNED_INT, ((OGLInternalObject3DBuffer *)_pMeshBuffer)->GetFirstTriangle()
							);

			glDisable( GL_TEXTURE_2D );

			glActiveTextureARB( GL_TEXTURE0_ARB );
			glClientActiveTextureARB( GL_TEXTURE0_ARB );
		}
	} else
	{
		if( activeLayerflag & 2)
		{
			glBindTexture( GL_TEXTURE_2D ,txtenv );
			if( (renderflag & InternalTexture::itf_Filtered ) )
				jj = GL_LINEAR ;
			else	jj = GL_NEAREST ;
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, jj );
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, jj );
			glEnable( GL_TEXTURE_GEN_S );
			glEnable( GL_TEXTURE_GEN_T );

			glDrawElements( GL_TRIANGLES , ((OGLInternalObject3DBuffer *)_pMeshBuffer)->m_NumberOfActiveTriangles * 3 ,
				GL_UNSIGNED_INT, ((OGLInternalObject3DBuffer *)_pMeshBuffer)->GetFirstTriangle()
							);
		} else
		{

			glBindTexture( GL_TEXTURE_2D ,txt );
			glDisable( GL_TEXTURE_2D );

			glDrawElements( GL_TRIANGLES , ((OGLInternalObject3DBuffer *)_pMeshBuffer)->m_NumberOfActiveTriangles * 3 ,
				GL_UNSIGNED_INT, ((OGLInternalObject3DBuffer *)_pMeshBuffer)->GetFirstTriangle()
							);
			glEnable( GL_TEXTURE_2D );
		}
	}

	if( ((OGLInternalViewPort *)GetRootViewPort())->m_pRenderTexture )
	{
		if(((OGLInternalViewPort *)GetRootViewPort())->m_pRenderTexture->m_frameBuffer)
		{

			glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

		} else
		{

		}
	}

}

void	OGLMachine::OGLInternalViewPort::DrawRGBARectangle(
							unsigned char	*_prgbaTable,
							unsigned int	_PixelWidth,
							unsigned int	_PixelHeight
							)
{
	if(!m_Valid) return;
	int	px1 = GetPixelX1() ;
	int	py1 = GetPixelY1() ;
	int	px2 = GetPixelX2() ;
	int	py2 = GetPixelY2() ;

	glViewport( px1,
				((OGLInternalViewPort *)GetRootViewPort())->GetPixelY2() - py2,
				px2 - px1,
				py2 - py1 );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glOrtho( -0.5f,0.5f,
			-0.5f ,0.5f,
			-1.0f, 1.0f);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glRasterPos2f(-0.5f,0.5f);
	float zx = (((float)( px2 -  px1))  )
					/(float)_PixelWidth ;
	float zy =	-(((float)( py2 -  py1)) )
				/(float)_PixelHeight ;
	glPixelZoom(zx,zy);

	glBindTexture( GL_TEXTURE_2D , 0 );

	glDrawPixels(
		_PixelWidth,
		_PixelHeight,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			_prgbaTable
			);

}

void	OGLMachine::OGLInternalViewPort::SetGLToViewPort( )
{

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

	glFrustum(
                (_ScaleClipX1-0.5f )*fnear,(_ScaleClipX2-0.5f)*fnear,
                (_ScaleClipY2-0.5f) *fnear,(_ScaleClipY1-0.5f)*fnear,
               m_fov * fnear ,m_FarCutPlane
                );
	glMatrixMode( GL_MODELVIEW );

}
OGLMachine::InternalVertex	 *OGLMachine::OGLInternalObject3DBuffer::GetFirstVertex(){ return m_pVertexBase; }
OGLMachine::InternalTriangle *OGLMachine::OGLInternalObject3DBuffer::GetFirstTriangle(){ return m_pTriangleBase+ m_FirstActiveTriangleIndex; }

void OGLMachine::OGLInternalObject3DBuffer::SetNumberOfActiveTriangle( unsigned int _CurrentNumberOfTriangle,unsigned int _FirstActiveTriangleIndex )
{
	if(_CurrentNumberOfTriangle>m_MaximumNumberOfTriangles ) m_NumberOfActiveTriangles=0;
	else m_NumberOfActiveTriangles =  _CurrentNumberOfTriangle ;
	m_FirstActiveTriangleIndex = _FirstActiveTriangleIndex ;
}

unsigned int OGLMachine::OGLInternalObject3DBuffer::GetNumberOfActiveTriangle()
{
	return m_NumberOfActiveTriangles;
}

unsigned int	OGLMachine::OGLInternalObject3DBuffer::GetMaximumNumberOfVertexes()
{
	return  m_MaximumNumberOfVertexes;
}

unsigned int	OGLMachine::OGLInternalObject3DBuffer::GetMaximumNumberOfTriangles()
{
	return m_MaximumNumberOfTriangles;
}

void	OGLMachine::OGLInternalObject3DBuffer::SetColor(float _red,float _green,float _blue,float _alpha)
{
	m_Color[0] = _red;
	m_Color[1] = _green;
	m_Color[2] = _blue;
	m_Color[3] = _alpha;
}
OGLMachine::OGLInternalTexture::OGLInternalTexture(void)
: m_flags(-1)
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

	glGenTextures( 1, &(m_Layer[0].m_OGLTextureHandler) );
	glGenTextures( 1, &(m_Layer[1].m_OGLTextureHandler) );
}
void OGLMachine::OGLInternalTexture::SetRenderFlags( unsigned int _flags )
{

	m_flags = _flags;

}

void	OGLMachine::OGLInternalTexture::SetBaseColor( float _rgba[4] )
{
	m_BaseColorRGBA[0] = _rgba[0];
	m_BaseColorRGBA[1] = _rgba[1];
	m_BaseColorRGBA[2] = _rgba[2];
	m_BaseColorRGBA[3] = _rgba[3];
}

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

	m_ActiveLayerFlag |= 1<<((int)_layerToSet);
	glBindTexture( GL_TEXTURE_2D , m_Layer[_layerToSet].m_OGLTextureHandler );

	glTexEnvi(  GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE,

					GL_MODULATE

					);

	switch( _pixelByteDepth)
	{
		case 1:
			glTexImage2D( GL_TEXTURE_2D, 0,
							GL_LUMINANCE,
							_pixelWidth ,
							_pixelHeight ,
							0,
							GL_LUMINANCE,
							GL_UNSIGNED_BYTE,
							_pByteChunk
							);
			break;
		case 4:
			glTexImage2D( GL_TEXTURE_2D, 0,
							GL_RGBA,
							_pixelWidth ,
							_pixelHeight ,
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
