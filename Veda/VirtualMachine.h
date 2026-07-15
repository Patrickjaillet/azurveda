/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_VirtualMachine_H
#define COM_M4NKIND_VirtualMachine_H

/*!
	\addtogroup	VirtualMachineDocGroup VirtualMachine Related Documentations
	\brief		Here are links to documentations related to VirtualMachine.
				It is a pure virtual class for hardware abstraction provided in the base library.
				Here, You can also access to the different groups of methods of 
				that class by theme, and possible inherited implementations ! 
				In each Machine project directory, you should find a version of
				VedaDefaultMachine.h , which typedefs a VedaDefaultMachine class to the 
				default Machine class.				
				Note VirtualMachine is completely independant from other APIs and includes,
				so this documentations will also keep independants.
*/
/*!
	\class	VirtualMachine
	\ingroup	VirtualMachineDocGroup
	\author	victorien -dot- ferry at free -dot- fr, with the mankind demo group team: www.m4nkind.com
	\brief	Pure Virtual class for all machine specific methods.
			These methods should be completed for each machines in separate inherited libraries.
			This class will keep 100% independant from any other classes or API.
			This should be possible to only use a VirtualMachine for a whole program,
			to reach hardware abstraction.<br>
			Methods documentation has been grouped by theme:
				- the \ref InitMethods "Init Methods"
				- the \ref Render3D "3D Render Nested Classes and Methods"
				- the \ref MachineSound "Sound Methods"
				- the \ref MachineCultural "Cultural Configuration Methods"
				- the \ref MachineInterface "Interface Methods"
				- the \ref MachineDOS "Disk Operating and System Methods"
*/

class VirtualMachine
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*! \addtogroup	InitMethods	VirtualMachine Init Methods
		\ingroup	VirtualMachineDocGroup
		\brief	Init Methods for VirtualMachine
		\{ 
	*/
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	VirtualMachine(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~VirtualMachine(void);
#endif
	/*!
		\typedef eVMResult
		\brief	error enum for InitMachine()
	*/
	typedef enum  {
		//! init OK
		vmr_OK=0,
		//! init failed
		vmr_FAILED
	} eVMResult;
	/*!
		\brief	Init the machine. It would open a screen and do the init for each theme (3D,sound,...)
				Then, the Main Screen stands as the ViewPort GetDefaultViewPort() . 
				the machine closing should be done at deletion.
		\return vmr_OK or vmr_FAILED
	*/
	virtual eVMResult	InitMachine()=0;
	//!	\}
/*==================================================================
					3D Render Model.
==================================================================*/
	/*!	\addtogroup	Render3D VirtualMachine 3D Render Methods
		\ingroup	VirtualMachineDocGroup
		\brief	Nested classes and methods for graphic rendering with a VirtualMachine.
				This is thought as short as possible, but with enough features
				to support a classic 3D rendering state of the art: It has multiple layer
				for textures (InternalTexture::SetImageBitmap() ), and 3D object compilation
				if an object got a static shape. ( InternalObject3DBuffer::CompileAsStatic() )
				It is also planned to support virtual screens and rectangle hierarchy
				with nested class InternalViewPort. You draw 3D objects with InternalViewPort::RenderMesh().
	*/
	/*!
		\class	InternalVertex
		\ingroup Render3D
		\brief Nested class that defines a vertex for use with
			InternalObject3DBuffer::GetFirstVertex(). Note: this vertex definition
			should not change (no more member or virtual methods).
			It is sometimes used as a list element.
	*/
	class	InternalVertex
	{ public:
		//! the vertex position. Always enabled.
		float	m_x,m_y,m_z;
		//! the vertex normal, norm must be 1.0.
		float	m_nx,m_ny,m_nz;
		//! the 2D texture coordinates if there are:
		float	m_u,m_v;
		//! the color/alpha transluency of the vertex.
		float m_ColorRGBA[4];
		/*!
			\brief	Set the x,y,z position of the vertex.
		*/	
		inline	void	SetPosition( float _x,float _y, float _z ){ m_x = _x; m_y = _y; m_z = _z ; };
		/*!
			\brief	Set the UV coordinate for texture mapping for the vertex. 
		*/	
		inline	void	SetUV( float _u,float _v){ m_u = _u; m_v = _v;};
		/*!
			\brief	set in the same time position of the vertex, and also apply to the UV texture coordinates.
		*/	
		inline	void	SetPositionUV( float _x,float _y, float _z )
								{ m_x = m_u = _x; m_v = m_y = _y; m_z = _z ; };
		/*!
			\brief	set Normal Vector for this vertex. It must be normalized.
		*/
		inline	void	SetNormalVector(  float _x,float _y, float _z ){ m_nx = _x; m_ny = _y; m_nz = _z ; };
	};
	/*!
		\class	InternalTriangle
		\ingroup Render3D
		\brief Nested class that defines a Triangle Polygon for use with 
			InternalObject3DBuffer::GetFirstTriangle(). Note: this triangle definition
			should not change (no more member or virtual methods).sizeof(InternalTriangle) 
			must always be 12 because it is sometimes used as a list element.
	*/
	class	InternalTriangle
	{ public:
		//! the indexes of the polygon.
		unsigned int m_p0,m_p1,m_p2;
	};
	/*!
		\class	InternalObject3DBuffer
		\ingroup Render3D
		\brief	Nested class to define a 3D object as a vertex and a polygon list.
				You ask the creation of it with VirtualMachine::NewObject3DBuffer(),
				then defines its shape by filling the list you get with GetFirstVertex()
				and GetFirstTriangle(), then it can be rendered with RenderMesh().
				In order to optimize a static shape, use CompileAsStatic()
				In edition mode, it can be destroyed with VirtualMachine::DeleteObject3DBuffer().
	*/
	class	InternalObject3DBuffer
	{ public:
		/*!
			\brief	Get the beginning of the vertex list. Assume the maximum number of vertex 
					was given by NewObject3DBuffer() and can be read with GetMaximumNumberOfVertexes().
			\return a pointer to the first Vertex.
		*/
		virtual InternalVertex	 *GetFirstVertex()=0;
		/*!
			\brief	Get the beginning of the Triangle list. Assume the maximum number of Triangle 
					was given by NewObject3DBuffer() and can be read with GetMaximumNumberOfTriangles().
					last Note: the first triangle returned  already use the offset given with SetNumberOfActiveTriangle(): _FirstActiveTriangleIndex.
			\return a pointer to the first Vertex.
		*/
		virtual InternalTriangle *GetFirstTriangle()=0;
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
		virtual void	SetNumberOfActiveTriangle( unsigned int _CurrentNumberOfTriangle,unsigned int _FirstActiveTriangleIndex=0 )=0;
		/*!
			\brief	Get the number of active triangle defined by SetNumberOfActiveTriangle().
			\return the number of triangle in the object.
		*/
		virtual unsigned int GetNumberOfActiveTriangle()=0;
		/*!
			\brief	the maximum number of elements in table GetFirstVertex()
			\return  the maximum number of elements in table GetFirstVertex() 
		*/
		virtual unsigned int	GetMaximumNumberOfVertexes()=0;
		/*!
			\brief	the maximum number of elements in table GetFirstTriangle()
			\return  the maximum number of elements in table GetFirstTriangle() 
		*/
		virtual unsigned int	GetMaximumNumberOfTriangles()=0;
		/*!
			\brief	 Set the color for the whole object. Active if object was not created with bOb3D_EnableRGBA.
			\param	_red	color component
			\param	_green	color component
			\param	_blue	color component
			\param	_alpha	color component
		*/
		virtual void	SetColor(float _red,float _green,float _blue,float _alpha=1.0f)=0;
		/*!
			\brief	A call to CompileAsStatic() will fix the current geometry of an object.
					It will try to use the 3D buffer compilation of a given machine, which can fasten the drawing.
					From that moment on, further modification to its shape will be useless.
		*/
		virtual void	CompileAsStatic()=0;
		/*!
			\brief	return true is CompileAsStatic() were used.
			\return	 true is CompileAsStatic() were used, false if still dynamic.
		*/
		virtual bool	IsCompiledAsStatic()=0;
	};
	/*!
		\class	InternalTexture
		\ingroup Render3D
		\brief	Nested class that defines a texture to render an object with VirtualMachine::RenderMesh().
	*/
	class	InternalTexture
	{ public:
		//! \brief InternalTexture::SetRenderFlags() enum.
		typedef enum {
			//! Bilinear filtering
			itf_Filtered=1
			//! drawing will be additive over previous drawings.
			,itf_Additive=2
			//! the environment layer will be additive over the mapping/color layer.
			,itf_envAdd=4
			//! texture bitmap horizontal clamping.
			,itf_ClampTxtU=8
			//! texture bitmap vertical clamping.
			,itf_ClampTxtV=16
			//! drawing writes the depth buffer.
			,itf_depthwrite=32
			//! drawing test the depth buffer.
			,itf_depthtest=64
			//! drawing will be substractive over previous drawings.
			,itf_substractive=128 // very rare bits should start at bit 7 for better compression with PackULong_Flags.
			//! back faces are also drawn.
			,itf_doublesided=256
		} InternalTextureFlagBitsEnum;
		/*!
			\brief	Set or change the render flags for a texture.
			\param	_flags or'ed bit flags ( InternalTextureFlagBitsEnum)
		*/
		virtual void	SetRenderFlags( unsigned int _flags )=0;
		/*!
			\brief	Set base color, default to 1.0f,1.0f,1.0f,1.0f
			\param	_rgba Red Green Blue and Alpha transparency components. 
		*/
		virtual void	SetBaseColor( float _rgba[4] )=0;
		/*!
			\brief	Get the render flags for a texture.
			\return	 or'ed bit flags (InternalTextureFlagBitsEnum)
		*/
		virtual unsigned int GetRenderFlags( )=0;
		//! \typedef TextureImageLayerEnum
		//! \brief InternalTexture image layer enum to use with SetImageBitmap()
		typedef	enum {
			//! for texture mapping layer
			itile_TextureMapping=0,
			//! for environment layer
			itile_Environment
		} TextureImageLayerEnum;
		/*!
			\brief	set an image layer for a texture. see layers with a TextureImageLayerEnum enums.
			\param	_layerToSet	the enum that define which layer to change.
			\param	_pByteChunk		pointer to a byte chunk. each pixels are _pixelByteDepth width.
			\param	_pixelByteDepth	number of bytes per pixel in _pByteChunk.
			\param	_pixelWidth		the number of pixel in a row
			\param	_pixelHeight	the number of pixel lines.
		*/
		virtual void	SetImageBitmap(	TextureImageLayerEnum _layerToSet,
									unsigned char *_pByteChunk,
									unsigned int _pixelByteDepth,
									unsigned int _pixelWidth,
									unsigned int _pixelHeight
									)=0;
	};
	/*!
		\class	InternalViewPort
		\ingroup Render3D
		\brief Nested class that defines a viewport where to draw (screen, sub-screen,texture rendering screens.)
			This is a virtual class. Construction must be done by NewViewPortChild(),
			NewViewPortOnTexture() or InitMachine() for the default viewport accessible with
			VirtualMachine::GetDefaultViewPort() which stands for the main screen.
	*/
	class	InternalViewPort
	{	public:
		/*!
			\brief	Reset color to black (or given color),and depth buffer, of this ViewPort.
			\param	_red [0.0,1.0]
			\param	_green [0.0,1.0]
			\param	_blue [0.0,1.0]
		*/
		virtual	void	Clear( float _red=0.0f,float _green=0.0f,float _blue=0.0f)=0;
		/*!
			\brief	Render a 3D Object. The vertexes are transformed with a matrix you set with matrix methods
					Matrix_LoadID() , Matrix_Translate() , Matrix_Rotate() , Matrix_Scale() , Matrix_Push() , Matrix_Pop() 
					the object shape is defined by _pMeshBuffer, the texture of the object is
					defined by _pTexture.
					Note drawings on the main screen needs a SwapBuffer() to be shown at each frames.
			\param	_pMeshBuffer InternalObject3DBuffer
			\param	_pTexture InternalTexture or null, it will shade vertex color.
		*/
		virtual	void	RenderMesh(
									InternalObject3DBuffer *_pMeshBuffer,
									InternalTexture *_pTexture=0L
									) =0 ;
		/*!
			\brief	Render directly a 32bit pixel bitmap image from memory on the whole viewport.
					note Position Coordinates are not used. the bitamp is just scaled to the viewport. 
					Note drawings on the main screen needs a SwapBuffer() to be shown at each frames.

			\param	_prgbaTable	pointer to the beginning of a 32bit RGBA table.
			\param	_PixelWidth number of pixel of a line
			\param	_PixelHeight number of pixel lines.
		*/
		virtual	void	DrawRGBARectangle( 
											unsigned char	*_prgbaTable,
											unsigned int	_PixelWidth,
											unsigned int	_PixelHeight
											) =0 ;

		/*!
			\brief	Swap buffers so that what was drawn on the viewport is shown.
					this is needed to active all Clear(), RenderMesh() ,...
		*/
		virtual	void	SwapBuffer()=0;
		/*!
			\brief	 manage 3D projection FOV
		*/
		inline	void	SetFOVLength( float _fovlength ){ m_fov = _fovlength; };
		/*!
			\brief	 manage 3D projection near cut plane distance.
		*/
		inline	void	SetNearCutPlane( float _neardistance ){ m_NearCutPlane = _neardistance; };
		/*!
			\brief	 manage 3D projection far cut plane.
		*/
		inline	void	SetFarCutPlane( float _fardistance ){ m_FarCutPlane = _fardistance; };

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
		virtual	void SetClippingCoordinates(float _ClippingX1,float _ClippingY1,float _ClippingX2,float _ClippingY2 )=0;
		/*
			\brief	Change the coodinates of the draw scale of a ViewPort, in the origin 
					of its father. default is 0.0,0.0,1.0,1.0
					If the ViewPort is a Root ViewPort, like the main screen,
					or a texture rendering, it is inactive.
					Note again, the coordinates given are in the father viewport origin.

			\param	_ScaleX1 upperleft corner in [0,1] scale for aspect ratio .
			\param	_ScaleY1 upperleft corner in [0,1] scale for aspect ratio .
			\param	_ScaleX2 downright corner in [0,1] scale for aspect ratio .
			\param	_ScaleY2 downright corner in [0,1] scale for aspect ratio .
		*/
		virtual void SetScaleCoordinates(float _ScaleX1,float _ScaleY1,float _ScaleX2,float _ScaleY2 )=0;
		
		/*!  
			\brief	Get upperleft horizontal the coordinate of this ViewPort, in the father's origin.
			\return coord in [0,1] scale.
		*/
		inline float	GetPositionX1(){ return m_ScaleX1; };
		/*!  
			\brief	Get the upperleft vertical coordinate of this ViewPort, in the father's origin.
			\return coord in [0,1] scale.
		*/
		inline float	GetPositionY1(){ return m_ScaleY1; };
		/*!  
			\brief	Get the downright horizontal coordinate of this ViewPort, in the father's origin.
			\return coord in [0,1] scale.
		*/
		inline float	GetPositionX2(){ return m_ScaleX2; };
		/*!  
			\brief	Get the downright  vertical coordinate of this ViewPort, in the father's origin.
			\return coord in [0,1] scale.
		*/
		inline float	GetPositionY2(){ return m_ScaleY2; };
		/*!  
			\brief	Get upperleft horizontal the coordinate of this ViewPort.
			\return coord in [0,1] scale.
		*/
		inline float	GetClippingX1(){ return m_ClippingX1; };
		/*!  
			\brief	Get the upperleft vertical coordinate of this ViewPort.
			\return coord in [0,1] scale.
		*/
		inline float	GetClippingY1(){ return m_ClippingY1; };
		/*!  
			\brief	Get the downright horizontal coordinate of this ViewPort.
			\return coord in [0,1] scale.
		*/
		inline float	GetClippingX2(){ return m_ClippingX2; };
		/*!  
			\brief	Get the downright  vertical coordinate of this ViewPort.
			\return coord in [0,1] scale.
		*/
		inline float	GetClippingY2(){ return m_ClippingY2; };
		/*!  
			\brief	 the root viewport if it is a sub-viewport, or 'this' if root.
			\return  the parent root viewport.
		*/
		inline InternalViewPort	*GetRootViewPort(){ return m_pRootViewPort; };
		/*!  
			\brief	Get the direct father viewport, or 0L if Root.
			\return  the father InternalViewPort or 0L.
		*/
		inline InternalViewPort	*GetFatherViewPort(){ return m_pClippingFatherViewPort; };
		/*!
			\typedef eLoadId
			\brief Matrix_LoadID() special enum
		*/
		typedef enum {
			//! all matrix terms are set to ID
			eLoadId_All=0,
			//! only rotation terms (3x3) are set to ID.
			eLoadId_Rotation
		} eLoadId ;
		/*!
			\brief	The matrix becomes an Identity matrix, which means no transformation at all.
					It cleans all previous transformations.
					_enum can be used to only affect the rotation terms (3x3 matrix)
			\param	_enum  
		*/
		virtual	void	Matrix_LoadID(eLoadId _enum=eLoadId_All)=0;
		/*!
			\brief	Add a translation transformation to the matrix.
			\param	_x x axis distance to move.
			\param	_y y axis distance to move.
			\param	_z z axis distance to move.
		*/
		virtual	void	Matrix_Translate(float _x,float _y,float _z)=0;
		/*!
			\brief	Add a Rotation to the matrix, using a quaternion.
			\param _angleRad an angle given in radian.
			\param	_x x normalized axis to rotate.
			\param	_y y normalized axis to rotate.
			\param	_z z normalized axis to rotate.
		*/
		virtual void    Matrix_Rotate( float _angleRad,float _x,float _y,float _z)=0;
		/*!
			\brief	Add a Scale to the matrix.
			\param	_x Rate to scale the X axis.
			\param	_y Rate to scale the Y axis.
			\param	_z Rate to scale the Z axis.
		*/
		virtual	void	Matrix_Scale(float _sx,float _sy,float _sz)=0;
		/*!
			\brief	Push the current state of the matrix in the stack.			
		*/		
		virtual	void	Matrix_Push()=0;
		/*!
			\brief	Pop the last state of the matrix from the stack.
		*/	
		virtual	void	Matrix_Pop()=0;
	protected:
		//! 2d coordinates of the rectangle scale, in [0,1] values, in the father position scale.
		float m_ScaleX1;	
		float m_ScaleY1;
		float m_ScaleX2;
		float m_ScaleY2;
		//! 2d coordinates of the rectangle bounds, in [0,1] float scale values.
		float m_ClippingX1;	
		float m_ClippingY1;
		float m_ClippingX2;
		float m_ClippingY2;
		//! the root viewport if it is a sub-viewport, or 'this' if root.
		InternalViewPort	*m_pRootViewPort;
		//! the father in the hierarchy of ViewPort
		InternalViewPort	*m_pPositionFatherViewPort;
		//! the first son in the hierarchy of ViewPort.
		InternalViewPort	*m_pPositionFirstSonViewPort;
		//! the next brother in the hierarchy of ViewPort.
		InternalViewPort	*m_pPositionNextBrotherViewPort;
		//! the father in the hierarchy of ViewPort
		InternalViewPort	*m_pClippingFatherViewPort;
		//! the first son in the hierarchy of ViewPort.
		InternalViewPort	*m_pClippingFirstSonViewPort;
		//! the next brother in the hierarchy of ViewPort.
		InternalViewPort	*m_pClippingNextBrotherViewPort;
		//! for 3D rendering, Distance between the projected plane and the center. must be >0.001
		float m_fov;	
		//!	for 3D rendering,Near cut plane, must be >0.001 and <= FarCutPlane
		float m_NearCutPlane;
		//!	for 3D rendering,Far cut plane, must be >= NearCutPlane
		float m_FarCutPlane;

	};
	/*!
		\typedef InternalObject3DBuffer_InitBit
		\ingroup Render3D
		\brief	see VirtualMachine::NewObject3DBuffer()
	*/
	typedef enum {
		//! Flag bit for NewObject3DBuffer(). Needed to enable InternalVertex::m_nx,m_ny,m_nz, for environment mapping and lightning.
		bOb3D_VertexNormal=1,
		//! Flag bit for NewObject3DBuffer(). Needed to enable InternalVertex::m_u,m_v.
		bOb3D_VertexUVMapping=2,
		//! Flag bit for NewObject3DBuffer(). Needed to enable InternalVertex::m_ColorRGBA
		bOb3D_VertexRGBA=4
	} InternalObject3DBuffer_InitBit ;

	/*!
		\ingroup Render3D
		\brief	Ask the Creation of a new InternalObject3DBuffer you can then fill with 
					InternalObject3DBuffer::GetFirstVertex() , InternalObject3DBuffer::GetFirstTriangle()
					and InternalObject3DBuffer::SetNumberOfActiveTriangle() .
					Then the object can be positionned with the matrix methods in VirtualMachine::InternalViewPort, and drawn with VirtualMachine::InternalViewPort::RenderMesh().
					The object shape can be dynamic and rebuilt, until a call to VirtualMachine::InternalObject3DBuffer::CompileAsStatic().
					In edition compilation mode, objects had to be destroyed with VirtualMachine::DeleteObject3DBuffer().
		\param	_ppVertexAndPolygonBufferOut	the object returned, or 0L if failed.(no mem ?)
		\param	_maxNbVertex					maximum number of vertex to fill.
		\param	_maxNbTriangle					maximum number of Triangle to fill.
		\param	_extraMemberFlags				or'ed bits (bOb3D_...) to activate extra vertex members.
	*/
	virtual void	NewObject3DBuffer(  InternalObject3DBuffer **_ppVertexAndPolygonBufferOut, 
							unsigned int _maxNbVertex,
							unsigned int _maxNbTriangle, 
							unsigned int _extraMemberFlags )=0;

	/*!
		\ingroup Render3D
		\brief		Destroy objects previously created with NewObject3DBuffer()
		\param		_ppBufferToDestroy object to destroy, and set to 0L.
	*/
	virtual void	DeleteObject3DBuffer(  InternalObject3DBuffer **_ppBufferToDestroy )=0;

	/*!
		\brief	Ask the Creation of a new InternalTexture, then used to render 3D objects with RenderMesh().
				In edition mode, InternalTexture has to be destroyed with DeleteTexture().
		\param	_ppTextureOut	the object returned, or 0L if failed.(no mem ?)
	*/
	virtual void	NewTexture(  InternalTexture **_ppTextureOut  )=0;

#ifdef _ENGINE_EDITABLE_
	/*!
		\ingroup Render3D
		\brief	Destroy objects previously created with NewTexture()
		\param	_ppTextureToDestroy	 object to destroy, and set to 0L.
	*/
	virtual void	DeleteTexture(  InternalTexture **_ppTextureToDestroy )=0;
#endif

	/*!
		\ingroup Render3D
		\brief	Ask the Creation of a new InternalViewPort, that must be the child of another ViewPort,
				specified by _pFatherViewPort.if no father is specified, the new ViewPort will be
				the child of the default root ViewPort. To create another Root ViewPort, create a texture
				and use VirtualMedia::NewViewPortOnTexture().
				In edition mode, each InternalViewPort has to be destroyed with DeleteViewPort().
		\param	_ppViewPortOut	the object returned, or 0L if failed.(no mem ?)
		\param	_pFatherViewPort the father viewport. If 0L, it uses the default root.
	*/
	virtual void	NewViewPortChild(  InternalViewPort **_ppViewPortOut,InternalViewPort *_pFatherViewPort=0L)=0;
	/*!
		\ingroup Render3D
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
											InternalTexture::TextureImageLayerEnum _layerToAffect)=0;

	/*!
		\ingroup Render3D
		\brief	Destroy any objects previously created with NewViewPortChild() or NewViewPortOnTexture().
				If it has children, children are detached , not destoyed, but are invalid.
		\param	_ppViewPortToDestroy	 pointer to object to destroy, and set to 0L.
	*/
	virtual void	DeleteViewPort(  InternalViewPort **_ppViewPortToDestroy )=0;

	/*!
		\ingroup Render3D
		\brief	Access to the default screen object created with InitMachine().
		\return the Viewport that stands for the whole screen, or NULL if not opened.
	*/	
	inline InternalViewPort	*GetDefaultViewPort(){ return m_pDefaultViewPort; };


/*==================================================================
			Sound Stuff
==================================================================*/
	/*!
		\addtogroup	MachineSound VirtualMachine Sound Methods
		\ingroup	VirtualMachineDocGroup
		\brief	Sound Classes and Methods for VirtualMachine. We use
			a very special way to support sound streams:
			We provide SoundInterface, a pure virtual class. You just got to extend
			it to another class and implement ProcessSoundInterupt() yourself.
			Then you must register an instance of it to the machine
			with EnableMediaSound(). as soon as it is registered, the machine
			will send your ProcessSoundInterupt(), which ask to <b>* add *</b> a
			stereo signal to a float sound stream described with class SoundBufferToAddYourSignal. 
			And your signal will be instantly heard. To stop it, call EnableMediaSound() with false.
			Of course, the sound interupt is managed privately by the machine.
			Look this \ref Example_MinimalMedia "Minimal Example". 
		\{
	*/
	/*!
		\class	SoundBufferToAddYourSignal
		\brief	Defines the sound buffer to be mixed by SoundInterface::ProcessSoundInterupt()
	*/
	class	SoundBufferToAddYourSignal
	{ public:
		/*! Pointer to the float buffer where to add your signal on.
		 it is a left+right stereo buffer with m_LengthToRender length.*/
		float	*m_pSoundBuffer;
		//! number of 2xfloat to add on m_pSoundBuffer.
		unsigned int m_LengthToRender;
		//! the constant frequency play in Herz(22050,44100,...) 
		float		m_PlayFrequency;
		/*! Time lapse of the buffer to mix, in seconds.( A given machine decide of the way calls are timed. )
		Time values should be close to the date passed to ProcessMedia(), and should look like an accumulation:
		on 2 consecutive calls, End of last call == beginning of next call */
		int				m_StartPlayTime_SecondUnit;
		//! m_StartPlayTime_SecondUnit 's after the point values.
		unsigned int	m_StartPlayTime_SecondFrac;
		//! time value at the end of the buffer, cf m_StartPlayTime_SecondUnit .
		int				m_EndPlayTime_SecondUnit;
		//! m_StartPlayTime_SecondUnit 's after the point values.
		unsigned int	m_EndPlayTime_SecondFrac;
	};
	/*!
		\class	SoundInterface
		\brief Virtual base class for any kind of object that need to generate
				a sound signal. You have to extend it and implement ProcessSoundInterupt().				
				(see exemple in VirtualMedia.cpp)
				For more informations, look VirtualMachine::EnableMediaSound()
	*/
	class	SoundInterface
	{ public:
		/*!
			\brief	Method thrown by the VirtualMachine to an object that must add a sound signal
					that will then be played As soon as possible.
					For more informations, look VirtualMachine::EnableMediaSound()
			\param	_SoundBufferToAddYourSignal description of the buffer to mix on.
		*/
		virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )=0;

	};
	/*!
		\brief	Return the machine main play frequency, in herz, (tick per sec).
		\return freq (44100.0,22050.0,...)
	*/
	virtual	float	GetPlayFrequency()=0;	

	/*!
		\brief	Register a SoundInterface object that will then generate a mixed sound through
				SoundInterface::ProcessSoundInterupt(). The sound will start instantly.
				if false, Unregister a media object previously registered
				with EnableMediaSound(,true). The sound stops instantly.
				Note, that if you choose to use the BaseObject
				implementation, class VirtualMedia will manage the sound
				in its own way, and you will not need to use this method.
		\param	_pNoisyObject the object that can generate noise.
		\param	_enable	true if you want to enable, false to disable.
	*/
	virtual	void	EnableMediaSound( SoundInterface *_pNoisyObject, bool _enable=true );			
	/*!
		\brief	force a date to the clock passed to the sound objects.	
		\param	_seconds	in second units.
		\param	_fraction	the following after-the-point-units of _seconds, like if the whole were a 64 bits integer.
	*/
	virtual	void	SetSoundCurrentTime(int _seconds,unsigned int _fraction )=0;
	/*!
		\brief	tool: change main volume [0,1].
				Yes, you can saturate when >1.
		\param	_newVolumeValue [0,1]
	*/
	virtual	void	SetSoundVolume(float _newVolumeValue)=0;
	/*!
		\brief	tool: get main volume value [0,1].
		\return	VolumeValue [0,1]
	*/
	virtual	float	GetSoundVolume()=0;
	/*!
		\brief	return an image of the current short sound buffer which is currently played.
				This is always a 2 float(stereo left,right) table.
				the play frequency of this sound signal is given by GetPlayFrequency() .
				The buffer size may vary in time and implementation, and this table is refresh on an undefined rate.
				It can be used by sound preview methods or live effects. 
		\param	_ppSoundBuffer	pointer on pointer on the buffer to return.
		\param	_pLength	pointer on a unsigned int that return the length.
	*/
	virtual	void	GetCurrentSoundBufferImage(float ** const _ppSoundBuffer,unsigned int *_pLength) const =0 ;

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Now, AzurVeda virtual machine implements a very efficient 
				BASS technology known as "ShutTheFuckUp" !
				Yes, it will unregister all sound objects to silent the machine.
	*/
	virtual	void	ShutTheFuckUp();
#endif
	//!	\}
/*==================================================================
			Cultural Stuff
==================================================================*/
	/*!
		\addtogroup	MachineCultural VirtualMachine Cultural Configuration Methods
		\ingroup	VirtualMachineDocGroup
		\brief Cultural configuration methods for VirtualMachine. We support this
			for localization purposes.
		\{
	*/
	/*!	\typedef eMachineLocalization
		\brief This enum spots a cultural langage. returned by GetCurrentWorldLocalizationEnum()
	*/
	typedef enum{
		//! means we just don't know which country the machine is for.
		eMachineLocalization_LocalisationNotImplemented=0, // means the machine just don't implement this info.
		//! english machine.
		eMachineLocalization_English,
		//! french machine.
		eMachineLocalization_French,
		//! Castellano machine.
		eMachineLocalization_SpanishCastellano,
		//! German machine !
		eMachineLocalization_German,	
		//! Italian machine.
		eMachineLocalization_Italian,	
		//! Polish machine.
		eMachineLocalization_Polish,
		//! portuguguese machine
		eMachineLocalization_Portuguese
	} eMachineLocalization;
	/*!
		\brief	Return the language of the machine, as a simple enum. It can be used
			to display one or the other langage.
		\return an enum integer.
	*/
	virtual eMachineLocalization	GetCurrentWorldLocalizationEnum()=0;
	/*!	\} */

/*==================================================================
			Interface Stuff
==================================================================*/
	/*!
		\addtogroup	MachineInterface VirtualMachine Interface Methods
		\ingroup	VirtualMachineDocGroup
		\brief		Methods to check peripherals and other things for VirtualMachine.
					For the moment it just check a general way to exit.
	*/
	/*!
		\ingroup	MachineInterface
		\brief	Process the Machine's interface messages (mouse,keyboard,...), in the real time.
				Then methods like VirtualMachine::GetQuitMessage() are updated.
	*/
	virtual void	ProcessInterface()=0;

	/*!
		\ingroup	MachineInterface
		\brief	If true, means that the user want to quit:
		\return	true to quit.
	*/
	inline	bool	GetQuitMessage(){ return m_QuitMessage; };

/*==================================================================
			Disk Operating and System Stuff.
==================================================================*/
	/*
		\brief	After init,Set a general machine title, for example, to be displayed
			as a windows title. You can change the machine title at any time.
		\param	_pMachineTitle	const string to be displayed.
	*/
	virtual	void	SetMachineTitle(const char *_pMachineTitle);
	/*!
		\addtogroup	MachineDOS VirtualMachine Disk Operating and System Methods
		\ingroup	VirtualMachineDocGroup
		\brief		Disk and system functions for VirtualMachine
		\{
	*/
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	In edition mode, ask a file path to a file requester.
			the string format should vary, but will be compatible with LoadFile() and SaveFile()
		\param	_pDisplayString	a string to inform to what's the file for.
		\param	_pResultFileName the file path , returned.
		\param	_nameMaxLength the maximum buffer size to write.
		\return	true if _pDisplayString valid.
	*/
	virtual	bool	FileRequester(const char *_pDisplayString,char *_pResultFileName, unsigned int _nameMaxLength )=0;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	<b>DEPRECATED for the moment: use class PackResource instead</b>
				load a file into a memory chunk. You can load all in one row, or stream.
				memory is to be deleted by the CALLER with operator: "delete [] pointer", for each 
				succesfull call. return 0L if failed.
		\param	_pFilePath			the path of the file to load
		\param	_FileStart			an offset of where to start reading the file. 0 if all file.
		\param	_LoadByteLength		return the exact real length of what was read and returned, if OK.
		\param	_maximumSizeLoad	the maximum length to read
		\return	the memory chunk, or 0L if failed.
	*/
//	virtual	const char *LoadFile(const char *_pFilePath,unsigned int _FileStart,unsigned int &_LoadByteLength, unsigned int _maximumSizeLoad=0xffffffff)=0;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	write a file from a memory chunk. You can save all in one row, or stream.
		\param	_pFilePath		the path of the file to write
		\param	_chunkToWrite	your memory chunk to save, as char *.
		\param	_ChunkLength	the length of _chunkToWrite
		\param	_append			if true, file is continued. if false, file is rewritten. default to false.
		\return	true if OK, false if not.
	*/
	virtual	bool SaveFile(const char *_pFilePath,const char *_chunkToWrite,unsigned int _ChunkLength,bool _append=false)=0;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	in edition mode, redirection of machine task wait.
		\param	_milliseconds time to wait.
	*/
	virtual void Sleep( unsigned int _milliseconds )=0;
#endif
	/*!		\} */
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! this is the default viewport, it stands the whole screen by default. Used by GetDefaultViewPort().
	InternalViewPort	*m_pDefaultViewPort;

	//! If true, means that the user want to quit. Updated by ProcessInterface().
	bool			m_QuitMessage;

	//!  nested class to manage update sound object list;
	class	SoundUpdateListCell	
	{ public:
		SoundUpdateListCell			*m_pNext;
		SoundInterface				*m_pMediaObject;
	};
	//!	start of the chained list of the sound object update method:
	SoundUpdateListCell	*m_pFirstSoundObject;
};
#endif
