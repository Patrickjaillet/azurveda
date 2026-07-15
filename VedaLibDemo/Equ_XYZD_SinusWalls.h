#ifndef	COM_M4NKIND_Equ_XYZD_SinusWalls_H
#define COM_M4NKIND_Equ_XYZD_SinusWalls_H

#include "Equ_XYZD_Virtual.h"
#include "PackFloat.h"
#include "PackULong_Flags.h"
#include "PackFloatByte_Limits.h"
#include "PackDynamicFloat.h"
/*!
	\class	Equ_XYZD_SinusWalls
	\ingroup	BaseObjectInherited_VedaLibDemo
	\brief	Equation For maetaballs
*/
class Equ_XYZD_SinusWalls : public Equ_XYZD_Virtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Equ_XYZD_SinusWalls(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Equ_XYZD_SinusWalls);
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
	typedef enum
	{
		swe_Floor=1,
		swe_Ceiling=2,
		swe_West=4,
		swe_East=8,
		swe_North=16,
		swe_South=32
	} SinusWallBits ;
	//! the flags
	PackULong_Flags		mSer_WallFlags;
	//! parameter for distortion
	PackDynamicFloat	mSer_SinAmp;
	//! parameter for distortion
	PackDynamicFloat	mSer_FloorAlt;
	//! parameter for distortion
	PackDynamicFloat	mSer_CeilingAlt;
	//! parameter for distortion
	PackFloatByte_Limits	mSer_Thickness;
	//! date term.
	float	m_y;
	float	m_yc;
	float	m_zc;
//	float	
	//! YZ term.
	float	m_zz;

};
#endif
