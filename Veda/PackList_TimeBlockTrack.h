/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackList_TimeBlockTrack_H
#define COM_M4NKIND_PackList_TimeBlockTrack_H

#include "PackList.h"
#include "PackFloat_FixedPoint.h"
/*!
	\class	PackList_TimeBlockTrack
	\ingroup	BaseSerializableClass
	\brief	 List that defines a set of TimeTrackElement extended objects.
				the difference with PackList_TimeTrack is that elements
				serialize block length and not dates...
*/

class PackList_TimeBlockTrack : public PackList
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	class TimeBlockTrackElement;
	//! TimeTrackElementCreatorCallBackFunction stands for a pointer to a static method that make a new TimeTrackElement.
	typedef TimeBlockTrackElement *(*TimeBlockTrackElementCreatorCallBackFunction)(void);

	/*!
		\brief	Constructor. Like PAckList you specify an element constructor,
				and the meaning of time by elements. Dates and length are then given in seconds.
		\param _func a function that return a new object to manage as an element of the list.
		\param _SerializeLengthBlock	if false, the date in seconds is serialized by elements,if true, the length of elements is serialized.
	*/
	PackList_TimeBlockTrack( TimeBlockTrackElementCreatorCallBackFunction _func/*,bool _SerializeLengthBlock=false*/);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "PackList::PackList_TimeBlockTrack"; };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Edition tool: Exchange 2 neighbour elements in the list, if there is a next element.
				if index is 0, 0 will be 1 and 1 will be 0.
		\param	_indexOfElementToSwapWithNext any row, from zero to GetNumberOfCell()-2
	*/
	virtual  void	SwapElements( unsigned int _indexOfElementToSwapWithNext);
#endif
#ifdef _ENGINE_EDITABLE_
	/*! \brief In edition mode, there is a default timelength you can set at init:
		\param	_DefaultTimeCellLength in seconds.
	*/
	void	SetDefaultTimeCellLength( double _DefaultTimeCellLength=8.0 );
#endif
	/*!
		\brief	Nested class that describe a dated element in the list. 
				It has to be extended with new serializable members (ex: splines, scripts, music tracks..).
				It manages the fact that the time are kept sorted in edition mode.
	*/
	class TimeBlockTrackElement : public PackStruct
	{
	public:
			//! constructor:
			TimeBlockTrackElement();
			//! \brief return the time date in second.
			inline float	GetTimeInSecond(){ return mSer_TimeBlock.Get() ; };

		#ifdef _ENGINE_EDITABLE_
			//! \brief Set the time in seconds in edition mode. It assures the dates are sorted.
			virtual void	SetTimeInSecond( double _timeSec );
		#endif
#ifdef _ENGINE_EDITABLE_
		/*!
			\brief	Nested class that patch the virtual PackFloat_FixedPoint::Set()
			in edition mode, in order to force a test to assure that the elements
			of the list are kept sorted by time date when edited. (yes, it is tricky, but private.)
		*/
		class PackFloat_FixedPoint_TimeBlock : public PackFloat_FixedPoint
		{public:
			PackFloat_FixedPoint_TimeBlock(float _fp) : PackFloat_FixedPoint(_fp){};
			 virtual void	Set(float _value); 
		protected:
		};
#else
		//in non-editable mode, the time date field in each element is a PackFloat_FixedPoint.
		typedef PackFloat_FixedPoint PackFloat_FixedPoint_TimeBlock;		
#endif
		// ------------ PROTECTED -----------
	protected:
		//! the time in second, as serialized.
		PackFloat_FixedPoint_TimeBlock	mSer_TimeBlock;
	};
	/*!
		\brief	Find the Cell which match a given date in the list. 
			It means the date is between this element and the next elment's date.
			Then get its TimeTrackElement with ->GetManagedObject().
		\param	_dateInSecond double date in second. 
		\return	the element, or the first elt if date before.
	*/
	virtual Cell *GetCellByDate( double _dateInSecond );
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
	virtual	Cell * AddElement( int _indexWhereToInsert=-1,BaseType *_pObjectToAdd=0L);
#endif

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	
#ifdef _ENGINE_EDITABLE_
		//! In edition mode, there is a default start timelength.
		double	m_DefaultTimeCellLength;
#endif
};

/*!
	\def	REGISTER_MEMBER_TIMEBLOCKTRACK

	\brief	This macro is used to register a serializable member in a class constructor.
			For editable mode, it uses _InfoString to explicit the use of the member, in order
			to display it in a GUI for example.
			\param	_object the member object
			\param	_InfoString the info string, 0L if not editable.
			\param	_DefaultTimeCellLengthSec a default time length, or 0L if no particular time length.
		
*/
#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_TIMEBLOCKTRACK(_object,_InfoString,_DefaultTimeCellLengthSec) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.SetDefaultTimeCellLength(_DefaultTimeCellLengthSec);
#else
#define		REGISTER_MEMBER_TIMEBLOCKTRACK(_object,_InfoString,_DefaultTimeLengthSec) \
RegisterSerializableMember(_object);
#endif
// end of file
#endif
