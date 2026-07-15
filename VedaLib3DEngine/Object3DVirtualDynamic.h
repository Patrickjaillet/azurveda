#ifndef	COM_M4NKIND_Object3DVirtualDynamic_H
#define COM_M4NKIND_Object3DVirtualDynamic_H

#include "Object3DMeshVirtual.h"

#include "PackLong_WithLimits.h"

/*!
	\class	Object3DVirtualDynamic
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	virtual Class defining a Object3D Mesh which shape is dynamic or not,
		like Object3DVirtual, but which number of vertex and polygons may vary
		according to the shape. So this class ask 2 attributes members:
		a maximum number of vertex and a maximum number of polygons, so that
		it allocates these list once.
*/

class Object3DVirtualDynamic : public Object3DMeshVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Object3DVirtualDynamic(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(Object3DVirtualDynamic);

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the maximum number of vertex.
	PackLong_WithLimits		mSer_MaximumNumberOfVertex;
	//! the maximum number of Triangle.
	PackLong_WithLimits		mSer_MaximumNumberOfTriangle;
	/*!
		\brief	Alloc one VirtualMachine::Internal3DBuffer with the number of vertex and triangle defined.
			this sould be used by CreateInternal or CreateShape().
		\param	_i3DBufferFlag the flags used to declare the vertex members.
		\return true if OK, false if error. 
	*/
	bool CreateInternal_Alloc1Internal3DBuffer(unsigned int _i3DBufferFlag);


};

#endif
