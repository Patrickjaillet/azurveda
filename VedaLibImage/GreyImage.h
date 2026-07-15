// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_GreyImage_H
#define COM_M4NKIND_GreyImage_H

#include "VirtualImage.h"

class GreyImage : public VirtualImage
{

public:

	BASEOBJECT_DEFINE_VIRTUALCLASS(GreyImage);

	virtual unsigned int	GetByteDepth(){ return(1); };

protected:

};

#endif
