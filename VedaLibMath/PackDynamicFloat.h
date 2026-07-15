// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackDynamicFloat_H
#define COM_M4NKIND_PackDynamicFloat_H

#include "PackDynamicType.h"
#include "PackFloat_FixedPoint.h"
#include "PackFloat.h"
#include "PackObjectReference.h"

class PackDynamicFloat : public PackDynamicType
{

public:

	PackDynamicFloat( PackFloat::VectorDimension _DimensionEnum);
#ifdef _ENGINE_EDITABLE_

	void	SetConstant(float _value,unsigned int _dimensionIndex=0);
#endif

	void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

protected:

	PackFloat				mSer_Constant;

	PackObjectReference		mSer_EquationReference;

	const unsigned int	m_NumberOfDimensionManaged;
};

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

#endif
