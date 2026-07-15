// SPDX-License-Identifier: LGPL-2.1-only

#include "PackList.h"
#include "BaseObject.h"
#include "BaseContext.h"
PackList::PackList( BaseTypeCreatorCallBackFunction _func) : PackStruct()
	,m_CreatorFunc(_func)
{

}

#ifdef _ENGINE_EDITABLE_
PackList::~PackList(void)
{
	DeleteAllElements();

}
#endif
#ifdef _ENGINE_EDITABLE_

PackStruct::Cell *PackList::AddElement(  int _indexWhereToInsert,BaseType *_uselessAlwaysNullSetToOverideMethod )
{

	BaseType *_pObjectToAdd = m_CreatorFunc();
	if( !_pObjectToAdd ) return(0L);

	BeginChangeNotification();

		Cell *pCell = PackStruct::AddElement(_indexWhereToInsert,_pObjectToAdd);
		if( !pCell ){
			delete _pObjectToAdd;
			EndChangeNotification();
			return(0L);
		}

	EndChangeNotification();

	return( pCell );
}
#endif

#ifdef _ENGINE_EDITABLE_

void PackList::DeleteElement( unsigned int _index)
{

	BeginChangeNotification();
		BaseType *pOb = PackStruct::Get(_index);
		if(pOb  ) delete pOb;
		PackStruct::DeleteElement(_index);
	EndChangeNotification();

	if( GetObjectThatManagesThis() && GetObjectThatManagesThis()->GetContext() )
		GetObjectThatManagesThis()->GetContext()->
			NotifyObjectChange( BaseContext::eVedaUpdate_MemberDestroyed,GetObjectThatManagesThis() , this );

}
#endif
#ifdef _ENGINE_EDITABLE_

void PackList::DeleteElement( BaseType *_pObWhichCellIsToDestroy)
{
	register Cell *pPrevCell,*pAfterCell,*pCell = m_pFirstCell ;

	while( pCell )
	{
		if( pCell->GetManagedObject() == _pObWhichCellIsToDestroy )
		{
			BeginChangeNotification();

					pPrevCell = pCell->GetPreviousBrotherCell();
					pAfterCell = pCell->GetNextBrotherCell();

					if( m_pLastCell == pCell) m_pLastCell = pPrevCell;
					if( m_pFirstCell == pCell) m_pFirstCell = pAfterCell;

					if( pPrevCell ) pPrevCell->SetNextBrotherCell(pAfterCell);
					if( pAfterCell ) pAfterCell->SetPreviousBrotherCell(pPrevCell);
					delete pCell;

					delete _pObWhichCellIsToDestroy;
					m_NumberOfCell--;
			EndChangeNotification();

			if( GetObjectThatManagesThis() && GetObjectThatManagesThis()->GetContext() )
				GetObjectThatManagesThis()->GetContext()->
					NotifyObjectChange( BaseContext::eVedaUpdate_MemberDestroyed,GetObjectThatManagesThis() , this );

			return;
		}

		pCell = pCell->GetNextBrotherCell();
	};

}
#endif

const unsigned char * PackList::Serialize_In( const unsigned char * _pDescriptionChunk)
{
#ifdef _ENGINE_EDITABLE_

		DeleteAllElements();
#endif

	register const unsigned char	*pEndOfChunk = _pDescriptionChunk ;
	register Cell	*pCell;

	_pDescriptionChunk = mSerUlong_ChunkLength.Serialize_In(_pDescriptionChunk);

	pEndOfChunk += mSerUlong_ChunkLength.Get() ;

	while( (_pDescriptionChunk < pEndOfChunk) )
	{

		pCell = 0L;
		BaseType *_pObjectToAdd = m_CreatorFunc();
		if( _pObjectToAdd )
		{
			pCell = PackStruct::AddElement(-1,_pObjectToAdd);
			if( !pCell ){ delete _pObjectToAdd; }
		}
		if( pCell )
			_pDescriptionChunk =_pObjectToAdd->Serialize_In( _pDescriptionChunk );
		else
			_pDescriptionChunk = pEndOfChunk;
	}

	return( pEndOfChunk );

}
#ifdef _ENGINE_EDITABLE_

const unsigned char * PackList::MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_pendName)
{

	register const unsigned char	*pEndOfChunk = _pDescriptionChunk ;
	register Cell	*pCell;

	_pDescriptionChunk = mSerUlong_ChunkLength.Serialize_In(_pDescriptionChunk);

	pEndOfChunk += mSerUlong_ChunkLength.Get() ;

	while( (_pDescriptionChunk < pEndOfChunk) )
	{
		pCell =  AddElement();
		if( pCell )
			_pDescriptionChunk = pCell->GetManagedObject()->Serialize_In( _pDescriptionChunk );

		pCell = pCell->GetNextBrotherCell();
	}

	return( _pDescriptionChunk );
}
#endif

#ifdef _ENGINE_EDITABLE_

unsigned int PackList::GetSerializedDescriptionSize(void)
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

unsigned char * PackList::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
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
#ifdef _ENGINE_EDITABLE_

void	PackList::SwapElements( unsigned int _indexOfElementToSwapWithNext)
{
	if(GetNumberOfCell()<2) return;
	if(_indexOfElementToSwapWithNext>(GetNumberOfCell()-2)) return;
	Cell *pCell1 = m_pFirstCell;
	while(_indexOfElementToSwapWithNext)
	{
		pCell1=pCell1->GetNextBrotherCell();
		_indexOfElementToSwapWithNext--;
	}
 BeginChangeNotification();

	Cell *pCell2 =pCell1->GetNextBrotherCell();
	Cell *pCell2Next =pCell2->GetNextBrotherCell();
	Cell *pCell1Prev =pCell1->GetPreviousBrotherCell();

	pCell1->SetPreviousBrotherCell(pCell2);
	pCell1->SetNextBrotherCell(pCell2Next);
	pCell2->SetNextBrotherCell(pCell1);
	pCell2->SetPreviousBrotherCell(pCell1Prev);
	if(pCell2Next) pCell2Next->SetPreviousBrotherCell(pCell1);
	else m_pLastCell = pCell1;
	if(pCell1Prev) pCell1Prev->SetNextBrotherCell(pCell2);
	else m_pFirstCell =  pCell2;
 EndChangeNotification();

	if( GetObjectThatManagesThis() && GetObjectThatManagesThis()->GetContext() )
		GetObjectThatManagesThis()->GetContext()->
			NotifyObjectChange( BaseContext::eVedaUpdate_MemberDestroyed,GetObjectThatManagesThis() , this );

}
#endif
#ifdef _ENGINE_EDITABLE_

void	PackList::CloneElement( unsigned int _indexOfElementToClone)
{

	BaseType *_pObjectToAdd = m_CreatorFunc();
	if( !_pObjectToAdd ) return;

	BeginChangeNotification();

		Cell *pCell = PackStruct::AddElement(_indexOfElementToClone,_pObjectToAdd);
		if( !pCell ){
			delete _pObjectToAdd;
			EndChangeNotification();
			return;
		}
		Cell *pSourceCell =pCell->GetNextBrotherCell();
		if(pSourceCell)
		{
			BaseType *pSourceObj = (BaseType *)(pSourceCell->GetManagedObject());
			unsigned int serlength = pSourceObj->GetSerializedDescriptionSize();
			unsigned char *pShape = new unsigned char[serlength];
			if(pShape)
			{
				BaseType *pNewObj = (BaseType *)(pCell->GetManagedObject());

				pSourceObj->Serialize_Out(pShape);
				pNewObj->Serialize_In(pShape);
				delete [] pShape;
			}

		}
	EndChangeNotification();

}
#endif
