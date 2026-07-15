// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_VirtualMachine_H
#define COM_M4NKIND_VirtualMachine_H

class VirtualMachine
{

public:

	VirtualMachine(void);
#ifdef _ENGINE_EDITABLE_

	virtual ~VirtualMachine(void);
#endif

	typedef enum  {

		vmr_OK=0,

		vmr_FAILED
	} eVMResult;

	virtual eVMResult	InitMachine()=0;

	class	InternalVertex
	{ public:

		float	m_x,m_y,m_z;

		float	m_nx,m_ny,m_nz;

		float	m_u,m_v;

		float m_ColorRGBA[4];

		inline	void	SetPosition( float _x,float _y, float _z ){ m_x = _x; m_y = _y; m_z = _z ; };

		inline	void	SetUV( float _u,float _v){ m_u = _u; m_v = _v;};

		inline	void	SetPositionUV( float _x,float _y, float _z )
								{ m_x = m_u = _x; m_v = m_y = _y; m_z = _z ; };

		inline	void	SetNormalVector(  float _x,float _y, float _z ){ m_nx = _x; m_ny = _y; m_nz = _z ; };
	};

	class	InternalTriangle
	{ public:

		unsigned int m_p0,m_p1,m_p2;
	};

	class	InternalObject3DBuffer
	{ public:

		virtual InternalVertex	 *GetFirstVertex()=0;

		virtual InternalTriangle *GetFirstTriangle()=0;

		virtual void	SetNumberOfActiveTriangle( unsigned int _CurrentNumberOfTriangle,unsigned int _FirstActiveTriangleIndex=0 )=0;

		virtual unsigned int GetNumberOfActiveTriangle()=0;

		virtual unsigned int	GetMaximumNumberOfVertexes()=0;

		virtual unsigned int	GetMaximumNumberOfTriangles()=0;

		virtual void	SetColor(float _red,float _green,float _blue,float _alpha=1.0f)=0;

		virtual void	CompileAsStatic()=0;

		virtual bool	IsCompiledAsStatic()=0;
	};

	class	InternalTexture
	{ public:

		typedef enum {

			itf_Filtered=1

			,itf_Additive=2

			,itf_envAdd=4

			,itf_ClampTxtU=8

			,itf_ClampTxtV=16

			,itf_depthwrite=32

			,itf_depthtest=64

			,itf_substractive=128

			,itf_doublesided=256
		} InternalTextureFlagBitsEnum;

		virtual void	SetRenderFlags( unsigned int _flags )=0;

		virtual void	SetBaseColor( float _rgba[4] )=0;

		virtual unsigned int GetRenderFlags( )=0;

		typedef	enum {

			itile_TextureMapping=0,

			itile_Environment
		} TextureImageLayerEnum;

		virtual void	SetImageBitmap(	TextureImageLayerEnum _layerToSet,
									unsigned char *_pByteChunk,
									unsigned int _pixelByteDepth,
									unsigned int _pixelWidth,
									unsigned int _pixelHeight
									)=0;
	};

	class	InternalViewPort
	{	public:

		virtual	void	Clear( float _red=0.0f,float _green=0.0f,float _blue=0.0f)=0;

		virtual	void	RenderMesh(
									InternalObject3DBuffer *_pMeshBuffer,
									InternalTexture *_pTexture=0L
									) =0 ;

		virtual	void	DrawRGBARectangle(
											unsigned char	*_prgbaTable,
											unsigned int	_PixelWidth,
											unsigned int	_PixelHeight
											) =0 ;

		virtual	void	SwapBuffer()=0;

		inline	void	SetFOVLength( float _fovlength ){ m_fov = _fovlength; };

		inline	void	SetNearCutPlane( float _neardistance ){ m_NearCutPlane = _neardistance; };

		inline	void	SetFarCutPlane( float _fardistance ){ m_FarCutPlane = _fardistance; };

		virtual	void SetClippingCoordinates(float _ClippingX1,float _ClippingY1,float _ClippingX2,float _ClippingY2 )=0;

		virtual void SetScaleCoordinates(float _ScaleX1,float _ScaleY1,float _ScaleX2,float _ScaleY2 )=0;

		inline float	GetPositionX1(){ return m_ScaleX1; };

		inline float	GetPositionY1(){ return m_ScaleY1; };

		inline float	GetPositionX2(){ return m_ScaleX2; };

		inline float	GetPositionY2(){ return m_ScaleY2; };

		inline float	GetClippingX1(){ return m_ClippingX1; };

		inline float	GetClippingY1(){ return m_ClippingY1; };

		inline float	GetClippingX2(){ return m_ClippingX2; };

		inline float	GetClippingY2(){ return m_ClippingY2; };

		inline InternalViewPort	*GetRootViewPort(){ return m_pRootViewPort; };

		inline InternalViewPort	*GetFatherViewPort(){ return m_pClippingFatherViewPort; };

		typedef enum {

			eLoadId_All=0,

			eLoadId_Rotation
		} eLoadId ;

		virtual	void	Matrix_LoadID(eLoadId _enum=eLoadId_All)=0;

		virtual	void	Matrix_Translate(float _x,float _y,float _z)=0;

		virtual void    Matrix_Rotate( float _angleRad,float _x,float _y,float _z)=0;

		virtual	void	Matrix_Scale(float _sx,float _sy,float _sz)=0;

		virtual	void	Matrix_Push()=0;

		virtual	void	Matrix_Pop()=0;
	protected:

		float m_ScaleX1;
		float m_ScaleY1;
		float m_ScaleX2;
		float m_ScaleY2;

		float m_ClippingX1;
		float m_ClippingY1;
		float m_ClippingX2;
		float m_ClippingY2;

		InternalViewPort	*m_pRootViewPort;

		InternalViewPort	*m_pPositionFatherViewPort;

		InternalViewPort	*m_pPositionFirstSonViewPort;

		InternalViewPort	*m_pPositionNextBrotherViewPort;

		InternalViewPort	*m_pClippingFatherViewPort;

		InternalViewPort	*m_pClippingFirstSonViewPort;

		InternalViewPort	*m_pClippingNextBrotherViewPort;

		float m_fov;

		float m_NearCutPlane;

		float m_FarCutPlane;

	};

	typedef enum {

		bOb3D_VertexNormal=1,

		bOb3D_VertexUVMapping=2,

		bOb3D_VertexRGBA=4
	} InternalObject3DBuffer_InitBit ;

	virtual void	NewObject3DBuffer(  InternalObject3DBuffer **_ppVertexAndPolygonBufferOut,
							unsigned int _maxNbVertex,
							unsigned int _maxNbTriangle,
							unsigned int _extraMemberFlags )=0;

	virtual void	DeleteObject3DBuffer(  InternalObject3DBuffer **_ppBufferToDestroy )=0;

	virtual void	NewTexture(  InternalTexture **_ppTextureOut  )=0;

#ifdef _ENGINE_EDITABLE_

	virtual void	DeleteTexture(  InternalTexture **_ppTextureToDestroy )=0;
#endif

	virtual void	NewViewPortChild(  InternalViewPort **_ppViewPortOut,InternalViewPort *_pFatherViewPort=0L)=0;

	virtual void	NewViewPortOnTexture(  InternalViewPort **_ppViewPortOut,
											InternalTexture *_pRootTexture,
											unsigned int _np2PixelWidth,
											unsigned int _np2PixelHeight,
											InternalTexture::TextureImageLayerEnum _layerToAffect)=0;

	virtual void	DeleteViewPort(  InternalViewPort **_ppViewPortToDestroy )=0;

	inline InternalViewPort	*GetDefaultViewPort(){ return m_pDefaultViewPort; };

	class	SoundBufferToAddYourSignal
	{ public:

		float	*m_pSoundBuffer;

		unsigned int m_LengthToRender;

		float		m_PlayFrequency;

		int				m_StartPlayTime_SecondUnit;

		unsigned int	m_StartPlayTime_SecondFrac;

		int				m_EndPlayTime_SecondUnit;

		unsigned int	m_EndPlayTime_SecondFrac;
	};

	class	SoundInterface
	{ public:

		virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )=0;

	};

	virtual	float	GetPlayFrequency()=0;

	virtual	void	EnableMediaSound( SoundInterface *_pNoisyObject, bool _enable=true );

	virtual	void	SetSoundCurrentTime(int _seconds,unsigned int _fraction )=0;

	virtual	void	SetSoundVolume(float _newVolumeValue)=0;

	virtual	float	GetSoundVolume()=0;

	virtual	void	GetCurrentSoundBufferImage(float ** const _ppSoundBuffer,unsigned int *_pLength) const =0 ;

#ifdef _ENGINE_EDITABLE_

	virtual	void	ShutTheFuckUp();
#endif

	typedef enum{

		eMachineLocalization_LocalisationNotImplemented=0,

		eMachineLocalization_English,

		eMachineLocalization_French,

		eMachineLocalization_SpanishCastellano,

		eMachineLocalization_German,

		eMachineLocalization_Italian,

		eMachineLocalization_Polish,

		eMachineLocalization_Portuguese
	} eMachineLocalization;

	virtual eMachineLocalization	GetCurrentWorldLocalizationEnum()=0;

	virtual void	ProcessInterface()=0;

	inline	bool	GetQuitMessage(){ return m_QuitMessage; };

	virtual	void	SetMachineTitle(const char *_pMachineTitle);

#ifdef _ENGINE_EDITABLE_

	virtual	bool	FileRequester(const char *_pDisplayString,char *_pResultFileName, unsigned int _nameMaxLength )=0;
#endif
#ifdef _ENGINE_EDITABLE_

#endif
#ifdef _ENGINE_EDITABLE_

	virtual	bool SaveFile(const char *_pFilePath,const char *_chunkToWrite,unsigned int _ChunkLength,bool _append=false)=0;
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void Sleep( unsigned int _milliseconds )=0;
#endif

protected:

	InternalViewPort	*m_pDefaultViewPort;

	bool			m_QuitMessage;

	class	SoundUpdateListCell
	{ public:
		SoundUpdateListCell			*m_pNext;
		SoundInterface				*m_pMediaObject;
	};

	SoundUpdateListCell	*m_pFirstSoundObject;
};
#endif
