// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackFloatByte_Color_H
#define COM_M4NKIND_PackFloatByte_Color_H

#include "PackFloatByte_Limits.h"

#ifdef _ENGINE_EDITABLE_

class PackFloatByte_Color : public PackFloatByte_Limits
{

public:

	PackFloatByte_Color(void);

	PackFloatByte_Color( VectorDimension _DimensionEnum );

	virtual ~PackFloatByte_Color(void);

	virtual const char *GetClassID() const { return "PackFloat::PackFloatByte_Limits::PackFloatByte_Color"; };

protected:

};
#else

typedef PackFloatByte_Limits PackFloatByte_Color;
#endif

#endif
