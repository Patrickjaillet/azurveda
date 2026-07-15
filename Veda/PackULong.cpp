/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackULong.h"
#include "PackString.h"

#ifdef _ENGINE_EDITABLE_
PackULong::PackULong(void): BaseType()
	,m_value(0L) //default
{

}
#endif
#ifdef _ENGINE_EDITABLE_
PackULong::~PackULong(void)
{

}
#endif
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char * PackULong::Serialize_In( const unsigned char * _pDescriptionChunk)
{
	// this should pack a [0,256Mb-1] integer on 1,2,3 or 4 bytes.
	register unsigned int value;
	register unsigned int nbByte;
	register unsigned int cc;
	register unsigned int bitToShift;
	bitToShift = value = nbByte = 0 ;
	do
	{	
		//value<<=7; // previous bytes are strong.
		cc = ( unsigned int) *(_pDescriptionChunk++);
		value |= ((cc & 127)<<bitToShift);
		bitToShift+=7;
		nbByte++;
	} while( nbByte<4 && (cc & 128) );

	m_value = value;

	return(_pDescriptionChunk);
}


#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.(1,2,3, or4)
*/
unsigned int PackULong:: GetSerializedDescriptionSize(void)
{
	register unsigned int value = m_value;
	register unsigned int nbByte=0;
	do
	{
		value>>=7;
		nbByte++;
	} while( value > 0 && (nbByte<4)  );

	return(nbByte);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * PackULong::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	register unsigned int value = m_value;
	register unsigned int nbByte=0;
	register unsigned int cc;
	do
	{
		cc = value & 127 ; //
		if( value>127 ) cc |= 128; // bit 7 indicates if value continue on next byte.
		*(_pDescriptionChunkToFill++) = cc;
		value>>=7;
		nbByte++;
	} while( value > 0 && (nbByte<4)  );

	return(_pDescriptionChunkToFill);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	change the value:
	\param	_value 28b unsigned int value.
*/
void	PackULong::Set(unsigned int _value)
{
	if( m_value == _value ) return;
	BeginChangeNotification();
		m_value = _value;
	EndChangeNotification();
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	convert the value of this object to an explicit string. The object manages the string privately,
		so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			Note: this is not virtual, but each class can manage m_pValueString or not.
	\return	the value as a const string. 
*/
const char	*PackULong::ValueToString()
{
	if( !m_pValueString ) m_pValueString = new PackString;
	if( m_pValueString )
	{	
		m_pValueString->Set("");
		m_pValueString->AddInt( m_value );
		return( m_pValueString->Get() );
	}
	return("");
}
#endif
