#pragma once
#include "OGLMachineWinDxSound.h"

class COpenGLWnd;

class CPreviewMachine :
	public OGLMachineWinDxSound
{
public:
	CPreviewMachine(void);
	virtual ~CPreviewMachine(void);

	void	InitFromMFCView(  COpenGLWnd *_pMFCOpenGLView );
	void	SetGLMFCView( COpenGLWnd	*_pView){ m_pGLMFCView = _pView; };

	virtual eVMResult	InitViewPort(  InternalViewPort **_ppViewPortOut  );

	virtual eVMResult	InitMachine();

	bool	SetContext(void);

	inline void	SetMachineLocalization( unsigned int _locCode)
	{
		m_MachineLocalization = (eMachineLocalization) _locCode;
	};

	virtual void	NewObject3DBuffer(  InternalObject3DBuffer **_ppVertexAndPolygonBufferOut,
							unsigned int _maxNbVertex,
							unsigned int _maxNbTriangle,
							unsigned int _extraMemberFlags );
	virtual void	NewTexture(  InternalTexture **_ppTextureOut  );

	virtual void	DeleteObject3DBuffer(  InternalObject3DBuffer **_ppBufferToDestroy );
	virtual void	DeleteTexture(  InternalTexture **_ppTextureToDestroy );

	virtual void	NewViewPortChild(  InternalViewPort **_ppViewPortOut,InternalViewPort *_pFatherViewPort=0L);

	virtual void	NewViewPortOnTexture(  InternalViewPort **_ppViewPortOut,InternalTexture *_pRootTexture );

	virtual	void	SwapMainScreenBuffer();

	void	OnSize( int _width, int _height);
#ifdef _ENGINE_EDITABLE_

	virtual void	DeleteViewPort(  InternalViewPort **_ppViewPortToDestroy );
#endif

	class	PreviewOGLInternalViewPort : public OGLInternalViewPort
	{	public:

		PreviewOGLInternalViewPort(OGLInternalViewPort *_pFather,CPreviewMachine *_pManager);
#ifdef _ENGINE_EDITABLE_

		virtual ~PreviewOGLInternalViewPort();
#endif

		virtual	void	DrawRGBARectangle(
											unsigned char	*_prgbaTable,
											unsigned int	_PixelWidth,
											unsigned int	_PixelHeight
											) ;

		virtual	void	Clear();

		virtual	void	RenderMesh(
									InternalObject3DBuffer *_pMeshBuffer,
									InternalTexture *_pTexture
									) ;

		virtual	void	SwapBuffer();

		virtual	void	Matrix_LoadID();
		virtual	void	Matrix_Translate(float _x,float _y,float _z);
		virtual void    Matrix_Rotate( float _angle,float _x,float _y,float _z);
		virtual	void	Matrix_Scale(float _sx,float _sy,float _sz);
		virtual	void	Matrix_Push();
		virtual	void	Matrix_Pop();

		PreviewOGLInternalViewPort *GetViewportAtPixel( int _xPosition,int _yPosition,
								float	&_returnXValue,float	&_returnYValue ) ;

	protected:

		virtual	void	SetGLToViewPort( );

		CPreviewMachine	*m_pManager;

	};

	class	PreviewOGLInternalTexture : public OGLInternalTexture
	{ public:

		PreviewOGLInternalTexture();
		virtual void	SetRenderFlags( unsigned int _flags );

		virtual void	SetImageBitmap(	TextureImageLayerEnum _layerToSet,
								unsigned char *_pByteChunk,
								unsigned int _pixelByteDepth,
								unsigned int _pixelWidth,
								unsigned int _pixelHeight
								);
		CPreviewMachine	*m_pPreviewMachine;

	};

	inline unsigned int	GetMaxNumberOfChildViewport(){ return m_MaxNumberOfChildView; };

	void	GetChildViewport(
		InternalViewPort *&_pchildViewport,
		InternalViewPort *&_ppositionViewPort,
		unsigned int _index=0);

protected:

	static	CPreviewMachine *m_pCurrentGLContextMachine;

	COpenGLWnd	*m_pGLMFCView;

	static const unsigned int	m_MaxNumberOfChildView=1;
	struct ChildViewPort
	{

		InternalViewPort	*m_pPositionInScreenViewPort;

		InternalViewPort	*m_ChildViewPort;
	};

	struct ChildViewPort m_ChildViewportTable[m_MaxNumberOfChildView];

};
