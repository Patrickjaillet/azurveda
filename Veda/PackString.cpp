/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
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
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char * PackString::Serialize_In(const unsigned char * _pDescriptionChunk)
{
#ifdef _ENGINE_EDITABLE_
	Free();
#endif
	m_StringBuffer = new char[ length( (const char *)_pDescriptionChunk) ];
	// strcopy return after read memory:
	return( (unsigned char *) strcopy(( char *)m_StringBuffer,(const char *)_pDescriptionChunk)  );
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.(1,2,3, or4)
*/
unsigned int PackString:: GetSerializedDescriptionSize(void)
{
	// shouldn't happen: serialisation output but string not inited.
	// no matter, write empty chain:
	if( !m_StringBuffer ) return(1);
	// normal case:
	return( length(m_StringBuffer) );
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * PackString::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	if( !m_StringBuffer )
	{	// shouldn't happen: serialisation output but string not inited.
		// no matter, write empty chain:
		*_pDescriptionChunkToFill++=0;
		return(_pDescriptionChunkToFill);
	}
	strcopy(( char *)_pDescriptionChunkToFill,m_StringBuffer);
	_pDescriptionChunkToFill += length( m_StringBuffer );
	return( _pDescriptionChunkToFill  );
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief set the string value. It reallocates.
	as it is used by serialisation, it returns a pointer on the read memory AFTER the zero for convenience.
	\param the string to copy.
	\return a pointer after read memory
*/

void PackString::Set( const char *  _pCharacterString )
{
//#ifdef _ENGINE_EDITABLE_
	// if it is the same value, do nothing
	unsigned int newLength = length(_pCharacterString);
	if(m_StringBuffer)
	{
		if(m_UsedByteLength == newLength) // optimisation.
		{
			if( Compare(m_StringBuffer,_pCharacterString) == 0 ) return;
		}
	}
	// all Set() have to begin with:
	BeginChangeNotification();
	// realloc if not enough data, or keep the same alloc:
	//old: Free();
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
		// note: for perfection, we should test if the new size is >m_TotalByteLength-m_GrowByteLength
		// then realloc * less * so that useless memory is freed.
		// re-use remaining bytes in the alloc:
		strcopy( m_StringBuffer, _pCharacterString );
		m_UsedByteLength = newLength;
	}
	// Notify change to context:
	EndChangeNotification();
//#else
	//  ---- non editable mode ----:
/*
	// change the managed string:
	m_StringBuffer = new char[ length(_pCharacterString) ];
	if(!m_StringBuffer )  return;
	strcopy( m_StringBuffer, _pCharacterString );
*/
//#endif

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief delete if was allocated.
*/
void PackString::Free()
{
	if( !m_StringBuffer ) return;
	delete [] m_StringBuffer;
	m_StringBuffer = 0L;
	m_UsedByteLength = 0;
	m_TotalByteLength = 0;
}
#endif
/*!
	\brief tool. like strlen().
*/
unsigned int PackString::length( const char *_pString)
{
	register int length = 1;
	while( *(_pString++) != 0 ) length++;
	return( length );
}
/*!
	\brief private tool. like strcpy(), but with +1 for the zero end.
	\param _pStringToFill destination.
	\param _pSourceString source.
	\return a pointer after read memory
*/
const char *  PackString::strcopy(char *_pStringToFill,const char *_pSourceString)
{	// work for 0 char strings.
	register char p;
	do 
	{	p = *(_pSourceString++);
		*(_pStringToFill++) = p;
	}while( p != 0 );
	return(_pSourceString);
}
/*!
	\brief compare a string. looks like strcmp().
	\param the string to compare to the value
	\param the other string
	\return 0 if the same. case sensitive test.
*/
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
/*!
	\brief	add a int value at the end of the current chain.
	\param	the integer value.
*/
void PackString::AddInt( int _value)
{	
	/* old
	unsigned int ll = length( m_StringBuffer );
	unsigned int newlengthmax = ll+20+1 ;
	char *pnewchar = new char[ newlengthmax ];
		sprintf( pnewchar, "%s%d", m_StringBuffer,_value  );
		Set( pnewchar );
	delete [] pnewchar;
	*/
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
			// not enough space in the buffer, realloc new big:
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
	/*!
		\brief	convert the value of this object to an explicit string. The object manages the string privately,
			so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			 Note: this is not virtual, but each class can manage m_pValueString or not.
		\return	the value as a const string. 
	*/
const char	*PackString::ValueToString()
{
	return Get();
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	apend a character string..
	\param	_pstr 
*/
void PackString::AddString( const char * _pstr)
{
	/*old
	unsigned int ll = length( m_StringBuffer );
	unsigned int newlengthmax = ll+ length( _pstr ); // +1
	char *pnewchar = new char[ newlengthmax ];
		sprintf( pnewchar, "%s%s", m_StringBuffer,_pstr  );
		Set( pnewchar );
	delete [] pnewchar;	
	*/
	BeginChangeNotification();
		unsigned int newlength = m_UsedByteLength-1 + length(_pstr) ;
		if(newlength<=m_TotalByteLength)
		{
			strcopy( m_StringBuffer+m_UsedByteLength-1, _pstr );
			m_UsedByteLength = newlength;
		} else
		{
			// not enough space in the buffer, realloc new big:
			unsigned int newTotal =(((newlength/m_GrowByteLength)+1)*m_GrowByteLength);
			char *pnewchar = new char[ newTotal ];
			if(!pnewchar) { EndChangeNotification(); return; }
			//sprintf( pnewchar, "%s%d", m_StringBuffer,_value  );
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
