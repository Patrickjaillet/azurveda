/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_VirtualEquation_H
#define COM_M4NKIND_VirtualEquation_H

#include "BaseObject.h"
/*!
	\class	VirtualEquation
	\ingroup BaseObjectInherited_VedaLibMath BaseObjectInheritedDocGroup
	\brief	virtual class Defining an equation of the form: result = func(x,y,z,time)
*/
class VirtualEquation : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	VirtualEquation();
#endif
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(VirtualEquation);
	/*!
		\brief	 Compute 1,2,3 or 4 value according to a parameter list (X,Y,Z,D).
				An equation can manage one or more dimensions on input and output.
				the result is <b>added</b> over the outgoing parameter table,
				and unused output parameters are left untouch, so you
				must init _OutgoingParameterTable before, with default values you will choose.
		\param	_OutgoingParameterTable result table. result is added over the table values.
		\param _IncomingParameterTable. float table, the (X,Y,Z,D) parameters.
	*/
	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )=0;
	
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
				created object. This is done with this method. sub classes can implement it (or not) in
				any way, to explicit current shape of an object.<br>

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration
	*/
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
	
/*==================================================================
								PROTECTED
==================================================================*/
protected:
#ifdef _ENGINE_EDITABLE_
	//! object created for preview.
	VirtualMachine::InternalObject3DBuffer	*m_pPreviewLine;
#endif
#ifdef _ENGINE_EDITABLE_
	static const unsigned int	m_PreviewLinePoints=196;
	/*!
		\brief	Preview Tool: Create a preview line 3D Object.
	*/
	virtual	void ProcessPreview_CreateLine(VirtualMachine::InternalObject3DBuffer **_ppVertexAndPolygonBufferOut, unsigned int _nbPoint=2);
#endif	
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	tool for ProcessPreview: draw a grid and the equation value, and let the current
					scale and matrix on the viewport.

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration
	*/
	virtual	void ProcessPreview_DrawEquation(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
};
#endif
