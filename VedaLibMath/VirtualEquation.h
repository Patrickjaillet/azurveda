// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_VirtualEquation_H
#define COM_M4NKIND_VirtualEquation_H

#include "BaseObject.h"

class VirtualEquation : public BaseObject
{

public:
#ifdef _ENGINE_EDITABLE_

	VirtualEquation();
#endif

	BASEOBJECT_DEFINE_VIRTUALCLASS(VirtualEquation);

	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )=0;

#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

protected:
#ifdef _ENGINE_EDITABLE_

	VirtualMachine::InternalObject3DBuffer	*m_pPreviewLine;
#endif
#ifdef _ENGINE_EDITABLE_
	static const unsigned int	m_PreviewLinePoints=196;

	virtual	void ProcessPreview_CreateLine(VirtualMachine::InternalObject3DBuffer **_ppVertexAndPolygonBufferOut, unsigned int _nbPoint=2);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview_DrawEquation(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
};
#endif
