// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_Object3DLWOResource_H
#define COM_M4NKIND_Object3DLWOResource_H
#include "Object3DMeshVirtual.h"

#include "PackResource.h"
#include "lwo2reader.h"
#include "PackList.h"

class Object3DLWOResource : public Object3DMeshVirtual
{

public:

	Object3DLWOResource(void);

	BASEOBJECT_DEFINE_CLASS(Object3DLWOResource);
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

	virtual void	RenderObject( double _frameDate,
									VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex=0,
									ShapeModifierList *_pModifierList=0L );

protected:

	PackResource	mSer_LWOFile;

	PackList		mSer_TextureReferenceList;

	sLwoObject	m_Lwo;

	class	LayerObject { public:

		VirtualMachine::InternalObject3DBuffer	*m_pObject3DBuffer;

		unsigned int	m_NumberOfTextureLayer;

		struct LwoSortedTriangleSurface	*m_pSurfaceSortedTriangleListIndex;
	};

	unsigned int	m_NbLayer;

	LayerObject	*m_pLayerObjects;

	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif

	static BaseType *NewTextureReference();
};

#endif
