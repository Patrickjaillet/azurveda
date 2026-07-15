// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_ModifierObject3DVirtual_H
#define COM_M4NKIND_ModifierObject3DVirtual_H
#include "BaseObject.h"
#include "VirtualMachine.h"

#include "PackFloatByte_Limits.h"
#include "PackDynamicType.h"
#include "PackLong_WithLimits.h"
#include "PackULong_Flags.h"
#include "PackTreeCell.h"

class ModifierObject3DVirtual : public BaseObject
{

public:

	BASEOBJECT_DEFINE_VIRTUALCLASS(ModifierObject3DVirtual);

	virtual	void Modify( VirtualMachine::InternalVertex *_pVertexList,
							const unsigned int _NbVertex,
							float _framedate)=0;

protected:

};

#endif
