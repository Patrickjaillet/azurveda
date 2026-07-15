/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "LightModel.h"
//#include "VirtualEquation.h"
//#include "PackFloat.h"

BASEOBJECT_DECLARE_CLASS( "lgmd", LightModel, BaseObject,"3DLight Model","LightModel","Describe a complex lighting. Used by Object3DMarchcube from now on.");
/*!
	\brief	Constructor. There should only be initialisation there.
*/
LightModel::LightModel( ) : BaseObject()
	,mSer_AmbientColor(PackFloat::vd_XYZ) // this float will manage RGBA.
	,mSer_LinearLightList(LightModel::NewLinearLight)
	,mSer_DifuseLightList(LightModel::NewDifuseLight)
{
	REGISTER_MEMBER_FLAG( mSer_Flags ,"Flags",0,"AmbOcc" );
	REGISTER_MEMBER_PACKDYNAMICFLOAT(mSer_AmbientColor,"AmbientColor",0.4f);
	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(mSer_OcclusionStrength,"Occlusion",0.25f,0.0f,0.25f);
	REGISTER_MEMBER( mSer_LinearLightList ,"LinearLights" );
	REGISTER_MEMBER( mSer_DifuseLightList ,"DifuseLights" );
}
//! \brief element constructor 
LightModel::LinearLight::LinearLight(void) : PackStruct()
	,mSer_Color(PackFloat::vd_XYZ) 
	,mSer_Vector(PackFloat::vd_XYZ) 
{
	REGISTER_MEMBER_PACKDYNAMICFLOAT(mSer_Color,"Color",1.0f);
	REGISTER_MEMBER_PACKDYNAMICFLOAT(mSer_Vector,"Vector",1.0f);
#ifdef _ENGINE_EDITABLE_
	// set default vector down:
	mSer_Vector.SetConstant(0.0f,0);
	mSer_Vector.SetConstant(1.0f,1);
	mSer_Vector.SetConstant(0.0f,2);
#endif

}//! \brief element constructor 
/*!
	\brief	static method passed to the list to create new elements.
	\return a new  object.
*/
BaseType *LightModel::NewLinearLight()
{	return new LinearLight();
}
LightModel::DifuseLight::DifuseLight(void): PackStruct()
	,mSer_Color(PackFloat::vd_XYZ) 
	,mSer_Position(PackFloat::vd_XYZ) 
{
	REGISTER_MEMBER_PACKDYNAMICFLOAT(mSer_Color,"Color",1.0f);
	REGISTER_MEMBER_PACKDYNAMICFLOAT(mSer_Position,"Position",0.0f);
}
/*!
	\brief	static method passed to the list to create new elements.
	\return a new  object.
*/
BaseType *LightModel::NewDifuseLight()
{	return new DifuseLight();
}
