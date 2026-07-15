/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_Object3DVirtual_H
#define COM_M4NKIND_Object3DVirtual_H

#include "BaseObject.h"
#include "VirtualMachine.h"
#include "PackULong_Flags.h"
#include "PackObjectReference.h"
#include "PackStruct.h"
#include "PackList.h"
#include "PackDynamicFloat.h"
/*!
	\class	Object3DVirtual
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	 Virtual Class defining a generic renderable Object3D, 
			it can be a polygon mesh, or even a whole scene.
			RenderObject() handles the rendering. for some classes,
			SetColor() can be used to define a general color for the object,
			before a call to RenderObject().
*/

class Object3DVirtual : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Object3DVirtual(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(Object3DVirtual);

	/*!
		\brief	Change a main color to render the object.
				An object cn use it or not according to the class.
		\param	_rgba red green blue , float table. alpha may be used for transparency.
	*/
	//old virtual void	SetColor( const float _rgba[4] );
	/*!
		\brief	 Nested class that describes
				a list of ShapeModifier to apply to a 3D object to 
				modify its shape.
	*/
	class ShapeModifierList : public PackList
	{
	public:
		//! \brief constructor . It registers the members.
		ShapeModifierList(void);
		/*!
			\brief	Apply the Modifier from mSer_ModifierList To a VertexList.
			\param	_pFirstVertex	pointer to the first.
			\param	_NumberOfVertex	number of vertex in the table.
			\param	_date In Seconds.
		*/
		void	ModifyVertexList(VirtualMachine::InternalVertex *_pFirstVertex,const unsigned int _NumberOfVertex,float _date);
	};
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
									ShapeModifierList *_pModifierList=0L )=0;
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
				created object. This is done with this method. sub classes can implement it (or not) in
				any way, to explicit current shape of an object.<br>

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration
	*/
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
	/*!
		\brief	 Nested class that describes a vertex shape modifier
		, used as element in the list.
	*/
	class ShapeModifier : public PackStruct
	{
	public:
		//! \brief constructor . It registers the members.
		ShapeModifier(void);
		//! identify the channel to modify
		PackObjectReference	mSer_ModifierReference;
		//! give  a static or dynamic value:
		PackDynamicFloat	mSer_Value;
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new PartToPlay object.
	*/
	static BaseType *NewShapeModifier();

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! current general color.
	//float	m_rgba[4];
	//! bool true if m_rgba used.
	//bool	m_RGBAUsed;
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Preview Tool: Create a preview line 3D Object.
		\param	_ppVertexAndPolygonBufferOut object to create.
		\param	_nbPoint number of point in the line. minimum=2.
	*/
	void ProcessPreview_CreateLine(VirtualMachine::InternalObject3DBuffer **_ppVertexAndPolygonBufferOut, unsigned int _nbPoint=2);
#endif	
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Preview Tool: draw a grid 10x10 axis grid in the viewport's matrix state.
		\param	_pPreviewViewPort where to draw.
	*/
	void ProcessPreview_DrawGrid(VirtualMachine::InternalViewPort *_pPreviewViewPort );
#endif
};

#endif
