/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_EquationSpline_X_H
#define COM_M4NKIND_EquationSpline_X_H

#include "VirtualEquationSpline.h"
#include "PackFloat.h"
#include "PackList_TimeTrack.h"
/*!
	\class	EquationSpline_X
	\ingroup BaseObjectInherited_VedaLibMath BaseObjectInheritedDocGroup
	\brief	Spline function for X
*/
class EquationSpline_X : public VirtualEquationSpline
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	EquationSpline_X(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(EquationSpline_X);

	/*!
		\brief	static method passed to the list to create new elements.
		\return a new element.
	*/
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
	/*!
		\brief	Tool Method main entry. In editable mode, the object can register a set of methods identified 
				with an ID number by using RegisterToolMethod(), and throw the methods through a switch in ToolMethod(). 
				Here, It is used to export wave sound format.
		\param	_MethodIDToExecute	the tool method ID, greater than 0.
	*/
	virtual void ExecuteToolMethod( unsigned int _MethodIDToExecute ) ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	
	*/
	void ToolMethod_Shift() ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	
	*/
	void ToolMethod_Scale() ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	
	*/
	void ToolMethod_Optimize() ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	
	*/
	void ToolMethod_ImportWaveVolume() ;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	
	*/
	void ToolMethod_ImportWaveFreq() ;
#endif	
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! list of objects in the scenes. According to the type of class, it can be serialized (Scene3D) or created ( Scene3DText )
	PackList_TimeTrack		mSer_List;
	/*!
		\brief	 Return a list that manages SplineElement.
		\return the list
	*/
	virtual	PackList_TimeTrack *GetList(){ return &mSer_List; };

#ifdef _ENGINE_EDITABLE_
	struct	waveInfo
	{
		short    nChannels;      // number of channels (i.e. mono, stereo...) 
		int   nSamplesPerSec;    // sample rate 
		unsigned int	dataStartOffset;
		unsigned int	dataEndOffset;
	};	 
	/*!
		\brief	tool.
	*/
	static bool ToolMethod_Import_StartLoadWave(PackResource &_resource,struct	waveInfo &_WaveInfo) ;
#endif
};
#endif
