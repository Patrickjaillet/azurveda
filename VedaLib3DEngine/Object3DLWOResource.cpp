/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
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
/*!
	\brief	static method passed to the list to create new elements.
	\return a new  object.
*/
BaseType *Object3DLWOResource::NewTextureReference()
{
	PackObjectReference *pnew = new PackObjectReference();
	// do something like REGISTER_MEMBER_REFERENCE
#ifdef _ENGINE_EDITABLE_
	pnew->SetInfoString("TextureRef");
	pnew->SetReferableBaseClass(Texture3D::m_Description);
#endif
	return pnew;
}
/*
	\brief	you must provide this allocator:
*/
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
/*
	\brief	you must provide this desallocator:
*/
void	lw_FreeAlloc( void	*_pAllocToClose )
{
	delete [] _pAllocToClose;
}
/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by CreateInternal().
*/
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
	// load engine init
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
	// prepare texture reference entries:
	/*while(mSer_TextureReferenceList.GetNumberOfCell()<m_Lwo.nbsurf)
	{
		mSer_TextureReferenceList.AddElement();
	}
	PackList::Cell *pCell = mSer_TextureReferenceList.GetFirstCell();
	while(pCell)
	{
		PackRefe pCell->GetManagedObject()
		pCell = pCell->GetNextBrotherCell();
	}*/
	// --------- translate LWO to vedamachine's 3D buffers ---------
	LayerObject *pLayerToBuild;
	m_pLayerObjects = pLayerToBuild = (LayerObject *)lw_CleanAlloc(sizeof(LayerObject)*m_Lwo.m_NumberOfLayers);
				//new LayerObject[m_Lwo.nblayer];
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
		// --- copy vertex base: ---
		VirtualMachine::InternalVertex *pVert =	p3DBuffer->GetFirstVertex();
		const sLwoVertex *pVertLWO =	pLayer->LWO_Vertex;
		int jj;
		for( jj=0 ; jj<pLayer->MaxNbVertex ; jj++ )
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
		// --- copy triangle base: ---
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
		// default triangle to trace: all triangle:
		p3DBuffer->SetNumberOfActiveTriangle(pLayer->m_MaxNbTriangle);
		// --- copy index texture->triangle list: ---		
		struct LwoSortedTriangleSurface*pwr,*prd;
		pLayerToBuild->m_NumberOfTextureLayer =  pLayer->m_NumberOfSurfaceUsed ;
		pLayerToBuild->m_pSurfaceSortedTriangleListIndex = pwr = (struct LwoSortedTriangleSurface*)
			lw_CleanAlloc(sizeof(struct LwoSortedTriangleSurface)*pLayer->m_NumberOfSurfaceUsed);
		if(pwr==0L) return false;
		prd=pLayer->m_pSurfaceSortedTriangleListIndex;
		for(jj=0;jj<pLayer->m_NumberOfSurfaceUsed;jj++)
		{
			pwr->LwoFileSurfaceIndex = prd->LwoFileSurfaceIndex;
			pwr->FirstTriangle = prd->FirstTriangle;
			pwr->NumberOfTriangle = prd->NumberOfTriangle;
			pwr++;
			prd++;
		}		
		pLayerToBuild++;
		pLayer++;
	} // loop by object layer.
	// free lwo loader's buffers at last.
	CloseLwo(&m_Lwo);
	return true;
	
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void	Object3DLWOResource::CloseInternal(void)
{
	if(m_pLayerObjects)
	{	int ii;
		LayerObject *pLayerToBuild = m_pLayerObjects;
		for(ii=0 ; ii<m_Lwo.m_NumberOfLayers ; ii++)
		{
			GetMachine()->DeleteObject3DBuffer( &(pLayerToBuild->m_pObject3DBuffer));
			if(pLayerToBuild->m_pSurfaceSortedTriangleListIndex)
				lw_FreeAlloc( pLayerToBuild->m_pSurfaceSortedTriangleListIndex);
			pLayerToBuild++;
		}
		//delete [] m_pLayerObjects;
		lw_FreeAlloc(m_pLayerObjects);
		m_pLayerObjects = 0L;
	}
	CloseLwo(&m_Lwo);
	//Object3DMeshVirtual::CloseInternal();
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. This is done with this method. sub classes can implement it (or not) in
			any way, to explicit current shape of an object.<br>
			Extended to draw.
	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
	\param	_pPreviewConfiguration
*/
void Object3DLWOResource::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

	// super call to set the matrix in the viewport:
	Object3DMeshVirtual::ProcessPreview(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);
	// then draw our objects:
	if(m_NbLayer==0) return;
	unsigned int shapeIndex = ((unsigned int)_frameDate) % m_NbLayer;
	
	const float rgba[4]={1.0f,1.0f,1.0f,1.0f};
	RenderObject(_frameDate,_pPreviewViewPort,rgba,shapeIndex);

}
#endif
/*!
	\brief	Process a real time computing and drawing. Can be extended.
	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pViewPort	where to render.
	\param	_shapeIndex	optional index for multiple shape objects. def to zero.
	\param _pModifierList optional ShapeModifierList pointer, to modify objects.
*/
void	Object3DLWOResource::RenderObject( double _frameDate, 
						VirtualMachine::InternalViewPort *_pViewPort,
						const float _rgba[4],
						unsigned int _shapeIndex,
						ShapeModifierList *_pModifierList )
{
	unsigned int ii;
	if(_shapeIndex>=m_NbLayer) return;
	LayerObject *pLayer = m_pLayerObjects+_shapeIndex;
	// render object by texture used:
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
