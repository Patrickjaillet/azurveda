/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_OGLMachine_H
#define COM_M4NKIND_OGLMachine_H

#include "VirtualMachine.h"

// cuno la muerte !!! Needed for gl.h under windows
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
// to avoid include troubles, we don't include any glext.h, as a lots
// of version can be found, with shader missing, or multitexture missing, or
// framebuffer missing. so, we define them here, if not done in gl.h
// and it should be OK for any configuration.

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
#define GLAPI extern
#endif
// multitexturing:
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
// framebuffer objects:
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
/*
typedef void (APIENTRYP PFNGLBINDRENDERBUFFEREXTPROC) (GLenum target, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSEXTPROC) (GLsizei n, const GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSEXTPROC) (GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC) (GLenum target, GLenum pname, GLint *params);
typedef GLboolean (APIENTRYP PFNGLISFRAMEBUFFEREXTPROC) (GLuint framebuffer);
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFEREXTPROC) (GLenum target, GLuint framebuffer);
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSEXTPROC) (GLsizei n, const GLuint *framebuffers);
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSEXTPROC) (GLsizei n, GLuint *framebuffers);
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) (GLenum target);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE1DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) (GLenum target, GLenum attachment, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGENERATEMIPMAPEXTPROC) (GLenum target);
*/
//test #include "glext.h"
//#include <GL/glext.h>

/*!
	\class	OGLMachine
	\ingroup	VirtualMachineDocGroup
	\brief	 Defines a \mainprojectname platform independant OpenGL Machine. 
			Still, this class 
			is virtual, doesn't manage screen opening, doesn't manage sounds,
			But unify all OpenGL code, for any machine willing to use OpenGL.
*/

class OGLMachine : public VirtualMachine
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
	*/
	OGLMachine(void);
	/*!
		\brief	Destructor.
	*/
	virtual ~OGLMachine(void);
	class OGLInternalTexture;
	/*!
		\class	InternalViewPort
		\brief Nested class that defines a viewport where to draw (screen, sub-screen,texture rendering screens.)
			It must be extented to other classes.
	*/
	class	OGLInternalViewPort : public InternalViewPort
	{	public:
		/*!
			\brief	Constructor. 
			\param	_pFather the father, or NULL if new root.
			\param	_pCreator the machine upon that manage the VP.
		*/
		OGLInternalViewPort(OGLInternalViewPort *_pFather, OGLMachine *_pCreator);
#ifdef _ENGINE_EDITABLE_
		/*!
			\brief	Destructor. See DeleteViewport() 
		*/
		virtual ~OGLInternalViewPort();
#endif
		/*!
			\brief	Reset color to black (or given color),and depth buffer, of this ViewPort.
			\param	_red [0.0,1.0]
			\param	_green [0.0,1.0]
			\param	_blue [0.0,1.0]
		*/
		virtual	void	Clear( float _red=0.0f,float _green=0.0f,float _blue=0.0f);
		/*!
			\brief	Render a 3D Object. The vertexes are transformed with matrix _pMatrix,
					the object shape is defined by _pMeshBuffer, the texture of the object is
					defined by _pTexture, the ViewPort that define the rectangle where to draw,
					and the camera parameters is _pViewPort, or the default viewport if not specified.	
					Note drawings on the main screen needs a SwapScreenBuffer() to be shown at each frames.
			\param	_pMatrix Internal3DMatrixStack
			\param	_pMeshBuffer InternalObject3DBuffer
			\param	_pTexture InternalTexture or nothing, will draw shades.
		*/
		virtual	void	RenderMesh(	InternalObject3DBuffer *_pMeshBuffer,
									InternalTexture *_pTexture=0L
									) ;
		/*!
			\brief	Render directly a 32bit pixel bitmap image from memory on a viewport.
					Note drawings on the main screen needs a SwapScreenBuffer() to be shown at each frames.
			\param	_prgbaTable	pointer to the beginning of a 32bit RGBA table.
			\param	_PixelWidth number of pixel of a line
			\param	_PixelHeight number of pixel lines.
		*/
		virtual	void	DrawRGBARectangle( 
											unsigned char	*_prgbaTable,
											unsigned int	_PixelWidth,
											unsigned int	_PixelHeight
											) ;
		/*!
			\brief	Swap buffer so that what was drawn on the viewport is shown.
					this is needed to active all Clear(), RenderMesh() ,...
					It has to be done with main screen viewport like texture-rendering
					viewports.
		*/
		virtual	void	SwapBuffer();
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
		virtual	void SetClippingCoordinates(float _ClippingX1,float _ClippingY1,float _ClippingX2,float _ClippingY2 );
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
		virtual void SetScaleCoordinates(float _ScaleX1,float _ScaleY1,float _ScaleX2,float _ScaleY2 );

		/*!
			\brief	Change the root viewport pixel resolution.This is a low level call in case of resize or screen init.		
			\param	_pixelWidth 
			\param	_pixelHeight
		*/	
		void	ResetRootPixelSize( unsigned int _pixelWidth,unsigned int _pixelHeight );
		/*!
			\brief	get the rectangle pixel coordinates in the root viewport bitmap.
			\return	pixel
		*/	
		inline	int	GetPixelX1(){ return m_pixel_x1; };
		/*!
			\brief	get the rectangle pixel coordinates in the root viewport bitmap.
			\return	pixel
		*/	
		inline	int	GetPixelY1(){ return m_pixel_y1; };
		/*!
			\brief	get the rectangle pixel coordinates in the root viewport bitmap.
			\return	pixel
		*/	
		inline	int	GetPixelX2(){ return m_pixel_x2; };
		/*!
			\brief	get the rectangle pixel coordinates in the root viewport bitmap.
			\return	pixel
		*/	
		inline	int	GetPixelY2(){ return m_pixel_y2; };

		//! 2d Scale coordinates of the rectangle recomputed in the Root ViewPort scale.
		float m_ScaleRootX1;	
		float m_ScaleRootY1;
		float m_ScaleRootX2;
		float m_ScaleRootY2;

		//! 2d coordinates of the rectangle recomputed in the Root ViewPort scale.
		float m_ClippingRootX1;	
		float m_ClippingRootY1;
		float m_ClippingRootX2;
		float m_ClippingRootY2;

		//! real clipped frustrum
		float	m_FinalClipFrustrumX1;
		float	m_FinalClipFrustrumY1;
		float	m_FinalClipFrustrumX2;
		float	m_FinalClipFrustrumY2;

		//! in case of texture rendering, otherwise 0L:
		OGLInternalTexture	*m_pRenderTexture;
		//! layer index affected on texture: m_pRenderTexture.
		int		m_RenderTextureLayer;

		/*!
			\brief	Tool for other methods: Set the current GL viewport matrix to an InternalViewPort.
		*/
		virtual	void	SetGLToViewPort();

		virtual	void	Matrix_LoadID(eLoadId _enum=eLoadId_All);
		virtual	void	Matrix_Translate(float _x,float _y,float _z);
		virtual void    Matrix_Rotate( float _angle,float _x,float _y,float _z);
		virtual	void	Matrix_Scale(float _sx,float _sy,float _sz);
		virtual	void	Matrix_Push();
		virtual	void	Matrix_Pop();
	protected:
		//! machine that created us:
		OGLMachine	*m_pOGLMachine;
		//! if false, can't be drawn, because invalid coordinates.
		bool	m_Valid;
		//! the rectangle coordinates in term of screen pixel. 
		int		m_pixel_x1;	
		//! the rectangle coordinates in term of screen pixel. 
		int		m_pixel_y1;
		//! the rectangle coordinates in term of screen pixel. 
		int		m_pixel_x2;
		//! the rectangle coordinates in term of screen pixel. 
		int		m_pixel_y2;		
		/*
			\brief	Change the coodinates of the ViewPort pixels, in its root origin.
					Used by SetCoordinates(). Non-Virtual. Reset the hierarchy down.
		*/
		void	ResetPixelCoordinates();

		//! the pointer where this viewport was attached in the clipping hierarchy, father or brother. 
		//! Used to destroy the object. Use ClippingHierarchy_AttachTo to change it.
		InternalViewPort	**m_ppClippingHierarchyAttached;
#ifdef _ENGINE_EDITABLE_
		/*
			\brief	tool: invalidate down the viewport hierachy when a viewport is destroyed.
		*/
		void	ClippingDetach();
#endif
	};

	/*!
		\class	OGLInternalObject3DBuffer
		\brief	Nested class that implements a 3D object as a vertex and a polygon list.
				You ask the creation of it with VirtualMachine::NewObject3DBuffer(),
				then defines its shape by filling the list you get with GetFirstVertex()
				and GetFirstTriangle(), then it can be rendered with VirtualMachine::RenderMesh().
				In order to optimize a static shape, use VirtualMachine::CompileAsStatic()
				In edition mode, it can be destroyed with VirtualMachine::DeleteObject3DBuffer().
	*/
	class	OGLInternalObject3DBuffer : public InternalObject3DBuffer
	{ public:
		/*!
			\brief	A call to CompileAsStatic() will fix the current geometry of an object.
					It will try to use the 3D buffer compilation of a given machine, which can fasten the drawing.
					From that moment on, further modification to its shape will be useless.
		*/
		virtual void	CompileAsStatic( );
		/*!
			\brief	return true is CompileAsStatic() were used.
			\return	 true is CompileAsStatic() were used, false if still dynamic.
		*/
		virtual bool	IsCompiledAsStatic();
		/*!
			\brief	Get the beginning of the vertex list. Assume the maximum number of vertex 
					was given by NewObject3DBuffer() and can be read with GetMaximumNumberOfVertexes().
			\return a pointer to the first Vertex.
		*/
		virtual InternalVertex	 *GetFirstVertex();
		/*!
			\brief	Get the beginning of the Triangle list. Assume the maximum number of Triangle 
					was given by NewObject3DBuffer() and can be read with GetMaximumNumberOfTriangles().
					last Note: the first triangle returned  already use the offset given with SetNumberOfActiveTriangle(): _FirstActiveTriangleIndex.
			\return a pointer to the first Vertex.
		*/
		virtual InternalTriangle *GetFirstTriangle();
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
		virtual void	SetNumberOfActiveTriangle( unsigned int _CurrentNumberOfTriangle,unsigned int _FirstActiveTriangleIndex=0 );
		/*!
			\brief	Get the number of active triangle defined by SetNumberOfActiveTriangle().
			\return the number of triangle in the object.
		*/
		virtual unsigned int GetNumberOfActiveTriangle();
		/*!
			\brief	the maximum number of elements in table GetFirstVertex()
			\return  the maximum number of elements in table GetFirstVertex() 
		*/
		virtual unsigned int	GetMaximumNumberOfVertexes();
		/*!
			\brief	the maximum number of elements in table GetFirstTriangle()
			\return  the maximum number of elements in table GetFirstTriangle() 
		*/
		virtual unsigned int	GetMaximumNumberOfTriangles();
		/*!
			\brief	 Set the color for the whole object. Active if object was not created with bOb3D_EnableRGB or bOb3D_EnableAlpha.
			\param	_red	color component
			\param	_green	color component
			\param	_blue	color component
			\param	_alpha	color component
		*/
		virtual void	SetColor(float _red,float _green,float _blue,float _alpha=1.0f);


		//! real implementation of the vertex base.
		InternalVertex		*m_pVertexBase;
		//! real implementation of the triangle base.
		InternalTriangle	*m_pTriangleBase;
		//! If !=0, It is an OpenGL compiled process: 
		unsigned int		m_CompiledObjectHandler;
		//! First Active Triangle Index
		unsigned int	m_FirstActiveTriangleIndex;
		//! current number of active triangle set by SetNumberOfActiveTriangle()
		unsigned int	m_NumberOfActiveTriangles;
		//! the maximum number of vertex, and the size of the vertex list returned by GetFirstVertex().
		unsigned int	m_MaximumNumberOfVertexes;
		//! the maximum number of Triangles, and the size of the triangle list returned by GetFirstTriangle().
		unsigned int	m_MaximumNumberOfTriangles;
		//! tells which vertex members are enabled; use VirtualMachine::bOB3d_xxx
		unsigned int	m_ExtraMemberFlags;
		//! main color of the object(RGBA, [0.0,1.0]): (note:there is also vertex colors) 
		float			m_Color[4];
	};
	/*!
		\class	OGLInternalTexture
				Nested class that defines a texture to render an object with VirtualMachine::RenderMesh().
	*/
	class	OGLInternalTexture : public InternalTexture
	{ public:
		/*!
			\brief	Constructor
		*/
		OGLInternalTexture(void);
		/*!
			\brief	Set or change the render flags for a texture.
			\param	_flags or'ed bit flags ( itf_... flags)
		*/
		virtual void	SetRenderFlags( unsigned int _flags );
		/*!
			\brief	Set base color, default to 1.0f,1.0f,1.0f,1.0f
			\param	_rgba Red Green Blue and Alpha transparency components. 
		*/
		virtual void	SetBaseColor( float _rgba[4] );
		/*!
			\brief	Get the render flags for a texture.
			\return	 or'ed bit flags (InternalTextureFlagBitsEnum)
		*/
		virtual unsigned int GetRenderFlags( );
		/*!
			\brief	set an image layer for a texture. see layers with itile_... enums.
			\param	_layerToSet	the enum that define which layer to change.
			\param	_pByteChunk		pointer to a byte chunk 
			\param	_pixelByteDepth	number of bytes per pixel in _pByteChunk.
			\param	_pixelWidth		the number of pixel in a row
			\param	_pixelHeight
		*/
		virtual void	SetImageBitmap(	TextureImageLayerEnum _layerToSet,
								unsigned char *_pByteChunk,
								unsigned int _pixelByteDepth,
								unsigned int _pixelWidth,
								unsigned int _pixelHeight
								);
		static const int	m_MaxNumberOfLayer=2;
		//! \class Layer
		//! `\brief nested class for private use.
		class Layer { public:
			unsigned int	m_OGLTextureHandler;
			unsigned int	m_BitmapPixelWidth;
			unsigned int	m_BitmapPixelHeight;
		};
		//! OGL texture handlers: simple map+env.
		Layer		m_Layer[m_MaxNumberOfLayer];
		//! current flags set
		unsigned int	m_flags;
		//! the base color, which is modulated on mapping layer.
		float		m_BaseColorRGBA[4];
		//! tell which layer is active:
		unsigned int	m_ActiveLayerFlag;
		//! in case of render to texture with render extension:
		GLuint m_frameBuffer;
		//! in case of render to texture with render extension:
		GLuint m_depthRenderBuffer;
	};
	
	/*!
		\brief	Init the machine. It would open a screen and do the init for each theme (3D,sound,...)
				the machine closing should be done at deletion.
		\return vmr_OK or vmr_FAILED
	*/
	virtual eVMResult	InitMachine();
	/*!
		\brief	Swap default VP buffer: so that what was drawn is shown.
	*/
	virtual	void	SwapMainScreenBuffer()=0;

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
	virtual void	NewObject3DBuffer(  InternalObject3DBuffer **_ppVertexAndPolygonBufferOut, 
							unsigned int _maxNbVertex,
							unsigned int _maxNbTriangle, 
							unsigned int _extraMemberFlags );

	/*
		\brief		Destroy objects previously created with NewObject3DBuffer()
		\param		_ppBufferToDestroy object to destroy, and set to 0L.
	*/
	virtual void	DeleteObject3DBuffer(  InternalObject3DBuffer **_ppBufferToDestroy );

	/*!
		\brief	Ask the Creation of a new InternalTexture, then used to render 3D objects with RenderMesh().
				In edition mode, InternalTexture has to be destroyed with DeleteTexture().
		\param	_ppTextureOut	the object returned, or 0L if failed.(no mem ?)
	*/
	virtual void	NewTexture(  InternalTexture **_ppTextureOut  );
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destroy objects previously created with NewTexture()
		\param	_ppTextureToDestroy	 object to destroy, and set to 0L.
	*/
	virtual void	DeleteTexture(  InternalTexture **_ppTextureToDestroy );
#endif

	/*!
		\brief	Ask the Creation of a new InternalViewPort, that must be the child of another ViewPort,
				specified by _pFatherViewPort.if no father is specified, the new ViewPort will be
				the child of the default root ViewPort. To create another Root ViewPort, create a texture
				and use VirtualMedia::NewViewPortOnTexture().
				In edition mode, each InternalViewPort has to be destroyed with DeleteViewPort().
		\param	_ppViewPortOut	the object returned, or 0L if failed.(no mem ?)
		\param	_pFatherViewPort the father viewport. If 0L, it uses the default root.
	*/
	virtual void	NewViewPortChild(  InternalViewPort **_ppViewPortOut,InternalViewPort *_pFatherViewPort=0L);
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
	virtual void	NewViewPortOnTexture(  InternalViewPort **_ppViewPortOut,
											InternalTexture *_pRootTexture,
											unsigned int _np2PixelWidth,
											unsigned int _np2PixelHeight,
											InternalTexture::TextureImageLayerEnum _layerToAffect);


	/*!
		\brief	Destroy any objects previously created with NewViewPortChild() or NewViewPortOnTexture().
		\param	_ppViewPortToDestroy	 pointer to object to destroy, and set to 0L.
	*/
	virtual void	DeleteViewPort(  InternalViewPort **_ppViewPortToDestroy );


	// extension for multiple texture units:
	static	PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
	static	PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB;
	
	// GL_EXT_framebuffer_object related extension, (render-to-texture)
	static	bool	m_RenderObjectExtension;
	// in order of appearance:
	static PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT; // init
	static PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT; // init
	static PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT ; // init
	static PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT; // init
	static PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT; // end
	static PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT; //end
	static PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT; // draw
	static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT; //draw
	static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT ; //draw


	//static PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT;
/*	static PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT = NULL;
	static PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT = NULL;

	static PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT = NULL;
	static PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT = NULL;
	static PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT = NULL;
	static PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
	static PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = NULL;

	static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT = NULL;
	static PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT = NULL;
	static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
	static PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT = NULL;
	static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT = NULL;
	static PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT = NULL;
	static PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT = NULL;
*/
		/*
		glIsRenderbufferEXT = (glIsRenderbufferEXTPROC) getGLExtensionProcAddress("glIsRenderbufferEXT");
		glBindRenderbufferEXT = (glBindRenderbufferEXTPROC) getGLExtensionProcAddress("glBindRenderbufferEXT");
		glDeleteRenderbuffersEXT = (glDeleteRenderbuffersEXTPROC) getGLExtensionProcAddress("glDeleteRenderbuffersEXT");
		glGenRenderbuffersEXT = (glGenRenderbuffersEXTPROC) getGLExtensionProcAddress("glGenRenderbuffersEXT");
		glRenderbufferStorageEXT = (glRenderbufferStorageEXTPROC) getGLExtensionProcAddress("glRenderbufferStorageEXT");
		glGetRenderbufferParameterivEXT = (glGetRenderbufferParameterivEXTPROC) getGLExtensionProcAddress("glGetRenderbufferParameterivEXT");
		glIsFramebufferEXT = (glIsFramebufferEXTPROC) getGLExtensionProcAddress("glIsFramebufferEXT");
		glBindFramebufferEXT = (glBindFramebufferEXTPROC) getGLExtensionProcAddress("glBindFramebufferEXT");
		glDeleteFramebuffersEXT = (glDeleteFramebuffersEXTPROC) getGLExtensionProcAddress("glDeleteFramebuffersEXT");
		glGenFramebuffersEXT = (glGenFramebuffersEXTPROC) getGLExtensionProcAddress("glGenFramebuffersEXT");
		glCheckFramebufferStatusEXT = (glCheckFramebufferStatusEXTPROC) getGLExtensionProcAddress("glCheckFramebufferStatusEXT");
		glFramebufferTexture1DEXT = (glFramebufferTexture1DEXTPROC) getGLExtensionProcAddress("glFramebufferTexture1DEXT");
		glFramebufferTexture2DEXT = (glFramebufferTexture2DEXTPROC) getGLExtensionProcAddress("glFramebufferTexture2DEXT");
		glFramebufferTexture3DEXT = (glFramebufferTexture3DEXTPROC) getGLExtensionProcAddress("glFramebufferTexture3DEXT");
		glFramebufferRenderbufferEXT = (glFramebufferRenderbufferEXTPROC) getGLExtensionProcAddress("glFramebufferRenderbufferEXT");
		glGetFramebufferAttachmentParameterivEXT = (glGetFramebufferAttachmentParameterivEXTPROC) getGLExtensionProcAddress("glGetFramebufferAttachmentParameterivEXT");
		glGenerateMipmapEXT = (glGenerateMipmapEXTPROC) getGLExtensionProcAddress("glGenerateMipmapEXT");
*/

/*==================================================================
								PROTECTED
==================================================================*/
protected:

};


#endif
