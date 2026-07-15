// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_Texture3D_H
#define COM_M4NKIND_Texture3D_H

#include "BaseObject.h"
#include "PackObjectReference.h"
#include "PackULong_Flags.h"
#include "VirtualMachine.h"
#include "PackDynamicFloat.h"

class Texture3D : public BaseObject
{

public:

	Texture3D(void);

	BASEOBJECT_DEFINE_CLASS(Texture3D);

	inline  VirtualMachine::InternalTexture	*GetVirtualMachineTexture(){ return m_pInternalTexture; };

	virtual  void	UpdateToFrame(float _framedate);

#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

protected:

	PackULong_Flags	mSer_Flags;

	PackDynamicFloat	mSer_BaseColor;

	PackObjectReference	mSerRef_MappingImage;

	PackObjectReference	mSerRef_EnvImage;

	VirtualMachine::InternalTexture		*m_pInternalTexture;

	float	m_LastUpdateDate;
#ifdef _ENGINE_EDITABLE_

	VirtualMachine::InternalObject3DBuffer *m_pPreviewObject3D;

	static const unsigned int		m_MaxNumberOfGridDivisionInWidth=14;

	static const unsigned int		m_MaxNumberOfGridDivisionInHeight=14;
#endif

	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif

};

#endif
