/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "Object3DVirtual.h"
#include "ModifierObject3DVirtual.h"
#include "Texture3D.h"
BASEOBJECT_DECLARE_VIRTUALCLASS( "3d", Object3DVirtual, BaseObject );

Object3DVirtual::Object3DVirtual() : BaseObject()
{
}

//! \brief constructor . It registers the members.
Object3DVirtual::ShapeModifier::ShapeModifier() : PackStruct()
,mSer_Value(PackFloat::vd_XYZ)
{
	REGISTER_MEMBER_REFERENCE(mSer_ModifierReference,"Modifier",ModifierObject3DVirtual::m_Description);
	REGISTER_MEMBER(mSer_Value ,"Value" );
}
/*!
	\brief	static method passed to the list to create new elements.
	\return a new PartToPlay object.
*/
BaseType *Object3DVirtual::NewShapeModifier()
{
	return new ShapeModifier();
}

//! \brief constructor . It registers the members.
Object3DVirtual::ShapeModifierList::ShapeModifierList() : PackList(NewShapeModifier)
{};

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. This is done with this method. sub classes can implement it (or not) in
			any way, to explicit current shape of an object.<br>

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
	\param	_pPreviewConfiguration
*/
void Object3DVirtual::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	float xx,yy,zz;
	float rotX,rotY,rotZ;
	_pPreviewConfiguration->GetPreviewPosition(xx,yy,zz);
	_pPreviewConfiguration->GetPreviewRotation(rotX,rotY,rotZ);
	yy += 0.1f;
	zz -= 1.5f;
	//--- do some exp:
	xx = (xx*zz*zz);
	yy = (yy*zz*zz);
	zz = (zz*zz*zz);

	_pPreviewViewPort->Matrix_LoadID();
	_pPreviewViewPort->Matrix_Translate(
					xx,
					yy,
					zz); // camera move

	_pPreviewViewPort->Matrix_Rotate(rotZ*4.0f ,0.0f,0.0f,1.0f);
	_pPreviewViewPort->Matrix_Rotate(rotY*4.0f,-1.0f,0.0f,0.0f);
	_pPreviewViewPort->Matrix_Rotate(rotX*4.0f,0.0f,-1.0f,0.0f);
	//not needed _pPreviewViewPort->Clear();
	// this method got to be extended to be active !
	_pPreviewViewPort->SetFOVLength(0.75f);
	//_pViewPort->m_fov = 0.75f;<
	// ------- draw grid:
	//_pPreviewConfiguration->GetDrawPreferenceFlag()
	ProcessPreview_DrawGrid(_pPreviewViewPort);

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Preview Tool: Create a preview line 3D Object.
*/
void Object3DVirtual::ProcessPreview_CreateLine(VirtualMachine::InternalObject3DBuffer **_ppVertexAndPolygonBufferOut, unsigned int _nbPoint)
{
	GetMachine()->NewObject3DBuffer( _ppVertexAndPolygonBufferOut ,
									(_nbPoint)*2,
									(_nbPoint-1)*2,
									0 );// num vertex,num triangles, flat no UV.
	if( !(*_ppVertexAndPolygonBufferOut)) return;

	VirtualMachine::InternalVertex *pVert = (*_ppVertexAndPolygonBufferOut)->GetFirstVertex();
	unsigned int ii;
	VirtualMachine::InternalTriangle *pTriangle = (*_ppVertexAndPolygonBufferOut)->GetFirstTriangle();
	// polygons are always linked to vertex the same way:
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
/*!
	\brief	tool for ProcessPreview.
*/
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
	//if(ycamera<0) width =-width;
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

	// draw a Z/X plane grid:
	// line X:
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
	// line Z:
	//float	fx = (((float)gridLength_X)*-0.5f*squareWidth_X);
	_pPreviewViewPort->Matrix_Push();
	_pPreviewViewPort->Matrix_Rotate(PackFloat::m_Pi*0.5f ,0.0f,1.0f,0.0f);
	_pPreviewViewPort->Matrix_Translate(fx,0.0f,0.0f);
	for( xx=0 ; xx<=gridLength_X ; xx++)
	{
		_pPreviewViewPort->RenderMesh(_pGridLine,pTexture);
		_pPreviewViewPort->Matrix_Translate(squareWidth_X,0.0f,0.0f);
	}
	_pPreviewViewPort->Matrix_Pop();
	// render Y axis:
	_pPreviewViewPort->Matrix_Push();
	_pGridLine->SetColor(0.0f,0.9f,0.5f,0.5f);
	//if(zcamera>0.0f)
	_pPreviewViewPort->Matrix_Rotate(PackFloat::m_Pi*0.5f ,-1.0f,0.0f,0.0f);
	
	_pPreviewViewPort->RenderMesh(_pGridLine,pTexture);
	_pPreviewViewPort->Matrix_Pop();
	GetMachine()->DeleteTexture(&pTexture);
	GetMachine()->DeleteObject3DBuffer(&_pGridLine);
}
#endif
/*
	\brief	Apply the Modifier from mSer_ModifierList To a VertexList.
	\param	_pFirstVertex	pointer to the first.
	\param	_NumberOfVertex	number of vertex in the table.
	\param	_date In Seconds.
*/
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
