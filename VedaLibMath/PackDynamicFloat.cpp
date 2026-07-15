/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackDynamicFloat.h"
#include "VirtualEquation.h"
#include "PackFloat.h"

/*!
	\brief	Constructor. There should only be initialisation there.
	\param _DimensionEnum the dimension of the value.:(1,2,3,4) PackFloat::vd_...
*/
PackDynamicFloat::PackDynamicFloat(PackFloat::VectorDimension _DimensionEnum) : PackDynamicType()
	,m_NumberOfDimensionManaged(_DimensionEnum)
	,mSer_Constant(_DimensionEnum)
{
	// possible shapes:
	// 1:
	REGISTER_MEMBER(mSer_Constant,"Constant");
	// 2:
	REGISTER_MEMBER_REFERENCE(mSer_EquationReference,"Cinematic", VirtualEquation::m_Description );

}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	editable mode accesor, that force a constant type, and its value.
	\param _value the new value.
	\param _dimensionIndex of the dimension affected. 0 is default.
*/
void	PackDynamicFloat::SetConstant(float _value,unsigned int _dimensionIndex)
{
	Set(1);
	mSer_Constant.Set(_dimensionIndex,_value);
}
#endif
/*!
	\brief	 Compute a scalar value according to a parameter list (X,Y,Z,D). SAME AS VirtualEquation.
		Note this version is not virtual, because there is no reason for extension a prioris.
	\param	_OutgoingParameterTable result table. elements are writen or not according to the equation.
	\param _IncomingParameterTable. float table, the (X,Y,Z,D) parameters.
*/
void PackDynamicFloat::Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )
{
	unsigned int ii;
	// default:
	// now default must be set before... parameters not managed because no sense in dimension are left.
	//for(ii=0;ii<m_NumberOfDimensionManaged;ii++) _OutgoingParameterTable[ii] = PackFloat::m_0p0;

	switch( mSer_TypeEnum.Get() )
	{
		default:	// in case of no selection of (2) OR no type .
		case 0: // no type selected:
			break;
		case 1: // scalar constant value:
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

