/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackLong.h"
#include "PackULong.h"
#include "PackString.h"
#ifdef _ENGINE_EDITABLE_
PackLong::PackLong(void): BaseType()
	,m_value(0) //default
{
	// that's all, it is a base type.
}
#endif
#ifdef _ENGINE_EDITABLE_
PackLong::~PackLong(void)
{

}
#endif
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char * PackLong::Serialize_In( const unsigned char * _pDescriptionChunk)
{
	PackULong bitfield;
	register const unsigned char *pnextchunk = bitfield.Serialize_In(_pDescriptionChunk);

	m_value = bitfield.Get()>>1;
	if( bitfield.Get() & 1 ) m_value = - m_value;

	return(pnextchunk);
}


#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.(1,2,3, or4)
*/
unsigned int PackLong:: GetSerializedDescriptionSize(void)
{
	PackULong bitfield;
	register signed int bitfieldvalue = m_value;
	if( m_value<0  ) bitfieldvalue = - bitfieldvalue;
	bitfieldvalue<<=1;
	if( m_value<0  ) bitfieldvalue |= 1;

	bitfield.Set( bitfieldvalue );
	return( bitfield.GetSerializedDescriptionSize() ) ;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * PackLong::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	PackULong bitfield;
	register unsigned int bitfieldvalue = (unsigned int)m_value;
	if( m_value<0  ) bitfieldvalue = - (int)bitfieldvalue;
	bitfieldvalue<<=1;
	if( m_value<0  ) bitfieldvalue |= 1;

	bitfield.Set( bitfieldvalue );
	
	return( bitfield.Serialize_Out(_pDescriptionChunkToFill) );
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief set the value of this type, which will be cut to teh defined limits.
	\param _value the value to set, which will be cut to teh defined limits.
*/
void	PackLong::Set(int _value)
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
const char	*PackLong::ValueToString()
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
