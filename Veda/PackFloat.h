// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackFloat_H
#define COM_M4NKIND_PackFloat_H

#include "BaseType.h"

class PackFloat : public BaseType
{

public:

	PackFloat(void);

	typedef	enum {
		vd_X=1,
		vd_XY=2,
		vd_XYZ=3,
		vd_XYZD=4
	}	VectorDimension	;

	PackFloat( VectorDimension _DimensionEnum );

#ifdef _ENGINE_EDITABLE_

	virtual ~PackFloat(void);
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackFloat"; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char	*ValueToString();
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void	Set(float _value);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void	Set(unsigned int _dimensionIndex,float _value);
#endif
#ifdef _ENGINE_EDITABLE_

	inline void	Set3f(float _x, float _y, float _z){ Set(_x); Set(1,_y); Set(2,_z); };
#endif

	inline float	Get() const { return m_value[0] ; };

	inline float	Get(unsigned int _dimensionIndex) const { return m_value[_dimensionIndex] ; };

	inline VectorDimension	GetVectorDimension(){return (VectorDimension)m_NumberOfDimensionManaged ; };

	static const float	m_0p0;
	static const float	m_0p01;
	static const float	m_0p00001;
	static const float	m_0p25;
	static const float	m_0p5;
	static const float	m_1p0;
	static const float	m_2p0;
	static const float	m_255p0;
	static const float	m_256p0;
	static const float	m_0p0625;
	static const float	m_100p0;
	static const float	m_1Div255;
	static const float	m_1Div256;
	static const float  m_1Div64;
	static const float	m_Pi;
	static const float	m_2Pi;
	static const float	m_180DivPi;
	static const float	m_32767p0;
	static const float	m_Max;

protected:

	float	m_value[vd_XYZD];

	const unsigned int	m_NumberOfDimensionManaged;
};

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKFLOAT(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.Set(_DefaultVal);
#else
#define		REGISTER_MEMBER_PACKFLOAT(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);
#endif

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKFLOAT_XY(_object,_InfoString,_DefaultValX,_DefaultValY) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.Set(0,_DefaultValX);\
_object.Set(1,_DefaultValY);
#else
#define		REGISTER_MEMBER_PACKFLOAT_XY(_object,_InfoString,_DefaultValX,_DefaultValY) \
RegisterSerializableMember(_object);
#endif

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKFLOAT_XYZ(_object,_InfoString,_DefaultValX,_DefaultValY,_DefaultValZ) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.Set(0,_DefaultValX);\
_object.Set(1,_DefaultValY);\
_object.Set(2,_DefaultValZ);
#else
#define		REGISTER_MEMBER_PACKFLOAT_XYZ(_object,_InfoString,_DefaultValX,_DefaultValY,_DefaultValZ) \
RegisterSerializableMember(_object);
#endif

#endif
