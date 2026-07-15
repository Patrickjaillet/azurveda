// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackString_MultipleLines_H
#define COM_M4NKIND_PackString_MultipleLines_H

#include "PackString.h"
#ifdef	_ENGINE_EDITABLE_

class PackString_MultipleLines : public PackString
{

public:

	PackString_MultipleLines(void);

	virtual ~PackString_MultipleLines(void);

	virtual const char *GetClassID() const { return "PackString::PackString_MultipleLines"; };

protected:

};
#else

typedef PackString PackString_MultipleLines;
#endif

#endif
