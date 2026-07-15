// SPDX-License-Identifier: LGPL-2.1-only

#include "PackLong_WithLimits.h"

#ifdef _ENGINE_EDITABLE_

PackLong_WithLimits::PackLong_WithLimits(void): PackLong()
{

}

PackLong_WithLimits::~PackLong_WithLimits(void)
{
}

void PackLong_WithLimits::Set( signed int _value )
{
	if( m_value == _value ) return;
	if( _value<m_Minimum ) _value=m_Minimum;
	if( _value>m_Maximum ) _value=m_Maximum;
	PackLong::Set(_value);
}

void	PackLong_WithLimits::SetMinMax( signed int _min, signed int _max )
{
	m_Minimum = _min;
	m_Maximum = _max;

	if( m_value<_min )m_value= _min;
	if( m_value>_max) m_value = _max;
}
#endif
