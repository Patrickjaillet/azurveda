// SPDX-License-Identifier: LGPL-2.1-only

#include "PackULong_Flags.h"
#include "PackString.h"

PackULong_Flags::PackULong_Flags(void): PackULong()
#ifdef _ENGINE_EDITABLE_
	,m_Bitfield_Meaning_InfoString(0L)
#endif
{

}
#ifdef _ENGINE_EDITABLE_
PackULong_Flags::~PackULong_Flags(void)
{
}
#endif
#ifdef _ENGINE_EDITABLE_

void PackULong_Flags::SetBitFieldInfo( const char *_meaning )
{
	m_Bitfield_Meaning_InfoString = _meaning ;

}
#endif
#ifdef _ENGINE_EDITABLE_

const char	*PackULong_Flags::ValueToString()
{
	const char *pEmpty="";
	if( !m_Bitfield_Meaning_InfoString ) return(pEmpty);
	if( !m_pValueString ) m_pValueString = new PackString;
	if( m_pValueString )
	{
		unsigned int	flagvalue = m_value;
		unsigned int	stringlength=1;
		bool			WordisActive=false;
		unsigned int	charIndex=0;
		char cc;
		if( (flagvalue & 1) != 0 ) WordisActive = true;

		while( (cc =  m_Bitfield_Meaning_InfoString[charIndex]) != 0 )
		{
			if( cc =='.' || cc ==0 )
			{

				flagvalue>>=1;
				WordisActive = ((flagvalue & 1) != 0 );
			}
			if( WordisActive ) stringlength++;
			charIndex++;
		}

		if( stringlength==0 ) return( pEmpty );

		char	*pWrite,*pTemp = new char[stringlength];
		if(!pTemp ) return(pEmpty);
		pWrite = pTemp ;

		charIndex = 0;
		flagvalue = m_value ;
		if( (flagvalue & 1) != 0 ) WordisActive = true;
		else  WordisActive = false;

		while( (cc =  m_Bitfield_Meaning_InfoString[charIndex]) != 0 )
		{
			if( cc =='.' || cc ==0 )
			{

				flagvalue>>=1;
				WordisActive = ((flagvalue & 1) != 0 );
			}
			if( WordisActive ) *(pWrite++) = cc;
			charIndex++;
		}
		*(pWrite++) = 0;
		m_pValueString->Set(pTemp);
		delete pTemp;

		return( m_pValueString->Get() );
	}
	return(pEmpty);
}
#endif
