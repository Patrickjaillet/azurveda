/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "NamedObject.h"

NamedObject::NamedObject() : PackStruct()
	,m_pBaseContext(0L)
{
	// all NamedObject are serialized with PackStruct::jumplength, 
	// then a packstring that indicates their reference name.
	REGISTER_MEMBER(mSerStr_ObjectName,"Name");

}
#ifdef _ENGINE_EDITABLE_
NamedObject::~NamedObject(void)
{

}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	set the name of the object as a character string. This is copied of course.
	\param _pname teh new name
*/
void  NamedObject::SetName( const char * _pname )
{
	mSerStr_ObjectName.Set( _pname );

}
#endif
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char * NamedObject::Serialize_In( const unsigned char * _pDescriptionChunk)
{
	// find the end of the list chunk:
	register const unsigned char	*pEndOfChunk = _pDescriptionChunk ;
	register Cell	*pCell = m_pFirstCell ;
	//  serialize the chunk length attribute:
	_pDescriptionChunk = mSerUlong_ChunkLength.Serialize_In(_pDescriptionChunk);
	// the end of the chunk is:
	pEndOfChunk += mSerUlong_ChunkLength.Get() ;
	// serialize all:
	while( (_pDescriptionChunk < pEndOfChunk) && pCell )
	{
		_pDescriptionChunk = pCell->GetManagedObject()->Serialize_In( _pDescriptionChunk );
		pCell = pCell->GetNextBrotherCell();
	}	

	return( _pDescriptionChunk );
}

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.
*/
unsigned int NamedObject::GetSerializedDescriptionSize(void)
{
	// size of mSerUlong_JumpToNextChunk if it were set to zero:
	register unsigned int deltaJumpLength; //mSerUlong_ChunkLength.GetSerializedDescriptionSize();
	register unsigned int length=1; //mSerUlong_ChunkLength.GetSerializedDescriptionSize();
	register Cell	*pCell = m_pFirstCell ;
	// add size of the objects in the list:
	while( pCell )
	{
		length += pCell->GetManagedObject()->GetSerializedDescriptionSize();
		pCell = pCell->GetNextBrotherCell();
	}
	// got to reset the chunk length member:
	//---------- we got to check that mSerUlong_JumpToNextChunk don't grow with the right size !
	mSerUlong_ChunkLength.Set( length );
	deltaJumpLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() - 1 ; //1= previous size that might changed.
	if( deltaJumpLength > 0 )
	{
		length += deltaJumpLength;
		//---------- we got to check that twice:
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
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
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
