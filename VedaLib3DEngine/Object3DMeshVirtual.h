/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_Object3DMeshVirtual_H
#define COM_M4NKIND_Object3DMeshVirtual_H

#include "Object3DVirtual.h"
#include "PackFloatByte_Limits.h"
#include "PackList.h"
#include "VirtualMachine.h"
/*!
	\class	Object3DMeshVirtual
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Define a renderable 3D object with a polygon mesh.
			The object can be defined in any way, so this class
			is still virtual.
*/

class Object3DMeshVirtual : public Object3DVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Object3DMeshVirtual(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(Object3DMeshVirtual);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
				created object. This is done with this method. sub classes can implement it (or not) in
				any way, to explicit current shape of an object.<br>
				Extended to draw.
		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration
	*/
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

	/*!
		\brief	Process a real time computing and drawing. Can be extended.
		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pViewPort description of the viewport where to render.
		\param	_rgba red green blue alpha component.
		\param	_shapeIndex	optional index for multiple shape objects. def to zero.
		\param _pModifierList optional ShapeModifierList pointer, to modify objects.
	*/
	virtual void	RenderObject( double _frameDate, 
									VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex=0,
									ShapeModifierList *_pModifierList=0L );
	/*
		\brief	get the VirtualMachine's object.
		\param	_shapeIndex	optional index for multiple shape objects. def to zero.
		\return a drawable 3d object .
	*/
	/* anti-object acess !!! inline VirtualMachine::InternalObject3DBuffer *GetObject3DBuffer(unsigned int _shapeIndex=0)
	{if( _shapeIndex>=m_CurrentNumberOf3DBuffer) return 0L;	
		return m_pObject3DBufferTable[ _shapeIndex ].m_pObject3DBuffer;
	};*/

	/*
		\brief	Access to the texture linked. 
			* DISABLED: ANTI-OBJECT!!!INTERFERENCE !! 
			it's a private affair ! ->MEGADEPRECATED *
		\return the hardware abstract texture.
	*/
	//VirtualMachine::InternalTexture	*GetTexture();

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	/*!
		\brief	update the shape of the object for this date, during
				the lifetime the object, when it is created.
				by defdault, does nothing, so static object can be created
				with CreateInternal() only.
		\param	_newShapeTime the new date.
		\param	_shapeIndex the index of the shape to modify.
	*/
	virtual void CreateShape(float _newShapeTime, unsigned int _shapeIndex=0);
	//! an object shape is relative to a time date, and the extended object's attributes.
	float		m_LastShapeTime;
	//! last shape modifier applied.
	ShapeModifierList	*m_pLastShapeModifierList;
	//! flag for each object shape:
	typedef enum {
		//! means object is compiled, cannot be reshaped.
		OB3DFlag_Static=1, 
		//! means the object always look the camera.
		OB3DFlag_LookCam=2,
		//! means object shape is static, and is pre-rendered to a 32x32 image.
		OB3DFlag_PreRender32=4, // 
		//! means object shape is static, and is pre-rendered to a 128x128 image.
		OB3DFlag_PreRender128=8, 
		//! means SetColor() affect the whole object, else color is computed per vertex.
		OB3DFlag_OneColor=16, 
		//! start of possible flag extension for inheritage.
		OB3DFlag_Object3DVirtual_LastFlag=32	
	} OB3DBitFlag ;

	//! property flags (OB3DFlag_...)
	PackULong_Flags	mSer_Flags;

	//! The only texture for the object. We support 1 texture by object in this class.
	PackObjectReference	mSer_TextureRef;

	/*
		\brief	Nested class that can describe a 3D Object, or a billboard
				square with the prerendered object (in that case, m_pRenderTexture is not null.)
	*/
	class Object3DBufferHandler {
		public:
		//! constructor that init pointers to null:
		Object3DBufferHandler();
		//! the hardwareAbstracted vertex buffer for this object:
		VirtualMachine::InternalObject3DBuffer	*m_pObject3DBuffer;
		//! current active number of vertex, <= m_pObject3DBuffer->totalnb
		unsigned int	m_ActiveNumberOfVertex;
		//! if pre-rendered, this texture holds the rendering, and override the Texture3D object:
		VirtualMachine::InternalTexture			*m_pRenderTexture;
	};
	//!, should be available if created:
	Object3DBufferHandler			*m_pObject3DBufferTable;
	
	//! current number of InternalObject3DBuffer created in table m_pObject3DVirtualBufferTable. Most of the time it's 1.
	unsigned int	m_CurrentNumberOf3DBuffer;

	//! \class	BoundingBox
	//! \brief nested class used to calculkate the bouding bow of a 3D shape.
	class BoundingBox {
	public:
		float	m_x1,m_x2,m_y1,m_y2,m_z1,m_z2;
		//! \brief init box.
		void	FindBoundingBox( VirtualMachine::InternalObject3DBuffer *_pOb3d );
	}  ;

	/*!
		\brief	tool for CreateInternal(). If mSer_Flags ask it,(OB3DFlag_PreRender)
				all objects previously constructed are rendered to a texture, and
				the object is replaced by a billboard with the same dimensions.
	*/
	bool CreateInternal_RenderObjectToTexture(void);
	/*!
		\brief	tool for CreateInternal(). If mSer_Flags ask it,(OB3DFlag_Static)
				once the object shape is created (to date 0 seconds), the object shape
				is frozen and changes will be ignored. Note: It applies to all shapes.
				It allows some optilizations...
		
	*/
	void CreateInternal_FreezeStaticShape(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	virtual void	CloseInternal(void);
#endif
};

#endif
