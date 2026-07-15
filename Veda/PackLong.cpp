// SPDX-License-Identifier: LGPL-2.1-only

#include "PackLong.h"
#include "PackULong.h"
#include "PackString.h"
#ifdef _ENGINE_EDITABLE_
PackLong::PackLong(void): BaseType()
	,m_value(0)
{

}
#endif
#ifdef _ENGINE_EDITABLE_
PackLong::~PackLong(void)
{

}
#endif

const unsigned char * PackLong::Serialize_In( const unsigned char * _pDescriptionChunk)
{
	PackULong bitfield;
	const unsigned char *pnextchunk = bitfield.Serialize_In(_pDescriptionChunk);

	m_value = bitfield.Get()>>1;
	if( bitfield.Get() & 1 ) m_value = - m_value;

	return(pnextchunk);
}

#ifdef _ENGINE_EDITABLE_

unsigned int PackLong:: GetSerializedDescriptionSize(void)
{
	PackULong bitfield;
	signed int bitfieldvalue = m_value;
	if( m_value<0  ) bitfieldvalue = - bitfieldvalue;
	bitfieldvalue<<=1;
	if( m_value<0  ) bitfieldvalue |= 1;

	bitfield.Set( bitfieldvalue );
	return( bitfield.GetSerializedDescriptionSize() ) ;
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * PackLong::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	PackULong bitfield;
	unsigned int bitfieldvalue = (unsigned int)m_value;
	if( m_value<0  ) bitfieldvalue = - (int)bitfieldvalue;
	bitfieldvalue<<=1;
	if( m_value<0  ) bitfieldvalue |= 1;

	bitfield.Set( bitfieldvalue );

	return( bitfield.Serialize_Out(_pDescriptionChunkToFill) );
}
#endif
#ifdef _ENGINE_EDITABLE_

void	PackLong::Set(int _value)
{
	if( m_value == _value ) return;
	BeginChangeNotification();
		m_value = _value;
	EndChangeNotification();
}
#endif
#ifdef _ENGINE_EDITABLE_

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
