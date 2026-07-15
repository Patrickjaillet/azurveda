// SPDX-License-Identifier: LGPL-2.1-only

#include "PackFloat.h"
#ifdef _ENGINE_EDITABLE_
#include <float.h>
#include "PackString.h"
#include "stdio.h"
#endif

const float	PackFloat::m_0p0	= 0.0f;
const float	PackFloat::m_0p01	= 0.01f;
const float	PackFloat::m_0p00001= 0.00001f;
const float	PackFloat::m_0p25	= 0.25f;
const float	PackFloat::m_0p5	= 0.5f;
const float	PackFloat::m_1p0	= 1.0f;
const float	PackFloat::m_2p0	= 2.0f;
const float	PackFloat::m_0p0625	= 0.0625f;
const float	PackFloat::m_1Div255 = 1.0f/255.0f;
const float	PackFloat::m_1Div256 = 1.0f/256.0f;
const float	PackFloat::m_100p0	= 100.0f;
const float	PackFloat::m_255p0	= 255.0f;
const float	PackFloat::m_256p0	= 256.0f;
const float PackFloat::m_1Div64 = 1.0f/64.0f;
const float PackFloat::m_Pi		 = (float)(3.14159265358979323846);
const float PackFloat::m_2Pi	 = (float)(3.14159265358979323846 * 2.0);
const float	PackFloat::m_180DivPi= 180.0f/3.14159265358979323846f;
const float	PackFloat::m_32767p0 = 32767.0f;
const float	PackFloat::m_Max = 3.402823466e+38F;

PackFloat::PackFloat(void): BaseType()
	,m_NumberOfDimensionManaged(1)
{
#ifdef _ENGINE_EDITABLE_
	m_value[0] = m_0p0;
#endif
}

PackFloat::PackFloat(VectorDimension _DimensionEnum): BaseType()
	,m_NumberOfDimensionManaged( (unsigned int )_DimensionEnum )
{
#ifdef _ENGINE_EDITABLE_
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )m_value[ii] =m_0p0;
#endif
}
#ifdef _ENGINE_EDITABLE_

PackFloat::~PackFloat(void)
{

}
#endif

const unsigned char * PackFloat::Serialize_In( const unsigned char * _pDescriptionChunk)
{

	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		unsigned int bitImage = (unsigned int)(*(_pDescriptionChunk++));
		if( bitImage == 0 )
		{
			m_value[ii] = m_0p0 ;
		} else
		{
			bitImage <<= 24;
			bitImage |= ((unsigned int)*(_pDescriptionChunk++))<<16;
			bitImage |= ((unsigned int)*(_pDescriptionChunk++))<<8;
			*(  (unsigned int *)(&(m_value[ii])) ) = bitImage;
		}
	}
	return( _pDescriptionChunk );

}
#ifdef _ENGINE_EDITABLE_

unsigned int PackFloat:: GetSerializedDescriptionSize(void)
{
	unsigned int size=0;
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		if( m_value[ii] == m_0p0 ) size++;
		else	size +=3;
	}
	return( size );
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * PackFloat::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		if( m_value[ii] == m_0p0 )
		{
			*(_pDescriptionChunkToFill++) = 0;

		}else
		{

			unsigned int bitImage = *(  (unsigned int *)(&(m_value[ii])) );

			*(_pDescriptionChunkToFill++) = (unsigned char)(bitImage>>24);
			*(_pDescriptionChunkToFill++) = (unsigned char)(bitImage>>16);
			*(_pDescriptionChunkToFill++) = (unsigned char)(bitImage>>8);
		}
	}
	return( _pDescriptionChunkToFill );
}
#endif
#ifdef _ENGINE_EDITABLE_

const char	*PackFloat::ValueToString()
{
	if( !m_pValueString ) m_pValueString = new PackString;
	if( m_pValueString )
	{
		char tmp[128];
		switch( m_NumberOfDimensionManaged )
		{
			case vd_XY:
				sprintf(tmp,"x:%f y:%f", m_value[0], m_value[1]);
				break;
			case vd_XYZ:
				sprintf(tmp,"x:%f y:%f z:%f", m_value[0], m_value[1], m_value[2]);
				break;
			case vd_XYZD:
				sprintf(tmp,"x:%f y:%f z:%f d:%f", m_value[0], m_value[1], m_value[2],m_value[3]);
				break;
			default:
				sprintf(tmp,"%f", m_value[0] );
			break;
		}
		m_pValueString->Set(tmp);
		return( m_pValueString->Get() );
	}
	return("");
}
#endif
#ifdef _ENGINE_EDITABLE_

void	PackFloat::Set(float _value)
{
	if( m_value[0] == _value ) return;
	BeginChangeNotification();
		m_value[0] = _value;
	EndChangeNotification();
}
#endif
#ifdef _ENGINE_EDITABLE_

void	PackFloat::Set(unsigned int _dimensionIndex,float _value)
{
	if( m_value[_dimensionIndex] == _value ) return;
	BeginChangeNotification();
		m_value[_dimensionIndex] = _value;
	EndChangeNotification();
}
#endif
