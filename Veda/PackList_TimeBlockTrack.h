// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackList_TimeBlockTrack_H
#define COM_M4NKIND_PackList_TimeBlockTrack_H

#include "PackList.h"
#include "PackFloat_FixedPoint.h"

class PackList_TimeBlockTrack : public PackList
{

public:
	class TimeBlockTrackElement;

	typedef TimeBlockTrackElement *(*TimeBlockTrackElementCreatorCallBackFunction)(void);

	PackList_TimeBlockTrack( TimeBlockTrackElementCreatorCallBackFunction _func);

#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackList::PackList_TimeBlockTrack"; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual  void	SwapElements( unsigned int _indexOfElementToSwapWithNext);
#endif
#ifdef _ENGINE_EDITABLE_

	void	SetDefaultTimeCellLength( double _DefaultTimeCellLength=8.0 );
#endif

	class TimeBlockTrackElement : public PackStruct
	{
	public:

			TimeBlockTrackElement();

			inline float	GetTimeInSecond(){ return mSer_TimeBlock.Get() ; };

		#ifdef _ENGINE_EDITABLE_

			virtual void	SetTimeInSecond( double _timeSec );
		#endif
#ifdef _ENGINE_EDITABLE_

		class PackFloat_FixedPoint_TimeBlock : public PackFloat_FixedPoint
		{public:
			PackFloat_FixedPoint_TimeBlock(float _fp) : PackFloat_FixedPoint(_fp){};
			 virtual void	Set(float _value);
		protected:
		};
#else

		typedef PackFloat_FixedPoint PackFloat_FixedPoint_TimeBlock;
#endif

	protected:

		PackFloat_FixedPoint_TimeBlock	mSer_TimeBlock;
	};

	virtual Cell *GetCellByDate( double _dateInSecond );
#ifdef _ENGINE_EDITABLE_

	virtual	Cell * AddElement( int _indexWhereToInsert=-1,BaseType *_pObjectToAdd=0L);
#endif

protected:

#ifdef _ENGINE_EDITABLE_

		double	m_DefaultTimeCellLength;
#endif
};

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_TIMEBLOCKTRACK(_object,_InfoString,_DefaultTimeCellLengthSec) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.SetDefaultTimeCellLength(_DefaultTimeCellLengthSec);
#else
#define		REGISTER_MEMBER_TIMEBLOCKTRACK(_object,_InfoString,_DefaultTimeLengthSec) \
RegisterSerializableMember(_object);
#endif

#endif
