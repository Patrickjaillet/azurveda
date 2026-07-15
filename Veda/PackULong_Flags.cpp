/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackULong_Flags.h"
#include "PackString.h"
// this class only exists when editable:
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
/*!
	\brief set an infostring for the bitfield meaning.
	\param a string looking like: "bit0.bit1.bit2"
*/
void PackULong_Flags::SetBitFieldInfo( const char *_meaning )
{
	m_Bitfield_Meaning_InfoString = _meaning ;

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	convert the value of this object to an explicit string. The object manages the string privately,
		so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			Note: this is not virtual, but each class can manage m_pValueString or not.
	\return	the value as a const string. 
*/
const char	*PackULong_Flags::ValueToString()
{
	char *pEmpty="";	// "" is actually global.
	if( !m_Bitfield_Meaning_InfoString ) return(pEmpty);
	if( !m_pValueString ) m_pValueString = new PackString;
	if( m_pValueString )
	{	
		unsigned int	flagvalue = m_value;
		unsigned int	stringlength=1; // for end '\0'
		bool			WordisActive=false;
		unsigned int	charIndex=0;
		char cc;
		if( (flagvalue & 1) != 0 ) WordisActive = true;
		// count total string length
		while( (cc =  m_Bitfield_Meaning_InfoString[charIndex]) != 0 )
		{
			if( cc =='.' || cc ==0 )
			{	
				// end of a 'bit' word:
				flagvalue>>=1;
				WordisActive = ((flagvalue & 1) != 0 );
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
		charIndex = 0;
		flagvalue = m_value ;
		if( (flagvalue & 1) != 0 ) WordisActive = true;
		else  WordisActive = false;
		// copy parts of string:
		while( (cc =  m_Bitfield_Meaning_InfoString[charIndex]) != 0 )
		{
			if( cc =='.' || cc ==0 )
			{	
				// end of a 'bit' word:
				flagvalue>>=1;
				WordisActive = ((flagvalue & 1) != 0 );
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
#endif