// SPDX-License-Identifier: LGPL-2.1-only

#include    <stdlib.h>
#include    <string.h>
#include    <math.h>
#ifndef sqrtf
#define	sqrtf sqrt
#endif
#include    "lwo2reader.h"

struct RememberVMAPChunk
{
	struct RememberVMAPChunk	*pNext;
	const char					*pVMAPName;
    sLwoObject *lwo;
    sLwoLayer *curlayer;
    int offset;
    int endOffset;
};

static float readFloat(sLwoObject *lwo,int ofs);
static int readInt(sLwoObject *lwo, int ofs);
static int passLWOString(sLwoObject *lwo, int ofs);
static int passLWOVX(sLwoObject *lwo, int ofs);
static char *readLWOString(sLwoObject *lwo, int ofs);
static int readLWOVX(sLwoObject *lwo, int ofs);
static int readShort(sLwoObject *lwo, int ofs);

static  void    initNormal( sLwoPolygon *lwf, sLwoVertex  *vv );
static  void    initVertexNormals( sLwoLayer *curlayer );

lwo2_error Read_TAGS_LWO2(sLwoObject *lwo,int offset,int endOffset  );
lwo2_error Read_PNTS_LWO(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  );
lwo2_error Read_POLS_LWOB(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  );
lwo2_error Read_POLS_LWO2(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  );
lwo2_error Read_VMAP_LWO2(sLwoObject *lwo,sLwoLayer *curlayer,int offset,int endOffset, int prepareis0  );
lwo2_error Read_VMAD_LWO2(sLwoObject *lwo,sLwoLayer *curlayer,unsigned int offset,unsigned int endOffset, struct LwTexture *texturetoapply );
lwo2_error Read_PTAG_LWO2(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  );
lwo2_error Read_SURF_LWO2( sLwoObject *lwo,
                    int ofs,
                    int endoffset
                    );
lwo2_error Read_SURF_LWO2sub(  sLwoObject *lwo,
                        int offset ,
                        int endofs,
                        struct LwTexture *buildme ,
                        struct LwTexture *father
                        );
lwo2_error Read_BLOK_LWO2SURF(  sLwoObject *lwo,
                        int offset ,
                        int endofs,
                        struct LwTexture *buildme ,
                        struct LwTexture *father );

lwo2_error Read_CLIP_LWO2( sLwoObject *lwo,
                    int offset,
                    int endofs,
                    struct  LwoClip *lwc
                    );
lwo2_error Read_SRFS_LWOB(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  );

char    *searchClip( sLwoObject *lwo,   int rr );

lwo2_error	ReadLwo(sLwoObject *_pLwoObject, const unsigned char *_pLwoFileChunk, unsigned int _lwoFileByteLength, const unsigned int _flags )
{

	{ int ii;
		unsigned char *pf = (unsigned char *)_pLwoObject;
		for(ii=0; ii<sizeof(sLwoObject) ; ii++) *(pf++)=0;
	}
	_pLwoObject->m_pMainChunk	= _pLwoFileChunk ;
    _pLwoObject->m_mainsize		= _lwoFileByteLength ;

	{
		int ii = readInt(_pLwoObject,0);
		if( ii != 0x464F524D  )
		{
			return(lwoerr_FileProblem);
		}

		ii= readInt(_pLwoObject, 8 ) ;
		switch( ii )
		{
			case 0x4C574F32:
				_pLwoObject->LWO2_true = -1;
			break;
			case 0x4C574F42:
				_pLwoObject->LWO2_true = 0;
			break;
			default:
				return lwoerr_FileProblem;
			break;
		}
	}

	{
		unsigned int offset=12;
        while (offset<_lwoFileByteLength)
        {
            switch( readInt(_pLwoObject,offset) )
			{
			case (('L'<<24)|('A'<<16)|('Y'<<8)|'R'): _pLwoObject->m_NumberOfLayers++; break;
			case (('E'<<24)|('N'<<16)|('V'<<8)|'L'): _pLwoObject->nbenvlp++; break;
			case (('C'<<24)|('L'<<16)|('I'<<8)|'P'): _pLwoObject->nbclip++; break;
			default: break;
			}
            offset +=( 8 + readInt(_pLwoObject,offset+4));
        }
	}

	{
		unsigned int ii;
		int nbcurrentLayer=-1;
		unsigned int offset=12;
        sLwoLayer    *pcurrentlayer;
        struct  LwoClip		*plwclip=NULL;
	    if( _pLwoObject->m_NumberOfLayers == 0 )
		{
			_pLwoObject->m_NumberOfLayers=1;
			nbcurrentLayer = 0;
		}
        _pLwoObject->m_LWO_Layer = (sLwoLayer *)lw_CleanAlloc( sizeof( sLwoLayer ) * _pLwoObject->m_NumberOfLayers );
        if( !(_pLwoObject->m_LWO_Layer) ){ return(lwoerr_AllocFailed); }
        pcurrentlayer = _pLwoObject->m_LWO_Layer ;

        for(ii=0 ; ii<_pLwoObject->m_NumberOfLayers ; ii++ )
        {
            _pLwoObject->m_LWO_Layer[ii].LWO_file     = _pLwoObject;
        }

        if( _pLwoObject->nbclip > 0 )
        {
			_pLwoObject->clipimagetable = plwclip = (struct LwoClip *) lw_CleanAlloc( sizeof( struct LwoClip ) * _pLwoObject->nbclip );
            if( !plwclip ) { CloseLwo(_pLwoObject); return(lwoerr_AllocFailed); }
        }

        if( _pLwoObject->nbenvlp > 0 )
        {
            _pLwoObject->envlp = (struct LwoIndexedEnvelope *)lw_CleanAlloc( sizeof( struct LwoIndexedEnvelope) * _pLwoObject->nbenvlp );
			if( !(_pLwoObject->envlp)){CloseLwo(_pLwoObject); return(lwoerr_AllocFailed); };
        }

		while (offset<_lwoFileByteLength)
        {
			lwo2_error	error = lwoerr_Ok;
			int endOffsetL2 = offset +  8 + readInt(_pLwoObject,offset+4);
			switch ( readInt(_pLwoObject,offset) )
            {
                case 1413564243:
					error = Read_TAGS_LWO2(_pLwoObject,offset+8, endOffsetL2 );
                break;

                case 1279351122:
				{int ofs;

                    nbcurrentLayer++;
                    pcurrentlayer = &(_pLwoObject->m_LWO_Layer[nbcurrentLayer]) ;

                    pcurrentlayer->LWO_file = _pLwoObject;
                    pcurrentlayer->m_LayerNumber = readShort(_pLwoObject,offset+8);
                    pcurrentlayer->flags = readShort(_pLwoObject,offset+8+2);

					pcurrentlayer->objectPos.T1X = readFloat(_pLwoObject,offset+8+4);
                    pcurrentlayer->objectPos.T1Y = readFloat(_pLwoObject,offset+8+8);
                    pcurrentlayer->objectPos.T1Z = readFloat(_pLwoObject,offset+8+12);

                    pcurrentlayer->name = readLWOString(_pLwoObject, offset + 8 + 16 );
#ifdef  LWDBG

#endif
                    ofs =  passLWOString(_pLwoObject,offset+8+16);
                    if (ofs < endOffsetL2) { pcurrentlayer->parent = readShort(_pLwoObject,ofs); }

				}break;

                case 1347310675:
                    error = Read_PNTS_LWO(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2 );
                break;

                case 1347374163:
                    if( _pLwoObject->LWO2_true )
                        error = Read_POLS_LWO2(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2 );
                    else
                        error = Read_POLS_LWOB(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2 );
                break;

                case 0x564D4150:
                    error = Read_VMAP_LWO2(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2,0 );
                break;

                case 0x564D4144:
                    error = Read_VMAD_LWO2(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2,NULL );
                break;

                case 1347699015:

                    error = Read_PTAG_LWO2(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2 );
                break;

                case (('S'<<24)|('R'<<16)|('F'<<8)|'S'):
                    error = Read_SRFS_LWOB(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2 );
                break;

                case 0x434C4950:

                    if( plwclip )
                    {
                        error = Read_CLIP_LWO2( _pLwoObject,offset+8,endOffsetL2, plwclip  );
                        plwclip++;
                    }
                break;

                case 1398100550:

                    error = Read_SURF_LWO2( _pLwoObject, offset+8,endOffsetL2  );
					#ifdef  LWDBG
						printf("        surf returned:%d\n", res );
					#endif
                break;

			}

			if(error != lwoerr_Ok)
			{
				CloseLwo(_pLwoObject);
				return error;
			}
			offset=endOffsetL2;
		}
	}

	{
		unsigned int iii,jjj,kkk,nbl;
        sLwoLayer    *pcurrentlayer;
		sLwoPolygon    *Face;
		struct  LwTexture  *srf;

		pcurrentlayer = _pLwoObject->m_LWO_Layer ;
		for( nbl=0 ; nbl<_pLwoObject->m_NumberOfLayers ; nbl++  )
		{
			srf = _pLwoObject->surfaces ;
			for(iii=0 ; iii< _pLwoObject->nbsurf ; iii++ )
			{

				if( srf->txuv_used != NULL )
				{
					struct RememberVMAPChunk *pVMAP=NULL;
					struct RememberVMAPChunk *pv = pcurrentlayer->m_pFirstVMAP;
					while(pv)
					{
						if( strcmp(pv->pVMAPName,srf->txuv_used )==0)pVMAP=pv;
						pv = pv->pNext;
					}
					if( pVMAP != NULL  )
					{

						lwo2_error	error = Read_VMAP_LWO2(_pLwoObject,pcurrentlayer,pVMAP->offset, pVMAP->endOffset, 1 );
						if( error != lwoerr_Ok )
						{
							CloseLwo(_pLwoObject);
							return( error );
						}

						Face = pcurrentlayer->LWO_Polygon;
						for(jjj=0 ; jjj< pcurrentlayer->MaxNbPolygon ;jjj++)
						{
							if( Face->surface == srf )
							{
								for(kkk=0 ; kkk<Face->m_NumberOfVertex ; kkk++)
								{
									Face->V[kkk].tU = pcurrentlayer->LWO_Vertex[  Face->V[kkk].V ].U;
									Face->V[kkk].tV = pcurrentlayer->LWO_Vertex[  Face->V[kkk].V ].V;
								}
							}
							Face++;
						};

						pVMAP=0L;
						pv = pcurrentlayer->m_pFirstVMAD;
						while(pv)
						{
							if( strcmp(pv->pVMAPName,srf->txuv_used )==0) pVMAP=pv;
							pv = pv->pNext;
						}
						if( pVMAP != NULL )
						{
							Read_VMAD_LWO2(_pLwoObject,pcurrentlayer,pVMAP->offset, pVMAP->endOffset,srf );
						}
					}

			}
			srf++;
		}

		{
		struct RememberVMAPChunk *pclose=pcurrentlayer->m_pFirstVMAP;
		while(pclose)
		{
			struct RememberVMAPChunk *pnexttoclose = pclose->pNext;
			lw_FreeAlloc(pclose);
			pclose = pnexttoclose;
		}
		pcurrentlayer->m_pFirstVMAP = 0L;
		pclose=pcurrentlayer->m_pFirstVMAD;
		while(pclose)
		{
			struct RememberVMAPChunk *pnexttoclose = pclose->pNext;
			lw_FreeAlloc(pclose);
			pclose = pnexttoclose;
		}
		pcurrentlayer->m_pFirstVMAD = 0L;
		}

		if( ((_flags& ReadLwoFlag_DiscontinuousUVToNewVertex )!=0) &&
			pcurrentlayer->NumberOfDiscontinousVertexToAdd>0)
		{
			unsigned int ii;
			unsigned int nbtotalvertex = pcurrentlayer->MaxNbVertex + pcurrentlayer->NumberOfDiscontinousVertexToAdd;
			sLwoVertex *pnewVertex = (sLwoVertex *)lw_CleanAlloc( sizeof( sLwoVertex  ) * nbtotalvertex);
			sLwoVertex *pnewVertex2 = pnewVertex;
			sLwoVertex *pOldVertex = pcurrentlayer->LWO_Vertex;
			if(pnewVertex == NULL )
			{
				CloseLwo(_pLwoObject);
				return(lwoerr_AllocFailed);
			}

			for(ii=0 ; ii<pcurrentlayer->MaxNbVertex ; ii++)
			{
				pnewVertex2->XI = pOldVertex->XI ;
				pnewVertex2->YI = pOldVertex->YI ;
				pnewVertex2->ZI = pOldVertex->ZI ;
				pnewVertex2->r = pOldVertex->r ;
				pnewVertex2->g = pOldVertex->g ;
				pnewVertex2->b = pOldVertex->b ;
				pnewVertex2->XNI = pOldVertex->XNI ;
				pnewVertex2->YNI = pOldVertex->YNI ;
				pnewVertex2->ZNI = pOldVertex->ZNI ;
				pnewVertex2->U = pOldVertex->U ;
				pnewVertex2->V = pOldVertex->V ;
				if(pOldVertex->FirstDiscontinuousUse)
				{
				    sLwoPolyToVertex *nextlink=pOldVertex->FirstDiscontinuousUse;
					while(nextlink)
					{
						if( nextlink->DiscontinuousMappingOffset>0)
						{
							unsigned int newindex = pcurrentlayer->MaxNbVertex+(nextlink->DiscontinuousMappingOffset-1);
							sLwoVertex *pnewVertex4 =
								&(pnewVertex[newindex]);
							pnewVertex4->XI = pOldVertex->XI ;
							pnewVertex4->YI = pOldVertex->YI ;
							pnewVertex4->ZI = pOldVertex->ZI ;
							pnewVertex4->r = pOldVertex->r ;
							pnewVertex4->g = pOldVertex->g ;
							pnewVertex4->b = pOldVertex->b ;
							pnewVertex4->XNI = pOldVertex->XNI ;
							pnewVertex4->YNI = pOldVertex->YNI ;
							pnewVertex4->ZNI = pOldVertex->ZNI ;

							pnewVertex4->U = nextlink->tU ;
							pnewVertex4->V = nextlink->tV ;
							nextlink->V = newindex;
						}
						nextlink = nextlink->NextDiscontinuousUse ;
					}
				}
				pnewVertex2++;
				pOldVertex++;
			}
			lw_FreeAlloc(pcurrentlayer->LWO_Vertex);
			pcurrentlayer->LWO_Vertex = pnewVertex ;
			pcurrentlayer->MaxNbVertex += pcurrentlayer->NumberOfDiscontinousVertexToAdd;
			pcurrentlayer->NumberOfDiscontinousVertexToAdd = 0;
		}
		if((_flags & ReadLwoFlag_SortTriangleBySurfaces)!=0)
		{
			unsigned int ii,jj;
			unsigned int nbTotalTriangle=0;
			sLwoPolygon *pFace;
			struct  LwoTriangle *pTriangle;
			struct LwoSortedTriangleSurface	*pSurfaceIndex;

			unsigned int *pSurfaceUseCount = (unsigned int *)lw_CleanAlloc(_pLwoObject->nbsurf*sizeof(unsigned int));
			if(!pSurfaceUseCount)
			{
				CloseLwo(_pLwoObject);
				return(lwoerr_AllocFailed);
			}
			for(ii=0;ii<_pLwoObject->nbsurf;ii++)
				pSurfaceUseCount[ii]=0;
			pFace = pcurrentlayer->LWO_Polygon;
			for(ii=0;ii<pcurrentlayer->MaxNbPolygon;ii++)
			{
				if(pFace->m_NumberOfVertex>2 && pFace->surface)
				{
					pSurfaceUseCount[pFace->surface->IndexInFile]+= pFace->m_NumberOfVertex-2;
					nbTotalTriangle += pFace->m_NumberOfVertex-2;
				}
				pFace++;
			}
			if(nbTotalTriangle!=0)
			{
				pcurrentlayer->m_LWO_Triangle = pTriangle = (struct LwoTriangle *)lw_CleanAlloc(nbTotalTriangle*sizeof(struct  LwoTriangle));
				if(pTriangle==0L)
				{
					lw_FreeAlloc(pSurfaceUseCount);
					CloseLwo(_pLwoObject);
					return(lwoerr_AllocFailed);
				}
				pcurrentlayer->m_MaxNbTriangle = nbTotalTriangle;

				jj=0;
				for(ii=0;ii<_pLwoObject->nbsurf;ii++)
					if(pSurfaceUseCount[ii] !=0) jj++;
				if(jj==0)
				{
					lw_FreeAlloc(pSurfaceUseCount);
					CloseLwo(_pLwoObject);
					return(lwoerr_FileProblem);
				}
				pcurrentlayer->m_pSurfaceSortedTriangleListIndex = pSurfaceIndex =
					lw_CleanAlloc( sizeof(struct LwoSortedTriangleSurface)*	jj);
				pcurrentlayer->m_NumberOfSurfaceUsed = jj;
				if(pSurfaceIndex == 0L)
				{
					lw_FreeAlloc(pSurfaceUseCount);
					CloseLwo(_pLwoObject);
					return(lwoerr_AllocFailed);
				}

				jj=0;
				for(ii=0;ii<_pLwoObject->nbsurf;ii++)
				{
					unsigned int prev;
					prev = pSurfaceUseCount[ii];
					if(prev !=0)
					{
						pSurfaceIndex->LwoFileSurfaceIndex = ii;
						pSurfaceIndex->FirstTriangle = jj;
						pSurfaceIndex->NumberOfTriangle = prev;
						pSurfaceIndex++;
					}
					pSurfaceUseCount[ii]=jj;
					jj+=prev;
				}
				pFace = pcurrentlayer->LWO_Polygon;
				for(ii=0;ii<pcurrentlayer->MaxNbPolygon;ii++)
				{
					if(pFace->m_NumberOfVertex>2 && pFace->surface)
					{
						unsigned int split=2;
						jj = pSurfaceUseCount[pFace->surface->IndexInFile];
						while(split<pFace->m_NumberOfVertex)
						{
							pTriangle[jj].v0 = pFace->V[0].V;
							pTriangle[jj].v1 = pFace->V[split-1].V;
							pTriangle[jj].v2 = pFace->V[split].V;
							jj++;
							split++;
						}
						pSurfaceUseCount[pFace->surface->IndexInFile]=jj;
					}
					pFace++;
				}
			}

			lw_FreeAlloc(pSurfaceUseCount);
		}
		pcurrentlayer++;
	}

	}

	return lwoerr_Ok;
}

void	CloseLwo(sLwoObject *_pLwoObject)
{
	unsigned int ii;
    sLwoLayer    *curlayer;

    curlayer = _pLwoObject->m_LWO_Layer ;
	if(curlayer)
    for(ii=0 ; ii<_pLwoObject->m_NumberOfLayers  ; ii++ )
    {
		struct RememberVMAPChunk *pclose=curlayer->m_pFirstVMAP;
		while(pclose)
		{
			struct RememberVMAPChunk *pnexttoclose = pclose->pNext;
			lw_FreeAlloc(pclose);
			pclose = pnexttoclose;
		}
		pclose=curlayer->m_pFirstVMAD;
		while(pclose)
		{
			struct RememberVMAPChunk *pnexttoclose = pclose->pNext;
			lw_FreeAlloc(pclose);
			pclose = pnexttoclose;
		}

        if( curlayer->LWO_Polygon != NULL )
        {
           lw_FreeAlloc( curlayer->LWO_Polygon);
        }
        if( curlayer->LWO_Vertex != NULL )
        {
            lw_FreeAlloc( curlayer->LWO_Vertex );
        }
		if( curlayer->m_LWO_Triangle != NULL )
        {
            lw_FreeAlloc( curlayer->m_LWO_Triangle );
        }
		if( curlayer->m_pSurfaceSortedTriangleListIndex != NULL )
        {
            lw_FreeAlloc( curlayer->m_pSurfaceSortedTriangleListIndex );
        }
        curlayer++;
    }

    if( _pLwoObject->surfaces != NULL )
    {
        lw_FreeAlloc( _pLwoObject->surfaces);
        _pLwoObject->surfaces = NULL;
    }
    if( _pLwoObject->clipimagetable != NULL )
    {
        lw_FreeAlloc( _pLwoObject->clipimagetable );
        _pLwoObject->clipimagetable = NULL;
    }

    if( _pLwoObject->m_LWO_Layer != NULL )
    {
        lw_FreeAlloc( _pLwoObject->m_LWO_Layer );
        _pLwoObject->m_LWO_Layer = NULL;
    }

}

static float readFloat(sLwoObject *lwo,int ofs)
{
        float   fi;
        int     *ip;
        ip = (int *)&fi;
        *(ip) = readInt(lwo,ofs);
        return fi;
}

static int readInt(sLwoObject *lwo, int ofs)
{
	const unsigned char *mm = lwo->m_pMainChunk ;
    return(
            (((int)mm[ofs]) <<24) |
        ( (int)(mm[ofs+1] )<<16) |
        ( (int)(mm[ofs+2] )<<8) |
        ((int)(mm[ofs+3]))
        );
}

static int passLWOString(sLwoObject *lwo, int ofs)
{
        while ((lwo->m_pMainChunk[ofs])!=0) {ofs++;}
        ofs++;
        if ((ofs & 0x00000001) == 1) {ofs++;}
        return ofs;
}

static int passLWOVX(sLwoObject *lwo, int ofs)
{
        if (lwo->m_pMainChunk[ofs] != 0x000000ff ) { return( 2 ); } else { return ( 4 );}
}

static char *readLWOString(sLwoObject *lwo, int ofs)
{

    return( (char *)&(lwo->m_pMainChunk[ofs])  );

}
static int readLWOVX(sLwoObject *lwo, int ofs)
{
        if (lwo->m_pMainChunk[ofs] != 0xff )
        {
            return(     (
                    (  ( (int)lwo->m_pMainChunk[ofs] )& 0x000000ff ) <<8
                    ) |

                    (lwo->m_pMainChunk[ofs+1]& 0x000000ff)
                  );
        } else
        {
            return (

                    ((int)lwo->m_pMainChunk[ofs+1] <<16)|
                    ((int)lwo->m_pMainChunk[ofs+2] <<8)|
                    ((int)lwo->m_pMainChunk[ofs+3] )
                    );
        }
}
static int readShort(sLwoObject *lwo, int ofs)
{
    int   aa;
    aa = (  (lwo->m_pMainChunk[ofs]<<8) & 0x0000ff00) |(lwo->m_pMainChunk[ofs+1]& 0x000000ff) ;
    return ( aa );
}

lwo2_error Read_TAGS_LWO2(sLwoObject *lwo,int offset,int endOffset  )
{
    int ii,ofs;
    struct LwTexture *txt;

    ii=0;
    ofs= offset ;
    while( ofs < endOffset )
    {
         ii++;
         ofs = passLWOString(lwo,ofs);
    }
    lwo->nbsurf=ii;
#ifdef  LWDBG
    printf("    nbtxt:%d\n    ",ii);
#endif
    if( ii==0 ) return(lwoerr_Ok);
	lwo->surfaces = lw_CleanAlloc( sizeof(struct LwTexture) * ii  );
    if( lwo->surfaces == NULL  ) return(lwoerr_AllocFailed);

     ofs =  offset;
     ii=0;
     while( ofs < endOffset )
     {

            txt = &( lwo->surfaces[ ii ]  );
			txt->IndexInFile = ii;
            txt->name = readLWOString( lwo, ofs );
            txt->txuv_used = NULL;

			txt->RGBAcolor[3] = 1.0f;
            txt->mapimage       =
            txt->bumpimage      =
            txt->reflectimage   = NULL;
            txt->additiveRate   = 0;
			txt->typebits = TBit_MIPMap ;

#ifdef  LWDBG
    printf(":%s:",lwo->surfaces[ ii ].name  );
#endif
         ii++;
         ofs = passLWOString(lwo,ofs);
     }
#ifdef  LWDBG
    printf("\n");
#endif
    return(lwoerr_Ok);
}

lwo2_error Read_PNTS_LWO(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  )
{
	int  ii,jj;
	sLwoVertex    *vv;
	float   maxv =3.402823466e+38F;
	float   minx,miny,minz;
	float   maxx,maxy,maxz;

        ii = (endOffset - offset) / 12;
        curlayer->MaxNbVertex = ii;
#ifdef  LWDBG
    printf("    nbpts:%d\n",ii);
#endif
        if(ii==0) return(lwoerr_Ok);
		curlayer->LWO_Vertex = (sLwoVertex *)lw_CleanAlloc( sizeof( sLwoVertex  ) * ii);
        if( curlayer->LWO_Vertex == NULL ) return(lwoerr_AllocFailed);
        minx=maxv;
        miny=maxv;
        minz=maxv;

        maxx=-maxv;
        maxy=-maxv;
        maxz=-maxv;

        vv = curlayer->LWO_Vertex ;
        for (jj=0 ; jj<ii ; jj++)
        {
			vv->XI = readFloat(lwo,offset) - curlayer->objectPos.T1X;

            offset+=4;
            vv->YI = readFloat(lwo,offset) - curlayer->objectPos.T1Y;

            offset+=4;
            vv->ZI = readFloat(lwo,offset) - curlayer->objectPos.T1Z;

            offset+=4;

            if( vv->XI < minx ) minx = vv->XI;
            if( vv->YI < miny ) miny = vv->YI;
            if( vv->ZI < minz ) minz = vv->ZI;

            if( vv->XI > maxx ) maxx = vv->XI;
            if( vv->YI > maxy ) maxy = vv->YI;
            if( vv->ZI > maxz ) maxz = vv->ZI;

			vv->r = vv->g = vv->b = 1.0f;

            vv++;
        }

        curlayer->m_boundMin[0] = minx ;
        curlayer->m_boundMin[1] = miny ;
        curlayer->m_boundMin[2] = minz ;

        curlayer->m_boundMax[0] = maxx ;
        curlayer->m_boundMax[1] = maxy ;
        curlayer->m_boundMax[2] = maxz ;

    return(lwoerr_Ok);
}

lwo2_error Read_POLS_LWOB(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  )
{
    int id2,ii,kk,jj;
    unsigned int    allocsize=0;
    sLwoPolyToVertex *Vstart;

    id2 = offset;
    ii=0;
    while( id2< endOffset)
    {
        kk= readShort(lwo,id2) ;
        id2 = id2 + ( kk *2   )+2;
        allocsize += ( kk * sizeof(sLwoPolyToVertex) );
        kk= readShort(lwo,id2) ;
        id2+=2;
        if(kk<0) id2+=2;
        ii++;
    }
    curlayer->MaxNbPolygon = ii;
#ifdef  LWDBG
    printf("    LWOB nbFACE:%d\n",ii);
#endif
    if( ii ==0 ) return(lwoerr_Ok);
    allocsize += ( ii * sizeof(sLwoPolygon) ) ;

	curlayer->LWO_Polygon = (sLwoPolygon *)lw_CleanAlloc( allocsize );
    if( curlayer->LWO_Polygon ==NULL ) return(lwoerr_AllocFailed);

    Vstart = (sLwoPolyToVertex *) &(curlayer->LWO_Polygon[ii]) ;

    for( jj=0 ; jj<ii ; jj++)
    {
        kk= readShort(lwo,offset) ;

        curlayer->LWO_Polygon[jj].m_NumberOfVertex = kk;
        curlayer->LWO_Polygon[jj].V = Vstart;
        curlayer->LWO_Polygon[jj].surface = &( lwo->surfaces[ ii ]  );

        offset+=2;
        for( id2=0 ; id2 < kk ; id2++ )
        {
            Vstart->V = readShort(lwo,offset);
            Vstart->tU = curlayer->LWO_Vertex[ Vstart->V ].U;
            Vstart->tV = curlayer->LWO_Vertex[ Vstart->V ].V;

            offset+=2;
            Vstart++;
        }

        initNormal( &(curlayer->LWO_Polygon[jj]),
                            curlayer->LWO_Vertex );

        kk= readShort(lwo,offset) ;
        offset+=2;
        if(kk<0)
        {
            kk=-kk;
            offset+=2;
        }
        curlayer->LWO_Polygon[jj].surface = &( lwo->surfaces[ kk-1 ]  );

    }
    initVertexNormals( curlayer  );

#ifdef  LWDBG
    printf("    pols end ok\n");
#endif

    return(lwoerr_Ok);
}

static  void    initNormal( sLwoPolygon *lwf, sLwoVertex  *vv )
{
	float   xa,ya,za;
	float   xb,yb,zb;
	float   xn,yn,zn;
	float   dd;
	sLwoVertex  *aa,*bb,*cc;
	int   nb = lwf->m_NumberOfVertex -1;
	if( lwf->m_NumberOfVertex <3 ) return;

	aa = &(vv[ lwf->V[0].V]);
	bb = &(vv[ lwf->V[1].V]);
	cc = &(vv[ lwf->V[nb].V]);

	xa =  bb->XI - aa->XI ;
	ya =  bb->YI - aa->YI ;
	za =  bb->ZI - aa->ZI ;

	xb =  cc->XI - aa->XI ;
	yb =  cc->YI - aa->YI ;
	zb =  cc->ZI - aa->ZI ;

	xn =    ya * zb - za * yb ;
	yn =    za * xb - xa * zb  ;
	zn =    xa * yb - ya * xb ;
	dd = (float) sqrtf( xn*xn + yn*yn + zn*zn ) ;
	if( dd == 0.0f ) dd = 0.0000000001f ;
	dd = 1.0f / dd ;
	lwf->XN = xn * dd;
	lwf->YN = yn *dd;
	lwf->ZN = zn *dd;
}

static void initVertexNormals( sLwoLayer *curlayer ) {
    unsigned int ii,jj;
    float   xx,yy,zz,nn;
    sLwoPolygon     *ff;
    sLwoVertex   *vv,*vv2;

	for( ii=0 ; ii<curlayer->MaxNbVertex ; ii++ )
    {
        vv = &(curlayer->LWO_Vertex[ii]) ;
        vv->XNI =
        vv->YNI =
        vv->ZNI =0.0f;
    }

    vv = curlayer->LWO_Vertex ;
	for( ii=0 ; ii<curlayer->MaxNbPolygon ; ii++ )
    {
        ff = &(curlayer->LWO_Polygon[ ii ]);

        xx = ff->XN;
        yy = ff->YN;
        zz = ff->ZN;

        for( jj=0 ; jj< ff->m_NumberOfVertex; jj++ )
        {
            vv2 = &vv[ ff->V[jj].V ] ;
            vv2->XNI += xx;
            vv2->YNI += yy;
            vv2->ZNI += zz;

        }
    }

	for( ii=0 ; ii<curlayer->MaxNbVertex ; ii++ )
    {
        vv = &(curlayer->LWO_Vertex[ii]) ;
        xx = vv->XNI ;
        yy = vv->YNI ;
        zz = vv->ZNI ;

        nn = (float)sqrtf( (xx*xx + yy*yy + zz*zz) );

        if(nn != 0.0f)
        {
            nn = 1.0f / nn;
            xx*= nn;
            yy*= nn;
            zz*= nn;

            vv->XNI =xx;
            vv->YNI =yy;
            vv->ZNI =zz;
        }
    }
}

lwo2_error Read_POLS_LWO2(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  )
{
    unsigned int allocsize=0;
    int ofs,jj,ii,kk,id2;
    sLwoPolyToVertex *Vstart;
    id2 = readInt(lwo,offset );

#ifdef DEBUG3D
    { char strlog[64];
        sprintf(strlog,"poltype:%c%c%c%c:\n",((id2>>24)&255),
                ((id2>>16)&255), ((id2>>8)&255), (id2&255)
                );
        (*KHardInfo->KHI_outprint)(strlog);
    }
#endif

    {

        ofs = offset + 4;
        jj=0;
        while (ofs<endOffset)
        {
            ii = readShort(lwo,ofs) & 0x000003ff;
            ofs+= 2;
            for (kk=0 ; kk<ii ; kk++ )
            {
                ofs += passLWOVX(lwo,ofs);
                allocsize+= sizeof( sLwoPolyToVertex ) ;
            }
            jj++;

        }

        curlayer->MaxNbPolygon = jj;
        if( jj == 0 ) return(0);

        allocsize+=  (sizeof(sLwoPolygon) * jj );

		curlayer->LWO_Polygon = (sLwoPolygon*)lw_CleanAlloc( allocsize );
        if( curlayer->LWO_Polygon ==NULL ) return(1);

        Vstart = (sLwoPolyToVertex *) &(curlayer->LWO_Polygon[jj]) ;

        ofs = offset + 4;
        jj=0;
        while (ofs<endOffset)
        {

            ii = readShort(lwo,ofs) & 0x03ff;
            curlayer->LWO_Polygon[jj].m_NumberOfVertex = ii;

            curlayer->LWO_Polygon[jj].V = Vstart ;
            curlayer->LWO_Polygon[jj].surface = NULL;

            ofs+= 2;
            for (kk=0 ; kk<ii ; kk++ )
            {
                Vstart->V = readLWOVX(lwo,ofs) ;
                Vstart->tU = curlayer->LWO_Vertex[ Vstart->V ].U;
                Vstart->tV = curlayer->LWO_Vertex[ Vstart->V ].V;

                ofs += passLWOVX(lwo,ofs);
                Vstart++;
            }

            initNormal( &(curlayer->LWO_Polygon[jj]),
                            curlayer->LWO_Vertex );
            jj++;
        }

    initVertexNormals( curlayer  );

#ifdef  LWDBG
    printf("    pols end ok\n");
#endif
    }
    return(lwoerr_Ok);
}

lwo2_error Read_PTAG_LWO2(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  )
{
        int ofs,ii,id2,jj;

        id2 = readInt(lwo,offset );
        ofs = offset + 4;
#ifdef DEBUG3D
    { char strlog[64];
        sprintf(strlog,"curlayer:%08x:\n", curlayer);
        (*KHardInfo->KHI_outprint)(strlog);
        sprintf(strlog,"->LWO_Polygon:%08x:\n", curlayer->LWO_Polygon);
        (*KHardInfo->KHI_outprint)(strlog);
    }
#endif
        while( ofs < endOffset )
        {
            ii =  readLWOVX(lwo, ofs);
            ofs += passLWOVX(lwo,ofs);
            jj = readShort(lwo,ofs);
            ofs +=2;
            switch(id2)
            {
                case 0x53555246:

                    if( curlayer->LWO_Polygon !=NULL )
                    {
                        curlayer->LWO_Polygon[ii].surface = &( lwo->surfaces[jj] ) ;
                    }
                break;
                case  0x50415254:
                break;
                case  0x534D4750:
                break;
            }
        }
    return(lwoerr_Ok);
}

lwo2_error Read_SRFS_LWOB(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  )
{
    (void)curlayer;
    int ii,ofs;
    struct LwTexture *txt;

    ii=0;
    ofs= offset ;
    while( ofs < endOffset )
    {
         ii++;
         ofs = passLWOString(lwo,ofs);
    }
    lwo->nbsurf=ii;
#ifdef  LWDBG
    printf("    nbtxt SRFS:%d\n    ",ii);
#endif
    if( ii==0 ) return(0);
	lwo->surfaces = txt = (struct LwTexture *)lw_CleanAlloc( sizeof(struct LwTexture) * ii  );
	if( lwo->surfaces == 0L  ) return(lwoerr_AllocFailed);

     ofs =  offset;
     ii=0;
     while( ofs < endOffset )
     {

			txt->IndexInFile = ii;
            txt = &( lwo->surfaces[ ii ]  );
            txt->name = readLWOString( lwo, ofs );

            txt->RGBAcolor[3] = 1.0f;
            txt->mapimage       =
            txt->bumpimage      =
            txt->reflectimage   = NULL;
            txt->additiveRate   = 0 ;
			txt->typebits = TBit_MIPMap ;

#ifdef  LWDBG
    printf(":%s:",lwo->surfaces[ ii ].name  );
#endif
         ii++;
		 txt++;
         ofs = passLWOString(lwo,ofs);
     }
#ifdef  LWDBG
    printf("\n");
#endif
    return(lwoerr_Ok);
}

lwo2_error Read_VMAP_LWO2(sLwoObject *lwo,
                   sLwoLayer *curlayer,
                    int offset,int endOffset,
                    int prepareis0  )
{
    int iiv,ii,ofs,id2;

    int     startofs= offset;
    id2 = readInt( lwo , offset );
#ifdef  LWDBG
    printf("    type:%c%c%c%c\n",lwo->mainchunk[offset],lwo->mainchunk[offset+1],lwo->mainchunk[offset+2],lwo->mainchunk[offset+3]   );
#endif

    if(
        id2 == 0x5049434B   ||
        id2 == 0x57474854   ||
        id2 == 0x4D4E5657   ||

        id2 == 0x4D4F5246   ||
        id2 == 0x53504F54
    ) return(lwoerr_Ok);
    ii= readShort( lwo , offset + 4 ) ;

	if(id2 == 0x52474241 )
	{
		curlayer->m_ColorFlag |=1;
	}

    if( prepareis0 == 0 )
    {

		struct RememberVMAPChunk *pnewVMAP = (struct RememberVMAPChunk *)lw_CleanAlloc(sizeof(struct RememberVMAPChunk));
		if( pnewVMAP == NULL  ) return(lwoerr_AllocFailed);
		pnewVMAP->pNext = curlayer->m_pFirstVMAP;
		curlayer->m_pFirstVMAP = pnewVMAP;
		pnewVMAP->pVMAPName =  readLWOString( lwo, offset + 6  ) ;
        pnewVMAP->lwo = lwo;
        pnewVMAP->curlayer = curlayer ;
        pnewVMAP->offset = startofs ;
        pnewVMAP->endOffset = endOffset ;
        return(lwoerr_Ok);
    }
#ifdef  LWDBG
    printf("    name:%s\n", readLWOString( lwo, offset + 6  )  );
#endif
    ofs =  passLWOString( lwo , offset + 6 );

    while( ofs < endOffset )
    {

        iiv =  readLWOVX( lwo , ofs);
        ofs += passLWOVX( lwo , ofs);
        switch( id2 )
        {
            case 0x54585556:

                curlayer->LWO_Vertex[iiv].U = readFloat( lwo, ofs );
                curlayer->LWO_Vertex[iiv].V = readFloat( lwo, ofs + 4 );
            break;
            case 0x52474241 :
                curlayer->LWO_Vertex[iiv].r = readFloat( lwo, ofs );
                curlayer->LWO_Vertex[iiv].g = readFloat( lwo, ofs+4 );
                curlayer->LWO_Vertex[iiv].b = readFloat( lwo, ofs+8 );
            break;

        }
        ofs += ( sizeof(float)*ii);
    }
    return(lwoerr_Ok);
}

lwo2_error Read_VMAD_LWO2(sLwoObject *lwo,
                   sLwoLayer *curlayer,
                    unsigned int offset,unsigned int endOffset,
                    struct LwTexture *texturetoapply )
{

    int startofs = offset ;
    unsigned int     id2,iiv,iip,ii,ofs,mm;
    int     jj,numvv;
    sLwoPolyToVertex     *nextlink;
    float   tuu,tvv;
    id2 = readInt( lwo, offset );

    if(
        id2 == 0x5049434B   ||
        id2 == 0x57474854   ||
        id2 == 0x4D4E5657   ||

        id2 == 0x4D4F5246   ||
        id2 == 0x53504F54
    ) return(lwoerr_Ok);
	ii= readShort( lwo, offset + 4 ) ;
	if( texturetoapply == NULL )
    {

		struct RememberVMAPChunk *pnewVMAD = (struct RememberVMAPChunk *)lw_CleanAlloc(sizeof(struct RememberVMAPChunk));
		if( pnewVMAD == NULL  ) return(lwoerr_AllocFailed);
		pnewVMAD->pNext = curlayer->m_pFirstVMAD;
		curlayer->m_pFirstVMAD = pnewVMAD;
		pnewVMAD->pVMAPName = readLWOString( lwo, offset + 6  ) ;
        pnewVMAD->lwo = lwo;
        pnewVMAD->curlayer = curlayer ;
        pnewVMAD->offset = startofs ;
        pnewVMAD->endOffset = endOffset ;
        return(lwoerr_Ok);
    }
    ofs =  passLWOString( lwo,  offset+6);

    while( ofs < endOffset )
    {

        iiv =  readLWOVX( lwo, ofs);
        ofs += passLWOVX( lwo, ofs);

        iip =  readLWOVX(lwo,ofs);
        ofs += passLWOVX(lwo,ofs);

        jj = -1 ;
        if( texturetoapply == curlayer->LWO_Polygon[ iip ].surface  )
        {
            for( mm=0 ; mm< curlayer->LWO_Polygon[ iip ].m_NumberOfVertex   ; mm++)
            {

                if(

                    curlayer->LWO_Polygon[ iip ].V[mm].V
                    ==
                    (int)iiv
                   )
                {

                    jj = mm;
                    mm = curlayer->LWO_Polygon[ iip ].m_NumberOfVertex;
                }
            }
        }
        if( jj!= -1 )
        {
            switch( id2 )
            {
                case 0x54585556:
                tuu =  readFloat( lwo, ofs );
                tvv =  readFloat( lwo, ofs+4 );
                curlayer->LWO_Polygon[ iip ].V[jj].tU = tuu ;
                curlayer->LWO_Polygon[ iip ].V[jj].tV = tvv ;

                if( curlayer->LWO_Polygon[ iip ].V[jj].DiscontinuousMappingOffset == 0 )
                {

                    if( curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse == NULL )
                    {

                      curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse = &(curlayer->LWO_Polygon[ iip ].V[jj]);

                      curlayer->LWO_Polygon[ iip ].V[jj].DiscontinuousMappingOffset = curlayer->NumberOfDiscontinousVertexToAdd+1 ;
                      curlayer->LWO_Polygon[ iip ].V[jj].tU = tuu ;
                      curlayer->LWO_Polygon[ iip ].V[jj].tV = tvv ;
                      curlayer->NumberOfDiscontinousVertexToAdd++;
                    } else
                    {

                      numvv= -1 ;
                      nextlink = curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse ;
                      while( nextlink != NULL )
                      {
                            if( tuu == nextlink->tU  &&
                                tvv == nextlink->tV
                                )
                            {

                                numvv = nextlink->DiscontinuousMappingOffset ;
                            }
                            nextlink = nextlink->NextDiscontinuousUse ;
                      }
                      if( numvv == -1 )
                      {

                        nextlink = curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse ;
                        curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse = &(curlayer->LWO_Polygon[ iip ].V[jj]);
                        curlayer->LWO_Polygon[ iip ].V[jj].NextDiscontinuousUse = nextlink ;

                        curlayer->LWO_Polygon[ iip ].V[jj].DiscontinuousMappingOffset = curlayer->NumberOfDiscontinousVertexToAdd+1 ;
                        curlayer->LWO_Polygon[ iip ].V[jj].tU = tuu ;
                        curlayer->LWO_Polygon[ iip ].V[jj].tV = tvv ;
                        curlayer->NumberOfDiscontinousVertexToAdd++;
                      } else
                      {

                        nextlink = curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse ;
                        curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse = &(curlayer->LWO_Polygon[ iip ].V[jj]);
                        curlayer->LWO_Polygon[ iip ].V[jj].NextDiscontinuousUse = nextlink ;

                        curlayer->LWO_Polygon[ iip ].V[jj].DiscontinuousMappingOffset = numvv;

                      }
                    }
                }
                break;

            }
        }
        ofs += (sizeof(float)*ii);
    }
    return(lwoerr_Ok);
}

lwo2_error Read_CLIP_LWO2( sLwoObject *lwo,
                    int offset,
                    int endofs,
                    struct  LwoClip *lwc
                    )
{
    int endOffsetL2;
    char *path=NULL;

    lwc->LWClipindex = readInt(lwo,offset);
    offset+=4;
    while ( offset < endofs )
    {
        int ii,id;
        id=readInt(lwo,offset);
        endOffsetL2 = offset +  6 + readShort(lwo,offset+4);

        if(id == 0x5354494C )
        {

                path = readLWOString( lwo, offset+6 );
                ii = (int) strlen(path) -1;
                while( ii>=0 )
                {
                    if( path[ii] == '/'  ) break;
                    ii--;
                }
                ii++;
                path = &(path[ii]);
#ifdef  LWDBG
    printf("    clip name:%s:\n",path );
#endif
        }

        offset=endOffsetL2;
    }
    lwc->imagename   = path ;
    return(lwoerr_Ok);
}

lwo2_error Read_SURF_LWO2( sLwoObject *lwo,
                    int ofs,
                    int endoffset
                    )
{
	unsigned int     ii;
	int     jj,kk;
	char  *name,*father;
	struct LwTexture  *buildme,*srfather;

	name = readLWOString( lwo, ofs );
	ofs = passLWOString( lwo, ofs);
	father = readLWOString(lwo, ofs );
	ofs = passLWOString(lwo, ofs);

#ifdef DEBUG3D

#endif

	jj=-1;
	kk=-1;
	for(ii=0 ; ii<lwo->nbsurf ; ii++)
	{
		if( strcmp(name, lwo->surfaces[ii].name ) == 0 ) jj = ii;
		if( strcmp(father, lwo->surfaces[ii].name ) ==0 ) kk = ii;
	}
	if(jj == -1 ) { return(lwoerr_Ok);  }
	buildme = &(lwo->surfaces[jj]) ;

	srfather =  NULL;
	if( kk != -1) srfather = &(lwo->surfaces[kk]) ;

	Read_SURF_LWO2sub(lwo,  ofs ,endoffset  , buildme , srfather );

    return(lwoerr_Ok);
}

lwo2_error Read_SURF_LWO2sub(  sLwoObject *lwo,
                        int offset ,
                        int endofs,
                        struct LwTexture *buildme ,
                        struct LwTexture *father )
{
	int endOffsetL2;
	int id;
	int rr;
	while ( offset < endofs )
	{
		id=readInt(lwo, offset);
		endOffsetL2 = offset +  6 + readShort(lwo,offset+4);

        switch(id)
        {
            case 0x434F4C52:

				buildme->RGBAcolor[0] = readFloat( lwo, offset +6);
				buildme->RGBAcolor[1] = readFloat( lwo, offset +10);
				buildme->RGBAcolor[2] = readFloat( lwo, offset +14);
				buildme->RGBAcolor[3] = 1.0f;
                break;
                case 0x52494D47:
                    rr =  readLWOVX(lwo, offset +6);

                    buildme->reflectimage = searchClip( lwo, rr );
#ifdef  LWDBG
    printf("    RIMG:%08x:\n", (int) buildme->reflectimage );
#endif
                break;

                case 0x424C4F4B:
                    Read_BLOK_LWO2SURF( lwo, offset+6,endOffsetL2,buildme, father );
                break;
                case 0x5452414E:

                        buildme->transparencyRate = readFloat(lwo, offset +6 );

                break;
                case  (('A'<<24)|('D'<<16)|('T'<<8)|'R' ):
                    buildme->additiveRate = readFloat(lwo, offset +6 );
                break;

            }

            offset=endOffsetL2;
        }
    return(lwoerr_Ok);
}

lwo2_error Read_BLOK_LWO2SURF(  sLwoObject *lwo,
                        int offset ,
                        int endofs,
                        struct LwTexture *buildme ,
                        struct LwTexture *father )
{
    (void)father;
    int ii;
    unsigned int    chan=0;
    int endOffsetL2;
    int id;

        id=readInt(lwo, offset);
        endOffsetL2 = offset +  6 + readShort( lwo, offset+4);

#ifdef DEBUG3D

#endif

        {
            int end2;
            int ofsblok = offset + 6 ;

            ofsblok = passLWOString(lwo,ofsblok);
            while( ofsblok < endOffsetL2  )
            {
                id=readInt( lwo, ofsblok);
                end2 = ofsblok + 6 + readShort( lwo, ofsblok+4);
                if( id == ((('C'<<24)|('H'<<16)|('A'<<8)|'N' ))  )
                {
                    chan = readInt( lwo, ofsblok+6 );

                }
                ofsblok += end2 ;
            }
        }
        offset=endOffsetL2;
        while ( offset < endofs )
        {
            id=readInt( lwo, offset);
            endOffsetL2 = offset +  6 + readShort( lwo, offset+4);

            switch(id)
            {
                case 0x50524F4A:

                break;
				case ('W'<<24) |('R'<<16)|('A'<<8)|('P'):
                    ii = readShort(lwo, offset+6);
					if(ii == 3 ) buildme->typebits |= TBit_ClampWidth ;
                    ii = readShort(lwo, offset+8);
					if(ii == 3 ) buildme->typebits |= TBit_ClampHeight ;
                break;
                case    0x564D4150:

                    buildme->txuv_used = readLWOString( lwo,offset+6) ;
                break;
                case    0x494D4147:
                    if( chan == (('C'<<24)|('O'<<16)|('L'<<8)|'R' )  )
                        buildme->mapimage = searchClip( lwo, readLWOVX(lwo, offset+6 ) );
                    else
                    if( chan == (('B'<<24)|('U'<<16)|('M'<<8)|'P' )  )
                        buildme->bumpimage = searchClip( lwo, readLWOVX(lwo, offset+6 ) );

                break;
            }

            offset=endOffsetL2;
        }
    return(lwoerr_Ok);
}

char *searchClip( sLwoObject *lwo,   int rr )
{
    unsigned int ii;
    for(ii=0 ; ii< lwo->nbclip ; ii++ )
    {
        if( lwo->clipimagetable[ii].LWClipindex == rr  )
            return( lwo->clipimagetable[ii].imagename );
    }
    return( NULL );
}

const sLwoLayer *LwoGetLayer(const sLwoObject *_pLwoObject, const unsigned int _layerIndex)
{
	unsigned int ii;
	for(ii=0 ; ii<_pLwoObject->m_NumberOfLayers ; ii++)
		if( _pLwoObject->m_LWO_Layer[ii].m_LayerNumber == (int)_layerIndex ) return
				&(_pLwoObject->m_LWO_Layer[ii]);

	return 0L;
}
