// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackLong_WithLimits_H
#define COM_M4NKIND_PackLong_WithLimits_H

#include "PackLong.h"

#ifdef _ENGINE_EDITABLE_

class PackLong_WithLimits : public PackLong
{

public:

	PackLong_WithLimits(void);

	virtual ~PackLong_WithLimits(void);

	virtual const char *GetClassID() const { return "PackLong_WithLimits"; };

	virtual void Set( signed int _value );

	void	SetMinMax( signed int _min, signed int _max );

	inline int GetMin(){return(m_Minimum);};

	inline int GetMax(){return(m_Maximum);};

protected:
	signed int	m_Minimum;
	signed int	m_Maximum;
};

#define		REGISTER_MEMBER_WITHLIMITS_SETMINMAX(_object,_InfoString,_DefaultVal,_min,_max) \
REGISTER_MEMBER(_object,_InfoString)\
_object.SetMinMax( _min, _max );\
_object.Set(_DefaultVal);
#else

typedef PackLong PackLong_WithLimits;

#define		REGISTER_MEMBER_WITHLIMITS_SETMINMAX(_object,_InfoString,_DefaultVal,_min,_max) \
REGISTER_MEMBER(_object,_InfoString)

#endif

#endif
