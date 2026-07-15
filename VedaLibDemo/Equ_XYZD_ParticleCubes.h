#ifndef	COM_M4NKIND_Equ_XYZD_ParticleCubes_H
#define COM_M4NKIND_Equ_XYZD_ParticleCubes_H

#include "Equ_XYZD_Particle.h"
#include "PackFloat.h"
#include "PackFloat_FixedPoint.h"
#include "PackList.h"
/*!
	\class	Equ_XYZD_ParticleCubes
	\ingroup	BaseObjectInherited_VedaLibDemo
	\brief	Equation For maetaballs
*/
class Equ_XYZD_ParticleCubes : public Equ_XYZD_Particle
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Equ_XYZD_ParticleCubes(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Equ_XYZD_ParticleCubes);
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
	float	m_raySpaceHalf;
};
#endif
