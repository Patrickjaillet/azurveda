// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_ModifierObject3DColor_H
#define COM_M4NKIND_ModifierObject3DColor_H
#include "ModifierObject3DVirtual.h"

#include "PackDynamicFloat.h"

class ModifierObject3DColor : public ModifierObject3DVirtual
{

public:

	ModifierObject3DColor(void);

	BASEOBJECT_DEFINE_CLASS(ModifierObject3DColor);

	virtual	void Modify( VirtualMachine::InternalVertex *_pVertexList,
							const unsigned int _NbVertex,
							float _framedate);

protected:

	PackDynamicFloat	mSer_BaseColor_RGB;

	PackDynamicFloat	mSer_BaseColor_Alpha;

	float	m_LastComputedDate;

	float	m_dateRGBA[4];

};

#endif
