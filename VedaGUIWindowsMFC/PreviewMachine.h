#pragma once
#include "OGLMachineWinDxSound.h"

class COpenGLWnd;
/*!
	\class	CPreviewMachine

	\brief	 It is an extension of VirtualMachine, that extends a machine for
			a COpenGLWnd preview use. It is the machine used by the context at edition. 

*/
class CPreviewMachine :
	public OGLMachineWinDxSound
{
public:
	CPreviewMachine(void);
	virtual ~CPreviewMachine(void);

	/*!
		\brief	Init
	*/
	void	InitFromMFCView(  COpenGLWnd *_pMFCOpenGLView );
	void	SetGLMFCView( COpenGLWnd	*_pView){ m_pGLMFCView = _pView; };
	/*!
		\brief	Init
	*/
	virtual eVMResult	InitViewPort(  InternalViewPort **_ppViewPortOut  );
	/*!
		\brief	Create and open the main screen
	*/
	virtual eVMResult	InitMachine();
	/*!
		\brief	Tool for other methods: Set the opengl context for this preview windows
		\return true if context switch suceed. else, no operation should be done.
	*/
	bool	SetContext(void);

	/*
		\brief force localization for preview:
	*/
	inline void	SetMachineLocalization( unsigned int _locCode)
	{
		m_MachineLocalization = (eMachineLocalization) _locCode;
	};

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
	virtual void	NewTexture(  InternalTexture **_ppTextureOut  );

	virtual void	DeleteObject3DBuffer(  InternalObject3DBuffer **_ppBufferToDestroy );
	virtual void	DeleteTexture(  InternalTexture **_ppTextureToDestroy );
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
	*/
	virtual void	NewViewPortOnTexture(  InternalViewPort **_ppViewPortOut,InternalTexture *_pRootTexture );
	/*!
		\brief	Swap buffer so that what was drawn is shown.
	*/
	virtual	void	SwapMainScreenBuffer();
	/*
		\brief notification of a window resize:
	*/
	void	OnSize( int _width, int _height);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destroy any objects previously created with NewViewPortChild() or NewViewPortOnTexture().
		\param	_ppViewPortToDestroy	 pointer to object to destroy, and set to 0L.
	*/
	virtual void	DeleteViewPort(  InternalViewPort **_ppViewPortToDestroy );
#endif

	/*!
		\class	nested class to define a viewport where to draw (screen, sub-screen,texture rendering screens.)
			It can be extented to subclasses.
	*/
	class	PreviewOGLInternalViewPort : public OGLInternalViewPort
	{	public:
		/*!
			\brief	Constructor. 
			\param	_pFather the father, or NULL if new root.
			\param	_pManager the machine that built and manage the viewport.
		*/
		PreviewOGLInternalViewPort(OGLInternalViewPort *_pFather,CPreviewMachine *_pManager);
#ifdef _ENGINE_EDITABLE_
		/*!
			\brief	Destructor. See DeleteViewport() 
		*/
		virtual ~PreviewOGLInternalViewPort();
#endif

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
			\brief	Reset color to black and depth buffer.
		*/
		virtual	void	Clear();
		/*!
			\brief	Render a 3D Object. The vertexes are transformed with matrix _pMatrix,
					the object shape is defined by _pMeshBuffer, the texture of the object is
					defined by _pTexture, the ViewPort that define the rectangle where to draw,
					and the camera parameters is _pViewPort, or the default viewport if not specified.	
					Note drawings on the main screen needs a SwapScreenBuffer() to be shown at each frames.
			\param	_pMeshBuffer InternalObject3DBuffer
			\param	_pTexture InternalTexture
		*/
		virtual	void	RenderMesh(
									InternalObject3DBuffer *_pMeshBuffer,
									InternalTexture *_pTexture
									) ;
		/*!
			\brief	Swap buffer so that what was drawn on the viewport is shown.
					this is needed to active all Clear(), RenderMesh() ,...
					It has to be done with main screen viewport like texture-rendering
					viewports.
		*/
		virtual	void	SwapBuffer();

		virtual	void	Matrix_LoadID();
		virtual	void	Matrix_Translate(float _x,float _y,float _z);
		virtual void    Matrix_Rotate( float _angle,float _x,float _y,float _z);
		virtual	void	Matrix_Scale(float _sx,float _sy,float _sz);
		virtual	void	Matrix_Push();
		virtual	void	Matrix_Pop();

		/*!
			\brief	return the viewport in which a pixel coordinate is.
					if a child viewport is also surrounding, the child is returned.
					It also return the value of this pixel position in the viewport's ratio.
			\param	_xPosition pixel position in the root viewport.
			\param	_yPosition pixel position in the root viewport.
			\param	_returnXValue	if return is not null, the pixel's value in the VP ratio.
			\param	_returnYValue	if return is not null, the pixel's value in the VP ratio.

		*/	
		PreviewOGLInternalViewPort *GetViewportAtPixel( int _xPosition,int _yPosition,
								float	&_returnXValue,float	&_returnYValue ) ;


	protected:
		/*!
			\brief	Tool for other methods: Set the current GL viewport matrix to an InternalViewPort.
		*/
		virtual	void	SetGLToViewPort( );
		//! machine that manages this viewport.
		CPreviewMachine	*m_pManager;

	};
	/*!
		\class	InternalTexture
				Nested class that define a texture
	*/
	class	PreviewOGLInternalTexture : public OGLInternalTexture
	{ public:
	
		// constructor;
		PreviewOGLInternalTexture();
		virtual void	SetRenderFlags( unsigned int _flags );
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
		CPreviewMachine	*m_pPreviewMachine;

	};

	/*!
		\brief	return the maximum number of preview viewport, childs of the main viewport.
		\return the maximum number of preview viewport
	*/
	inline unsigned int	GetMaxNumberOfChildViewport(){ return m_MaxNumberOfChildView; };
	/*!
		\brief	return a preview viewport, child of the main viewport.
		\param	_childViewport an InternalViewPort ready to draw, or NULL if index too high.
		\param	_positionViewPort 
		\param	_index the index since zero, or nothing.

	*/
	void	GetChildViewport(
		InternalViewPort *&_pchildViewport,
		InternalViewPort *&_ppositionViewPort,
		unsigned int _index=0);

protected:
	//! the machine currently set for a wgl context:
	static	CPreviewMachine *m_pCurrentGLContextMachine;
	//! the view that manage this machine, in the MFC way.
	COpenGLWnd	*m_pGLMFCView;
	//! the number of child view
	static const unsigned int	m_MaxNumberOfChildView=1;
	struct ChildViewPort
	{
		//! a first viewport son is used to scale inside main preview
		InternalViewPort	*m_pPositionInScreenViewPort;
		//! and another child is up to the preview function.
		InternalViewPort	*m_ChildViewPort;		
	};
	//! a table of child of VirtualMachine::m_pDefaultViewPort.
	//old InternalViewPort		*m_ChildViewportTable[m_MaxNumberOfChildView];
	struct ChildViewPort m_ChildViewportTable[m_MaxNumberOfChildView];

};
