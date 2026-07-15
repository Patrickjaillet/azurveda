/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackLong_WithLimits.h"
// this class only exists when editable:
#ifdef _ENGINE_EDITABLE_

PackLong_WithLimits::PackLong_WithLimits(void): PackLong()
{

}

PackLong_WithLimits::~PackLong_WithLimits(void)
{
}

/*!
	\brief set the value of this type, which will be cut to teh defined limits.
	\param _value the value to set, which will be cut to teh defined limits.
*/
void PackLong_WithLimits::Set( signed int _value )
{
	if( m_value == _value ) return;
	if( _value<m_Minimum ) _value=m_Minimum;
	if( _value>m_Maximum ) _value=m_Maximum;
	PackLong::Set(_value);
}

/*!
	\brief set the minimum and maximum values possible for this type.
	\param _min the minimum value possible for this object.
	\param _max the maximum value possible for this object.
*/
void	PackLong_WithLimits::SetMinMax( signed int _min, signed int _max )
{
	m_Minimum = _min;
	m_Maximum = _max;
	// check the value for this range:
	if( m_value<_min )m_value= _min;
	if( m_value>_max) m_value = _max;
}
#endif