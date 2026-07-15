#ifndef	COM_M4NKIND_Equ_XYZD_Virtual_H
#define COM_M4NKIND_Equ_XYZD_Virtual_H

#include "BaseObject.h"
#include "PackFloat.h"
#include "VirtualEquation.h"
class MarchinCubeSpace;

/*!
	\class	Equ_XYZD_Virtual
	\ingroup	BaseObjectInherited_VedaLibDemo
	\brief	virtual class Defining an equation of the form: result = func(x,y,z,time)
*/
class Equ_XYZD_Virtual : public VirtualEquation
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Equ_XYZD_Virtual(void);
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(Equ_XYZD_Virtual);

	/*!
		\brief	 Set the framedate, and compute all terms with a constant date.
			pass a int which must be equal to 0 at start.
		\param	_date			in seconds
		\param	_iTermIndex		an index of the equation term to process. 
		\param	_pbound			a 6 float table, xmin,xman,ymin,ymax,zmin,mzmax of the cube where values are !=1.0
		\return	false if index is out of bounds.
	*/
	virtual bool SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] )=0;
	//virtual float SetYZ( float _y,float _z )=0;
	/*!
		\brief	 set X and Y constant. If it return false, all values are 1.0f
				and ComputeByX() is false.
		\param	
	*/
	virtual bool	SetYZConstant( float _y,float _z)=0;
	/*!
		\brief	 
		\param	
	*/
	virtual float ComputeByX( float _x)=0;

	/*!
		\brief	 Compute a scalar value according to a parameter list (X,Y,Z,D).
		\param	_OutgoingParameterTable result table. elements are writen or not according to the equation.
		\param _IncomingParameterTable. float table, the (X,Y,Z,D) parameters.
		\return	scalar value in space.
	*/
	float Compute( float _x,float _y,float _z );
	/*!
		\brief	VirtualEquation 's virtual method redirection
		\param	_OutgoingParameterTable result table. elements are writen or not according to the equation.
		\param _IncomingParameterTable. float table, the (X,Y,Z,D) parameters.
	*/
	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
				created object. This is done with this method. sub classes can implement it (or not) in
				any way, to explicit current shape of an object.<br>

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration
	*/
	//virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
	/*!
		\brief	Tool: make an inverted parabolic curve that look like sqrt between 0 and 1.
				if >1, 1 is returned.
		\param	_vv value
		\return  1-((1-_vv)^2)
	*/
	static inline float QuickSqrtM(float _vv)
	{ 
		register const float v1p0=/*PackFloat::m_1p0*/1.0f; // 1.0f 
		if(_vv>v1p0) return v1p0;
		_vv = v1p0-_vv;
		_vv = _vv*_vv;
		return v1p0-_vv ;
	};
			
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the constant Y parameter set by SetYZConstant()
	float	m_y;
	//! the constant Z parameter set by SetYZConstant()
	float	m_z;
	//! for allegeance to VirtualEquation Compte() system, we can keep the last date used for SetFrameDate()
	float	m_LastComputedDate;

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	protected extention for ProcessPreview() that allows to draw the equation on a bitmap.
		\param	_frameDate a date, in second, which defines the effect cinematic.
	*/
	//virtual	void	ProcessPreview_Equation(double _frameDate);
#endif
};
#endif
