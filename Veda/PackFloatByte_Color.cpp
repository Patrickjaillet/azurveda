/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackFloatByte_Color.h"

// this class only exists when editable:
#ifdef _ENGINE_EDITABLE_
PackFloatByte_Color::PackFloatByte_Color(void): PackFloatByte_Limits()
{
	// for colors, default is more often 1 than 0.
	m_value[0]=
	m_value[1]=
	m_value[2]=
	m_value[3]= PackFloat::m_1p0 ;

}
/*!
	\brief	Constructor To initialize PackFloat as a multi dimentional vector. 
	\param _DimensionEnum on of the 3 VectorDimension enum.
*/
PackFloatByte_Color::PackFloatByte_Color( VectorDimension _DimensionEnum ) : PackFloatByte_Limits(_DimensionEnum)
{
	// for colors, default is more often 1 than 0.
	m_value[0]=
	m_value[1]=
	m_value[2]=
	m_value[3]= PackFloat::m_1p0 ;
}
PackFloatByte_Color::~PackFloatByte_Color(void)
{

}
#endif
