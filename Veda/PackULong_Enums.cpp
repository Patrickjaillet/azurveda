/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackULong_Enums.h"
#include "PackString.h"
// this class only exists when editable:
#ifdef _ENGINE_EDITABLE_

PackULong_Enums::PackULong_Enums(void): PackULong()
	,m_Enums_Meaning_InfoString(0L)
{

}

PackULong_Enums::~PackULong_Enums(void)
{
}


/*!
	\brief set an infostring for the bitfield meaning.
	\param a string looking like: "bit0.bit1.bit2"
*/
void PackULong_Enums::SetEnumsInfo( const char *_meaning )
{
	m_Enums_Meaning_InfoString = _meaning ;
}
/*!
	\brief	convert the value of this object to an explicit string. The object manages the string privately,
		so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			Note: this is not virtual, but each class can manage m_pValueString or not.
	\return	the value as a const string. 
*/
const char	*PackULong_Enums::ValueToString()
{
	const char *pEmpty="";	// "" is actually global.
	if( !m_Enums_Meaning_InfoString ) return(pEmpty);
	if( !m_pValueString ) m_pValueString = new PackString;
	if( m_pValueString )
	{
		unsigned int	enumvalue = m_value;
		unsigned int	stringlength=1; // for end '\0'
		unsigned int	enumindex=0;
		bool			WordisActive=false;
		unsigned int	charIndex=0;
		char cc;
		WordisActive = (enumvalue == enumindex );
		// count total string length
		while( (cc =  m_Enums_Meaning_InfoString[charIndex]) != 0 )
		{
			if( cc =='.' || cc ==0 )
			{	
				// end of a 'enum' word:
				enumindex++;
				WordisActive = (enumvalue == enumindex );
			}
			if( WordisActive ) stringlength++;
			charIndex++;
		}

		if( stringlength==0 ) return( pEmpty );
		// create temporary new string:
		char	*pWrite,*pTemp = new char[stringlength]; // with the last 0.
		if(!pTemp ) return(pEmpty);
		pWrite = pTemp ;
		// scan again and copy the string.
		enumindex=0;
		charIndex = 0;
		WordisActive = (enumvalue == enumindex );
		// copy parts of string:
		while( (cc =  m_Enums_Meaning_InfoString[charIndex]) != 0 )
		{
			if( cc =='.' || cc ==0 )
			{	
				// end of a 'bit' word:
				enumindex++;
				WordisActive = (enumvalue == enumindex );
			}
			if( WordisActive ) *(pWrite++) = cc;
			charIndex++;
		} 
		*(pWrite++) = 0;
		m_pValueString->Set(pTemp); // so that the string is persistent after the call.
		delete pTemp;

		return( m_pValueString->Get() );
	}
	return(pEmpty);
}
// end if editable:
#endif
