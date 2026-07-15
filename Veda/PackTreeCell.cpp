// SPDX-License-Identifier: LGPL-2.1-only

#include "PackTreeCell.h"
#include "BaseContext.h"

PackTreeCell::PackTreeCell(BaseTypeCreatorCallBackFunction _func) : BaseType()
	,m_CreatorFunction( _func )
	,m_pObjectManaged(0L)
	,m_pFirstSerializedSubCell(0L)
	,m_pNextSerializedBrotherCell(0L)
	,m_ppPointerToAttachNextSerializedSubCellAtEnd( &m_pFirstSerializedSubCell )
{

	m_pObjectManaged = _func();
}

#ifdef _ENGINE_EDITABLE_
PackTreeCell::~PackTreeCell(void)
{

	DeleteAllSubCells();

	if( m_pObjectManaged )
		delete m_pObjectManaged ;

}
#endif

void	PackTreeCell::SetObjectThatManagesThis(BaseObject *_pManager)
{
	m_pObjectThatManagesThis = _pManager;
	m_pObjectManaged->SetObjectThatManagesThis( _pManager );

	PackTreeCell **ppll = &m_pFirstSerializedSubCell ;
	while( *ppll )
	{
		(*ppll)->SetObjectThatManagesThis(_pManager);
		ppll = &((*ppll)->m_pNextSerializedBrotherCell) ;
	};
}

PackTreeCell * PackTreeCell::AddSubCellToThisCell( int _indexWhereToInsert )
{
#ifdef _ENGINE_EDITABLE_
	BeginChangeNotification();

		PackTreeCell *pNewCell = new PackTreeCell(m_CreatorFunction);
		if( !pNewCell ){ return(0L);  }

		pNewCell->SetObjectThatManagesThis(m_pObjectThatManagesThis);
		pNewCell->SetInfoString(m_pInfoString);

		if( _indexWhereToInsert == -1 )
		{

			*(m_ppPointerToAttachNextSerializedSubCellAtEnd) = pNewCell;

			m_ppPointerToAttachNextSerializedSubCellAtEnd = &(pNewCell->m_pNextSerializedBrotherCell);

		} else
		{

			PackTreeCell **ppll = &m_pFirstSerializedSubCell ;
			int nn = 0;
			while( *ppll && nn != _indexWhereToInsert )
			{
				ppll = &((*ppll)->m_pNextSerializedBrotherCell) ;
				nn++;
			};

			pNewCell->m_pNextSerializedBrotherCell = *ppll;
			if( *ppll == 0L )
			{

				m_ppPointerToAttachNextSerializedSubCellAtEnd =  &(pNewCell->m_pNextSerializedBrotherCell);
			}
			*ppll = pNewCell;
		}
	EndChangeNotification();
#else

		PackTreeCell *pNewCell = new PackTreeCell(m_CreatorFunction);
		if( !pNewCell ){ return(0L);  }

		pNewCell->SetObjectThatManagesThis(m_pObjectThatManagesThis);

		*(m_ppPointerToAttachNextSerializedSubCellAtEnd) = pNewCell;

		m_ppPointerToAttachNextSerializedSubCellAtEnd = &(pNewCell->m_pNextSerializedBrotherCell);

#endif

	return(pNewCell);
}

#ifdef _ENGINE_EDITABLE_

void PackTreeCell::DeleteSubCells( PackTreeCell *_pSubCellToDelete)
{
	if(!_pSubCellToDelete) return;
	BeginChangeNotification();
		DeleteSubCellsProtectedRecursive(_pSubCellToDelete);
	EndChangeNotification();

	if( GetObjectThatManagesThis() && GetObjectThatManagesThis()->GetContext() )
		GetObjectThatManagesThis()->GetContext()->
			NotifyObjectChange( BaseContext::eVedaUpdate_MemberDestroyed,GetObjectThatManagesThis() , this );

}
#endif
#ifdef _ENGINE_EDITABLE_

void PackTreeCell::DeleteSubCellsProtectedRecursive( PackTreeCell *_pSubCellToDelete)
{

	PackTreeCell **ppll = &m_pFirstSerializedSubCell ;
	while( *ppll )
	{
		PackTreeCell *pNext = (*ppll)->m_pNextSerializedBrotherCell;

		if( *ppll == _pSubCellToDelete )
		{

			if( m_ppPointerToAttachNextSerializedSubCellAtEnd == &(_pSubCellToDelete->m_pNextSerializedBrotherCell) )
			{
				m_ppPointerToAttachNextSerializedSubCellAtEnd = ppll;
			}

			delete _pSubCellToDelete;

			*ppll = pNext;

			return;
		}

		(*ppll)->DeleteSubCells(_pSubCellToDelete);
		ppll = &((*ppll)->m_pNextSerializedBrotherCell) ;

	};

}
#endif
#ifdef _ENGINE_EDITABLE_

void PackTreeCell::DeleteAllSubCells( void )
{
	PackTreeCell *pSubCell = m_pFirstSerializedSubCell;
	while( pSubCell )
	{
		PackTreeCell *pNext = pSubCell->m_pNextSerializedBrotherCell ;
		delete pSubCell;
		pSubCell = pNext ;
	}
	m_pFirstSerializedSubCell = 0L;
	m_ppPointerToAttachNextSerializedSubCellAtEnd = &m_pFirstSerializedSubCell ;

}
#endif

const unsigned char * PackTreeCell::Serialize_In( const unsigned char * _pDescriptionChunk)
{

#ifdef _ENGINE_EDITABLE_
	DeleteAllSubCells();
#endif

	const unsigned char * pChunkEnd = _pDescriptionChunk;
	_pDescriptionChunk = mSerUlong_JumpToNextChunk.Serialize_In(_pDescriptionChunk);
	pChunkEnd += mSerUlong_JumpToNextChunk.Get();

	_pDescriptionChunk = m_pObjectManaged->Serialize_In(_pDescriptionChunk);

	while( _pDescriptionChunk < pChunkEnd )
	{
		PackTreeCell *pCell = AddSubCellToThisCell() ;
		_pDescriptionChunk = pCell->Serialize_In( _pDescriptionChunk );
	}
	return(_pDescriptionChunk);
}

#ifdef _ENGINE_EDITABLE_

unsigned int PackTreeCell:: GetSerializedDescriptionSize(void)
{

	unsigned int deltaJumpLength,length= 1;

	length += m_pObjectManaged->GetSerializedDescriptionSize();
	PackTreeCell  *pSubCell = m_pFirstSerializedSubCell;
	while( pSubCell )
	{
		length += pSubCell->GetSerializedDescriptionSize();
		pSubCell = pSubCell->m_pNextSerializedBrotherCell ;
	}

	mSerUlong_JumpToNextChunk.Set( length );
	deltaJumpLength = mSerUlong_JumpToNextChunk.GetSerializedDescriptionSize() - 1 ;
	if( deltaJumpLength > 0 )
	{
		length += deltaJumpLength;

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

unsigned char * PackTreeCell::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{

	_pDescriptionChunkToFill = mSerUlong_JumpToNextChunk.Serialize_Out( _pDescriptionChunkToFill );

	_pDescriptionChunkToFill = m_pObjectManaged->Serialize_Out( _pDescriptionChunkToFill );
	PackTreeCell  *pSubCell = m_pFirstSerializedSubCell;
	while( pSubCell )
	{
		_pDescriptionChunkToFill = pSubCell->Serialize_Out( _pDescriptionChunkToFill );
		pSubCell = pSubCell->m_pNextSerializedBrotherCell ;
	}
	return(_pDescriptionChunkToFill);
}
#endif
#ifdef _ENGINE_EDITABLE_

#endif
#ifdef _ENGINE_EDITABLE_

#endif
