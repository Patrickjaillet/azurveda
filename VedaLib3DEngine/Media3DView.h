/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_Media3DView_H
#define COM_M4NKIND_Media3DView_H

#include "VirtualMedia.h"
#include "PackList.h"
#include "PackDynamicFloat.h"
#include "PackULong_Flags.h"
/*!
	\class	Media3DView
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief  Define a 3D view of any Object3DVirtual (it implies Object3DScene ,etc...)
			by linking camera movements, and an optional rendering 2D target.
			It is a VirtualMedia, and should be used for any 3D rendering.
*/

class Media3DView : public VirtualMedia
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Media3DView(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Media3DView);

	/*!
		\brief	 Process a media at a given date,
				or do anything) using a machine. You must process the media on machine this->GetMachine().
				Note there are no play/pause or speed concept here, by default, a date is passed.

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pViewPort the viewport to render. Can't be 0L.
	*/
	virtual void ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort );

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	typedef enum{
		scfl_NoClear=1
	} RenderFlag ;
	//! property flags for the scene rendering:
	PackULong_Flags		mSer_Flags;
	//! property flags for the scene rendering:
	PackFloat			mSer_TimeLength;
	//! Reference to Object or scene to render
	PackObjectReference	mSer_MainObject;
	//! viewport rectangle where to render.
	PackObjectReference	mSer_RenderTargetRef;
	//! camera coordinates and cinematic (same as objects)
	//! the reference to the cinematic parameters:
	PackDynamicFloat	mSer_PositionRef;
	//! the reference to the cinematic parameters:
	PackDynamicFloat	mSer_TargetPositionRef;
	//! the reference to the cinematic parameters:
	PackDynamicFloat	mSer_FlipRot;
	//! the reference to the cinematic parameters:
	PackDynamicFloat	mSer_FOV;


	/*!
		\brief	Tool: Find 2 angles that define a vector, using atan2()
		\param	
	*/
	void	AtanO1O2( float *po1, float xd,float yd,float zd );

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	// disabled because, no rule for time length.
	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	//disabled because == super.
	//virtual void	CloseInternal(void);
#endif

};

#endif
