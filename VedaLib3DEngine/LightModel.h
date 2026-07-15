/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_LightModel_H
#define COM_M4NKIND_LightModel_H
#include "BaseObject.h"
#include "PackULong_Flags.h"
#include "PackDynamicFloat.h"
#include "PackList.h"
#include "PackObjectReference.h"
#include "PackFloatByte_Limits.h"
/*!
	\class	LightModel
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Describe a light model and provide some methods.
			(not final ATM. Experimental.)
*/
class LightModel : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be initialisation there.
	*/
	LightModel( );
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(LightModel);
	/*
		\brief	return true is ambient occlusion should be processed.
		\return true or false.
	*/
	inline bool IsAmbientOcclusionActive() { return mSer_Flags.TestFlags(lmf_AmbientOcclusion);  };
	/*
		\brief	return the first description of a LinearLight object, or null if no linear lights at all.
					the LinearLight is get from the cell with GetManagedObject(), and the next cell
					is found with GetNextBrother()
		\return the first list cell.
	*/
	inline PackList::Cell *GetFirstLinearLight() { return mSer_LinearLightList.GetFirstCell(); };
	/*
		\brief	return the first description of a DifuseLight object, or null if no linear lights at all.
					the DifuseLight is get from the cell with GetManagedObject(), and the next cell
					is found with GetNextBrother()
		\return the first list cell.
	*/
	inline PackList::Cell *GetFirstDifuseLight() { return mSer_DifuseLightList.GetFirstCell(); };

	/*!
		\brief	 Nested class that describes an element of the Linear Light list.
	*/
	class LinearLight : public PackStruct
	{
	public:
		//! \brief element constructor 
		LinearLight(void);
		//! the color. Should be higher than Ambient.
		PackDynamicFloat	mSer_Color;
		//! light linear vector. souldn't be 0,0,0.
		PackDynamicFloat	mSer_Vector;		
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new  object.
	*/
	static BaseType *NewLinearLight();
	/*!
		\brief	 Nested class that describes an element of the difuse light list.			
	*/
	class DifuseLight : public PackStruct
	{
	public:
		//! \brief element constructor 
		DifuseLight(void);
		//! the color. Should be higher than Ambient. manage also light strength.
		PackDynamicFloat	mSer_Color;
		//! light linear vector. souldn't be 0,0,0.
		PackDynamicFloat	mSer_Position;		
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new  object.
	*/
	static BaseType *NewDifuseLight();
		typedef enum
	{
		lmf_AmbientOcclusion=1
	} lightmodelflag ;
	//! select properties of the model:
	PackULong_Flags		mSer_Flags;
	//! ambient occlusion strength.
	PackFloatByte_Limits	mSer_OcclusionStrength;
	//! ambient color: average color in perfect diffusion
	PackDynamicFloat	mSer_AmbientColor;
	//! list of away-linear lighnings.
	PackList			mSer_LinearLightList;
	//! list of near dynamic light sources.
	PackList			mSer_DifuseLightList;

/*==================================================================
								PROTECTED
==================================================================*/
protected:

};
#endif
