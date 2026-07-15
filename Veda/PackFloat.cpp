/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackFloat.h"
#ifdef _ENGINE_EDITABLE_
#include <float.h>
#include "PackString.h"
#include "stdio.h"
#endif

//! public const float values in order to optimize size/cache access for equations:
const float	PackFloat::m_0p0	= 0.0f; 
const float	PackFloat::m_0p01	= 0.01f;
const float	PackFloat::m_0p00001= 0.00001f;
const float	PackFloat::m_0p25	= 0.25f;
const float	PackFloat::m_0p5	= 0.5f;
const float	PackFloat::m_1p0	= 1.0f;
const float	PackFloat::m_2p0	= 2.0f;
const float	PackFloat::m_0p0625	= 0.0625f; // 1/16
const float	PackFloat::m_1Div255 = 1.0f/255.0f; // 1/255
const float	PackFloat::m_1Div256 = 1.0f/256.0f; // 1/255
const float	PackFloat::m_100p0	= 100.0f;
const float	PackFloat::m_255p0	= 255.0f;
const float	PackFloat::m_256p0	= 256.0f;
const float PackFloat::m_1Div64 = 1.0f/64.0f;
const float PackFloat::m_Pi		 = (float)(3.14159265358979323846);
const float PackFloat::m_2Pi	 = (float)(3.14159265358979323846 * 2.0);
const float	PackFloat::m_180DivPi= 180.0f/3.14159265358979323846f;
const float	PackFloat::m_32767p0 = 32767.0f; //(1<<15)-1 , to make float-1,1 to 16bit conversion.
const float	PackFloat::m_Max = 3.402823466e+38F; // this is max positive.

/*!
	\brief	Default Constructor. 
*/
PackFloat::PackFloat(void): BaseType()
	,m_NumberOfDimensionManaged(1)
{
#ifdef _ENGINE_EDITABLE_
	m_value[0] = m_0p0;
#endif
}
/*!
	\brief	Constructor To initialize PackFloat as a multi dimentional vector. 
*/
PackFloat::PackFloat(VectorDimension _DimensionEnum): BaseType()
	,m_NumberOfDimensionManaged( (unsigned int )_DimensionEnum )
{
#ifdef _ENGINE_EDITABLE_
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )m_value[ii] =m_0p0;
#endif
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Destructor. 
*/
PackFloat::~PackFloat(void)
{

}
#endif
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char * PackFloat::Serialize_In( const unsigned char * _pDescriptionChunk)
{	
	// should work on big endian and little endian systems
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
			*(  (unsigned int *)(&(m_value[ii])) ) = bitImage; // copy int bitImage to a float memory space
		}
	}
	return( _pDescriptionChunk );

}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.(1,2,3, or4)
*/
unsigned int PackFloat:: GetSerializedDescriptionSize(void)
{
	register unsigned int size=0;
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		if( m_value[ii] == m_0p0 ) size++;
		else	size +=3;
	}
	return( size ); 
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * PackFloat::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		if( m_value[ii] == m_0p0 )
		{	// if first character is 0, it means the whole float is 0.
			*(_pDescriptionChunkToFill++) = 0;

		}else
		{
			// should work on big endian and little endian systems
			register unsigned int bitImage = *(  (unsigned int *)(&(m_value[ii])) );
				
			*(_pDescriptionChunkToFill++) = bitImage>>24;
			*(_pDescriptionChunkToFill++) = bitImage>>16;
			*(_pDescriptionChunkToFill++) = bitImage>>8;
		}
	}
	return( _pDescriptionChunkToFill );
}
#endif	
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	convert the value of this object to an explicit string. The object manages the string privately,
		so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			Note: this is not virtual, but each class can manage m_pValueString or not.
	\return	the value as a const string. 
*/
const char	*PackFloat::ValueToString()
{
	if( !m_pValueString ) m_pValueString = new PackString;
	if( m_pValueString )
	{	
		char tmp[128]; // 64 not enough
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
/*!
	\brief	change the value:
	\param	_value 28b unsigned int value.
*/
void	PackFloat::Set(float _value)
{
	if( m_value[0] == _value ) return;
	BeginChangeNotification();
		m_value[0] = _value;
	EndChangeNotification();	
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	change the value of a given dimension:
	\param _dimensionIndex	0 for X, 1 for Y, 2 for Z
	\param	_value  value
*/
void	PackFloat::Set(unsigned int _dimensionIndex,float _value)
{
	if( m_value[_dimensionIndex] == _value ) return;
	BeginChangeNotification();
		m_value[_dimensionIndex] = _value;
	EndChangeNotification();	
}
#endif
