// SPDX-License-Identifier: LGPL-2.1-only

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

PackStruct::Cell *PackStruct::AddElement(  int _indexWhereToInsert,BaseType *_pObjectToAdd )
{

	Cell *pNewCell = new Cell();
	if( !pNewCell ) return(0L);

#ifdef _ENGINE_EDITABLE_

	if(_pObjectToAdd) _pObjectToAdd->SetObjectThatManagesThis( m_pObjectThatManagesThis );
#endif

	pNewCell->SetManagedObject( _pObjectToAdd );

#ifdef _ENGINE_EDITABLE_
	if( _indexWhereToInsert == -1 )
	{

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

		Cell *pPrevCell=0L,*pCell = m_pFirstCell ;
		int nn = 0;
		while( pCell && nn != _indexWhereToInsert )
		{
			pPrevCell = pCell;
			pCell = pCell->GetNextBrotherCell();
			nn++;
		};

		pNewCell->SetPreviousBrotherCell( pPrevCell );
		pNewCell->SetNextBrotherCell( pCell );

		if( pPrevCell ) pPrevCell->SetNextBrotherCell(pNewCell);
		else m_pFirstCell = pNewCell;

		if( pCell ) pCell->SetPreviousBrotherCell(pNewCell);
		else m_pLastCell = pNewCell ;

	}

#else

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

BaseType *PackStruct::Get( unsigned int _index)
{
	unsigned int nn=0;
	Cell *pCell = m_pFirstCell;
	while( pCell )
	{
		if( nn == _index ) return( pCell->GetManagedObject() );
		pCell = pCell->GetNextBrotherCell();
		nn++;
	}
	return(0L);
}
#endif
#ifdef _ENGINE_EDITABLE_

void PackStruct::DeleteElement( unsigned int _index)
{
	Cell *pPrevCell,*pAfterCell,*pCell = m_pFirstCell ;
	unsigned int nn = 0;
	while( pCell )
	{
		if( nn == _index )
		{

			pPrevCell = pCell->GetPreviousBrotherCell();
			pAfterCell = pCell->GetNextBrotherCell();

			if( m_pLastCell == pCell) m_pLastCell = pPrevCell;
			if( m_pFirstCell == pCell) m_pFirstCell = pAfterCell;

			if( pPrevCell ) pPrevCell->SetNextBrotherCell(pAfterCell);
			if( pAfterCell ) pAfterCell->SetPreviousBrotherCell(pPrevCell);
			delete pCell;
			m_NumberOfCell--;

			return;
		}
		pCell = pCell->GetNextBrotherCell();
		nn++;
	};

}
#endif
#ifdef _ENGINE_EDITABLE_

void PackStruct::DeleteAllElements( void )
{
	while( m_pFirstCell ) DeleteElement((unsigned int)0);
}
#endif

const unsigned char * PackStruct::Serialize_In( const unsigned char * _pDescriptionChunk)
{

	Cell	*pCell = m_pFirstCell ;

	while( pCell )
	{
		_pDescriptionChunk = pCell->GetManagedObject()->Serialize_In( _pDescriptionChunk );
		pCell = pCell->GetNextBrotherCell();
	}

	return( _pDescriptionChunk );
}

#ifdef _ENGINE_EDITABLE_

unsigned int PackStruct:: GetSerializedDescriptionSize(void)
{

	unsigned int length=0;
	Cell	*pCell = m_pFirstCell ;

	while( pCell )
	{
		length += pCell->GetManagedObject()->GetSerializedDescriptionSize();
		pCell = pCell->GetNextBrotherCell();
	}

	return( length );
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * PackStruct::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	Cell	*pCell = m_pFirstCell ;

	while( pCell )
	{
		_pDescriptionChunkToFill = pCell->GetManagedObject()->Serialize_Out( _pDescriptionChunkToFill );
		pCell = pCell->GetNextBrotherCell();
	}
	return(_pDescriptionChunkToFill);
}
#endif

void PackStruct::RegisterSerializableMember( BaseType &_object )
{

	AddElement(-1, &_object);
}

void	PackStruct::SetObjectThatManagesThis(BaseObject *_pManager)
{
	m_pObjectThatManagesThis = _pManager;

	Cell	*pCell = m_pFirstCell ;
	while( pCell )
	{
		pCell->GetManagedObject()->SetObjectThatManagesThis(_pManager);
		pCell = pCell->GetNextBrotherCell();
	}
}

#ifdef _ENGINE_EDITABLE_

const char	*PackStruct::ValueToString()
{
	const char *pEmpty="";

	if( !m_pValueString ) m_pValueString = new PackString;
	if( !m_pValueString ) return(pEmpty);

	Cell	*pCell = m_pFirstCell ;
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
