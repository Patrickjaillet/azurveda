/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_Object3DLWOResource_H
#define COM_M4NKIND_Object3DLWOResource_H
#include "Object3DMeshVirtual.h"
//#include "Object3DMeshVirtual.h"
#include "PackResource.h"
#include "lwo2reader.h"
#include "PackList.h"
//#include "PackFloatByte_Limits.h"

//#include "PackTreeCell.h"
/*!
	\class	Object3DLWOResource
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Define an object with a Lightwave File resource.
			Lightwave is a commercial 3D Modeler from NewTek.
			We support .lwo object files with this class.
*/

class Object3DLWOResource : public Object3DMeshVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Object3DLWOResource(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Object3DLWOResource);
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
	
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	// the file describing one or more objects
	PackResource	mSer_LWOFile;
	// remap the lightwave textures to context texture object:
	PackList		mSer_TextureReferenceList;
	// the LWO reader engine:
	sLwoObject	m_Lwo;
	//!\class 	LayerObject
	//!\brief private use.
	class	LayerObject { public:
		//! the hardwareAbstracted vertex buffer for this object:
		VirtualMachine::InternalObject3DBuffer	*m_pObject3DBuffer;
		//! each layer have 1 or more texture used.
		unsigned int	m_NumberOfTextureLayer;	
		//! base of texture layer, with triangle offset on the triangle list:
		struct LwoSortedTriangleSurface	*m_pSurfaceSortedTriangleListIndex;	
	};
	//! 
	unsigned int	m_NbLayer;
	//!
	LayerObject	*m_pLayerObjects;

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	virtual void	CloseInternal(void);
#endif
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new  object.
	*/
	static BaseType *NewTextureReference();
};

#endif
