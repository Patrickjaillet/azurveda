// SPDX-License-Identifier: LGPL-2.1-only

#include "PackFloatByte_Color.h"

#ifdef _ENGINE_EDITABLE_
PackFloatByte_Color::PackFloatByte_Color(void): PackFloatByte_Limits()
{

	m_value[0]=
	m_value[1]=
	m_value[2]=
	m_value[3]= PackFloat::m_1p0 ;

}

PackFloatByte_Color::PackFloatByte_Color( VectorDimension _DimensionEnum ) : PackFloatByte_Limits(_DimensionEnum)
{

	m_value[0]=
	m_value[1]=
	m_value[2]=
	m_value[3]= PackFloat::m_1p0 ;
}
PackFloatByte_Color::~PackFloatByte_Color(void)
{

}
#endif
