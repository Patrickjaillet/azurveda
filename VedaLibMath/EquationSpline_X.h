// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_EquationSpline_X_H
#define COM_M4NKIND_EquationSpline_X_H

#include "VirtualEquationSpline.h"
#include "PackFloat.h"
#include "PackList_TimeTrack.h"

class EquationSpline_X : public VirtualEquationSpline
{

public:

	EquationSpline_X(void);

	BASEOBJECT_DEFINE_CLASS(EquationSpline_X);

	static PackList_TimeTrack::TimeTrackElement *NewSplineElementX(){ return new SplineElement(PackFloat::vd_X); };

#ifdef _ENGINE_EDITABLE_
	typedef enum {
		tm_VEquationSpline_Shift=tm_BaseObject_FirstInheritageToolMethod,
		tm_VEquationSpline_Scale,
		tm_VEquationSpline_Optimize,
		tm_VEquationSpline_ImportWaveVolume,
		tm_VEquationSpline_ImportWaveFreq,
		tm_VEquationSpline_FirstInheritageToolMethod,
	};

	virtual void ExecuteToolMethod( unsigned int _MethodIDToExecute ) ;
#endif
#ifdef _ENGINE_EDITABLE_

	void ToolMethod_Shift() ;
#endif
#ifdef _ENGINE_EDITABLE_

	void ToolMethod_Scale() ;
#endif
#ifdef _ENGINE_EDITABLE_

	void ToolMethod_Optimize() ;
#endif
#ifdef _ENGINE_EDITABLE_

	void ToolMethod_ImportWaveVolume() ;
#endif
#ifdef _ENGINE_EDITABLE_

	void ToolMethod_ImportWaveFreq() ;
#endif

protected:

	PackList_TimeTrack		mSer_List;

	virtual	PackList_TimeTrack *GetList(){ return &mSer_List; };

#ifdef _ENGINE_EDITABLE_
	struct	waveInfo
	{
		short    nChannels;
		int   nSamplesPerSec;
		unsigned int	dataStartOffset;
		unsigned int	dataEndOffset;
	};

	static bool ToolMethod_Import_StartLoadWave(PackResource &_resource,struct	waveInfo &_WaveInfo) ;
#endif
};
#endif
