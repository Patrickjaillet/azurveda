/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_Object3DMesh3D_H
#define COM_M4NKIND_Object3DMesh3D_H
#include "Object3DMeshVirtual.h"
#include "PackFloatByte_Limits.h"
#include "PackList.h"
//#include "PackTreeCell.h"
#include "PackULong_Flags.h"
#include "PackLong_WithLimits.h"
/*!
	\class	Object3DMesh3D
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Define an object with a list of vertex and polygons.(TODO)
*/

class Object3DMesh3D : public Object3DMeshVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Object3DMesh3D(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Object3DMesh3D);

	/*!
		\typedef ePolygon
		\brief	kind of polygon the list defines.
	*/
	typedef enum {
		ePMode_Strips=1,
	} ePolygonModeFlags ;
	/*!
		\class PolygonList
		\brief	polygon list serialized shape.
	*/
	class PolygonList : public PackStruct
	{
	public:
		/*!
			\brief	constructor
		*/
		PolygonList();
		//! ePolygonModeFlags enum to define 
		PackULong_Flags		mSer_Flags;
		//! number of vertex by polygon.
		PackLong_WithLimits	mSer_VertPerPoly;
		//! vertex reference list in the shape defined by mSer_ListType
		PackList		mSer_RefList;
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new object.
	*/
	static BaseType *NewPackULong();
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new object.
	*/
	static BaseType *NewPolygonList();

#ifdef _ENGINE_EDITABLE_
	//! ToolMethod enum for ExecuteToolMethod()
	typedef enum {
		tm_Object3DMesh3D_SetAsCube=tm_BaseObject_FirstInheritageToolMethod,
		tm_Object3DMesh3D_LoadLWO,
		tm_Object3DMesh3D_FirstInheritageToolMethod,
	};
	/*!
		\brief	Tool Method main entry. In editable mode, the object can register a set of methods identified 
				with an ID number by using RegisterToolMethod(), and throw the methods through a switch in ToolMethod(). 
				Here, It is used to export wave sound format.
		\param	_MethodIDToExecute	the tool method ID, greater than 0.
	*/
	virtual void ExecuteToolMethod( unsigned int _MethodIDToExecute ) ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Tool Method
	*/
	void ToolMethod_SetAsCube() ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Tool Method
	*/
	void ToolMethod_LoadLWO() ;
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new object.
	*/
	static BaseType *NewPackFloatByte_Limits();

	//! 0 means vertex normals are interpolated, others are split. 
	PackFloatByte_Limits	mSer_NormalSplit;
	//! center of the object
	PackFloatByte_Limits	mSer_Pivot;
	//! Scale of the object.
	PackFloat	mSer_Scale;
	//! Subdivision of the polygons
	PackLong_WithLimits	mSer_Subdivision;
	//! general spline curve factor
	PackFloatByte_Limits	mSer_SplineFactor;
	//! list of serialized vertex in the object. 
	PackList		mSer_StaticVertexList;
	//! list of polygons of the object.
	PackList		mSer_PolygonList;
	//! computed Total Number of Vertex
	//old, now in Object3DBufferHandler unsigned int	m_TotalNbVertex;
	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	virtual bool CreateInternal(void);
	/*!
		\brief	update the shape of the object for this date, during
				the lifetime the object, when it is created.
				by defdault, does nothing, so static object can be created
				with CreateInternal() only.
		\param	_newShapeTime the new date.
		\param	_shapeIndex the index of the shape to modify.
	*/
	virtual void CreateShape(float _newShapeTime, unsigned int _shapeIndex=0);
};

#endif
