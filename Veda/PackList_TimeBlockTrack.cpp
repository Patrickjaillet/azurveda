// SPDX-License-Identifier: LGPL-2.1-only

#include "PackList_TimeBlockTrack.h"
#include "PackFloat.h"
PackList_TimeBlockTrack::PackList_TimeBlockTrack( TimeBlockTrackElementCreatorCallBackFunction _func)
: PackList((BaseTypeCreatorCallBackFunction)_func)
#ifdef _ENGINE_EDITABLE_
	,m_DefaultTimeCellLength(8.0)
#endif
{

}

PackList_TimeBlockTrack::TimeBlockTrackElement::TimeBlockTrackElement(void) : PackStruct()
	,mSer_TimeBlock(PackFloat::m_1Div64)
{

	REGISTER_MEMBER_PACKFLOAT(mSer_TimeBlock,"TimeLength",0.0f);

}

#ifdef _ENGINE_EDITABLE_

PackList::Cell * PackList_TimeBlockTrack::AddElement( int _indexWhereToInsert,BaseType *_pObjectToAdd)
{

	Cell *pCell = PackList::AddElement(_indexWhereToInsert,_pObjectToAdd );
	TimeBlockTrackElement	*pElement = (TimeBlockTrackElement *) pCell->GetManagedObject();

		pElement->SetTimeInSecond(m_DefaultTimeCellLength);
	return pCell;
}
#endif
#ifdef _ENGINE_EDITABLE_

void	PackList_TimeBlockTrack::SetDefaultTimeCellLength( double _DefaultTimeLength )
{
	m_DefaultTimeCellLength = _DefaultTimeLength ;
}
#endif

#ifdef _ENGINE_EDITABLE_

void	PackList_TimeBlockTrack::SwapElements( unsigned int _indexOfElementToSwapWithNext)
{
	if(_indexOfElementToSwapWithNext>(GetNumberOfCell()-2)) return;
	PackList::SwapElements(_indexOfElementToSwapWithNext);

}
#endif
#ifdef _ENGINE_EDITABLE_

void	PackList_TimeBlockTrack::TimeBlockTrackElement::SetTimeInSecond(
		double _date )
{
	mSer_TimeBlock.Set( (float)_date );
}
#endif

PackStruct::Cell *
	PackList_TimeBlockTrack::GetCellByDate( double _dateInSecond )
{
	Cell *pCell,*prev;

	pCell = GetFirstCell();
	double date=PackFloat::m_0p0;
	prev = pCell;
	while( pCell )
	{
		if(_dateInSecond<date)
		{
			break;
		}
		date += ((TimeBlockTrackElement *)(pCell->GetManagedObject()))->GetTimeInSecond();
		prev = pCell;
		pCell = pCell->GetNextBrotherCell();
	}
	return prev;
}
#ifdef _ENGINE_EDITABLE_
void	PackList_TimeBlockTrack::
			TimeBlockTrackElement::
				PackFloat_FixedPoint_TimeBlock::Set(float _value)
{
	if(_value<0.0f) _value=0.0f;
	PackFloat_FixedPoint::Set( _value );
}
#endif
