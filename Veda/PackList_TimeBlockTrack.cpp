/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackList_TimeBlockTrack.h"
#include "PackFloat.h"
PackList_TimeBlockTrack::PackList_TimeBlockTrack( TimeBlockTrackElementCreatorCallBackFunction _func)
: PackList((BaseTypeCreatorCallBackFunction)_func) 
#ifdef _ENGINE_EDITABLE_
	,m_DefaultTimeCellLength(8.0)
#endif
{

}
//! \brief constructor . It registers the members
PackList_TimeBlockTrack::TimeBlockTrackElement::TimeBlockTrackElement(void) : PackStruct()
	,mSer_TimeBlock(PackFloat::m_1Div64)
{

	REGISTER_MEMBER_PACKFLOAT(mSer_TimeBlock,"TimeLength",0.0f);	
	
}

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Override PackList to assure the list is date-sorted.
			Add a new element, at the given index, in edition context.
			in non-editable, it shouldnt be used, and serialization recreate the elements.
			when asked a first element, as the track must have a length,
			we create 2 elements: a zero timed one, and a end with default time length.

	\param	_indexWhereToInsert the index where to insert. if -1, at the end.
	\param _pObjectToAdd object to manage, or NULL to set it after on the cell.
	\return  the element created.
*/
PackList::Cell * PackList_TimeBlockTrack::AddElement( int _indexWhereToInsert,BaseType *_pObjectToAdd)
{
	// call to super AddElement
	Cell *pCell = PackList::AddElement(_indexWhereToInsert,_pObjectToAdd );
	TimeBlockTrackElement	*pElement = (TimeBlockTrackElement *) pCell->GetManagedObject();

		pElement->SetTimeInSecond(m_DefaultTimeCellLength);
	return pCell;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*! \brief In edition mode, there is a default start timelength you can set at init:
	\param	_DefaultTimeLength in seconds.
*/
void	PackList_TimeBlockTrack::SetDefaultTimeCellLength( double _DefaultTimeLength )
{
	m_DefaultTimeCellLength = _DefaultTimeLength ;
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Edition tool: Exchange 2 neighbour elements in the list, if there is a next element.
			if index is 0, 0 will be 1 and 1 will be 0.
	\param	_indexOfElementToSwapWithNext any row, from zero to GetNumberOfCell()-2
*/
void	PackList_TimeBlockTrack::SwapElements( unsigned int _indexOfElementToSwapWithNext)
{
	if(_indexOfElementToSwapWithNext>(GetNumberOfCell()-2)) return;
	PackList::SwapElements(_indexOfElementToSwapWithNext);
	// that's all... classic packlist behaviour. no PackList_TimeTrack use.
}
#endif
#ifdef _ENGINE_EDITABLE_
//! \brief Set the date in seconds in edition mode. It assures the dates are sorted.
void	PackList_TimeBlockTrack::TimeBlockTrackElement::SetTimeInSecond(
		double _date )
{
	mSer_TimeBlock.Set( (float)_date );
}
#endif
/*!
	\brief	Find the element which match a given date in the list. 
		It means the date is between this element and the next elment's date.
	\param	_dateInSecond double date in second. 
	\return	the element, or the first elt if date before.
*/
PackStruct::Cell *
	PackList_TimeBlockTrack::GetCellByDate( double _dateInSecond )
{
	Cell *pCell,*prev;
	// search by loop.
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