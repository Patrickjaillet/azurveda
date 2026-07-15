/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackTreeCell.h"
#include "BaseContext.h"

PackTreeCell::PackTreeCell(BaseTypeCreatorCallBackFunction _func) : BaseType() 
	,m_CreatorFunction( _func )
	,m_pObjectManaged(0L)
	,m_pFirstSerializedSubCell(0L)
	,m_pNextSerializedBrotherCell(0L)
	,m_ppPointerToAttachNextSerializedSubCellAtEnd( &m_pFirstSerializedSubCell ) 
{
	// to create a cell means to create an object managed:
	m_pObjectManaged = _func();
}

#ifdef _ENGINE_EDITABLE_
PackTreeCell::~PackTreeCell(void)
{ 
	//recursive delete DeleteSubCells();
	// deleting FIRST subcells is very important to keep
	// a coherent tree when deleting upper branches.
	// (updates can occurs within the deletion)
	DeleteAllSubCells();
	// and then
	if( m_pObjectManaged )
		delete m_pObjectManaged ;

}
#endif
/*!
	\brief	The Object this one belong to as a member. Should only be used at init.
	\param _pBaseContext
*/
void	PackTreeCell::SetObjectThatManagesThis(BaseObject *_pManager)
{
	m_pObjectThatManagesThis = _pManager;
	m_pObjectManaged->SetObjectThatManagesThis( _pManager );
	// reset all sons cells:
	register PackTreeCell **ppll = &m_pFirstSerializedSubCell ;
	while( *ppll ) 
	{
		(*ppll)->SetObjectThatManagesThis(_pManager);
		ppll = &((*ppll)->m_pNextSerializedBrotherCell) ;
	};
}

/*!
	\brief	Add a new sub cell element, at the given index, to this Cell.
	\param	_indexWhereToInsert the index where to insert. if -1, at the end.
	\param _pFatherOject	the tree cell the object is created under.
	\return  the element created.
*/
PackTreeCell * PackTreeCell::AddSubCellToThisCell( int _indexWhereToInsert )
{
#ifdef _ENGINE_EDITABLE_
	BeginChangeNotification();
		// ------------ try to create the node:
		register PackTreeCell *pNewCell = new PackTreeCell(m_CreatorFunction); // m_CreatorFunction();
		if( !pNewCell ){ return(0L);  }
		//--- the new element is OK here.
		pNewCell->SetObjectThatManagesThis(m_pObjectThatManagesThis);
		pNewCell->SetInfoString(m_pInfoString);

		if( _indexWhereToInsert == -1 )
		{
			// link cell at the end:
			// grow the member list by the end:
			*(m_ppPointerToAttachNextSerializedSubCellAtEnd) = pNewCell;
			// the new end is:
			m_ppPointerToAttachNextSerializedSubCellAtEnd = &(pNewCell->m_pNextSerializedBrotherCell);

		} else
		{
			// link cell at 'n' or at end:
			register PackTreeCell **ppll = &m_pFirstSerializedSubCell ;
			register int nn = 0;
			while( *ppll && nn != _indexWhereToInsert ) 
			{
				ppll = &((*ppll)->m_pNextSerializedBrotherCell) ;
				nn++;
			};
			// attach at the right level:
			pNewCell->m_pNextSerializedBrotherCell = *ppll;
			if( *ppll == 0L )
			{
				// it was added at the end: we have to update the end of the chain pointer:
				m_ppPointerToAttachNextSerializedSubCellAtEnd =  &(pNewCell->m_pNextSerializedBrotherCell);
			}
			*ppll = pNewCell;
		}	
	EndChangeNotification();
#else
		// ------------ try to create the node:
		register PackTreeCell *pNewCell = new PackTreeCell(m_CreatorFunction); // m_CreatorFunction();
		if( !pNewCell ){ return(0L);  }
		//--- the new element is OK here.
		pNewCell->SetObjectThatManagesThis(m_pObjectThatManagesThis);
		// grow the member list by the end:
		*(m_ppPointerToAttachNextSerializedSubCellAtEnd) = pNewCell;
		// the new end is:
		m_ppPointerToAttachNextSerializedSubCellAtEnd = &(pNewCell->m_pNextSerializedBrotherCell);

#endif

	return(pNewCell);
}


#ifdef _ENGINE_EDITABLE_
/*!
	\brief delete an element and its whole sub elements. the first to delete can be at any level.
	\param	_pSubCellToDelete pointer to subcell.
*/
void PackTreeCell::DeleteSubCells( PackTreeCell *_pSubCellToDelete)
{
	if(!_pSubCellToDelete) return;
	BeginChangeNotification();
		DeleteSubCellsProtectedRecursive(_pSubCellToDelete);
	EndChangeNotification();
	// send special eVedaUpdate_MemberDestroyed message:
	if( GetObjectThatManagesThis() && GetObjectThatManagesThis()->GetContext() )
		GetObjectThatManagesThis()->GetContext()->
			NotifyObjectChange( BaseContext::eVedaUpdate_MemberDestroyed,GetObjectThatManagesThis() , this );

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief delete an element and its whole sub elements. the first to delete can be at any level.
	\param	_pSubCellToDelete pointer to subcell.
*/
void PackTreeCell::DeleteSubCellsProtectedRecursive( PackTreeCell *_pSubCellToDelete)
{

	register PackTreeCell **ppll = &m_pFirstSerializedSubCell ;
	while( *ppll ) 
	{		
		register PackTreeCell *pNext = (*ppll)->m_pNextSerializedBrotherCell;
		//*ppll = pCell->GetNextBrotherCell() ;
		if( *ppll == _pSubCellToDelete )
		{
			//end of sonhood pointer could have change.
			if( m_ppPointerToAttachNextSerializedSubCellAtEnd == &(_pSubCellToDelete->m_pNextSerializedBrotherCell) )
			{
				m_ppPointerToAttachNextSerializedSubCellAtEnd = ppll;
			}
			// this occurs recursive deletion:
			delete _pSubCellToDelete;
			//relink:
			*ppll = pNext;

			// OK
			return;
		}
		// search down:
		(*ppll)->DeleteSubCells(_pSubCellToDelete);
		ppll = &((*ppll)->m_pNextSerializedBrotherCell) ;

	};
	// if here, failed.
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief close and destroy the whole list.  protected;.
*/
void PackTreeCell::DeleteAllSubCells( void )
{
	register PackTreeCell *pSubCell = m_pFirstSerializedSubCell;
	while( pSubCell )
	{
		PackTreeCell *pNext = pSubCell->m_pNextSerializedBrotherCell ;
		delete pSubCell;
		pSubCell = pNext ;
	}
	m_pFirstSerializedSubCell = 0L;
	m_ppPointerToAttachNextSerializedSubCellAtEnd = &m_pFirstSerializedSubCell ;
//	mSerUlong_NumberOfSubCells.Set(0);
}
#endif
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char * PackTreeCell::Serialize_In( const unsigned char * _pDescriptionChunk)
{
	// delete  all serializable members if there were any:
#ifdef _ENGINE_EDITABLE_
	DeleteAllSubCells();
#endif
	// just serialize the chunk length attribute:
	register const unsigned char * pChunkEnd = _pDescriptionChunk;
	_pDescriptionChunk = mSerUlong_JumpToNextChunk.Serialize_In(_pDescriptionChunk);
	pChunkEnd += mSerUlong_JumpToNextChunk.Get();
	// serialize managed object:
	_pDescriptionChunk = m_pObjectManaged->Serialize_In(_pDescriptionChunk);
	// recreate all sub elements:
	while( _pDescriptionChunk < pChunkEnd )
	{
		PackTreeCell *pCell = AddSubCellToThisCell() ; // grow by the end.
		_pDescriptionChunk = pCell->Serialize_In( _pDescriptionChunk );
	}
	return(_pDescriptionChunk);
}

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.
*/
unsigned int PackTreeCell:: GetSerializedDescriptionSize(void)
{
	//mSerUlong_JumpToNextChunk.Set(0);
	// size of mSerUlong_JumpToNextChunk if it were set to zero:
	register unsigned int deltaJumpLength,length= 1; //mSerUlong_JumpToNextChunk.GetSerializedDescriptionSize();
	// size of the object:
	length += m_pObjectManaged->GetSerializedDescriptionSize();
	register PackTreeCell  *pSubCell = m_pFirstSerializedSubCell;
	while( pSubCell )
	{
		length += pSubCell->GetSerializedDescriptionSize();
		pSubCell = pSubCell->m_pNextSerializedBrotherCell ;
	}
	// got to reset the chunk length member:
	//---------- we got to check that mSerUlong_JumpToNextChunk don't grow with the right size !
	mSerUlong_JumpToNextChunk.Set( length );
	deltaJumpLength = mSerUlong_JumpToNextChunk.GetSerializedDescriptionSize() - 1 ;
	if( deltaJumpLength > 0 )
	{
		length += deltaJumpLength;
		//---------- we got to check that twice:
		deltaJumpLength = mSerUlong_JumpToNextChunk.GetSerializedDescriptionSize() ;
		mSerUlong_JumpToNextChunk.Set( length );
		deltaJumpLength = mSerUlong_JumpToNextChunk.GetSerializedDescriptionSize() - deltaJumpLength ;
		if( deltaJumpLength > 0 )
		{
			length += deltaJumpLength;
			mSerUlong_JumpToNextChunk.Set( length );
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
unsigned char * PackTreeCell::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	// write chunk length:
	_pDescriptionChunkToFill = mSerUlong_JumpToNextChunk.Serialize_Out( _pDescriptionChunkToFill );
	// write managed object:
	_pDescriptionChunkToFill = m_pObjectManaged->Serialize_Out( _pDescriptionChunkToFill );
	register PackTreeCell  *pSubCell = m_pFirstSerializedSubCell;
	while( pSubCell )
	{
		_pDescriptionChunkToFill = pSubCell->Serialize_Out( _pDescriptionChunkToFill );
		pSubCell = pSubCell->m_pNextSerializedBrotherCell ;
	}
	return(_pDescriptionChunkToFill);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Edition tool: Exchange 2 cells in the tree.
			You can't exchange the root cell. if one of the 2 cells
			is the root, nothing happens.
	\param_pFirstSubCell 
	\param _pSecondSubCell
*/
/*todo void	PackTreeCell::SwapElements( PackTreeCell *_pFirstSubCell,PackTreeCell *_pSecondSubCell )
{

}*/
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Edition tool: Insert a new element after _indexOfElementToClone,
		and clone it with the previous element shape. Note this
		can exclude some data members, like Object reference.
	\param	_pSubCellToClone the cell element to clone.
*/
/*todo void	PackTreeCell::CloneElementAsBrother( PackTreeCell *_pSubCellToClone)
{
	
}*/
#endif