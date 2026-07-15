// SPDX-License-Identifier: LGPL-2.1-only

#include "PackFloat_FixedPoint.h"
#include "PackLong.h"
#include "PackString.h"

PackFloat_FixedPoint::PackFloat_FixedPoint() : PackFloat()
	,m_ValueToMultiplyFromIntToFloat(PackFloat::m_1Div64)
{

}
PackFloat_FixedPoint::PackFloat_FixedPoint(const float _ValueToMultiplyFromIntToFloat): PackFloat()
	,m_ValueToMultiplyFromIntToFloat(_ValueToMultiplyFromIntToFloat)
{

}

PackFloat_FixedPoint::PackFloat_FixedPoint( VectorDimension _DimensionEnum,const float _ValueToMultiplyFromIntToFloat ) : PackFloat(_DimensionEnum)
	,m_ValueToMultiplyFromIntToFloat(_ValueToMultiplyFromIntToFloat)
{

}
#ifdef _ENGINE_EDITABLE_
PackFloat_FixedPoint::~PackFloat_FixedPoint(void)
{

}
#endif

const unsigned char * PackFloat_FixedPoint::Serialize_In( const unsigned char * _pDescriptionChunk)
{
	PackLong serlong;
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		_pDescriptionChunk = serlong.Serialize_In(_pDescriptionChunk);
		m_value[ii] = ((float)(serlong.Get())) * m_ValueToMultiplyFromIntToFloat ;
	}
	return(_pDescriptionChunk);
}

#ifdef _ENGINE_EDITABLE_

unsigned int PackFloat_FixedPoint::GetSerializedDescriptionSize(void)
{
	unsigned int size=0;
	PackLong serlong;
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		serlong.Set( (int)(m_value[ii] / m_ValueToMultiplyFromIntToFloat) );
		size += serlong.GetSerializedDescriptionSize();
	}
	return( size ) ;
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * PackFloat_FixedPoint::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	PackLong serlong;
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		serlong.Set( (int)(m_value[ii] / m_ValueToMultiplyFromIntToFloat) );
		_pDescriptionChunkToFill =
				serlong.Serialize_Out( _pDescriptionChunkToFill );
	}
	return( _pDescriptionChunkToFill ) ;
}
#endif
