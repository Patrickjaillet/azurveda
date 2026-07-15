#ifndef	COM_M4NKIND_Equ_XYZD_Tore_H
#define COM_M4NKIND_Equ_XYZD_Tore_H

#include "Equ_XYZD_Virtual.h"
#include "PackFloat.h"
#include "PackFloatByte_Limits.h"
#include "PackDynamicFloat.h"
/*!
	\class	Equ_XYZD_Tore
	\ingroup	BaseObjectInherited_VedaLibDemo
	\brief	Equation For maetaballs
*/
class Equ_XYZD_Tore : public Equ_XYZD_Virtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Equ_XYZD_Tore(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Equ_XYZD_Tore);
	/*!
		\brief	 Set the framedate, and compute all terms with a constant date.
			pass a int which must be equal to 0 at start.
		\param	_date			in seconds
		\param	_iTermIndex		an index of the equation term to process. 
		\param	_pbound			a 6 float table, xmin,xman,ymin,ymax,zmin,mzmax of the cube where values are !=1.0
		\return	false if index is out of bounds.
	*/
	virtual bool SetFrameDate( float _date, unsigned int _iTermIndex,float _pbound[6] );

	/*!
		\brief	 set X and Y constant. If it return false, all values are 1.0f
				and ComputeByX() is false.
		\param	
	*/
	virtual bool SetYZConstant( float _y,float _z);
	/*!
		\brief	 
		\param	
	*/
	virtual float ComputeByX( float _x);

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! parameter for sinus rotation
	PackDynamicFloat	mSer_Center;
	//! parameter for geometry
	PackFloatByte_Limits	mSer_BigRay;
	//! parameter for distortion
	PackFloatByte_Limits	mSer_LittleRay;
	//! parameter for sinus rotation
	PackDynamicFloat	mSer_AxisRotation;
	//! added to mSer_BigRay
	PackDynamicFloat	mSer_BigRayVariation;
	//! added to mSer_LittleRay
	PackDynamicFloat	mSer_LittleRayVariation;

	//! rotation term, update by date.
	float	m_sinMove;
	//! rotation term, update by date.
	float	m_cosMove;
	//!
	float	m_littleRayConst;
	//!
	float	m_BigRayConst;
	//! update by date.
	float	m_ydyd;
	//! update by date.
	float	m_zz;
	float	m_center[4];
	float m_centerX,m_centerY;
};
#endif
