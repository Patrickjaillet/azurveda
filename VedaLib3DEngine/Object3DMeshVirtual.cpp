// SPDX-License-Identifier: LGPL-2.1-only

#include "Object3DMeshVirtual.h"
#include "Texture3D.h"
#include "ModifierObject3DVirtual.h"

BASEOBJECT_DECLARE_VIRTUALCLASS( "msh", Object3DMeshVirtual, Object3DVirtual);

Object3DMeshVirtual::Object3DMeshVirtual() : Object3DVirtual()
	,m_pObject3DBufferTable(0L)
	,m_pLastShapeModifierList(0L)
	,m_CurrentNumberOf3DBuffer(0L)
	,m_LastShapeTime(PackFloat::m_Max)
{
	REGISTER_MEMBER_FLAG( mSer_Flags ,"Flags",0,"Static.LookCam.Rend32.Rend128.OneColor" );
	REGISTER_MEMBER_REFERENCE(	mSer_TextureRef,"Texture",Texture3D::m_Description );
}

Object3DMeshVirtual::Object3DBufferHandler::Object3DBufferHandler()
: m_pObject3DBuffer(0L)
,m_pRenderTexture(0L)
{

}
#ifdef _ENGINE_EDITABLE_

void Object3DMeshVirtual::CloseInternal(void)
{

	Object3DBufferHandler *p3dBuff = m_pObject3DBufferTable ;
	if( !p3dBuff ) return;
	for(unsigned int ii=0 ; ii<m_CurrentNumberOf3DBuffer ; ii++)
	{
		GetMachine()->DeleteObject3DBuffer(&(p3dBuff->m_pObject3DBuffer));
		GetMachine()->DeleteTexture( &(p3dBuff->m_pRenderTexture) );
		p3dBuff++;
	}
	delete [] m_pObject3DBufferTable;
	m_pObject3DBufferTable = 0L;
	m_CurrentNumberOf3DBuffer = 0;

	m_LastShapeTime = PackFloat::m_Max;
}
#endif
#ifdef _ENGINE_EDITABLE_

void Object3DMeshVirtual::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

	Object3DVirtual::ProcessPreview(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);

	if(m_CurrentNumberOf3DBuffer==0) return;
	unsigned int shapeIndex = ((unsigned int)_frameDate) % m_CurrentNumberOf3DBuffer;
	const float white[4]={1.0f,1.0f,1.0f,1.0f};
	RenderObject(_frameDate,_pPreviewViewPort,white,shapeIndex);

}
#endif

void	Object3DMeshVirtual::RenderObject( double _frameDate,
								   VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex,
									ShapeModifierList *_pModifierList )
{
	float fdate = (float)_frameDate;
	if( _shapeIndex>=m_CurrentNumberOf3DBuffer) return;

	VirtualMachine::InternalObject3DBuffer *p3DBuf = m_pObject3DBufferTable[ _shapeIndex ].m_pObject3DBuffer;

	if( (fdate != m_LastShapeTime) ||
		m_pLastShapeModifierList != _pModifierList)
	{

		CreateShape(fdate,_shapeIndex);

		if(_pModifierList)
			_pModifierList->ModifyVertexList(p3DBuf->GetFirstVertex(),
					m_pObject3DBufferTable[ _shapeIndex ].m_ActiveNumberOfVertex,
					fdate);

		m_LastShapeTime = fdate;
		m_pLastShapeModifierList = _pModifierList ;
	}
	VirtualMachine::InternalTexture *pTexture =  m_pObject3DBufferTable[ _shapeIndex ].m_pRenderTexture ;
	if(!pTexture)
	{
		Texture3D *pTextureObject =(Texture3D *) mSer_TextureRef.GetObjectPointer();
		if(pTextureObject)
		{
			pTextureObject->UpdateToFrame(fdate);
			pTexture = pTextureObject->GetVirtualMachineTexture();
		}
	}
	p3DBuf->SetColor(_rgba[0], _rgba[1],_rgba[2],_rgba[3]);

	if( mSer_Flags.TestFlags(OB3DFlag_LookCam))
	{
		_pViewPort->Matrix_Push();
			_pViewPort->Matrix_LoadID( VirtualMachine::InternalViewPort::eLoadId_Rotation );
			_pViewPort->RenderMesh( p3DBuf, pTexture );
		_pViewPort->Matrix_Pop();
	} else
		_pViewPort->RenderMesh( p3DBuf, pTexture );
}

bool Object3DMeshVirtual::CreateInternal_RenderObjectToTexture(void)
{
 if(!(mSer_Flags.TestFlags(OB3DFlag_PreRender32) | mSer_Flags.TestFlags(OB3DFlag_PreRender128)) ) return true;
	int texturewidth = 6;
	if( mSer_Flags.TestFlags(OB3DFlag_PreRender128) ) texturewidth=7;

	VirtualMachine::InternalTexture *pVMTexture =0L;
	Texture3D *pTextureObject =(Texture3D *) mSer_TextureRef.GetObjectPointer();
	if(pTextureObject) pVMTexture = pTextureObject->GetVirtualMachineTexture();

	unsigned int ii;
	for(ii=0 ; ii<m_CurrentNumberOf3DBuffer ; ii++)
	{
		VirtualMachine::InternalTexture *pRenderTexture;
		VirtualMachine::InternalViewPort *pRenderViewport;

		GetMachine()->NewTexture( &pRenderTexture );
		m_pObject3DBufferTable[ii].m_pRenderTexture = pRenderTexture;
		if(!pRenderTexture) return false;

		unsigned int flags=0;
			if(pVMTexture) flags |= pVMTexture->GetRenderFlags() &
			(
			VirtualMachine::InternalTexture::itf_Filtered|
			VirtualMachine::InternalTexture::itf_Additive|
			VirtualMachine::InternalTexture::itf_substractive|
			VirtualMachine::InternalTexture::itf_doublesided
			);

		pRenderTexture->SetRenderFlags(flags|
					VirtualMachine::InternalTexture::itf_ClampTxtU|
					VirtualMachine::InternalTexture::itf_ClampTxtV

					);

		GetMachine()->NewViewPortOnTexture(&pRenderViewport,pRenderTexture,
													texturewidth,
													texturewidth,
							VirtualMachine::InternalTexture::itile_TextureMapping
													);
		if(!pRenderViewport) return false;

		BoundingBox	sBoundingBox;
		sBoundingBox.FindBoundingBox(m_pObject3DBufferTable[ii].m_pObject3DBuffer);
		float cenx=(sBoundingBox.m_x2+sBoundingBox.m_x1)*PackFloat::m_0p5 ;
		float ceny=(sBoundingBox.m_y2+sBoundingBox.m_y1)*PackFloat::m_0p5 ;

		pRenderViewport->Matrix_LoadID();
		pRenderViewport->Matrix_Scale(1.0f/(sBoundingBox.m_x2-sBoundingBox.m_x1),
									1.0f/(sBoundingBox.m_y2-sBoundingBox.m_y1),1.0f );
		pRenderViewport->Matrix_Translate(-cenx,-ceny,-0.5f);
		pRenderViewport->SetFOVLength(0.5f);

		pRenderViewport->Clear(0.0f,0.0f,0.0f);
		pRenderViewport->RenderMesh(m_pObject3DBufferTable[ii].m_pObject3DBuffer,pVMTexture);
		pRenderViewport->SwapBuffer();

		GetMachine()->DeleteViewPort( &pRenderViewport );

		GetMachine()->DeleteObject3DBuffer(  &(m_pObject3DBufferTable[ii].m_pObject3DBuffer) );

		VirtualMachine::InternalObject3DBuffer	*pbillboard;
		GetMachine()->NewObject3DBuffer(&pbillboard,4,2,VirtualMachine::bOb3D_VertexUVMapping);
		if(!pbillboard) return false;
		VirtualMachine::InternalVertex *pVert = pbillboard->GetFirstVertex();
		const float	v0 = PackFloat::m_0p0;
		const float	v1 = PackFloat::m_1p0;
		pVert->m_x = sBoundingBox.m_x1;
		pVert->m_y = sBoundingBox.m_y1;
		pVert->m_z = v0;
		pVert->m_u = v0;
		pVert->m_v = v1;
		pVert++;
		pVert->m_x = sBoundingBox.m_x2;
		pVert->m_y = sBoundingBox.m_y1;
		pVert->m_z = v0;
		pVert->m_u = v1;
		pVert->m_v = v1;
		pVert++;
		pVert->m_x = sBoundingBox.m_x2;
		pVert->m_y = sBoundingBox.m_y2;
		pVert->m_z = v0;
		pVert->m_u = v1;
		pVert->m_v = v0;
		pVert++;
		pVert->m_x = sBoundingBox.m_x1;
		pVert->m_y = sBoundingBox.m_y2;
		pVert->m_z = v0;
		pVert->m_u = v0;
		pVert->m_v = v0;

		VirtualMachine::InternalTriangle *pTriangle = pbillboard->GetFirstTriangle();
		pTriangle->m_p0 = 1;
		pTriangle->m_p1 = 2;
		pTriangle->m_p2 = 0;
		pTriangle++;
		pTriangle->m_p0 = 0;
		pTriangle->m_p1 = 2;
		pTriangle->m_p2 = 3;
		pbillboard->SetNumberOfActiveTriangle(2);
		pbillboard->CompileAsStatic();
		m_pObject3DBufferTable[ii].m_pObject3DBuffer = pbillboard;
	}

	return true;
}

void	Object3DMeshVirtual::BoundingBox::FindBoundingBox( VirtualMachine::InternalObject3DBuffer *_pOb3d )
{
	unsigned int ii,nbtr = _pOb3d->GetNumberOfActiveTriangle() ;
	VirtualMachine::InternalVertex *pVertv,*pVert = _pOb3d->GetFirstVertex();
	VirtualMachine::InternalTriangle *pTri = _pOb3d->GetFirstTriangle();
	m_x1=m_y1=m_z1 = PackFloat::m_Max;
	m_x2=m_y2=m_z2 = -m_x1;
	float xx,yy,zz;
	nbtr *= 3;
	unsigned int *pIndex= (unsigned int *)pTri;
	for(ii=0 ; ii<nbtr ; ii++)
	{
		pVertv = pVert+(*pIndex++);
		xx = pVertv->m_x ;
		if(xx>m_x2) m_x2 =xx;
		if(xx<m_x1) m_x1 =xx;
		yy = pVertv->m_y ;
		if(yy>m_y2) m_y2 =yy;
		if(yy<m_y1) m_y1 =yy;
		zz = pVertv->m_z ;
		if(zz>m_z2) m_z2 =zz;
		if(zz<m_z1) m_z1 =zz;
	}
}

void Object3DMeshVirtual::CreateInternal_FreezeStaticShape(void)
{
	unsigned int  ii;
	for(ii=0 ; ii<m_CurrentNumberOf3DBuffer ; ii++)
	{
		m_pObject3DBufferTable[ii].m_pObject3DBuffer->CompileAsStatic();
	}
}

void Object3DMeshVirtual::CreateShape(float , unsigned int  )
{

}
