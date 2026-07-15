// SPDX-License-Identifier: LGPL-2.1-only

#include "PackULong.h"
#include "PackString.h"

#ifdef _ENGINE_EDITABLE_
PackULong::PackULong(void): BaseType()
	,m_value(0L)
{

}
#endif
#ifdef _ENGINE_EDITABLE_
PackULong::~PackULong(void)
{

}
#endif

const unsigned char * PackULong::Serialize_In( const unsigned char * _pDescriptionChunk)
{

	unsigned int value;
	unsigned int nbByte;
	unsigned int cc;
	unsigned int bitToShift;
	bitToShift = value = nbByte = 0 ;
	do
	{

		cc = ( unsigned int) *(_pDescriptionChunk++);
		value |= ((cc & 127)<<bitToShift);
		bitToShift+=7;
		nbByte++;
	} while( nbByte<4 && (cc & 128) );

	m_value = value;

	return(_pDescriptionChunk);
}

#ifdef _ENGINE_EDITABLE_

unsigned int PackULong:: GetSerializedDescriptionSize(void)
{
	unsigned int value = m_value;
	unsigned int nbByte=0;
	do
	{
		value>>=7;
		nbByte++;
	} while( value > 0 && (nbByte<4)  );

	return(nbByte);
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * PackULong::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	unsigned int value = m_value;
	unsigned int nbByte=0;
	unsigned int cc;
	do
	{
		cc = value & 127 ;
		if( value>127 ) cc |= 128;
		*(_pDescriptionChunkToFill++) = cc;
		value>>=7;
		nbByte++;
	} while( value > 0 && (nbByte<4)  );

	return(_pDescriptionChunkToFill);
}
#endif
#ifdef _ENGINE_EDITABLE_

void	PackULong::Set(unsigned int _value)
{
	if( m_value == _value ) return;
	BeginChangeNotification();
		m_value = _value;
	EndChangeNotification();
}
#endif
#ifdef _ENGINE_EDITABLE_

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
