/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackList_TimeTrack_H
#define COM_M4NKIND_PackList_TimeTrack_H

#include "PackList.h"
#include "PackFloat_FixedPoint.h"
#include "PackLong.h"
/*!
	\class	PackList_TimeTrack
	\ingroup	BaseSerializableClass
	\brief	 List that defines a set of TimeTrackElement extended objects.
				Each elements define when they begin, when they end, and
				an order number, which is different from zero if another element
				is in the same time range.			
			The Goal with this class is to unify the ways to create timed lists,
			that can be managed by a single GUI gadget for script or spline management.
			It was actually made to be used in spline key list, a script manager 
			this is only a simple serializable class.
*/

class PackList_TimeTrack : public PackList
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	class TimeTrackElement;
	//! TimeTrackElementCreatorCallBackFunction stands for a pointer to a static method that make a new TimeTrackElement.
	typedef TimeTrackElement *(*TimeTrackElementCreatorCallBackFunction)(void);

	/*!
		\brief	Constructor. Like PAckList you specify an element constructor,
				and the meaning of time by elements. Dates and length are then given in seconds.
		\param _func a function that return a new object to manage as an element of the list.
		\param _SerializeLengthBlock	if false, the date in seconds is serialized by elements,if true, the length of elements is serialized.
	*/
	PackList_TimeTrack( TimeTrackElementCreatorCallBackFunction _func /*,bool _SerializeLengthBlock=false*/);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "PackList::PackList_TimeTrack"; };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Edition tool: Exchange 2 neighbour elements in the list, if there is a next element.
				if index is 0, 0 will be 1 and 1 will be 0.
		\param	_indexOfElementToSwapWithNext any row, from zero to GetNumberOfCell()-2
	*/
	virtual  void	SwapElements( unsigned int _indexOfElementToSwapWithNext);
#endif
	friend class TimeTrackElement;
	/*!
		\brief	Nested class that describe a dated element in the list. 
				It has to be extended with new serializable members (ex: splines, scripts, music tracks..).
				It manages the fact that the time are kept sorted in edition mode.
	*/
	class TimeTrackElement : public PackStruct
	{
	public:
			//! constructor:
			TimeTrackElement();
		#ifdef _ENGINE_EDITABLE_
			//! \brief Set the time in seconds in edition mode. It assures the dates are sorted.
			virtual void	SetTimeInSecond( double _timeSec );
		#endif
		//! \brief return the time date in second.
		inline float	GetTimeInSecond(){ return mSer_Time.Get() ; };
		#ifdef _ENGINE_EDITABLE_
			//! \brief Private use: In edition mode, used at cell creation:
			inline	void	SetListThatManageItAsACell(
						PackList_TimeTrack	*_pList,
						PackStruct::Cell			*_pCell
			){	//m_pListThatManageItAsAElement = _pList ; 
				m_pCellThatManageIt = _pCell ; 
			};
		#endif
#ifdef _ENGINE_EDITABLE_
		/*!
			\brief	Nested class that patch the virtual PackFloat_FixedPoint::Set()
			in edition mode, in order to force a test to assure that the elements
			of the list are kept sorted by time date when edited. (yes, it is tricky, but private.)
		*/
		class PackFloat_FixedPoint_Time : public PackFloat_FixedPoint
		{public:
			PackFloat_FixedPoint_Time(float _fp) : PackFloat_FixedPoint(_fp){};
			 virtual void	Set(float _value); 
			// void	SetNotSorted(float _value); 
			 inline void	SetElement(TimeTrackElement	*_pElement){ m_pElement = _pElement; };
		protected:
			TimeTrackElement	*m_pElement;
		};
		friend class PackFloat_FixedPoint_Time;
#else
		//in non-editable mode, the time date field in each element is a PackFloat_FixedPoint.
		typedef PackFloat_FixedPoint PackFloat_FixedPoint_Time;		
#endif

		// ------------ PROTECTED -----------
	protected:
		//! the time in second, as serialized.
		PackFloat_FixedPoint_Time	mSer_Time;
#ifdef _ENGINE_EDITABLE_
		//! in edition mode, the element must know the list that manage it:
		//PackList_TimeTrack	*m_pListThatManageItAsAElement;
		//! in edition mode, the element must know the Cell that manage it:
		PackStruct::Cell			*m_pCellThatManageIt;
#endif

	};
	// too complex for gcc3.4.4! friend class TimeTrackElement::PackFloat_FixedPoint_Time;

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
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief delete an element at a given index. It closes and destroys the object.
		\param _pObWhichCellIsToDestroy ob WhichCellIsToDestroy.
	*/
	virtual void DeleteElement( BaseType *_pObWhichCellIsToDestroy);
#endif

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Special patch in edition mode to add an init to the elements.
		\param	_pDescriptionChunk the objet description chunk. 
		\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
	*/
	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);
#endif
	/*!
		\brief	 returns the Total Time length of this media, in seconds, when played at 
				normal speed. This is just an information about the time length of this media,
				and you can pass any date to ProcessMedia().
		\return	Total Time length of this media, in seconds, when played at normal speed. Can be used or not.
	*/
	inline float	GetEndTime(){ 
		if( GetLastCell() ) return  ((TimeTrackElement *)GetLastCell()->GetManagedObject())->GetTimeInSecond();
		else return 0.0f;
	};
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! if false, the date in seconds is serialized by elements,if true, the length of elements is serialized.
	//const bool m_SerializeLengthBlock;

	//! optimisation: GetCellByDate() keep the last searched Cell: most of the time, it is the same from a frame to another.
	Cell	*m_pLastCellFound;

};
// end of file
#endif
