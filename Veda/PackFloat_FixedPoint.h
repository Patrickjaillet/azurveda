// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackFloat_FixedPoint_H
#define COM_M4NKIND_PackFloat_FixedPoint_H

#include "PackFloat.h"

class PackFloat_FixedPoint : public PackFloat
{

public:

	PackFloat_FixedPoint();

	PackFloat_FixedPoint(const float _ValueToMultiplyFromIntToFloat );

	PackFloat_FixedPoint( VectorDimension _DimensionEnum,const float _ValueToMultiplyFromIntToFloat );

#ifdef _ENGINE_EDITABLE_

	virtual ~PackFloat_FixedPoint(void);
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackFloat::PackFloat_FixedPoint"; };
#endif

protected:

	float	m_ValueToMultiplyFromIntToFloat;
};

#endif
