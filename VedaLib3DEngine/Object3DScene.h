/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_Object3DScene_H
#define COM_M4NKIND_Object3DScene_H

//#include "VirtualMedia.h"
#include "PackDynamicFloat.h"
#include "PackDynamicFloat.h"
#include "Object3DVirtual.h"
#include "VirtualEquation.h"
#include "PackTreeCell.h"
#include "PackULong_Enums.h"
#include "PackList.h"
#include "PackLong_WithLimits.h"

/*!
	\class	Object3DScene
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief  Base class for all class that stands for a 3D Scene, which means
			an ensemble of Object3DVirtual, as a Object3DVirtual. It should look like
			an Object list, and some cinematic informations about how they move. 
*/

class Object3DScene : public Object3DVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Object3DScene(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Object3DScene);
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
	/*
		\typedef eOb3DChannel
		\brief define possible manner a refered object can be positionned,
			recolored, shaded in any form when drawn. All channels
			use a 1,2,3 or 4-dimensionned floats as parameters.
			this is used by nested class Object3DReference.
	*/
	typedef enum {
		//! Move the object in the scene space according to its father in the tree hierarchy.
		eOb3DChannel_Position=0,
		//! Rotate the object in the scene space according to its father in the tree hierarchy.
		eOb3DChannel_Rotation,
		//! Scale the object in the scene space according to its father in the tree hierarchy.
		eOb3DChannel_Scale,
		//! change all object's color, including alpha
		eOb3DChannel_Color,
		//! Transparency
		eOb3DChannel_Transparency
	} eOb3DChannel ;

	/*!
		\brief	 Nested class that describes a reference to Object3DVirtual, used as element in the list.
	*/
	class ChannelModifier : public PackStruct
	{
	public:
		//! \brief constructor . It registers the members.
		ChannelModifier(void);
		//! idenify the channel to modify
		PackULong_Enums		mSer_ChannelID;
		//! give  a static or dynamic value:
		PackDynamicFloat	mSer_Value;
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new PartToPlay object.
	*/
	static BaseType *NewChannelModifier();

	/*!
		\brief	 Nested class that describes a reference to Object3DVirtual, used as element in the list.
	*/
	class Object3DReference : public PackStruct
	{
	public:
		//! \brief constructor . It registers the members.
		Object3DReference(void);
		//! the reference to an object:
		PackObjectReference	mSer_Object3DReference;
		//! layer on the refered object:
		PackULong	mSer_Object3DLayerIndex;
		//! 
		PackLong_WithLimits	mSer_BeginAtNumber;
		//! recursion
		PackLong_WithLimits	mSer_RecursionNumber;
		//! list of channels to modify or dynamise:
		PackList	mSer_ChannelModifierList;
		//! list of channels to modify or dynamise:
		ShapeModifierList mSer_ShapeModifierList;
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new PartToPlay object.
	*/
	static BaseType *NewObject3DReference();

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! list of objects in the scenes. According to the type of class, it can be serialized (Scene3D) or created ( Scene3DText )
	PackTreeCell		mSer_ObjectTree;
	/*!
		\brief	Recursive processing of the tree.
		\param	_pCell tree cell.
		\param	_pVp viewport
		\param	_rgba shadable color state.
		\param	_frameDate a date, in second, which defines the effect cinematic.
	*/
	void	ProcessObject( PackTreeCell* _pCell,VirtualMachine::InternalViewPort *_pVp
							,const float _rgba[4]
							,double _frameDate );
};

#endif

