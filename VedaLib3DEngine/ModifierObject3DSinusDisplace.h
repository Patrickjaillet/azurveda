// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_ModifierObject3DSinusDisplace_H
#define COM_M4NKIND_ModifierObject3DSinusDisplace_H
#include "ModifierObject3DVirtual.h"
#include "PackDynamicFloat.h"

class ModifierObject3DSinusDisplace : public ModifierObject3DVirtual
{

public:

	ModifierObject3DSinusDisplace(void);

	BASEOBJECT_DEFINE_CLASS(ModifierObject3DSinusDisplace);

	virtual	void Modify( VirtualMachine::InternalVertex *_pVertexList,
							const unsigned int _NbVertex,
							float _framedate);

protected:

	PackDynamicFloat	mSer_Center;

	PackDynamicFloat	mSer_Frequence;

	PackDynamicFloat	mSer_Amplitude;

	float	m_LastComputedDate;

	float	m_Center[4];

	float	m_Freq[4];

	float	m_Amp[4];

};

#endif
