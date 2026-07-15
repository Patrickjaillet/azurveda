// SPDX-License-Identifier: LGPL-2.1-only

#include "VirtualEquation.h"
#include "VirtualMachine.h"
#include "PackFloat.h"
BASEOBJECT_DECLARE_VIRTUALCLASS( "Equ", VirtualEquation, BaseObject );

#ifdef _ENGINE_EDITABLE_
VirtualEquation::VirtualEquation() : BaseObject()
	,m_pPreviewLine(0L)
{

}
#endif

#ifdef _ENGINE_EDITABLE_

void VirtualEquation::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	ProcessPreview_DrawEquation(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);

}
#endif

#ifdef _ENGINE_EDITABLE_

void VirtualEquation::ProcessPreview_CreateLine(VirtualMachine::InternalObject3DBuffer **_ppVertexAndPolygonBufferOut, unsigned int _nbPoint)
{
	GetMachine()->NewObject3DBuffer( _ppVertexAndPolygonBufferOut ,
									(_nbPoint)*2,
									(_nbPoint-1)*2,0);
	if( !(*_ppVertexAndPolygonBufferOut)) return;
	int ii;
	VirtualMachine::InternalTriangle *pTriangle = (*_ppVertexAndPolygonBufferOut)->GetFirstTriangle();

	for(ii=0 ; ii<(_nbPoint-1) ; ii++)
	{
		pTriangle->m_p0 = 0+(ii<<1);
		pTriangle->m_p2 = 1+(ii<<1);
		pTriangle->m_p1 = 3+(ii<<1);
		pTriangle++;
		pTriangle->m_p0 = 0+(ii<<1);
		pTriangle->m_p1 = 2+(ii<<1);
		pTriangle->m_p2 = 3+(ii<<1);
		pTriangle++;
	}
	(*_ppVertexAndPolygonBufferOut)->SetNumberOfActiveTriangle((_nbPoint-1)*2);
}
#endif
#ifdef _ENGINE_EDITABLE_

void VirtualEquation::ProcessPreview_DrawEquation(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *)
{
	VirtualMachine::InternalObject3DBuffer	*m_pPreviewLine;
	VirtualMachine::InternalObject3DBuffer	*m_pPreviewLineY;
	VirtualMachine::InternalObject3DBuffer	*m_pPreviewLineZ;
	VirtualMachine::InternalObject3DBuffer	*m_pPreviewLineD;
	VirtualMachine::InternalObject3DBuffer	*m_pGridLine;
	ProcessPreview_CreateLine(&m_pGridLine,2);
	if(!m_pGridLine) return;
	ProcessPreview_CreateLine(&m_pPreviewLine,m_PreviewLinePoints);
	ProcessPreview_CreateLine(&m_pPreviewLineY,m_PreviewLinePoints);
	ProcessPreview_CreateLine(&m_pPreviewLineZ,m_PreviewLinePoints);
	ProcessPreview_CreateLine(&m_pPreviewLineD,m_PreviewLinePoints);
	if(!m_pPreviewLine || !m_pPreviewLineY || !m_pPreviewLineZ || !m_pPreviewLineD )
	{
		GetMachine()->DeleteObject3DBuffer(&m_pGridLine);
		GetMachine()->DeleteObject3DBuffer(&m_pPreviewLine);
		GetMachine()->DeleteObject3DBuffer(&m_pPreviewLineY);
		GetMachine()->DeleteObject3DBuffer(&m_pPreviewLineZ);
		GetMachine()->DeleteObject3DBuffer(&m_pPreviewLineD);
		return;
	}
	m_pPreviewLine->SetColor(1.0f,0.0f,0.0f,0.6f);
	m_pPreviewLineY->SetColor(0.0f,1.0f,0.0f,0.6f);
	m_pPreviewLineZ->SetColor(0.0f,0.0f,1.0f,0.6f);
	m_pPreviewLineD->SetColor(0.5f,0.5f,0.5f,0.5f);

		_pPreviewViewPort->Clear(0.0f,0.0f,0.05f);
		_pPreviewViewPort->Matrix_LoadID();
		_pPreviewViewPort->Matrix_Translate(0.0f,0.0f,-1.0f);
		_pPreviewViewPort->SetFOVLength(1.0f);

		float xds1= (-_pPreviewViewPort->GetPositionX1() ) /
			(_pPreviewViewPort->GetPositionX2()-_pPreviewViewPort->GetPositionX1())
			-0.5f ;

		float xds2= (1.0f-_pPreviewViewPort->GetPositionX1()) /
			(_pPreviewViewPort->GetPositionX2()-_pPreviewViewPort->GetPositionX1())
			-0.5f ;
		float yds1= (-_pPreviewViewPort->GetPositionY1() ) /
			(_pPreviewViewPort->GetPositionY2()-_pPreviewViewPort->GetPositionY1())
			-0.5f ;

		float yds2= (1.0f-_pPreviewViewPort->GetPositionY1()) /
			(_pPreviewViewPort->GetPositionY2()-_pPreviewViewPort->GetPositionY1())
			-0.5f ;

		float vecx = xds2-xds1;
		vecx/=((float)m_PreviewLinePoints);

		float ywidth = 0.0035f /
			(_pPreviewViewPort->GetPositionY2()-_pPreviewViewPort->GetPositionY1()) ;
		float xwidth = 0.0032f /
			(_pPreviewViewPort->GetPositionX2()-_pPreviewViewPort->GetPositionX1()) ;

		float ratio = xds2-xds1;
		float	gridWidthX=1.0f;
		float	firstLine = (float)((int)xds1);
		int		nbLine = (int)(xds2-xds1);
		if(ratio>20.0f)
		{
			gridWidthX = 10.0f;
			firstLine = ((float)((int)(xds1/10.0f)))*10.0f;
			nbLine/=10;
		}
		if(ratio>200.0f)
		{
			gridWidthX = 100.0f;
			firstLine = ((float)((int)(xds1/100.0f)))*100.0f;
			nbLine/=10;
		}
		nbLine++;
		int ii;
		VirtualMachine::InternalVertex *pVert,*pVertY,*pVertZ,*pVertD;
	m_pGridLine->SetColor(0.0f,0.9f,0.0f,0.5f);
		for(ii=0 ; ii<nbLine ; ii++)
		{
			pVert = m_pGridLine->GetFirstVertex();
			pVert->m_x = firstLine-xwidth*0.5f;
			pVert->m_y = yds2;
			pVert->m_z = 0.0f;
			pVert++;
			pVert->m_x = firstLine+xwidth*0.5f;
			pVert->m_y = yds2;
			pVert->m_z = 0.0f;
			pVert++;
			pVert->m_x = firstLine-xwidth*0.5f;
			pVert->m_y = yds1;
			pVert->m_z = 0.0f;
			pVert++;
			pVert->m_x = firstLine+xwidth*0.5f;
			pVert->m_y = yds1;
			pVert->m_z = 0.0f;

			_pPreviewViewPort->RenderMesh(m_pGridLine);
			firstLine += gridWidthX;
		}

		 ratio = yds2-yds1;
			gridWidthX=1.0f;
			firstLine = (float)((int)yds1);
			nbLine = (int)(xds2-xds1);
		if(ratio>20.0f)
		{
			gridWidthX = 10.0f;
			firstLine = ((float)((int)(yds1/10.0f)))*10.0f;
			nbLine/=10;
		}
		if(ratio>200.0f)
		{
			gridWidthX = 100.0f;
			firstLine = ((float)((int)(yds1/100.0f)))*100.0f;
			nbLine/=10;
		}
		nbLine++;
		for(ii=0 ; ii<nbLine ; ii++)
		{
			pVert = m_pGridLine->GetFirstVertex();
			pVert->m_y = firstLine-ywidth*0.5f;
			pVert->m_x = xds1;
			pVert->m_z = 0.0f;
			pVert++;
			pVert->m_y = firstLine+ywidth*0.5f;
			pVert->m_x = xds1;
			pVert->m_z = 0.0f;
			pVert++;
			pVert->m_y = firstLine-ywidth*0.5f;
			pVert->m_x = xds2;
			pVert->m_z = 0.0f;
			pVert++;
			pVert->m_y = firstLine+ywidth*0.5f;
			pVert->m_x = xds2;
			pVert->m_z = 0.0f;

			_pPreviewViewPort->RenderMesh(m_pGridLine);
			firstLine += gridWidthX;
		}

	m_pGridLine->SetColor(0.8f,0.4f,0.8f,0.75f);
			pVert = m_pGridLine->GetFirstVertex();
			pVert->m_x = _frameDate-xwidth*0.5f;
			pVert->m_y = yds2;
			pVert->m_z = 0.0f;
			pVert++;
			pVert->m_x = _frameDate+xwidth*0.5f;
			pVert->m_y = yds2;
			pVert->m_z = 0.0f;
			pVert++;
			pVert->m_x = _frameDate-xwidth*0.5f;
			pVert->m_y = yds1;
			pVert->m_z = 0.0f;
			pVert++;
			pVert->m_x = _frameDate+xwidth*0.5f;
			pVert->m_y = yds1;
			pVert->m_z = 0.0f;
	_pPreviewViewPort->RenderMesh(m_pGridLine);

		pVert = m_pPreviewLine->GetFirstVertex();
		pVertY = m_pPreviewLineY->GetFirstVertex();
		pVertZ = m_pPreviewLineZ->GetFirstVertex();
		pVertD = m_pPreviewLineD->GetFirstVertex();
		float	param[4];
		float value[4];
		float	pos;

		for(ii=0 ; ii<(m_PreviewLinePoints) ; ii++)
		{
			value[0]=value[1]=value[2]=value[3]=PackFloat::m_0p0 ;
			param[0] = param[3] = xds1;
				Compute( value, &(param[0]) );

			pVertY->m_x = pVertZ->m_x = pVertD->m_x =
			pVert->m_x = xds1;
			pVert->m_y = -value[0]-ywidth;
			pVertY->m_y = -value[1]-ywidth;
			pVertZ->m_y = -value[2]-ywidth;
			pVertD->m_y = -value[3]-ywidth;
			pVert->m_z = pVertY->m_z = pVertZ->m_z = pVertD->m_z = 0.0f;
			pVert++; pVertY++; pVertZ++; pVertD++;

			pVertY->m_x = pVertZ->m_x = pVertD->m_x =
			pVert->m_x = xds1;
			pVert->m_y = -value[0]+ywidth;
			pVertY->m_y = -value[1]+ywidth;
			pVertZ->m_y = -value[2]+ywidth;
			pVertD->m_y = -value[3]+ywidth;
			pVert->m_z =pVertY->m_z = pVertZ->m_z = pVertD->m_z= 0.0f;

			pVert++; pVertY++; pVertZ++; pVertD++;
			xds1+=vecx;
		}

	_pPreviewViewPort->RenderMesh(m_pPreviewLine);
	_pPreviewViewPort->RenderMesh(m_pPreviewLineY);
	_pPreviewViewPort->RenderMesh(m_pPreviewLineZ);
	_pPreviewViewPort->RenderMesh(m_pPreviewLineD);
	GetMachine()->DeleteObject3DBuffer(&m_pGridLine);
	GetMachine()->DeleteObject3DBuffer(&m_pPreviewLine);
	GetMachine()->DeleteObject3DBuffer(&m_pPreviewLineY);
	GetMachine()->DeleteObject3DBuffer(&m_pPreviewLineZ);
	GetMachine()->DeleteObject3DBuffer(&m_pPreviewLineD);
}
#endif
