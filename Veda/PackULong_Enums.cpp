// SPDX-License-Identifier: LGPL-2.1-only

#include "PackULong_Enums.h"
#include "PackString.h"

#ifdef _ENGINE_EDITABLE_

PackULong_Enums::PackULong_Enums(void): PackULong()
	,m_Enums_Meaning_InfoString(0L)
{

}

PackULong_Enums::~PackULong_Enums(void)
{
}

void PackULong_Enums::SetEnumsInfo( const char *_meaning )
{
	m_Enums_Meaning_InfoString = _meaning ;
}

const char	*PackULong_Enums::ValueToString()
{
	const char *pEmpty="";
	if( !m_Enums_Meaning_InfoString ) return(pEmpty);
	if( !m_pValueString ) m_pValueString = new PackString;
	if( m_pValueString )
	{
		unsigned int	enumvalue = m_value;
		unsigned int	stringlength=1;
		unsigned int	enumindex=0;
		bool			WordisActive=false;
		unsigned int	charIndex=0;
		char cc;
		WordisActive = (enumvalue == enumindex );

		while( (cc =  m_Enums_Meaning_InfoString[charIndex]) != 0 )
		{
			if( cc =='.' || cc ==0 )
			{

				enumindex++;
				WordisActive = (enumvalue == enumindex );
			}
			if( WordisActive ) stringlength++;
			charIndex++;
		}

		if( stringlength==0 ) return( pEmpty );

		char	*pWrite,*pTemp = new char[stringlength];
		if(!pTemp ) return(pEmpty);
		pWrite = pTemp ;

		enumindex=0;
		charIndex = 0;
		WordisActive = (enumvalue == enumindex );

		while( (cc =  m_Enums_Meaning_InfoString[charIndex]) != 0 )
		{
			if( cc =='.' || cc ==0 )
			{

				enumindex++;
				WordisActive = (enumvalue == enumindex );
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
