// SPDX-License-Identifier: LGPL-2.1-only

#include "PackFloatByte_Limits.h"

PackFloatByte_Limits::PackFloatByte_Limits(void): PackFloat()
	,m_MinValue(m_0p0)
	,m_DomainLength(m_1p0)
{

}

PackFloatByte_Limits::PackFloatByte_Limits( VectorDimension _DimensionEnum ) : PackFloat(_DimensionEnum)
	,m_MinValue(m_0p0)
	,m_DomainLength(m_1p0)
{

}
#ifdef _ENGINE_EDITABLE_
PackFloatByte_Limits::~PackFloatByte_Limits(void)
{

}
#endif

const unsigned char * PackFloatByte_Limits::Serialize_In( const unsigned char * _pDescriptionChunk)
{
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		unsigned int bitImage = (unsigned int)(*(_pDescriptionChunk++));
		m_value[ii] = m_MinValue + ((float)bitImage)*m_1Div255*m_DomainLength ;
	}
	return( _pDescriptionChunk );
}
#ifdef _ENGINE_EDITABLE_

unsigned int PackFloatByte_Limits:: GetSerializedDescriptionSize(void)
{
	return( m_NumberOfDimensionManaged );
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * PackFloatByte_Limits::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		*(_pDescriptionChunkToFill++) = (unsigned char)((m_value[ii]-m_MinValue)*m_255p0 / m_DomainLength);
	}
	return( _pDescriptionChunkToFill );
}
#endif

#ifdef _ENGINE_EDITABLE_

void	PackFloatByte_Limits::Set(float _value)
{

	Set(0,_value);
}
#endif
#ifdef _ENGINE_EDITABLE_

void	PackFloatByte_Limits::Set(unsigned int _dimensionIndex,float _value)
{
	int vv = (int)((_value-m_MinValue)*m_256p0 / m_DomainLength);
	if( vv<0 ) vv=0;
	if( vv>255 ) vv=255;
	_value = m_MinValue + ((float)vv)*m_1Div256*m_DomainLength ;
	PackFloat::Set(_dimensionIndex,_value);
}
#endif

void	PackFloatByte_Limits::SetMinMax(float _min,float _max)
{
	m_MinValue = _min;
	m_DomainLength = _max-_min;
}
