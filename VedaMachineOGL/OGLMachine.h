// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_OGLMachine_H
#define COM_M4NKIND_OGLMachine_H

#include "VirtualMachine.h"

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
#define GLAPI extern
#endif

#ifndef	PFNGLACTIVETEXTUREARBPROC
typedef void (APIENTRYP PFNGLACTIVETEXTUREARBPROC) (GLenum texture);
#endif
#ifndef	PFNGLCLIENTACTIVETEXTUREARBPROC
typedef void (APIENTRYP PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);
#endif
#ifndef	GL_TEXTURE0_ARB
#define GL_TEXTURE0_ARB                   0x84C0
#define GL_TEXTURE1_ARB                   0x84C1
#endif

#ifndef	PFNGLGENRENDERBUFFERSEXTPROC
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSEXTPROC) (GLsizei n, GLuint *renderbuffers);
#endif
#ifndef	PFNGLGENFRAMEBUFFERSEXTPROC
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSEXTPROC) (GLsizei n, GLuint *framebuffers);
#endif
#ifndef	PFNGLBINDRENDERBUFFEREXTPROC
typedef void (APIENTRYP PFNGLBINDRENDERBUFFEREXTPROC) (GLenum target, GLuint renderbuffer);
#endif
#ifndef	PFNGLRENDERBUFFERSTORAGEEXTPROC
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
#endif
#ifndef	PFNGLDELETEFRAMEBUFFERSEXTPROC
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSEXTPROC) (GLsizei n, const GLuint *framebuffers);
#endif
#ifndef	PFNGLDELETERENDERBUFFERSEXTPROC
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSEXTPROC) (GLsizei n, const GLuint *renderbuffers);
#endif
#ifndef	PFNGLBINDFRAMEBUFFEREXTPROC
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFEREXTPROC) (GLenum target, GLuint framebuffer);
#endif
#ifndef	PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
#endif
#ifndef	PFNGLFRAMEBUFFERTEXTURE2DEXTPROC
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
#endif
#ifndef	GL_FRAMEBUFFER_EXT
#define GL_FRAMEBUFFER_EXT                0x8D40
#endif
#ifndef	GL_RENDERBUFFER_EXT
#define GL_RENDERBUFFER_EXT               0x8D41
#endif
#ifndef	GL_DEPTH_COMPONENT16
#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#endif
#ifndef	GL_COLOR_ATTACHMENT0_EXT
#define GL_COLOR_ATTACHMENT0_EXT          0x8CE0
#endif
#ifndef	GL_DEPTH_ATTACHMENT_EXT
#define GL_DEPTH_ATTACHMENT_EXT           0x8D00
#endif

class OGLMachine : public VirtualMachine
{

public:

	OGLMachine(void);

	virtual ~OGLMachine(void);
	class OGLInternalTexture;

	class	OGLInternalViewPort : public InternalViewPort
	{	public:

		OGLInternalViewPort(OGLInternalViewPort *_pFather, OGLMachine *_pCreator);
#ifdef _ENGINE_EDITABLE_

		virtual ~OGLInternalViewPort();
#endif

		virtual	void	Clear( float _red=0.0f,float _green=0.0f,float _blue=0.0f);

		virtual	void	RenderMesh(	InternalObject3DBuffer *_pMeshBuffer,
									InternalTexture *_pTexture=0L
									) ;

		virtual	void	DrawRGBARectangle(
											unsigned char	*_prgbaTable,
											unsigned int	_PixelWidth,
											unsigned int	_PixelHeight
											) ;

		virtual	void	SwapBuffer();

		virtual	void SetClippingCoordinates(float _ClippingX1,float _ClippingY1,float _ClippingX2,float _ClippingY2 );

		virtual void SetScaleCoordinates(float _ScaleX1,float _ScaleY1,float _ScaleX2,float _ScaleY2 );

		void	ResetRootPixelSize( unsigned int _pixelWidth,unsigned int _pixelHeight );

		inline	int	GetPixelX1(){ return m_pixel_x1; };

		inline	int	GetPixelY1(){ return m_pixel_y1; };

		inline	int	GetPixelX2(){ return m_pixel_x2; };

		inline	int	GetPixelY2(){ return m_pixel_y2; };

		float m_ScaleRootX1;
		float m_ScaleRootY1;
		float m_ScaleRootX2;
		float m_ScaleRootY2;

		float m_ClippingRootX1;
		float m_ClippingRootY1;
		float m_ClippingRootX2;
		float m_ClippingRootY2;

		float	m_FinalClipFrustrumX1;
		float	m_FinalClipFrustrumY1;
		float	m_FinalClipFrustrumX2;
		float	m_FinalClipFrustrumY2;

		OGLInternalTexture	*m_pRenderTexture;

		int		m_RenderTextureLayer;

		virtual	void	SetGLToViewPort();

		virtual	void	Matrix_LoadID(eLoadId _enum=eLoadId_All);
		virtual	void	Matrix_Translate(float _x,float _y,float _z);
		virtual void    Matrix_Rotate( float _angle,float _x,float _y,float _z);
		virtual	void	Matrix_Scale(float _sx,float _sy,float _sz);
		virtual	void	Matrix_Push();
		virtual	void	Matrix_Pop();
	protected:

		OGLMachine	*m_pOGLMachine;

		bool	m_Valid;

		int		m_pixel_x1;

		int		m_pixel_y1;

		int		m_pixel_x2;

		int		m_pixel_y2;

		void	ResetPixelCoordinates();

		InternalViewPort	**m_ppClippingHierarchyAttached;
#ifdef _ENGINE_EDITABLE_

		void	ClippingDetach();
#endif
	};

	class	OGLInternalObject3DBuffer : public InternalObject3DBuffer
	{ public:

		virtual void	CompileAsStatic( );

		virtual bool	IsCompiledAsStatic();

		virtual InternalVertex	 *GetFirstVertex();

		virtual InternalTriangle *GetFirstTriangle();

		virtual void	SetNumberOfActiveTriangle( unsigned int _CurrentNumberOfTriangle,unsigned int _FirstActiveTriangleIndex=0 );

		virtual unsigned int GetNumberOfActiveTriangle();

		virtual unsigned int	GetMaximumNumberOfVertexes();

		virtual unsigned int	GetMaximumNumberOfTriangles();

		virtual void	SetColor(float _red,float _green,float _blue,float _alpha=1.0f);

		InternalVertex		*m_pVertexBase;

		InternalTriangle	*m_pTriangleBase;

		unsigned int		m_CompiledObjectHandler;

		unsigned int	m_FirstActiveTriangleIndex;

		unsigned int	m_NumberOfActiveTriangles;

		unsigned int	m_MaximumNumberOfVertexes;

		unsigned int	m_MaximumNumberOfTriangles;

		unsigned int	m_ExtraMemberFlags;

		float			m_Color[4];
	};

	class	OGLInternalTexture : public InternalTexture
	{ public:

		OGLInternalTexture(void);

		virtual void	SetRenderFlags( unsigned int _flags );

		virtual void	SetBaseColor( float _rgba[4] );

		virtual unsigned int GetRenderFlags( );

		virtual void	SetImageBitmap(	TextureImageLayerEnum _layerToSet,
								unsigned char *_pByteChunk,
								unsigned int _pixelByteDepth,
								unsigned int _pixelWidth,
								unsigned int _pixelHeight
								);
		static const int	m_MaxNumberOfLayer=2;

		class Layer { public:
			unsigned int	m_OGLTextureHandler;
			unsigned int	m_BitmapPixelWidth;
			unsigned int	m_BitmapPixelHeight;
		};

		Layer		m_Layer[m_MaxNumberOfLayer];

		unsigned int	m_flags;

		float		m_BaseColorRGBA[4];

		unsigned int	m_ActiveLayerFlag;

		GLuint m_frameBuffer;

		GLuint m_depthRenderBuffer;
	};

	virtual eVMResult	InitMachine();

	virtual	void	SwapMainScreenBuffer()=0;

	virtual void	NewObject3DBuffer(  InternalObject3DBuffer **_ppVertexAndPolygonBufferOut,
							unsigned int _maxNbVertex,
							unsigned int _maxNbTriangle,
							unsigned int _extraMemberFlags );

	virtual void	DeleteObject3DBuffer(  InternalObject3DBuffer **_ppBufferToDestroy );

	virtual void	NewTexture(  InternalTexture **_ppTextureOut  );
#ifdef _ENGINE_EDITABLE_

	virtual void	DeleteTexture(  InternalTexture **_ppTextureToDestroy );
#endif

	virtual void	NewViewPortChild(  InternalViewPort **_ppViewPortOut,InternalViewPort *_pFatherViewPort=0L);

	virtual void	NewViewPortOnTexture(  InternalViewPort **_ppViewPortOut,
											InternalTexture *_pRootTexture,
											unsigned int _np2PixelWidth,
											unsigned int _np2PixelHeight,
											InternalTexture::TextureImageLayerEnum _layerToAffect);

	virtual void	DeleteViewPort(  InternalViewPort **_ppViewPortToDestroy );

	static	PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
	static	PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB;

	static	bool	m_RenderObjectExtension;

	static PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
	static PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
	static PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT ;
	static PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
	static PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
	static PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
	static PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
	static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
	static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT ;

protected:

};

#endif
