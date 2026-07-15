/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackStruct.h"
#include "PackString.h"

PackStruct::PackStruct() : BaseType() 
	,m_pFirstCell(0L)
	,m_pLastCell(0L) 
	,m_NumberOfCell(0L)
{

}

#ifdef _ENGINE_EDITABLE_
PackStruct::~PackStruct(void)
{ 
	DeleteAllElements();

}
#endif

/*!
	\brief	Add a new element, at the given index.
	\param	_indexWhereToInsert the index where to insert. if -1, at the end.
	\param _pObjectToAdd object to manage, or NULL to set it after on the cell.
	\return  the element created.

*/
PackStruct::Cell *PackStruct::AddElement(  int _indexWhereToInsert,BaseType *_pObjectToAdd )
{
	// try to add new cell:
	register Cell *pNewCell = new Cell();
	if( !pNewCell ) return(0L); // failed

#ifdef _ENGINE_EDITABLE_
	// if this object is a member of a context's BaseObject, then the objects in the list are too:
	// no use to test it: (old but good:) if( m_pObjectThatManagesThis ) 
	if(_pObjectToAdd) _pObjectToAdd->SetObjectThatManagesThis( m_pObjectThatManagesThis );
#endif
	// this cell manages this object: (_pObjectToAdd can be NULL)
	pNewCell->SetManagedObject( _pObjectToAdd );

#ifdef _ENGINE_EDITABLE_
	if( _indexWhereToInsert == -1 )
	{
		// link cell at the end:
		if( !m_pFirstCell ) 
		{
			m_pFirstCell = pNewCell ;
		} else
		{
			pNewCell->SetPreviousBrotherCell(m_pLastCell);
			m_pLastCell->SetNextBrotherCell(pNewCell);	
		}
		m_pLastCell = pNewCell ;
	} else
	{
		// link cell at 'n' or at end:
	
		register Cell *pPrevCell=0L,*pCell = m_pFirstCell ;
		register int nn = 0;
		while( pCell && nn != _indexWhereToInsert ) 
		{	
			pPrevCell = pCell;
			pCell = pCell->GetNextBrotherCell();
			nn++;
		};
		
		pNewCell->SetPreviousBrotherCell( pPrevCell );
		pNewCell->SetNextBrotherCell( pCell );
		
		// if added at start:
		if( pPrevCell ) pPrevCell->SetNextBrotherCell(pNewCell);
		else m_pFirstCell = pNewCell;
		// if added at the end:
		if( pCell ) pCell->SetPreviousBrotherCell(pNewCell);
		else m_pLastCell = pNewCell ;
		
	}
	
#else
		// link cell at the end:
		if( !m_pFirstCell ) 
		{
			m_pFirstCell = pNewCell ;
		} else
		{
			m_pLastCell->SetNextBrotherCell(pNewCell);	
		}
		m_pLastCell = pNewCell ;
#endif
	m_NumberOfCell++;
	return(pNewCell);
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief get an element at a given index.
	\param	_index the index of the leemnt
	\return the element at the index or NULL if failed.
*/
BaseType *PackStruct::Get( unsigned int _index)
{
	register unsigned int nn=0;
	register Cell *pCell = m_pFirstCell;
	while( pCell )
	{
		if( nn == _index ) return( pCell->GetManagedObject() );
		pCell = pCell->GetNextBrotherCell();
		nn++;
	}
	return(0L); //failed
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief delete an element at a given index. It close and destroy the object.
	\param	_index index of the object to destroy.
*/
void PackStruct::DeleteElement( unsigned int _index)
{
	register Cell *pPrevCell,*pAfterCell,*pCell = m_pFirstCell ;
	register unsigned int nn = 0;
	while( pCell ) 
	{
		if( nn == _index )
		{	// pCell to destroy
			// unlink:
			pPrevCell = pCell->GetPreviousBrotherCell();
			pAfterCell = pCell->GetNextBrotherCell();

			if( m_pLastCell == pCell) m_pLastCell = pPrevCell;
			if( m_pFirstCell == pCell) m_pFirstCell = pAfterCell;

			if( pPrevCell ) pPrevCell->SetNextBrotherCell(pAfterCell);
			if( pAfterCell ) pAfterCell->SetPreviousBrotherCell(pPrevCell);
			delete pCell;
			m_NumberOfCell--;
			// cell deleted:
			return;
		}
		pCell = pCell->GetNextBrotherCell();
		nn++;
	};	
	// if here, failed.
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief close and destroy the whole list.  protected;.
*/
void PackStruct::DeleteAllElements( void )
{
	while( m_pFirstCell ) DeleteElement((unsigned int)0);
}
#endif
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char * PackStruct::Serialize_In( const unsigned char * _pDescriptionChunk)
{
	// find the end of the list chunk:
//old	register unsigned char	*pEndOfChunk = _pDescriptionChunk ;
	register Cell	*pCell = m_pFirstCell ;
	//  serialize the chunk length attribute:
//old	_pDescriptionChunk = mSerUlong_ChunkLength.Serialize_In(_pDescriptionChunk);
	// the end of the chunk is:
//old	pEndOfChunk += mSerUlong_ChunkLength.Get() ;
//old (_pDescriptionChunk < pEndOfChunk) &&
	// serialize all:
	while( pCell )
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
unsigned int PackStruct:: GetSerializedDescriptionSize(void)
{
	// size of mSerUlong_JumpToNextChunk if it were set to zero:
	//register unsigned int deltaJumpLength; // old 1; //mSerUlong_ChunkLength.GetSerializedDescriptionSize();
	register unsigned int length=0; // old 1; //mSerUlong_ChunkLength.GetSerializedDescriptionSize();
	register Cell	*pCell = m_pFirstCell ;
	// add size of the objects in the list:
	while( pCell )
	{
		length += pCell->GetManagedObject()->GetSerializedDescriptionSize();
		pCell = pCell->GetNextBrotherCell();
	}
	// got to reset the chunk length member:
	//---------- we got to check that mSerUlong_JumpToNextChunk don't grow with the right size !
/*old	mSerUlong_ChunkLength.Set( length );
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
	*/
	return( length );
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * PackStruct::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	register Cell	*pCell = m_pFirstCell ;
	//old _pDescriptionChunkToFill = mSerUlong_ChunkLength.Serialize_Out( _pDescriptionChunkToFill );
	while( pCell )
	{
		_pDescriptionChunkToFill = pCell->GetManagedObject()->Serialize_Out( _pDescriptionChunkToFill );
		pCell = pCell->GetNextBrotherCell();
	}
	return(_pDescriptionChunkToFill);
}
#endif

/*!
	\brief Register an object member or another PackStruct member to serialize for read or write operation.
			Each class derived from PackStruct should add a list of it in its Constructor for all serializable members. 
			The object are serialized in the file in the same order as they are given by this method.
	\param	_object reference to the member, which must inherit from BaseType.
*/
void PackStruct::RegisterSerializableMember( BaseType &_object )
{
	// grow the member list by the end:
	AddElement(-1, &_object);
}

/*!
	\brief	The Object this one belong to as a member. Should only be used at init.
	\param _pBaseContext
*/
void	PackStruct::SetObjectThatManagesThis(BaseObject *_pManager)
{
	m_pObjectThatManagesThis = _pManager;
	//test printf("PackStruct::m_pFirstCell:%08x\n",m_pFirstCell);
	register Cell	*pCell = m_pFirstCell ;
	while( pCell )
	{
		pCell->GetManagedObject()->SetObjectThatManagesThis(_pManager); 
		pCell = pCell->GetNextBrotherCell();
	}
}

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	convert the value of this object to an explicit string. The object manages the string privately,
		so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			Note: this is not virtual, but each class can manage m_pValueString or not.
	\return	the value as a const string. 
*/
const char	*PackStruct::ValueToString()
{
	char *pEmpty="";	// "" is actually global.

	if( !m_pValueString ) m_pValueString = new PackString;
	if( !m_pValueString ) return(pEmpty);

	register Cell	*pCell = m_pFirstCell ;
	m_pValueString->Set(pEmpty);
	while( pCell )
	{
		const char *toadd = pCell->GetManagedObject()->ValueToString();
		if(toadd == 0L)
		{
			toadd="";
		}
		m_pValueString->AddString( toadd );
		m_pValueString->AddString("; ");
		pCell = pCell->GetNextBrotherCell();
	}
	return(m_pValueString->Get() );
}
#endif
