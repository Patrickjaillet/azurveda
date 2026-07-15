// SPDX-License-Identifier: LGPL-2.1-only

#include "PackString.h"

#ifdef _ENGINE_EDITABLE_
#include "stdio.h"
#endif

#ifdef	_ENGINE_EDITABLE_
PackString::PackString(void): BaseType(),
	m_StringBuffer(0L)
#ifdef _ENGINE_EDITABLE_
	,m_UsedByteLength(0L)
	,m_TotalByteLength(0L)
#endif
{
	Set("");
}
#endif
#ifdef _ENGINE_EDITABLE_
PackString::~PackString(void)
{
	Free();
}
#endif

const unsigned char * PackString::Serialize_In(const unsigned char * _pDescriptionChunk)
{
#ifdef _ENGINE_EDITABLE_
	Free();
#endif
	m_StringBuffer = new char[ length( (const char *)_pDescriptionChunk) ];

	return( (unsigned char *) strcopy(( char *)m_StringBuffer,(const char *)_pDescriptionChunk)  );
}
#ifdef _ENGINE_EDITABLE_

unsigned int PackString:: GetSerializedDescriptionSize(void)
{

	if( !m_StringBuffer ) return(1);

	return( length(m_StringBuffer) );
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * PackString::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	if( !m_StringBuffer )
	{

		*_pDescriptionChunkToFill++=0;
		return(_pDescriptionChunkToFill);
	}
	strcopy(( char *)_pDescriptionChunkToFill,m_StringBuffer);
	_pDescriptionChunkToFill += length( m_StringBuffer );
	return( _pDescriptionChunkToFill  );
}
#endif
#ifdef _ENGINE_EDITABLE_

void PackString::Set( const char *  _pCharacterString )
{

	unsigned int newLength = length(_pCharacterString);
	if(m_StringBuffer)
	{
		if(m_UsedByteLength == newLength)
		{
			if( Compare(m_StringBuffer,_pCharacterString) == 0 ) return;
		}
	}

	BeginChangeNotification();

	if(m_TotalByteLength<newLength)
	{
		Free();
		m_TotalByteLength = (((newLength/m_GrowByteLength)+1)*m_GrowByteLength);
		m_StringBuffer = new char[ m_TotalByteLength ];
		if(!m_StringBuffer) { m_TotalByteLength=0; EndChangeNotification(); return; }
		strcopy( m_StringBuffer, _pCharacterString );
		m_UsedByteLength = newLength;
	} else
	{

		strcopy( m_StringBuffer, _pCharacterString );
		m_UsedByteLength = newLength;
	}

	EndChangeNotification();

}
#endif
#ifdef _ENGINE_EDITABLE_

void PackString::Free()
{
	if( !m_StringBuffer ) return;
	delete [] m_StringBuffer;
	m_StringBuffer = 0L;
	m_UsedByteLength = 0;
	m_TotalByteLength = 0;
}
#endif

unsigned int PackString::length( const char *_pString)
{
	register int length = 1;
	while( *(_pString++) != 0 ) length++;
	return( length );
}

const char *  PackString::strcopy(char *_pStringToFill,const char *_pSourceString)
{
	register char p;
	do
	{	p = *(_pSourceString++);
		*(_pStringToFill++) = p;
	}while( p != 0 );
	return(_pSourceString);
}

int	PackString::Compare( const char *  _pStringToCompare, const char *  _pOtherStringToCompare )
{
	char aa,bb;
	do
	{
		aa = *_pOtherStringToCompare;
		_pOtherStringToCompare++;
		bb= *_pStringToCompare;
		_pStringToCompare++;
		if(aa<bb) return(-1);
		else if(aa>bb) return(1);
	} while( aa != 0 && bb != 0);
	if( aa==0 && bb!=0 ) return(-1);
	if( bb==0 && aa!=0 ) return(1);
	return(0);

}
#ifdef _ENGINE_EDITABLE_

void PackString::AddInt( int _value)
{

	BeginChangeNotification();
		const unsigned int maxdigit = 11;
		unsigned int newlengthmax = m_UsedByteLength + maxdigit ;
		if(newlengthmax<=m_TotalByteLength)
		{
			sprintf( m_StringBuffer+m_UsedByteLength-1, "%d", _value  );
			unsigned int newLength = m_UsedByteLength;
			while(m_StringBuffer[newLength] != 0) newLength++;
			newLength++;
			m_UsedByteLength = newLength;
		} else
		{

			unsigned int newTotal =(((newlengthmax/m_GrowByteLength)+1)*m_GrowByteLength);
			char *pnewchar = new char[ newTotal ];
			if(!pnewchar) { EndChangeNotification(); return; }
			sprintf( pnewchar, "%s%d", m_StringBuffer,_value  );
			delete [] m_StringBuffer;
			m_StringBuffer = pnewchar;
			m_TotalByteLength = newTotal;
			unsigned int newLength = m_UsedByteLength;
			while(m_StringBuffer[newLength] != 0) newLength++;
			newLength++;
			m_UsedByteLength = newLength;
		}
	EndChangeNotification();
}

#endif
#ifdef _ENGINE_EDITABLE_

const char	*PackString::ValueToString()
{
	return Get();
}
#endif
#ifdef _ENGINE_EDITABLE_

void PackString::AddString( const char * _pstr)
{

	BeginChangeNotification();
		unsigned int newlength = m_UsedByteLength-1 + length(_pstr) ;
		if(newlength<=m_TotalByteLength)
		{
			strcopy( m_StringBuffer+m_UsedByteLength-1, _pstr );
			m_UsedByteLength = newlength;
		} else
		{

			unsigned int newTotal =(((newlength/m_GrowByteLength)+1)*m_GrowByteLength);
			char *pnewchar = new char[ newTotal ];
			if(!pnewchar) { EndChangeNotification(); return; }

			strcopy(pnewchar, m_StringBuffer );
			strcopy(pnewchar+m_UsedByteLength-1, _pstr );
			delete [] m_StringBuffer;
			m_StringBuffer = pnewchar;
			m_TotalByteLength = newTotal;
			m_UsedByteLength = newlength;
		}
	EndChangeNotification();
}
#endif
