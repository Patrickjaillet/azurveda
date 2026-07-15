// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackList_TimeTrack_H
#define COM_M4NKIND_PackList_TimeTrack_H

#include "PackList.h"
#include "PackFloat_FixedPoint.h"
#include "PackLong.h"

class PackList_TimeTrack : public PackList
{

public:
	class TimeTrackElement;

	typedef TimeTrackElement *(*TimeTrackElementCreatorCallBackFunction)(void);

	PackList_TimeTrack( TimeTrackElementCreatorCallBackFunction _func );

#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackList::PackList_TimeTrack"; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual  void	SwapElements( unsigned int _indexOfElementToSwapWithNext);
#endif
	friend class TimeTrackElement;

	class TimeTrackElement : public PackStruct
	{
	public:

			TimeTrackElement();
		#ifdef _ENGINE_EDITABLE_

			virtual void	SetTimeInSecond( double _timeSec );
		#endif

		inline float	GetTimeInSecond(){ return mSer_Time.Get() ; };
		#ifdef _ENGINE_EDITABLE_

			inline	void	SetListThatManageItAsACell(
						PackList_TimeTrack	*,
						PackStruct::Cell			*_pCell
			){
				m_pCellThatManageIt = _pCell ;
			};
		#endif
#ifdef _ENGINE_EDITABLE_

		class PackFloat_FixedPoint_Time : public PackFloat_FixedPoint
		{public:
			PackFloat_FixedPoint_Time(float _fp) : PackFloat_FixedPoint(_fp){};
			 virtual void	Set(float _value);

			 inline void	SetElement(TimeTrackElement	*_pElement){ m_pElement = _pElement; };
		protected:
			TimeTrackElement	*m_pElement;
		};
		friend class PackFloat_FixedPoint_Time;
#else

		typedef PackFloat_FixedPoint PackFloat_FixedPoint_Time;
#endif

	protected:

		PackFloat_FixedPoint_Time	mSer_Time;
#ifdef _ENGINE_EDITABLE_

		PackStruct::Cell			*m_pCellThatManageIt;
#endif

	};

	virtual Cell *GetCellByDate( double _dateInSecond );
#ifdef _ENGINE_EDITABLE_

	virtual	Cell * AddElement( int _indexWhereToInsert=-1,BaseType *_pObjectToAdd=0L);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void DeleteElement( BaseType *_pObWhichCellIsToDestroy);
#endif

#ifdef _ENGINE_EDITABLE_

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);
#endif

	inline float	GetEndTime(){
		if( GetLastCell() ) return  ((TimeTrackElement *)GetLastCell()->GetManagedObject())->GetTimeInSecond();
		else return 0.0f;
	};

protected:

	Cell	*m_pLastCellFound;

};

#endif
