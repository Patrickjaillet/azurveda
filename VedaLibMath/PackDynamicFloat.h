/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackDynamicFloat_H
#define COM_M4NKIND_PackDynamicFloat_H

#include "PackDynamicType.h"
#include "PackFloat_FixedPoint.h"
#include "PackFloat.h"
#include "PackObjectReference.h"
/*!
	\class	PackDynamicFloat
	\brief	Base type that lets you choose between a constant scalar expression or a reference to an equation.
			It supports 1,2 or 3 vector like PackFloat, but is not extended from packfloat.
*/
class PackDynamicFloat : public PackDynamicType
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be initialisation there.
		\param _DimensionEnum the dimension of the value.:(1,2,3,4) PackFloat::vd_...
	*/
	PackDynamicFloat( PackFloat::VectorDimension _DimensionEnum);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	editable mode accesor, that force a constant type, and its value.
		\param _value the new value.
		\param _dimensionIndex of the dimension affected. 0 is default.
	*/
	void	SetConstant(float _value,unsigned int _dimensionIndex=0);
#endif
	/*!
		\brief	 Compute 1,2,3 or 4 value according to a parameter list (X,Y,Z,D).
				An equation can manage one or more dimensions on input and output.
				the unused output parameters are left untouch, so that you
				can set your default values there.
				Note this version is not virtual, because there is no reason for extension a prioris.
		\param	_OutgoingParameterTable result table. elements are writen or not according to the equation.
		\param _IncomingParameterTable. float table, the (X,Y,Z,D) parameters.
	*/
	void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the optionnal constant, valid if PackDynamicType selected.
	PackFloat				mSer_Constant;
	//! the optional reference to an equation, valid if PackDynamicType selected.
	PackObjectReference		mSer_EquationReference;

	//! the number of Dimension managed if PackFloat was initialized as a vector
	const unsigned int	m_NumberOfDimensionManaged;
};
/*!
	\def	REGISTER_MEMBER_PACKDYNAMICFLOAT

	\brief	This macro is used to register a serializable member in a class constructor.
			For editable mode, it uses _InfoString to explicit the use of the member, in order
			to display it in a GUI for example.
		
*/
#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKDYNAMICFLOAT(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.SetConstant(_DefaultVal,0);\
_object.SetConstant(_DefaultVal,1);\
_object.SetConstant(_DefaultVal,2);
#else
#define		REGISTER_MEMBER_PACKDYNAMICFLOAT(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);
#endif
/*!
	\def	REGISTER_MEMBER_PACKDYNAMICFLOAT_XY

	\brief	This macro is used to register a serializable member in a class constructor.
			For editable mode, it uses _InfoString to explicit the use of the member, in order
			to display it in a GUI for example.
		
*/
#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKDYNAMICFLOAT_XY(_object,_InfoString,_DefaultValX,_DefaultValY) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.SetConstant(_DefaultValX,0);\
_object.SetConstant(_DefaultValY,1);
#else
#define		REGISTER_MEMBER_PACKDYNAMICFLOAT_XY(_object,_InfoString,_DefaultValX,_DefaultValY) \
RegisterSerializableMember(_object);
#endif
/*!
	\def	REGISTER_MEMBER_PACKDYNAMICFLOAT_XYZ

	\brief	This macro is used to register a serializable member in a class constructor.
			For editable mode, it uses _InfoString to explicit the use of the member, in order
			to display it in a GUI for example.
		
*/
#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKDYNAMICFLOAT_XYZ(_object,_InfoString,_DefaultValX,_DefaultValY,_DefaultValZ) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.SetConstant(_DefaultValX,0);\
_object.SetConstant(_DefaultValY,1);\
_object.SetConstant(_DefaultValZ,2);
#else
#define		REGISTER_MEMBER_PACKDYNAMICFLOAT_XYZ(_object,_InfoString,_DefaultValX,_DefaultValY,_DefaultValZ) \
RegisterSerializableMember(_object);
#endif
//end of file
#endif
