// SPDX-License-Identifier: LGPL-2.1-only

#include "PackDynamicFloat.h"
#include "VirtualEquation.h"
#include "PackFloat.h"

PackDynamicFloat::PackDynamicFloat(PackFloat::VectorDimension _DimensionEnum) : PackDynamicType()
	,m_NumberOfDimensionManaged(_DimensionEnum)
	,mSer_Constant(_DimensionEnum)
{

	REGISTER_MEMBER(mSer_Constant,"Constant");

	REGISTER_MEMBER_REFERENCE(mSer_EquationReference,"Cinematic", VirtualEquation::m_Description );

}
#ifdef _ENGINE_EDITABLE_

void	PackDynamicFloat::SetConstant(float _value,unsigned int _dimensionIndex)
{
	Set(1);
	mSer_Constant.Set(_dimensionIndex,_value);
}
#endif

void PackDynamicFloat::Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )
{
	unsigned int ii;

	switch( mSer_TypeEnum.Get() )
	{
		default:
		case 0:
			break;
		case 1:
			for(ii=0;ii<m_NumberOfDimensionManaged;ii++) _OutgoingParameterTable[ii] += mSer_Constant.Get(ii);
			break;
		case 2:
			{
			VirtualEquation *pEquation = (VirtualEquation *)mSer_EquationReference.GetObjectPointer();
			if(pEquation) pEquation->Compute(_OutgoingParameterTable,_IncomingParameterTable);
			}
			break;
	}

}
