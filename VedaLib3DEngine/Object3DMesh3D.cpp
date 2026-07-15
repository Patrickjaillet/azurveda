// SPDX-License-Identifier: LGPL-2.1-only

#include "Object3DMesh3D.h"
#include "PackFloat.h"

#include "lwo2reader.h"

#include <math.h>

#ifdef _ENGINE_EDITABLE_
#include <PackResource.h>
#endif

BASEOBJECT_DECLARE_CLASS( "3", Object3DMesh3D, Object3DMeshVirtual,"3D Object Mesh","Mesh3D","Object defined by a list of vertex points and a list of polygons.");

BaseType *Object3DMesh3D::NewPackFloatByte_Limits()
{
	PackFloatByte_Limits *pFloat3 = new PackFloatByte_Limits(PackFloat::vd_XYZ);

#ifdef _ENGINE_EDITABLE_
	pFloat3->SetInfoString("Vertex");
#endif
	return pFloat3 ;
};

BaseType *Object3DMesh3D::NewPolygonList(){ return new PolygonList;  }

Object3DMesh3D::Object3DMesh3D() : Object3DMeshVirtual()
	,mSer_Pivot(PackFloat::vd_XYZ)
	,mSer_Scale(PackFloat::vd_XYZ)
	,mSer_StaticVertexList( NewPackFloatByte_Limits )
	,mSer_PolygonList( NewPolygonList )
{
	REGISTER_MEMBER_PACKFLOATBYTE_0_1(mSer_NormalSplit,"NormalSplit",0.0f);
	REGISTER_MEMBER_PACKFLOAT_XYZ(mSer_Pivot,"Pivot",0.0f,0.0f,0.0f);
	REGISTER_MEMBER_PACKFLOAT_XYZ(mSer_Scale,"Scale",1.0f,1.0f,1.0f);
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_Subdivision,0L,1,1,12);
	REGISTER_MEMBER_PACKFLOATBYTE_0_1(mSer_SplineFactor,0L,0.0f);
	REGISTER_MEMBER( mSer_StaticVertexList,"VertexList" );
	REGISTER_MEMBER( mSer_PolygonList,"PolygonList" );

	BASEOBJECT_REGISTER_TOOLMETHOD( tm_Object3DMesh3D_SetAsCube,0,"Set cube","Reset the shape to a 1,1,1 cube.");
	BASEOBJECT_REGISTER_TOOLMETHOD( tm_Object3DMesh3D_LoadLWO,0,"Load LWO","Shape according to a Lightwave object");

}

BaseType *Object3DMesh3D::NewPackULong()
{
	PackULong *pULong = new PackULong();
#ifdef _ENGINE_EDITABLE_
	pULong->SetInfoString("Vertex");
#endif
	return pULong;
}

Object3DMesh3D::PolygonList::PolygonList()
 : mSer_RefList(Object3DMesh3D::NewPackULong)
{
	REGISTER_MEMBER_FLAG(mSer_Flags,0L,0,"Strips");
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_VertPerPoly,"VertPerPoly",3,3,126);
	REGISTER_MEMBER(mSer_RefList,"List");

}

bool Object3DMesh3D::CreateInternal(void)
{

	m_pObject3DBufferTable = new Object3DBufferHandler[1];
	if( !m_pObject3DBufferTable ) return false;

	const unsigned int	nbBaseVertex = mSer_StaticVertexList.GetNumberOfCell();
	unsigned int nbDiscontinuousVertex = 0;

	unsigned int	nbTriangles = 0;
	PackList::Cell *pCell = mSer_PolygonList.GetFirstCell();
	while( pCell )
	{
		PolygonList *pPolygonList = (PolygonList *)pCell->GetManagedObject();
		const unsigned int vPerPoly = pPolygonList->mSer_VertPerPoly.Get();
		unsigned int nbvref = pPolygonList->mSer_RefList.GetNumberOfCell();
		if( pPolygonList->mSer_Flags.TestFlags(ePMode_Strips) )
		{

		} else
		{
			int nbpoly = nbvref / vPerPoly;
			nbTriangles += (nbpoly * (vPerPoly-2)) ;
			nbDiscontinuousVertex +=( vPerPoly * nbpoly );
		}
		pCell = pCell->GetNextBrotherCell();
	}
	if(nbBaseVertex == 0) return true;
	if(nbTriangles == 0) return true;

	unsigned int totalNbVertex = nbBaseVertex+nbDiscontinuousVertex;
	m_pObject3DBufferTable->m_ActiveNumberOfVertex = totalNbVertex;
	unsigned int meshflag =
			VirtualMachine::bOb3D_VertexNormal|
			VirtualMachine::bOb3D_VertexUVMapping;
	if( !mSer_Flags.TestFlags(OB3DFlag_OneColor) )
		meshflag |= VirtualMachine::bOb3D_VertexRGBA ;

	GetMachine()->NewObject3DBuffer(  &(m_pObject3DBufferTable->m_pObject3DBuffer),
								totalNbVertex,nbTriangles,meshflag);

	VirtualMachine::InternalObject3DBuffer *p3DBuffer =  m_pObject3DBufferTable->m_pObject3DBuffer;
	if(!p3DBuffer) return false;
	m_CurrentNumberOf3DBuffer = 1;

#ifdef _ENGINE_EDITABLE_
	m_ObjectInfoLine.Set("NbV:");
	m_ObjectInfoLine.AddInt(nbBaseVertex);
	m_ObjectInfoLine.AddString("  nbTr:");
	m_ObjectInfoLine.AddInt(nbTriangles);
#endif

	CreateShape(0.0f,0);

	return true;
}

void Object3DMesh3D::CreateShape(float , unsigned int  )
{
	VirtualMachine::InternalObject3DBuffer *p3DBuffer =  m_pObject3DBufferTable->m_pObject3DBuffer;

	const unsigned int	nbBaseVertex = mSer_StaticVertexList.GetNumberOfCell();

	VirtualMachine::InternalVertex *pVert = p3DBuffer->GetFirstVertex();
	PackList::Cell *pCell = mSer_StaticVertexList.GetFirstCell();
	while(pCell)
	{
		PackFloatByte_Limits *pV3 = ((PackFloatByte_Limits *)pCell->GetManagedObject());
		pVert->m_x = pV3->Get(0);
		pVert->m_y = pV3->Get(1);
		pVert->m_z = pV3->Get(2);

		pVert->m_nx =
		pVert->m_ny =
		pVert->m_nz = 0.0f;
		pVert++;
		pCell = pCell->GetNextBrotherCell();
	}

	VirtualMachine::InternalTriangle *pTri = p3DBuffer->GetFirstTriangle();
	pVert = p3DBuffer->GetFirstVertex();
	pCell = mSer_PolygonList.GetFirstCell();
	unsigned int validTri=0;
	unsigned int discontinuousIndex = nbBaseVertex ;
	float fNormalSplit = mSer_NormalSplit.Get();
	const float normalLength = 0.85f;
	while( pCell )
	{
		PolygonList *pPolygonList = (PolygonList *)pCell->GetManagedObject();
		const unsigned int vPerPoly = pPolygonList->mSer_VertPerPoly.Get();
		if( pPolygonList->mSer_Flags.TestFlags(ePMode_Strips) )
		{

		} else
		{
			PackList::Cell *pPolyFirstCell = 0L;
			PackList::Cell *prCell =pPolygonList->mSer_RefList.GetFirstCell();
			unsigned int nbRef = pPolygonList->mSer_RefList.GetNumberOfCell();

			unsigned int v0,v1;
			unsigned int nbv=vPerPoly;
			float nx,ny,nz;
			while(prCell )
			{
				if(nbv==vPerPoly)
				{

					pPolyFirstCell = prCell;
					if(nbRef<vPerPoly) break;
					v0 = ((PackULong *)prCell->GetManagedObject() )->Get();
					prCell = prCell->GetNextBrotherCell();
					if(!prCell) break;
					v1 = ((PackULong *)prCell->GetManagedObject() )->Get();
					prCell = prCell->GetNextBrotherCell();
					if(!prCell) break;
					nbv=2;
					nbRef-=2;
				}
				unsigned int v2 = ((PackULong *)prCell->GetManagedObject() )->Get();
				if(nbv==2)
				{

					float xa,ya,za;
					float xb,yb,zb,dd;
					xa =  pVert[v1].m_x - pVert[v0].m_x ;
					ya =  pVert[v1].m_y - pVert[v0].m_y ;
					za =  pVert[v1].m_z - pVert[v0].m_z ;
					xb =  pVert[v2].m_x - pVert[v0].m_x ;
					yb =  pVert[v2].m_y - pVert[v0].m_y ;
					zb =  pVert[v2].m_z - pVert[v0].m_z ;
					nx =  ya * zb - za * yb ;
					ny =  za * xb - xa * zb ;
					nz =  xa * yb - ya * xb ;

					dd = (float) sqrtf( nx*nx + ny*ny + nz*nz ) ;
					if( dd == 0.0f ) dd = 0.000001f ;
					dd = normalLength / sqrtf( dd );
					nx *= dd;
					ny *= dd;
					nz *= dd;
				}

				if( fNormalSplit == 0.0f )
				{
					pTri->m_p0 = v0;
					pTri->m_p1 = v1;
					pTri->m_p2 = v2;
				} else
				{
					pTri->m_p0 = discontinuousIndex ;
					pTri->m_p1 = discontinuousIndex + nbv-1;
					pTri->m_p2 = discontinuousIndex + nbv;
				}
				validTri++;
				pTri++;
				nbv++;
				v1 = v2;
				prCell = prCell->GetNextBrotherCell();
				nbRef--;

				if(nbv==vPerPoly)
				{
					unsigned int nnbv = 0;
					while(pPolyFirstCell && nnbv<vPerPoly)
					{
						v0 = ((PackULong *)pPolyFirstCell->GetManagedObject() )->Get();
						VirtualMachine::InternalVertex *pVertB = &(pVert[v0]);

						pVertB->m_nx += nx;
						pVertB->m_ny += ny;
						pVertB->m_nz += nz;

						if( fNormalSplit != 0.0f )
						{
							VirtualMachine::InternalVertex *pVertD = &(pVert[discontinuousIndex+nnbv]);
							pVertD->m_x =  pVertB->m_x ;
							pVertD->m_y =  pVertB->m_y ;
							pVertD->m_z =  pVertB->m_z ;

							pVertD->m_nx = nx;
							pVertD->m_ny = ny;
							pVertD->m_nz = nz;

							*((VirtualMachine::InternalVertex **)&(pVertD->m_ColorRGBA[0]))
								 = pVertB;

						}
						pPolyFirstCell = pPolyFirstCell->GetNextBrotherCell();
						nnbv++;
					}
					discontinuousIndex +=vPerPoly ;
				}
			}

		}

		pCell = pCell->GetNextBrotherCell();
	}

	if(fNormalSplit == 0.0f)
	{
		unsigned int ii;
		for(ii=0; ii<nbBaseVertex ; ii++)
		{
			float nnx = pVert->m_nx;
			float nny = pVert->m_ny;
			float nnz = pVert->m_nz;
			float dd = nnx*nnx+nny*nny+nnz*nnz;
			if( dd == 0.0f ) dd = 0.000001f ;
			dd = normalLength / sqrtf(dd) ;
			pVert->m_nx = nnx * dd;
			pVert->m_ny = nny * dd;
			pVert->m_nz = nnz * dd;

			pVert->m_x = pVert->m_u = pVert->m_x- mSer_Pivot.Get(0);
			pVert->m_x *= mSer_Scale.Get(0);
			pVert->m_y = pVert->m_v = pVert->m_y- mSer_Pivot.Get(1);
			pVert->m_y *= mSer_Scale.Get(1);
			pVert->m_z = (pVert->m_z- mSer_Pivot.Get(2)) * mSer_Scale.Get(2);
			pVert++;
		}
	} else
	{
		unsigned int ii;
		pVert = pVert+(nbBaseVertex);
		const float rest = 1.0f - fNormalSplit;
		for(ii=nbBaseVertex; ii<m_pObject3DBufferTable->m_ActiveNumberOfVertex ; ii++)
		{
			float nnx = pVert->m_nx;
			float nny = pVert->m_ny;
			float nnz = pVert->m_nz;
			float dd = (nnx*nnx)+(nny*nny)+(nnz*nnz);
			if( dd == 0.0f ) dd = 0.000001f ;
			dd = normalLength /sqrtf(dd) ;
			nnx *= dd;
			nny *= dd;
			nnz *= dd;

			VirtualMachine::InternalVertex *pVertO =
				*((VirtualMachine::InternalVertex **)&(pVert->m_ColorRGBA[0])) ;
			pVert->m_ColorRGBA[0]=1.0f;
			nnx = (nnx* fNormalSplit) + (pVertO->m_nx*rest);
			nny = (nny* fNormalSplit) + (pVertO->m_ny*rest);
			nnz = (nnz* fNormalSplit) + (pVertO->m_nz*rest);

			dd = nnx*nnx+nny*nny+nnz*nnz;
			if( dd == 0.0f ) dd = 0.000001f ;
			dd = normalLength / dd ;
			nnx *= dd;
			nny *= dd;
			nnz *= dd;
			pVert->m_nx = nnx;
			pVert->m_ny = nny;
			pVert->m_nz = nnz;

			pVert->m_x = pVert->m_u = pVert->m_x- mSer_Pivot.Get(0);
			pVert->m_x *= mSer_Scale.Get(0);
			pVert->m_y = pVert->m_v = pVert->m_y- mSer_Pivot.Get(1);
			pVert->m_y *= mSer_Scale.Get(1);
			pVert->m_z = (pVert->m_z- mSer_Pivot.Get(2)) * mSer_Scale.Get(2);

			pVert++;
		}
	}
	p3DBuffer->SetNumberOfActiveTriangle(validTri);
}

#ifdef _ENGINE_EDITABLE_

void Object3DMesh3D::ExecuteToolMethod( unsigned int _MethodIDToExecute )
{
	switch(_MethodIDToExecute)
	{
		case tm_Object3DMesh3D_SetAsCube:
			ToolMethod_SetAsCube();
			break;
		case tm_Object3DMesh3D_LoadLWO:
			ToolMethod_LoadLWO();
			break;
		default:
			break;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

void Object3DMesh3D::ToolMethod_SetAsCube()
{

	mSer_StaticVertexList.DeleteAllElements();
	mSer_PolygonList.DeleteAllElements();
	static const float cubetable[]=
	{
		0.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,
		0.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		0.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f
	};
	int ii;
	const float *pFloat = &cubetable[0];
	for( ii=0 ; ii<(sizeof(cubetable)/(sizeof(float)*3)) ; ii++ )
	{
		PackList::Cell *pCell = mSer_StaticVertexList.AddElement();
		if(!pCell) return;
		PackFloatByte_Limits *pVert = ((PackFloatByte_Limits *)pCell->GetManagedObject());
		pVert->Set3f(pFloat[0],pFloat[1],pFloat[2]);
		pFloat+=3;
	}

	PackList::Cell *pPCell = mSer_PolygonList.AddElement();
	if(!pPCell) return;
	PolygonList *pPList = (PolygonList *)pPCell->GetManagedObject();
	pPList->mSer_VertPerPoly.Set(4);

	static const unsigned char CubetableQuad[]=
	{
		0,1,3,2,
		2,3,7,6,
		6,7,5,4,
		4,5,1,0,
		1,5,7,3,
		4,0,2,6
	};
	for( ii=0 ; ii<(sizeof(CubetableQuad)/sizeof(unsigned char)) ; ii++ )
	{
		PackList::Cell *pCell = pPList->mSer_RefList.AddElement();
		if(!pCell) return;
		PackULong *pVRef = ((PackULong *)pCell->GetManagedObject());
		pVRef->Set(CubetableQuad[ii]);
	}
	mSer_Pivot.Set3f(0.5f,0.5f,0.5f);
}
#endif
#ifdef _ENGINE_EDITABLE_

void Object3DMesh3D::ToolMethod_LoadLWO()
{
	sLwoObject olwo;
	char str[256];
	GetMachine()->FileRequester("Choose a LWO File",str,255);

	PackResource res;
	res.SetFile(str);
	unsigned int loaded;
	const unsigned char *pFileChunk;
	res.GetBinaryAtOffset(pFileChunk,loaded,0);
	if(!pFileChunk) return;

	lwo2_error err = ReadLwo(&olwo, pFileChunk,loaded, 0 );
	if(err != lwoerr_Ok) return;

	mSer_StaticVertexList.DeleteAllElements();
	mSer_PolygonList.DeleteAllElements();

	const sLwoLayer *pLayer = LwoGetLayer( &olwo, 0);
	if(!pLayer) goto closeend;

	{

	unsigned int ii;
	sLwoVertex *pLWOVert = pLayer->LWO_Vertex;

	float divX = pLayer->m_boundMax[0]-pLayer->m_boundMin[0];
	float divY = pLayer->m_boundMax[1]-pLayer->m_boundMin[1];
	float divZ = pLayer->m_boundMax[2]-pLayer->m_boundMin[2];
	mSer_Scale.Set3f(divX,divY,divZ);
	if(divX!=0.0f) divX = 1.0f/divX;
	if(divY!=0.0f) divY = 1.0f/divY;
	if(divZ!=0.0f) divZ = 1.0f/divZ;
	mSer_Pivot.Set3f(-pLayer->m_boundMin[0]*divX,
						(1.0f-pLayer->m_boundMin[1])*divY,
						(1.0f-pLayer->m_boundMin[2])*divZ);

	for( ii=0 ; ii<pLayer->MaxNbVertex ; ii++ )
	{
		PackList::Cell *pCell = mSer_StaticVertexList.AddElement();
		if(!pCell) goto closeend;
		PackFloatByte_Limits *pVert = ((PackFloatByte_Limits *)pCell->GetManagedObject());
		pVert->Set3f(
			(pLWOVert->XI-pLayer->m_boundMin[0]) * divX,
			1.0f-((pLWOVert->YI-pLayer->m_boundMin[1]) * divY),
			1.0f-((pLWOVert->ZI-pLayer->m_boundMin[2]) * divZ)
			);
		pLWOVert++;
	}

	const unsigned int maxvertexperpoly=16;
	PolygonList *pNGoneList[maxvertexperpoly];
	for(ii=0 ; ii<maxvertexperpoly ; ii++) pNGoneList[ii]= 0L;

	sLwoPolygon *pPolygonTable = pLayer->LWO_Polygon;
	for( ii=0 ; ii<pLayer->MaxNbPolygon ; ii++ )
	{
		if( pPolygonTable->m_NumberOfVertex <maxvertexperpoly)
		{
			PolygonList *pPList = pNGoneList[pPolygonTable->m_NumberOfVertex];

			if(!pPList)
			{
				PackList::Cell *pPCell = mSer_PolygonList.AddElement();
				if(!pPCell) break;
				pPList = (PolygonList *)pPCell->GetManagedObject();
				pNGoneList[pPolygonTable->m_NumberOfVertex] = pPList ;
				pPList->mSer_VertPerPoly.Set(pPolygonTable->m_NumberOfVertex);
			}

			unsigned int jj;
			for(jj=0; jj<pPolygonTable->m_NumberOfVertex ; jj++ )
			{
				PackList::Cell *pCell = pPList->mSer_RefList.AddElement();
				if(!pCell) break;
				PackULong *pVRef = ((PackULong *)pCell->GetManagedObject());
				pVRef->Set(pPolygonTable->V[jj].V);
			}
		}
		pPolygonTable++;
	}
	}
closeend:

	CloseLwo(&olwo);
}
#endif
