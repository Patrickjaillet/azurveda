// SPDX-License-Identifier: LGPL-2.1-only

#include "Object3DVirtual.h"
#include "ModifierObject3DVirtual.h"
#include "Texture3D.h"
BASEOBJECT_DECLARE_VIRTUALCLASS( "3d", Object3DVirtual, BaseObject );

Object3DVirtual::Object3DVirtual() : BaseObject()
{
}

Object3DVirtual::ShapeModifier::ShapeModifier() : PackStruct()
,mSer_Value(PackFloat::vd_XYZ)
{
	REGISTER_MEMBER_REFERENCE(mSer_ModifierReference,"Modifier",ModifierObject3DVirtual::m_Description);
	REGISTER_MEMBER(mSer_Value ,"Value" );
}

BaseType *Object3DVirtual::NewShapeModifier()
{
	return new ShapeModifier();
}

Object3DVirtual::ShapeModifierList::ShapeModifierList() : PackList(NewShapeModifier)
{};

#ifdef _ENGINE_EDITABLE_

void Object3DVirtual::ProcessPreview(double ,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	float xx,yy,zz;
	float rotX,rotY,rotZ;
	_pPreviewConfiguration->GetPreviewPosition(xx,yy,zz);
	_pPreviewConfiguration->GetPreviewRotation(rotX,rotY,rotZ);
	yy += 0.1f;
	zz -= 1.5f;

	xx = (xx*zz*zz);
	yy = (yy*zz*zz);
	zz = (zz*zz*zz);

	_pPreviewViewPort->Matrix_LoadID();
	_pPreviewViewPort->Matrix_Translate(
					xx,
					yy,
					zz);

	_pPreviewViewPort->Matrix_Rotate(rotZ*4.0f ,0.0f,0.0f,1.0f);
	_pPreviewViewPort->Matrix_Rotate(rotY*4.0f,-1.0f,0.0f,0.0f);
	_pPreviewViewPort->Matrix_Rotate(rotX*4.0f,0.0f,-1.0f,0.0f);

	_pPreviewViewPort->SetFOVLength(0.75f);

	ProcessPreview_DrawGrid(_pPreviewViewPort);

}
#endif
#ifdef _ENGINE_EDITABLE_

void Object3DVirtual::ProcessPreview_CreateLine(VirtualMachine::InternalObject3DBuffer **_ppVertexAndPolygonBufferOut, unsigned int _nbPoint)
{
	GetMachine()->NewObject3DBuffer( _ppVertexAndPolygonBufferOut ,
									(_nbPoint)*2,
									(_nbPoint-1)*2,
									0 );
	if( !(*_ppVertexAndPolygonBufferOut)) return;

	unsigned int ii;
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

void Object3DVirtual::ProcessPreview_DrawGrid(VirtualMachine::InternalViewPort *_pPreviewViewPort )
{
	static const unsigned int gridLength_X=10;
	static const float		squareWidth_X=1.0f;
	static const unsigned int gridLength_Z=10;
	static const float	fgridLength_Z = (float)(gridLength_Z);

	VirtualMachine::InternalObject3DBuffer	*_pGridLine;
	ProcessPreview_CreateLine(&_pGridLine,2);
	if(!_pGridLine) return;
	VirtualMachine::InternalTexture *pTexture;
	GetMachine()->NewTexture(&pTexture);
	if(!pTexture)
	{
		GetMachine()->DeleteObject3DBuffer(&_pGridLine);
		return;
	}
	pTexture->SetRenderFlags(
		VirtualMachine::InternalTexture::itf_depthtest|
		VirtualMachine::InternalTexture::itf_depthwrite|
		VirtualMachine::InternalTexture::itf_doublesided
		);
	VirtualMachine::InternalVertex *pVert;
	pVert = _pGridLine->GetFirstVertex();
	_pGridLine->SetColor(0.0f,0.9f,0.125f,0.75f);
	float	width = 0.01f;

		pVert->m_x = -width;
		pVert->m_y = 0.0f;
		pVert->m_z = -fgridLength_Z*0.5f;
		pVert++;
		pVert->m_x = width;
		pVert->m_y = 0.0f;
		pVert->m_z = -fgridLength_Z*0.5f;
		pVert++;
		pVert->m_x =-width;
		pVert->m_y = 0.0f;
		pVert->m_z = fgridLength_Z*0.5f;
		pVert++;
		pVert->m_x = width;
		pVert->m_y = 0.0f;
		pVert->m_z = fgridLength_Z*0.5f;

	unsigned int xx;
	float	fx = (((float)gridLength_X)*-0.5f*squareWidth_X);
	_pPreviewViewPort->Matrix_Push();
	_pPreviewViewPort->Matrix_Translate(fx,0.0f,0.0f);
	for( xx=0 ; xx<=gridLength_X ; xx++)
	{
		_pPreviewViewPort->RenderMesh(_pGridLine,pTexture);
		_pPreviewViewPort->Matrix_Translate(squareWidth_X,0.0f,0.0f);
	}
	_pPreviewViewPort->Matrix_Pop();

	_pPreviewViewPort->Matrix_Push();
	_pPreviewViewPort->Matrix_Rotate(PackFloat::m_Pi*0.5f ,0.0f,1.0f,0.0f);
	_pPreviewViewPort->Matrix_Translate(fx,0.0f,0.0f);
	for( xx=0 ; xx<=gridLength_X ; xx++)
	{
		_pPreviewViewPort->RenderMesh(_pGridLine,pTexture);
		_pPreviewViewPort->Matrix_Translate(squareWidth_X,0.0f,0.0f);
	}
	_pPreviewViewPort->Matrix_Pop();

	_pPreviewViewPort->Matrix_Push();
	_pGridLine->SetColor(0.0f,0.9f,0.5f,0.5f);

	_pPreviewViewPort->Matrix_Rotate(PackFloat::m_Pi*0.5f ,-1.0f,0.0f,0.0f);

	_pPreviewViewPort->RenderMesh(_pGridLine,pTexture);
	_pPreviewViewPort->Matrix_Pop();
	GetMachine()->DeleteTexture(&pTexture);
	GetMachine()->DeleteObject3DBuffer(&_pGridLine);
}
#endif

void Object3DVirtual::ShapeModifierList::ModifyVertexList(VirtualMachine::InternalVertex *_pFirstVertex,const unsigned int _NumberOfVertex,float _date)
{
	PackList::Cell *pCell = GetFirstCell();
	while(pCell)
	{
		ShapeModifier *pModifierElement = (ShapeModifier *) pCell->GetManagedObject();
		ModifierObject3DVirtual *pModifier = (ModifierObject3DVirtual *) pModifierElement->mSer_ModifierReference.GetObjectPointer();
		if(pModifier) pModifier->Modify(_pFirstVertex,_NumberOfVertex,_date);
		pCell = pCell->GetNextBrotherCell();
	}
}
