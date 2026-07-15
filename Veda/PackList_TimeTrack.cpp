/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackList_TimeTrack.h"
PackList_TimeTrack::PackList_TimeTrack( TimeTrackElementCreatorCallBackFunction _func) : PackList((BaseTypeCreatorCallBackFunction)_func) 
	,m_pLastCellFound(0L)
//old	,m_SerializeLengthBlock(_SerializeLengthBlock) // const initialization.
{

}
//! \brief constructor . It registers the members
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
/*!
	\brief	Find the element which match a given date in the list. 
		It means the date is between this element and the next elment's date.
	\param	_dateInSecond double date in second. 
	\return	the element, or the first elt if date before.
*/
PackStruct::Cell *
	PackList_TimeTrack::GetCellByDate( double _dateInSecond )
{
	Cell *pCell,*pNext;
	// if last cell found match, return it (optimisation).
	if( m_pLastCellFound )
	{
		float  d1= ((TimeTrackElement *)(m_pLastCellFound->GetManagedObject()))->GetTimeInSecond();
		if( _dateInSecond>=d1 ) 
		{
			pNext = m_pLastCellFound->GetNextBrotherCell();
			//if last cell:
			if( !pNext ) return m_pLastCellFound ;
			float  d2= ((TimeTrackElement *)(pNext->GetManagedObject()))->GetTimeInSecond();
			if( _dateInSecond< d2) return m_pLastCellFound;	
		}
	}
	// search by loop.
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
/*!
	\brief	Override PackList to assure the list is date-sorted.
			Add a new element, at the given index, in edition context.
			in non-editable, it shouldnt be used, and serialization recreate the elements.
	\param	_indexWhereToInsert the index where to insert. if -1, at the end.
	\param _pObjectToAdd object to manage, or NULL to set it after on the cell.
	\return  the element created.
*/
PackStruct::Cell * PackList_TimeTrack::AddElement(
	int _indexWhereToInsert,BaseType *_pObjectToAdd)
{
	// call to super AddElement
	Cell *pCell = PackList::AddElement(_indexWhereToInsert,_pObjectToAdd );
	TimeTrackElement	*pElement = (TimeTrackElement *) pCell->GetManagedObject();
	// add to initialisation of the element: it knows its list.
	// that way, when the date will be change, we will be able to check
	// the date is sorted according to the prev/next elements.
	pElement->SetListThatManageItAsACell(this,pCell);
	return(pCell);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief delete an element at a given index. It closes and destroys the object.
	\param _pObWhichCellIsToDestroy ob WhichCellIsToDestroy.
*/
void PackList_TimeTrack::DeleteElement( BaseType *_pObWhichCellIsToDestroy)
{
	/* old: it is completely doubtfull to use.
	register Cell *pPrevCell,*pAfterCell,*pCell = m_pFirstCell ;
	// search cell that will be destroyed by supercall PackList::DeleteElement,
	// look its timelength, and shift down all next time.
	while( pCell ) 
	{
		if( pCell->GetManagedObject() == _pObWhichCellIsToDestroy
			&& pCell->GetNextBrotherCell() )
		{	
			double removedLength = 
				((TimeTrackElement *)pCell->GetNextBrotherCell()->GetManagedObject())->GetTimeInSecond()
				-
				((TimeTrackElement *)pCell->GetManagedObject())->GetTimeInSecond();
			pCell = pCell->GetNextBrotherCell();
			while( pCell )
			{
				TimeTrackElement *pPart =(TimeTrackElement *) pCell->GetManagedObject();
				pPart->SetTimeInSecond(pPart->GetTimeInSecond()-removedLength);
				pCell = pCell->GetPreviousBrotherCell();
			}
			// we have
			break;
		}	
		pCell = pCell->GetNextBrotherCell();
	}*/

	m_pLastCellFound = 0L;
	PackList::DeleteElement( _pObWhichCellIsToDestroy );
}
#endif

#ifdef _ENGINE_EDITABLE_
//! \brief Set the date in seconds in edition mode. It assures the dates are sorted.
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
	// redirect the date edition change to the element, not the member.
	// this way, the classic PackFloat Set() is patched, and the element's date
	// is always sorted.
	if( Get() == _value ) return ;
	PackStruct::Cell *_pCellToCheck = m_pElement->m_pCellThatManageIt ;
	if( !_pCellToCheck ) return; // not yet inited.
	PackStruct::Cell *pPrev = _pCellToCheck->GetPreviousBrotherCell();
	PackStruct::Cell *pNext = _pCellToCheck->GetNextBrotherCell();
	/*bullshit. if( pPrev && pNext ) 
	{	// middle case:
		float  d1= ((TimeTrackElement *)(pPrev->GetManagedObject()))->GetTimeInSecond();
		float  d2= ((TimeTrackElement *)(pNext->GetManagedObject()))->GetTimeInSecond();
		if(_value<d1 || _value>d2 )	
		{
			_value = (d1+d2)* PackFloat::m_0p5;
		}
	} else
		*/
	if( pNext /*&& !pPrev*/ )  
	{	// next OK: first element.
		float  d2= ((TimeTrackElement *)(pNext->GetManagedObject()))->GetTimeInSecond();
		if( _value>d2 )	
		{
			_value = d2 ;
		}
	} 
	if( pPrev/* && !pNext*/ )  
	{	// prev OK: last element.
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
/*!
	\brief	Special patch in edition mode to add an init to the elements.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char * PackList_TimeTrack::Serialize_In( const unsigned char * _pDescriptionChunk)
{
	const unsigned char *_pChunk = PackList::Serialize_In(_pDescriptionChunk);
	// link info to cells... it's complex... see SetListThatManageItAsACell()
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
/*!
	\brief	Edition tool: Exchange 2 neighbour elements in the list, if there is a next element.
			if index is 0, 0 will be 1 and 1 will be 0.
	\param	_indexOfElementToSwapWithNext any row, from zero to GetNumberOfCell()-2
*/
void	PackList_TimeTrack::SwapElements( unsigned int _indexOfElementToSwapWithNext)
{
	if(_indexOfElementToSwapWithNext>(GetNumberOfCell()-2)) return;
	PackList::SwapElements(_indexOfElementToSwapWithNext);
	// ... and then re-swap dates in date mode:

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