#ifndef	COM_M4NKIND_Object3DVirtualDynamic_H
#define COM_M4NKIND_Object3DVirtualDynamic_H

#include "Object3DMeshVirtual.h"

#include "PackLong_WithLimits.h"

class Object3DVirtualDynamic : public Object3DMeshVirtual
{

public:

	Object3DVirtualDynamic(void);

	BASEOBJECT_DEFINE_VIRTUALCLASS(Object3DVirtualDynamic);

protected:

	PackLong_WithLimits		mSer_MaximumNumberOfVertex;

	PackLong_WithLimits		mSer_MaximumNumberOfTriangle;

	bool CreateInternal_Alloc1Internal3DBuffer(unsigned int _i3DBufferFlag);

};

#endif
