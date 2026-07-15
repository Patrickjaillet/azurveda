// SPDX-License-Identifier: LGPL-2.1-only

#include "PackList_TimeTrack.h"
PackList_TimeTrack::PackList_TimeTrack( TimeTrackElementCreatorCallBackFunction _func) : PackList((BaseTypeCreatorCallBackFunction)_func)
	,m_pLastCellFound(0L)

{

}

PackList_TimeTrack::TimeTrackElement::TimeTrackElement(void) : PackStruct()
	,mSer_Time(PackFloat::m_1Div64)
#ifdef _ENGINE_EDITABLE_
	,m_pCellThatManageIt(0L)
#endif
{
#ifdef _ENGINE_EDITABLE_
	mSer_Time.SetElement(this);
#endif
	REGISTER_MEMBER_PACKFLOAT( mSer_Time,"TimeSec.",0.0f);

}

PackStruct::Cell *
	PackList_TimeTrack::GetCellByDate( double _dateInSecond )
{
	Cell *pCell,*pNext;

	if( m_pLastCellFound )
	{
		float  d1= ((TimeTrackElement *)(m_pLastCellFound->GetManagedObject()))->GetTimeInSecond();
		if( _dateInSecond>=d1 )
		{
			pNext = m_pLastCellFound->GetNextBrotherCell();

			if( !pNext ) return m_pLastCellFound ;
			float  d2= ((TimeTrackElement *)(pNext->GetManagedObject()))->GetTimeInSecond();
			if( _dateInSecond< d2) return m_pLastCellFound;
		}
	}

	Cell *pPrev=0L;
	pCell = GetFirstCell();
	if(!pCell) return 0L;

	while( pCell )
	{
		float  d1= ((TimeTrackElement *)(pCell->GetManagedObject()))->GetTimeInSecond();
		if(_dateInSecond<d1) break;
		pPrev = pCell;
		pCell = pCell->GetNextBrotherCell();
	}
	m_pLastCellFound = pPrev;
	return pPrev;
}
#ifdef _ENGINE_EDITABLE_

PackStruct::Cell * PackList_TimeTrack::AddElement(
	int _indexWhereToInsert,BaseType *_pObjectToAdd)
{

	Cell *pCell = PackList::AddElement(_indexWhereToInsert,_pObjectToAdd );
	TimeTrackElement	*pElement = (TimeTrackElement *) pCell->GetManagedObject();

	pElement->SetListThatManageItAsACell(this,pCell);
	return(pCell);
}
#endif
#ifdef _ENGINE_EDITABLE_

void PackList_TimeTrack::DeleteElement( BaseType *_pObWhichCellIsToDestroy)
{

	m_pLastCellFound = 0L;
	PackList::DeleteElement( _pObWhichCellIsToDestroy );
}
#endif

#ifdef _ENGINE_EDITABLE_

void	PackList_TimeTrack::TimeTrackElement::SetTimeInSecond(
		double _date )
{
	mSer_Time.Set( (float)_date );
}
#endif
#ifdef _ENGINE_EDITABLE_
void	PackList_TimeTrack::
			TimeTrackElement::
				PackFloat_FixedPoint_Time::Set(float _value)
{

	if( Get() == _value ) return ;
	PackStruct::Cell *_pCellToCheck = m_pElement->m_pCellThatManageIt ;
	if( !_pCellToCheck ) return;
	PackStruct::Cell *pPrev = _pCellToCheck->GetPreviousBrotherCell();
	PackStruct::Cell *pNext = _pCellToCheck->GetNextBrotherCell();

	if( pNext  )
	{
		float  d2= ((TimeTrackElement *)(pNext->GetManagedObject()))->GetTimeInSecond();
		if( _value>d2 )
		{
			_value = d2 ;
		}
	}
	if( pPrev )
	{
		float  d1= ((TimeTrackElement *)(pPrev->GetManagedObject()))->GetTimeInSecond();
		if( _value<d1 )
		{
			_value = d1  ;
		}
	}

	PackFloat_FixedPoint::Set( _value );

}
#endif

#ifdef _ENGINE_EDITABLE_

const unsigned char * PackList_TimeTrack::Serialize_In( const unsigned char * _pDescriptionChunk)
{
	const unsigned char *_pChunk = PackList::Serialize_In(_pDescriptionChunk);

	Cell *pCell= GetFirstCell();
	while(pCell)
	{
		TimeTrackElement	*pElement = (TimeTrackElement *) pCell->GetManagedObject();
		pElement->SetListThatManageItAsACell(this,pCell);
		pCell = pCell->GetNextBrotherCell();
	}
	return _pChunk;
}
#endif

#ifdef _ENGINE_EDITABLE_

void	PackList_TimeTrack::SwapElements( unsigned int _indexOfElementToSwapWithNext)
{
	if(_indexOfElementToSwapWithNext>(GetNumberOfCell()-2)) return;
	PackList::SwapElements(_indexOfElementToSwapWithNext);

	Cell *pCell1 = m_pFirstCell;
	while(_indexOfElementToSwapWithNext)
	{
		pCell1=pCell1->GetNextBrotherCell();
		_indexOfElementToSwapWithNext--;

	}
	Cell *pCell2 =pCell1->GetNextBrotherCell();
	float d1 = ((TimeTrackElement*)pCell1->GetManagedObject())->GetTimeInSecond();
	float d2 = ((TimeTrackElement*)pCell2->GetManagedObject())->GetTimeInSecond();
	((TimeTrackElement*)pCell1->GetManagedObject())->SetTimeInSecond(d2);
	((TimeTrackElement*)pCell2->GetManagedObject())->SetTimeInSecond(d1);

}
#endif
