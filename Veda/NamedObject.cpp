// SPDX-License-Identifier: LGPL-2.1-only

#include "NamedObject.h"

NamedObject::NamedObject() : PackStruct()
	,m_pBaseContext(0L)
{

	REGISTER_MEMBER(mSerStr_ObjectName,"Name");

}
#ifdef _ENGINE_EDITABLE_
NamedObject::~NamedObject(void)
{

}
#endif

#ifdef _ENGINE_EDITABLE_

void  NamedObject::SetName( const char * _pname )
{
	mSerStr_ObjectName.Set( _pname );

}
#endif

const unsigned char * NamedObject::Serialize_In( const unsigned char * _pDescriptionChunk)
{

	register const unsigned char	*pEndOfChunk = _pDescriptionChunk ;
	register Cell	*pCell = m_pFirstCell ;

	_pDescriptionChunk = mSerUlong_ChunkLength.Serialize_In(_pDescriptionChunk);

	pEndOfChunk += mSerUlong_ChunkLength.Get() ;

	while( (_pDescriptionChunk < pEndOfChunk) && pCell )
	{
		_pDescriptionChunk = pCell->GetManagedObject()->Serialize_In( _pDescriptionChunk );
		pCell = pCell->GetNextBrotherCell();
	}

	return( _pDescriptionChunk );
}

#ifdef _ENGINE_EDITABLE_

unsigned int NamedObject::GetSerializedDescriptionSize(void)
{

	register unsigned int deltaJumpLength;
	register unsigned int length=1;
	register Cell	*pCell = m_pFirstCell ;

	while( pCell )
	{
		length += pCell->GetManagedObject()->GetSerializedDescriptionSize();
		pCell = pCell->GetNextBrotherCell();
	}

	mSerUlong_ChunkLength.Set( length );
	deltaJumpLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() - 1 ;
	if( deltaJumpLength > 0 )
	{
		length += deltaJumpLength;

		deltaJumpLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() ;
		mSerUlong_ChunkLength.Set( length );
		deltaJumpLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() - deltaJumpLength ;
		if( deltaJumpLength > 0 )
		{
			length += deltaJumpLength;
			mSerUlong_ChunkLength.Set( length );
		}
	}
	return( length );
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * NamedObject::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	register Cell	*pCell = m_pFirstCell ;
	_pDescriptionChunkToFill = mSerUlong_ChunkLength.Serialize_Out( _pDescriptionChunkToFill );
	while( pCell )
	{
		_pDescriptionChunkToFill = pCell->GetManagedObject()->Serialize_Out( _pDescriptionChunkToFill );
		pCell = pCell->GetNextBrotherCell();
	}
	return(_pDescriptionChunkToFill);
}
#endif
