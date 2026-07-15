// SPDX-License-Identifier: LGPL-2.1-only

#include "Object3DLWOResource.h"
#include "PackFloat.h"
#include "PackObjectReference.h"
#include "Texture3D.h"
BASEOBJECT_DECLARE_CLASS( "lwo", Object3DLWOResource, Object3DMeshVirtual,"3D Object LWO File","MeshLwo3D","Objects defined by a LightWave Object file. Contains many layers");

Object3DLWOResource::Object3DLWOResource() : Object3DMeshVirtual()
	,mSer_TextureReferenceList(NewTextureReference)
	,m_pLayerObjects(0L)
{
	REGISTER_MEMBER_RESOURCE( mSer_LWOFile,"LWOFile","lwo" );
	REGISTER_MEMBER(mSer_TextureReferenceList,"TextureRef");
	m_Lwo.m_LWO_Layer=0L;
	m_Lwo.surfaces=0L;
	m_Lwo.envlp=0L;
	m_Lwo.clipimagetable=0L;
}

BaseType *Object3DLWOResource::NewTextureReference()
{
	PackObjectReference *pnew = new PackObjectReference();

#ifdef _ENGINE_EDITABLE_
	pnew->SetInfoString("TextureRef");
	pnew->SetReferableBaseClass(Texture3D::m_Description);
#endif
	return pnew;
}

void	*lw_CleanAlloc( unsigned int _bytesize )
{
	unsigned int ii;
	unsigned char *palloc2;
	unsigned char *palloc = palloc2 = new unsigned char[_bytesize];
	if(!palloc) return 0L;
	for(ii=0;ii<_bytesize;ii++)
		*palloc2++ = 0;
	return palloc;
}

void	lw_FreeAlloc( void	*_pAllocToClose )
{
	delete [] _pAllocToClose;
}

bool Object3DLWOResource::CreateInternal(void)
{
	const unsigned char *pFileChunk;
	unsigned int	fileSize;
	fileSize = mSer_LWOFile.GetByteLength();
	mSer_LWOFile.GetBinaryAtOffset(pFileChunk,fileSize);
	if(!pFileChunk)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_CANTOPENFILE();
		return false;
	}

	lwo2_error err = ReadLwo( &m_Lwo, pFileChunk, fileSize,
		ReadLwoFlag_DiscontinuousUVToNewVertex|
		ReadLwoFlag_SortTriangleBySurfaces);
	if(err != lwoerr_Ok)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR("Lightwave file format error");
		return false;
	}
	m_NbLayer = m_Lwo.m_NumberOfLayers;
	if(m_NbLayer == 0) return true;

	LayerObject *pLayerToBuild;
	m_pLayerObjects = pLayerToBuild = (LayerObject *)lw_CleanAlloc(sizeof(LayerObject)*m_Lwo.m_NumberOfLayers);

	if( !pLayerToBuild ) return false;
	unsigned int ii;
	sLwoLayer *pLayer =  m_Lwo.m_LWO_Layer;
	for(ii=0 ; ii<m_Lwo.m_NumberOfLayers ; ii++)
	{
		VirtualMachine::InternalObject3DBuffer *p3DBuffer;
		unsigned int vertexmemberFlag=VirtualMachine::bOb3D_VertexNormal|VirtualMachine::bOb3D_VertexUVMapping;
		if((pLayer->m_ColorFlag&1) !=0)vertexmemberFlag|=VirtualMachine::bOb3D_VertexRGBA;

		GetMachine()->NewObject3DBuffer( &p3DBuffer,
				pLayer->MaxNbVertex,pLayer->m_MaxNbTriangle,vertexmemberFlag);

		if(!p3DBuffer) return false;
		pLayerToBuild->m_pObject3DBuffer = p3DBuffer;

		VirtualMachine::InternalVertex *pVert =	p3DBuffer->GetFirstVertex();
		const sLwoVertex *pVertLWO =	pLayer->LWO_Vertex;
		int jj;
		for( jj=0 ; jj<(int)pLayer->MaxNbVertex ; jj++ )
		{
			pVert->m_x = pVertLWO->XI;
			pVert->m_y = -pVertLWO->YI;
			pVert->m_z = -pVertLWO->ZI;
			pVert->m_nx = pVertLWO->XNI*0.5f;
			pVert->m_ny = -pVertLWO->YNI*0.5f;
			pVert->m_nz = -pVertLWO->ZNI*0.5f;
			pVert->m_u = pVertLWO->U;
			pVert->m_v = 1.0f - pVertLWO->V;
			if((pLayer->m_ColorFlag&1) !=0)
			{
				pVert->m_ColorRGBA[0]= pVertLWO->r;
				pVert->m_ColorRGBA[1]= pVertLWO->g;
				pVert->m_ColorRGBA[2]= pVertLWO->b;
			}
			pVertLWO++;
			pVert++;
		}

		VirtualMachine::InternalTriangle *pTri = p3DBuffer->GetFirstTriangle();
		struct  LwoTriangle    *plwoTri = pLayer->m_LWO_Triangle;
		for(jj=0;jj<pLayer->m_MaxNbTriangle;jj++)
		{
			pTri->m_p0 = plwoTri->v0;
			pTri->m_p1 = plwoTri->v1;
			pTri->m_p2 = plwoTri->v2;
			pTri++;
			plwoTri++;
		}

		p3DBuffer->SetNumberOfActiveTriangle(pLayer->m_MaxNbTriangle);

		struct LwoSortedTriangleSurface*pwr,*prd;
		pLayerToBuild->m_NumberOfTextureLayer =  pLayer->m_NumberOfSurfaceUsed ;
		pLayerToBuild->m_pSurfaceSortedTriangleListIndex = pwr = (struct LwoSortedTriangleSurface*)
			lw_CleanAlloc(sizeof(struct LwoSortedTriangleSurface)*pLayer->m_NumberOfSurfaceUsed);
		if(pwr==0L) return false;
		prd=pLayer->m_pSurfaceSortedTriangleListIndex;
		for(jj=0;jj<(int)pLayer->m_NumberOfSurfaceUsed;jj++)
		{
			pwr->LwoFileSurfaceIndex = prd->LwoFileSurfaceIndex;
			pwr->FirstTriangle = prd->FirstTriangle;
			pwr->NumberOfTriangle = prd->NumberOfTriangle;
			pwr++;
			prd++;
		}
		pLayerToBuild++;
		pLayer++;
	}

	CloseLwo(&m_Lwo);
	return true;

}
#ifdef _ENGINE_EDITABLE_

void	Object3DLWOResource::CloseInternal(void)
{
	if(m_pLayerObjects)
	{	unsigned int ii;
		LayerObject *pLayerToBuild = m_pLayerObjects;
		for(ii=0 ; ii<m_Lwo.m_NumberOfLayers ; ii++)
		{
			GetMachine()->DeleteObject3DBuffer( &(pLayerToBuild->m_pObject3DBuffer));
			if(pLayerToBuild->m_pSurfaceSortedTriangleListIndex)
				lw_FreeAlloc( pLayerToBuild->m_pSurfaceSortedTriangleListIndex);
			pLayerToBuild++;
		}

		lw_FreeAlloc(m_pLayerObjects);
		m_pLayerObjects = 0L;
	}
	CloseLwo(&m_Lwo);

}
#endif
#ifdef _ENGINE_EDITABLE_

void Object3DLWOResource::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

	Object3DMeshVirtual::ProcessPreview(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);

	if(m_NbLayer==0) return;
	unsigned int shapeIndex = ((unsigned int)_frameDate) % m_NbLayer;

	const float rgba[4]={1.0f,1.0f,1.0f,1.0f};
	RenderObject(_frameDate,_pPreviewViewPort,rgba,shapeIndex);

}
#endif

void	Object3DLWOResource::RenderObject( double _frameDate,
						VirtualMachine::InternalViewPort *_pViewPort,
						const float _rgba[4],
						unsigned int _shapeIndex,
						ShapeModifierList * )
{
	unsigned int ii;
	if(_shapeIndex>=m_NbLayer) return;
	LayerObject *pLayer = m_pLayerObjects+_shapeIndex;

	struct LwoSortedTriangleSurface	*pTextureIndex = pLayer->m_pSurfaceSortedTriangleListIndex;
	VirtualMachine::InternalObject3DBuffer	*p3DBuffer = pLayer->m_pObject3DBuffer;
	PackList::Cell *pCell = mSer_TextureReferenceList.GetFirstCell();
	VirtualMachine::InternalTexture *pVMTexture;
	for(ii=0 ; ii<pLayer->m_NumberOfTextureLayer ; ii++)
	{
		pVMTexture=0L;
		if(pCell)
		{
			Texture3D *pTexture = (Texture3D *)
				(((PackObjectReference *)(pCell->GetManagedObject()))->GetObjectPointer());
			if(pTexture)
			{
				pTexture->UpdateToFrame(_frameDate);
				pVMTexture=	pTexture->GetVirtualMachineTexture();
			}
			pCell = pCell->GetNextBrotherCell();
		}
		p3DBuffer->SetNumberOfActiveTriangle(pTextureIndex->NumberOfTriangle,pTextureIndex->FirstTriangle);
		p3DBuffer->SetColor(_rgba[0],_rgba[1],_rgba[2],_rgba[3]);
		_pViewPort->RenderMesh(p3DBuffer,pVMTexture);
		pTextureIndex++;
	}

}
