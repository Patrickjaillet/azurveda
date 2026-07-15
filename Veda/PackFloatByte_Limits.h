// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackFloatByte_Limits_H
#define COM_M4NKIND_PackFloatByte_Limits_H

#include "PackFloat.h"

class PackFloatByte_Limits : public PackFloat
{

public:

	PackFloatByte_Limits(void);

	PackFloatByte_Limits( VectorDimension _DimensionEnum );

#ifdef _ENGINE_EDITABLE_

	virtual ~PackFloatByte_Limits(void);
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackFloat::PackFloatByte_Limits"; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void	Set(float _value);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void	Set(unsigned int _dimensionIndex,float _value);
#endif

	void	SetMinMax(float _min,float _max);

	inline float GetMin(){return(m_MinValue);};

	inline float GetMax(){return(m_MinValue+m_DomainLength);};

protected:

	float	m_MinValue;

	float	m_DomainLength;

};

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(_object,_InfoString,_DefaultVal,_min,_max) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.SetMinMax( _min , _max );\
_object.Set(_DefaultVal);
#define		REGISTER_MEMBER_PACKFLOATBYTE_0_1(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.Set(_DefaultVal);
#else

#define		REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(_object,_InfoString,_DefaultVal,_min,_max) \
RegisterSerializableMember(_object);\
_object.SetMinMax( _min , _max );
#define		REGISTER_MEMBER_PACKFLOATBYTE_0_1(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);

#endif

#endif
