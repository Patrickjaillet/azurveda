/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackFloatByte_Limits.h"

PackFloatByte_Limits::PackFloatByte_Limits(void): PackFloat()
	,m_MinValue(m_0p0)
	,m_DomainLength(m_1p0)
{

}
/*!
	\brief	Constructor To initialize PackFloat as a multi dimentional vector. 
	\param _DimensionEnum on of the 3 VectorDimension enum.
*/
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
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
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
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.(1,2,3, or4)
*/
unsigned int PackFloatByte_Limits:: GetSerializedDescriptionSize(void)
{
	return( m_NumberOfDimensionManaged ); // 1 byte per dim. 
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
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
/*!
	\brief	change the value:
	\param	_value 28b unsigned int value.
*/
void	PackFloatByte_Limits::Set(float _value)
{
	// assure that we are casted like serialized values:
	Set(0,_value);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	change the value of a given dimension:
	\param _dimensionIndex	0 for X, 1 for Y, 2 for Z
	\param	_value  value
*/
void	PackFloatByte_Limits::Set(unsigned int _dimensionIndex,float _value)
{
	int vv = (int)((_value-m_MinValue)*m_256p0 / m_DomainLength);	
	if( vv<0 ) vv=0;
	if( vv>255 ) vv=255;
	_value = m_MinValue + ((float)vv)*m_1Div256*m_DomainLength ;
	PackFloat::Set(_dimensionIndex,_value);
}
#endif
/*!
	\brief	Initialize the variation domain with 2 floats, then the 256 possible value will be defined inside.
	\param	_min minimum
	\param _max maximum.
*/
void	PackFloatByte_Limits::SetMinMax(float _min,float _max)
{
	m_MinValue = _min;
	m_DomainLength = _max-_min;
}