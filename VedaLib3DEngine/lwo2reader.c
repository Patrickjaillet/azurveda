/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
/*
	\file		lwo2reader.c
	\brief		Completely API independant lightwave object file format reader.	
				by krb, 02/11/2006, based on:
					- the karate 68k-no fpu version from 2004, that
						can manage many VMAP/VMAD by surface and objects.
					- the unused "sb" karateGL version, which does a great algo
						for the discontinuous vertex thing, and also read the
						envelopes stuffs.
*/

//#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <math.h> 
#ifndef sqrtf
#define	sqrtf sqrt
#endif
#include    "lwo2reader.h"
// struct used for chained lists to stock the VMAP chunks:
struct RememberVMAPChunk
{
	struct RememberVMAPChunk	*pNext;
	const char					*pVMAPName;
    sLwoObject *lwo;
    sLwoLayer *curlayer;
    int offset;
    int endOffset;
};

///-------- static protos
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

/*
	\brief	Read a File, open all layers in tables. CloseLwo() must be called at destruction and will close all allocs.
	\param	_pLwoObject	reference to an object that manage the LWO.
	\param	_pLwoFileChunk	pointer to the whole file chunk.
	\param	_lwoFileByteLength the length of _pLwoFileChunk in bytes.
	\param	_flags	look ReadLwoFlag enum up there. 
*/
lwo2_error	ReadLwo(sLwoObject *_pLwoObject, const unsigned char *_pLwoFileChunk, unsigned int _lwoFileByteLength, const unsigned int _flags )
{
	// first clear the struct, needed if close() is thrown after:
	{ int ii;
		unsigned char *pf = (unsigned char *)_pLwoObject;
		for(ii=0; ii<sizeof(sLwoObject) ; ii++) *(pf++)=0;
	}
	_pLwoObject->m_pMainChunk	= _pLwoFileChunk ;
    _pLwoObject->m_mainsize		= _lwoFileByteLength ;
    // error if not iff 
	{
		int ii = readInt(_pLwoObject,0);
		if( ii != 0x464F524D  ) // FORM
		{   
			return(lwoerr_FileProblem);
		}
		/*---- check if LWO or LWO2 (or whatever if FORM) -------*/
		ii= readInt(_pLwoObject, 8 ) ;
		switch( ii )
		{
			case 0x4C574F32: // LWO2 
				_pLwoObject->LWO2_true = -1;
			break;
			case 0x4C574F42: // LWOB (old) 
				_pLwoObject->LWO2_true = 0;
			break;
			default://error !
				return lwoerr_FileProblem;
			break;
		}
	}
	/*---- read level 1 chunks ---*/
    /* pass1: Count numbah of layers & alloc alphatable */
    // already cleared: lwo->m_NumberOfLayers = lwo->nbenvlp = lwo->nbclip = 0;
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
    /* read chunk level1 pass2: read for real */
	{
		unsigned int ii;
		int nbcurrentLayer=-1; // we do+1 and first layer is zero.
		unsigned int offset=12;
        sLwoLayer    *pcurrentlayer;
        struct  LwoClip		*plwclip;
	    if( _pLwoObject->m_NumberOfLayers == 0 )
		{	/* very important for LWOB: if no layer, then: only one: the default. */
			_pLwoObject->m_NumberOfLayers=1;
			nbcurrentLayer = 0;
		} 
        _pLwoObject->m_LWO_Layer = (sLwoLayer *)lw_CleanAlloc( sizeof( sLwoLayer ) * _pLwoObject->m_NumberOfLayers );
        if( !(_pLwoObject->m_LWO_Layer) ){ return(lwoerr_AllocFailed); }
        pcurrentlayer = _pLwoObject->m_LWO_Layer ; // default for LWOB.
         /*-------- initialise layer pointers ----*/
        for(ii=0 ; ii<_pLwoObject->m_NumberOfLayers ; ii++ )
        {/* already clean
            _pLwoObject->m_LWO_Layer[ii].LWO_Vertex   = NULL ;
            _pLwoObject->m_LWO_Layer[ii].LWO_Polygon     = NULL ;
            _pLwoObject->m_LWO_Layer[ii].firstmorph   = NULL;*/
            _pLwoObject->m_LWO_Layer[ii].LWO_file     = _pLwoObject;
        }
		/*----- alloc clip image name table -------*/
        // already clean:_pLwoObject->clipimagetable = 0L ;
        if( _pLwoObject->nbclip > 0 )
        {
			_pLwoObject->clipimagetable = plwclip = (struct LwoClip *) lw_CleanAlloc( sizeof( struct LwoClip ) * _pLwoObject->nbclip );
            if( !plwclip ) { CloseLwo(_pLwoObject); return(lwoerr_AllocFailed); }
        }
       //------- envelopes to build:
        if( _pLwoObject->nbenvlp > 0 )
        {
            _pLwoObject->envlp = (struct LwoIndexedEnvelope *)lw_CleanAlloc( sizeof( struct LwoIndexedEnvelope) * _pLwoObject->nbenvlp );
			if( !(_pLwoObject->envlp)){CloseLwo(_pLwoObject); return(lwoerr_AllocFailed); };
        }
		//----- now, we read the chunks from level 2:
		while (offset<_lwoFileByteLength)
        {
			lwo2_error	error = lwoerr_Ok;
			int endOffsetL2 = offset +  8 + readInt(_pLwoObject,offset+4);
			switch ( readInt(_pLwoObject,offset) )
            {
                case 1413564243: // TAGS (LWO2)   Lists the tag strings that can be associated with polygons by the PTAG chunk.
					error = Read_TAGS_LWO2(_pLwoObject,offset+8, endOffsetL2 );
                break;
				/*----------------------------------------------------------*/
                case 1279351122: // LAYR
				{int ofs;
                    // next layer to work with/fill.
                    nbcurrentLayer++;
                    pcurrentlayer = &(_pLwoObject->m_LWO_Layer[nbcurrentLayer]) ;
                    /* LAYER CONSTRUCTOR HERE */
                    pcurrentlayer->LWO_file = _pLwoObject;
                    pcurrentlayer->m_LayerNumber = readShort(_pLwoObject,offset+8);
                    pcurrentlayer->flags = readShort(_pLwoObject,offset+8+2);
                    // pivot:
					pcurrentlayer->objectPos.T1X = readFloat(_pLwoObject,offset+8+4);
                    pcurrentlayer->objectPos.T1Y = readFloat(_pLwoObject,offset+8+8);
                    pcurrentlayer->objectPos.T1Z = readFloat(_pLwoObject,offset+8+12);

                    pcurrentlayer->name = readLWOString(_pLwoObject, offset + 8 + 16 );
#ifdef  LWDBG
 //   printf("    name:%s: index:%d:\n",curlayer->name,curlayer->LayerNumber  );
#endif
                    ofs =  passLWOString(_pLwoObject,offset+8+16);
                    if (ofs < endOffsetL2) { pcurrentlayer->parent = readShort(_pLwoObject,ofs); }
                    ///parent exist
                    // LAYR { number[U2], flags[U2], pivot[VEC12], name[S0], parent[U2] ?
                    //----------- the vmap/vmad txuv base is cleared.
                   // we can free vmap base here.
                   //re?->no, was debugID pcurrentlayer->txuvVector_vmap.MSR_ChunkID = M_ID('l','w','m','p');
                   //re?->no, was debugID  pcurrentlayer->txuvVector_vmad.MSR_ChunkID = M_ID('l','w','m','d');
				}break;
                /*----------------------------------------------------------*/
                case 1347310675: // PNTS
                    error = Read_PNTS_LWO(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2 );
                break;
                /*----------------------------------------------------------*/
               // case 1111641944: // BBOX
               // break;
                /*----------------------------------------------------------*/
                case 1347374163: // POLS
                    if( _pLwoObject->LWO2_true )
                        error = Read_POLS_LWO2(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2 );
                    else
                        error = Read_POLS_LWOB(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2 );
                break;
                /*----------------------------------------------------------*/
                case 0x564D4150: // VMAP    vertex mapping & morph. LWO2
                    error = Read_VMAP_LWO2(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2,0 ); // just prepare
                break;
                /*----------------------------------------------------------*/
                case 0x564D4144: // VMAD    discont' vertex mapping & morph. LWO2
                    error = Read_VMAD_LWO2(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2,NULL ); // just prepare.
                break;
                /*----------------------------------------------------------*/
                case 1347699015: // PTAG (LWO2)
                    // PTAG { type[ID4], ( poly[VX], tag[U2] )* }
                    error = Read_PTAG_LWO2(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2 );
                break;
                /*----------------------------------------------------------*/
                case (('S'<<24)|('R'<<16)|('F'<<8)|'S'): // LWOB textures.
                    error = Read_SRFS_LWOB(_pLwoObject,pcurrentlayer,offset + 8, endOffsetL2 );
                break;
                /*----------------------------------------------------------*/
                case 0x434C4950: // CLIP { index[U4], attributes[SUB-CHUNK] * }
					// clips are the images and 2 anim base:
                    error = Read_CLIP_LWO2( _pLwoObject,offset+8,endOffsetL2, plwclip  );
                    plwclip++;
                break;
                /*----------------------------------------------------------*/
				/*     case 0x454E564C:
					not for the moment:
						error = Read_ENVL_LWO2( lwo,offset+8,endOffsetL2  );
						if( res != 0 ) return( res );
					break;
				*/
                /*----------------------------------------------------------*/
                case 1398100550: // SURF
                // BEWARE: in LWO2, surfaces are hierarchized !!
                    error = Read_SURF_LWO2( _pLwoObject, offset+8,endOffsetL2  );
					#ifdef  LWDBG
						printf("        surf returned:%d\n", res );
					#endif
                break;
                /*----------------------------------------------------------*/
			} // switch end.
			// error management for all switch return:
			if(error != lwoerr_Ok) 
			{
				CloseLwo(_pLwoObject);
				return error;
			}
			offset=endOffsetL2;
		} // end of while() to check level1 chunks.
	} // end of pass paragraph.
//----------------------------------------------------------------
	//---- extra operation to synchronize uvmap/uvmad with surfaces:
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
				// a vmap/vmad name was found:
				if( srf->txuv_used != NULL ) // VMAP name.
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
						// apply the vmap on classic vertex.
						lwo2_error	error = Read_VMAP_LWO2(_pLwoObject,pcurrentlayer,pVMAP->offset, pVMAP->endOffset, 1 );
						if( error != lwoerr_Ok )
						{
							CloseLwo(_pLwoObject);
							return( error );
						}
						// for all poly with the texture, get vertex uv.
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
						// then apply vmad discontinuity if found
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
					} // end if( pVMAP != NULL  )


			}
			srf++;
		} // loop by srf
		// close vmap bases here:
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
		//---- extra operation to split UV-Discontinuous vertexes:
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
			// recopy first vertexes:
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
							// discontinuous !
							pnewVertex4->U = nextlink->tU ;
							pnewVertex4->V = nextlink->tV ;
							nextlink->V = newindex;	
						}
						nextlink = nextlink->NextDiscontinuousUse ;
					}
				}
				pnewVertex2++;
				pOldVertex++;
			} // end loop by vertex.
			lw_FreeAlloc(pcurrentlayer->LWO_Vertex);
			pcurrentlayer->LWO_Vertex = pnewVertex ;
			pcurrentlayer->MaxNbVertex += pcurrentlayer->NumberOfDiscontinousVertexToAdd;
			pcurrentlayer->NumberOfDiscontinousVertexToAdd = 0; // done.
		} // end test if discontinutity.
		if((_flags & ReadLwoFlag_SortTriangleBySurfaces)!=0)
		{
			unsigned int ii,jj;
			unsigned int nbTotalTriangle=0;
			sLwoPolygon *pFace;
			struct  LwoTriangle *pTriangle;
			struct LwoSortedTriangleSurface	*pSurfaceIndex;
			// split polygons in triangles 
			// count number of triangles for each surface:
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
			if(nbTotalTriangle!=0) // we can have vertex-only or edge-only objects.
			{
				pcurrentlayer->m_LWO_Triangle = pTriangle = (struct LwoTriangle *)lw_CleanAlloc(nbTotalTriangle*sizeof(struct  LwoTriangle));
				if(pTriangle==0L) 			
				{
					lw_FreeAlloc(pSurfaceUseCount);
					CloseLwo(_pLwoObject);
					return(lwoerr_AllocFailed);
				}
				pcurrentlayer->m_MaxNbTriangle = nbTotalTriangle;
				// -- count number of surface really used and alloc index toward triangle list: --
				jj=0;
				for(ii=0;ii<_pLwoObject->nbsurf;ii++)
					if(pSurfaceUseCount[ii] !=0) jj++; // this surf is used.
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
				// --radix-like sorting: create index of the place to write:
				jj=0;
				for(ii=0;ii<_pLwoObject->nbsurf;ii++)
				{
					unsigned int prev;
					prev = pSurfaceUseCount[ii];
					if(prev !=0)
					{	// fill index table:
						pSurfaceIndex->LwoFileSurfaceIndex = ii;
						pSurfaceIndex->FirstTriangle = jj;
						pSurfaceIndex->NumberOfTriangle = prev;
						pSurfaceIndex++;
					}
					pSurfaceUseCount[ii]=jj;
					jj+=prev;
				} // here, jj should be ==nbTotalTriangle			
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
			} // end of test if totalnbtriangle==0
			//free temp:
			lw_FreeAlloc(pSurfaceUseCount);
		}
		pcurrentlayer++;
	} // loop by layer

	} // end subfunc

	return lwoerr_Ok;
}
/*
	\brief	Close all allocs from ReadLwo().
	\param	_pLwoObject	reference to an object that manage the LWO.
*/
void	CloseLwo(sLwoObject *_pLwoObject)
{
	unsigned int ii;
    sLwoLayer    *curlayer;
    //------------ destroy by layers:
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
        //re?(*khi->KHI_CloseAllocRmbStack)( &(curlayer->txuvVector_vmap ) ); // if some rest.
        //re?(*khi->KHI_CloseAllocRmbStack)( &(curlayer->txuvVector_vmad ) );
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
    //-------------------- other bases:
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
/*--------------------- iff primitive type little or big-endian readers -------*/
static float readFloat(sLwoObject *lwo,int ofs)
{
        float   fi;
        int     *ip;
        ip = (int *)&fi;
        *(ip) = readInt(lwo,ofs);
        return fi;
}
///
///--------- readInt
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
///
///--------- passLWOString
static int passLWOString(sLwoObject *lwo, int ofs)
{
        while ((lwo->m_pMainChunk[ofs])!=0) {ofs++;}
        ofs++;
        if ((ofs & 0x00000001) == 1) {ofs++;}
        return ofs;
}
///
///--------- passLWOVX
static int passLWOVX(sLwoObject *lwo, int ofs)
{
        if (lwo->m_pMainChunk[ofs] != 0x000000ff ) { return( 2 ); } else { return ( 4 );}
}
///
///--------- readLWOString
static char *readLWOString(sLwoObject *lwo, int ofs)
{
//      String result=new String();
/* java        int old=ofs;
        UBYTE inByte;
        while ((inByte=mainchunk[ofs])!=0) { ofs++;  }
        return new String( mainchunk , old , ofs-old );
*/
    /*--- IMPORTANT: NO ALLOC !! ----*/
    return( (char *)&(lwo->m_pMainChunk[ofs])  );

}
static int readLWOVX(sLwoObject *lwo, int ofs)
{   // bastard index form from LW.
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

/*--------------------- chunk readers --*/
///-------- Read_TAGS_LWO2
lwo2_error Read_TAGS_LWO2(sLwoObject *lwo,int offset,int endOffset  )
{
    int ii,ofs;
    struct LwTexture *txt;
    //----- count numbers of tags. -----
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
	lwo->surfaces = lw_CleanAlloc( sizeof(struct LwTexture) * ii  ); //new LWTexture[ii];
    if( lwo->surfaces == NULL  ) return(lwoerr_AllocFailed);

     //------ create strings representing the surfaces -----
     ofs =  offset;
     ii=0;
     while( ofs < endOffset )
     {
         //was java lwo->surfaces[ ii ] = new LWTexture() ;
            txt = &( lwo->surfaces[ ii ]  );
			txt->IndexInFile = ii;
            txt->name = readLWOString( lwo, ofs );
            txt->txuv_used = NULL; // at start.
            //txt->kindex = ii;
			txt->RGBAcolor[3] = 1.0f;
            txt->mapimage       =
            txt->bumpimage      =
            txt->reflectimage   = NULL;
            txt->additiveRate   = 0;
			txt->typebits = TBit_MIPMap ; // By default 
//old            txt->typebits = TBIT_MIPMAPME ; // when image genered, it will be mipmaped.
//old            txt->OGLTextureDimension = 0; // means not set.
//old            txt->OGLTextureNumber = 0;  // means not created.
//old        txt->OGLContextList = 0;
//re            txt->transparency.val = 0; // can mean no transp.
//re            txt->transparency.lwEnvlope = NULL ;
//old        txt->OGLTextureDimension2 = 0; // means not set.
//old            txt->OGLTextureNumber2 = 0;  // means not created.
//old        txt->OGLContextList2 = 0;
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
///
///-------- Read_PNTS_LWO
lwo2_error Read_PNTS_LWO(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  )
{
	int  ii,jj;
	sLwoVertex    *vv;
	float   maxv =3.402823466e+38F; // this is max positive.
	float   minx,miny,minz;
	float   maxx,maxy,maxz;

        /*---------------------------------*/
        ii = (endOffset - offset) / 12;
        curlayer->MaxNbVertex = ii;
#ifdef  LWDBG
    printf("    nbpts:%d\n",ii);
#endif
        if(ii==0) return(lwoerr_Ok);    /* 0-vertex object possible */
		curlayer->LWO_Vertex = (sLwoVertex *)lw_CleanAlloc( sizeof( sLwoVertex  ) * ii);  /* java new KVertexC1[ii]; */
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
            //printf("nn:%08d   X: %08x :",jj,vv->XI);
            offset+=4;
            vv->YI = readFloat(lwo,offset) - curlayer->objectPos.T1Y;
            //printf("Y: %08x :",vv->YI);
            offset+=4;
            vv->ZI = readFloat(lwo,offset) - curlayer->objectPos.T1Z;
            //printf("Z: %08x :\n",vv->ZI);
            offset+=4;

            if( vv->XI < minx ) minx = vv->XI;
            if( vv->YI < miny ) miny = vv->YI;
            if( vv->ZI < minz ) minz = vv->ZI;

            if( vv->XI > maxx ) maxx = vv->XI;
            if( vv->YI > maxy ) maxy = vv->YI;
            if( vv->ZI > maxz ) maxz = vv->ZI;

			vv->r = vv->g = vv->b = 1.0f;
            // next vert.
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
///
///-------- Read_POLS_LWOB
lwo2_error Read_POLS_LWOB(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  )
{
    int id2,ii,kk,jj;
    unsigned int    allocsize=0;
    sLwoPolyToVertex *Vstart;
    // 1st count nb of polys:
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
    if( ii ==0 ) return(lwoerr_Ok); /* 0 pols possible */
    allocsize += ( ii * sizeof(sLwoPolygon) ) ;

	curlayer->LWO_Polygon = (sLwoPolygon *)lw_CleanAlloc( allocsize ); //new KFaceC1[ii];
    if( curlayer->LWO_Polygon ==NULL ) return(lwoerr_AllocFailed);
    /*  the faces' Vertex ref tables start after the face table
    */
    Vstart = (sLwoPolyToVertex *) &(curlayer->LWO_Polygon[ii]) ;

    for( jj=0 ; jj<ii ; jj++)
    {
        kk= readShort(lwo,offset) ;
//        curlayer->LWO_Polygon[jj].uv = new float[5][kk];
        curlayer->LWO_Polygon[jj].m_NumberOfVertex = kk;
        curlayer->LWO_Polygon[jj].V = Vstart;
        curlayer->LWO_Polygon[jj].surface = &( lwo->surfaces[ ii ]  );

        offset+=2;
        for( id2=0 ; id2 < kk ; id2++ )
        {
            Vstart->V = readShort(lwo,offset);
            Vstart->tU = curlayer->LWO_Vertex[ Vstart->V ].U;
            Vstart->tV = curlayer->LWO_Vertex[ Vstart->V ].V;
//            Vstart->DiscontinuousMappingOffset = 0;
//            Vstart->DontRecreateMeDiscontinuous =0;
//            Vstart->NextDiscontinuousUse = NULL ;
            offset+=2;
            Vstart++;
        }
        /*---- we find the normal of the face now. ------*/
        initNormal( &(curlayer->LWO_Polygon[jj]),
                            curlayer->LWO_Vertex );

        // now kk surface number: (from 1 to n)
        kk= readShort(lwo,offset) ;
        offset+=2;
        if(kk<0) // unregular: is followed by coplanar other pols.
        {
            kk=-kk;
            offset+=2; // number of polys following being coplanar: (we dont care)
        }
        curlayer->LWO_Polygon[jj].surface = &( lwo->surfaces[ kk-1 ]  );

    }
    initVertexNormals( curlayer  );

#ifdef  LWDBG
    printf("    pols end ok\n");
#endif

    return(lwoerr_Ok);
}
///
///--------------------- initNormal
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
///
///--------------------- initVertexNormals
static void initVertexNormals( sLwoLayer *curlayer ) {
    unsigned int ii,jj;
    float   xx,yy,zz,nn;
    sLwoPolygon     *ff;
    sLwoVertex   *vv,*vv2;

    //--- all V to 0
	for( ii=0 ; ii<curlayer->MaxNbVertex ; ii++ )
    {
        vv = &(curlayer->LWO_Vertex[ii]) ;
        vv->XNI =
        vv->YNI =
        vv->ZNI =0.0f;
    }

    //--- add all that.
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
//          ff.V[jj].XNF += 1.0f;
        }
    }
    // average & normalize all that.

	for( ii=0 ; ii<curlayer->MaxNbVertex ; ii++ )
    {
        vv = &(curlayer->LWO_Vertex[ii]) ;
        xx = vv->XNI ;
        yy = vv->YNI ;
        zz = vv->ZNI ;
//      nn = vv.XNF ;   // vertex XNF is used as the number of poly attached here.
        nn = (float)sqrtf( (xx*xx + yy*yy + zz*zz) );

        if(nn != 0.0f)
        {
            nn = 1.0f / nn;
            xx*= nn;
            yy*= nn;    // norm = 1.0f;
            zz*= nn;

            vv->XNI =xx;
            vv->YNI =yy;
            vv->ZNI =zz;
        }
    }
}
///
///-------- Read_POLS_LWO2
lwo2_error Read_POLS_LWO2(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  )
{
    unsigned int allocsize=0;
    int ofs,jj,ii,kk,id2;
    sLwoPolyToVertex *Vstart;
    id2 = readInt(lwo,offset );

    /*===================================================*/
    /*                  FACE                             */
    /*===================================================*/
#ifdef DEBUG3D
    { char strlog[64];
        sprintf(strlog,"poltype:%c%c%c%c:\n",((id2>>24)&255),
                ((id2>>16)&255), ((id2>>8)&255), (id2&255)
                );
        (*KHardInfo->KHI_outprint)(strlog);
    }
#endif

// ALL TYPE GOT TO BE CHECK THE SAME WAY:
//    if (id2 == 1178682181 ) // FACE
    {
        // compte nbre de polys
        ofs = offset + 4;
        jj=0;
        while (ofs<endOffset)
        {
            ii = readShort(lwo,ofs) & 0x000003ff; //nbv
            ofs+= 2;
            for (kk=0 ; kk<ii ; kk++ ) //by vertex/p
            {
                ofs += passLWOVX(lwo,ofs);
                allocsize+= sizeof( sLwoPolyToVertex ) ;  /* the size of the int* table by face */
            }
            jj++;

        }
        // nb of ALL polys
        curlayer->MaxNbPolygon = jj;
        if( jj == 0 ) return(0); /* 0 poly possible */

        /*----------------------------------------------*/
        allocsize+=  (sizeof(sLwoPolygon) * jj );

		curlayer->LWO_Polygon = (sLwoPolygon*)lw_CleanAlloc( allocsize ); /*new KFaceC1[jj];*/
        if( curlayer->LWO_Polygon ==NULL ) return(1);
        /*
            the faces' Vertex ref tables start after the face table
        */
        Vstart = (sLwoPolyToVertex *) &(curlayer->LWO_Polygon[jj]) ;

        // compte nbre de polys
        ofs = offset + 4;
        jj=0;
        while (ofs<endOffset)
        {
            //curlayer->LWO_Polygon[jj] = new KFaceC1();
            ii = readShort(lwo,ofs) & 0x03ff; //nbv
            curlayer->LWO_Polygon[jj].m_NumberOfVertex = ii;

            curlayer->LWO_Polygon[jj].V = Vstart ;
            curlayer->LWO_Polygon[jj].surface = NULL;

            ofs+= 2;
            for (kk=0 ; kk<ii ; kk++ ) //by vertex/p
            {
                Vstart->V = readLWOVX(lwo,ofs) ;
                Vstart->tU = curlayer->LWO_Vertex[ Vstart->V ].U;
                Vstart->tV = curlayer->LWO_Vertex[ Vstart->V ].V;
//                Vstart->DiscontinuousMappingOffset = 0;
//                Vstart->DontRecreateMeDiscontinuous =0;
//                Vstart->NextDiscontinuousUse = NULL ;
                ofs += passLWOVX(lwo,ofs);
                Vstart++;
            }
            /*---- we find the normal of the face now. ------*/
            initNormal( &(curlayer->LWO_Polygon[jj]),
                            curlayer->LWO_Vertex );
            jj++;
        } /* end while->face  */
    /*----- we find the average normal for all the vertex here (after pols) -----*/
    initVertexNormals( curlayer  );
    /*
    LWO2:
    FACE    "Regular" polygons, the most common.
    CURV    Catmull-Rom splines. These are used during modeling and are currently ignored by the renderer.
    PTCH    Subdivision patches. The POLS chunk contains the definition of the control cage polygons, and the patch is created by subdividing these polygons. The renderable geometry that results from subdivision is determined interactively by the user through settings within LightWave. The subdivision method is undocumented.
    MBAL    Metaballs. These are single-point polygons. The points are associated with a VMAP of type MBAL that contains the radius of influence of each metaball. The renderable polygonal surface constructed from a set of metaballs is inferred as an isosurface on a scalar field derived from the sum of the influences of all of the metaball points.
    BONE    Line segments representing the object's skeleton. These are converted to bones for deformation during rendering.
    */
#ifdef  LWDBG
    printf("    pols end ok\n");
#endif
    }
    return(lwoerr_Ok);
}
///-------- Read_PTAG_LWO2
lwo2_error Read_PTAG_LWO2(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  )
{     // PTAG { type[ID4], ( poly[VX], tag[U2] )* }
        int ofs,ii,id2,jj;
/*
SURF
The surface assigned to the polygon. The actual surface attributes are found by matching the name in the TAGS chunk with the name in a SURF chunk.
PART
The part the polygon belongs to. Parts are named groups of polygons analogous to point selection sets (but a polygon can belong to only one part).
SMGP
The smoothing group the polygon belongs to. Shading is only interpolated within a smoothing group, not across groups.
*/
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
                case 0x53555246: // SURF subid.
                    // pol list can be NULL if patached.
                    if( curlayer->LWO_Polygon !=NULL )
                    {
                        curlayer->LWO_Polygon[ii].surface = &( lwo->surfaces[jj] ) ;
                    }
                break;
                case  0x50415254: // PART
                break;
                case  0x534D4750: // SMGP
                break;
            }
        }
    return(lwoerr_Ok);
}
///
///-------- Read_SRFS_LWOB
lwo2_error Read_SRFS_LWOB(  sLwoObject *lwo,
                    sLwoLayer *curlayer,
                    int offset,
                    int endOffset  )
{
    int ii,ofs;
    struct LwTexture *txt;
    //----- count numbers of tags. -----
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
	lwo->surfaces = txt = (struct LwTexture *)lw_CleanAlloc( sizeof(struct LwTexture) * ii  ); //new LWTexture[ii];
	if( lwo->surfaces == 0L  ) return(lwoerr_AllocFailed);
     //------ create strings representing the surfaces -----
     ofs =  offset;
     ii=0;
     while( ofs < endOffset )
     {
         //was java lwo->surfaces[ ii ] = new LWTexture() ;
			txt->IndexInFile = ii;
            txt = &( lwo->surfaces[ ii ]  );
            txt->name = readLWOString( lwo, ofs );
            //txt->kindex = ii;
            txt->RGBAcolor[3] = 1.0f;
            txt->mapimage       =
            txt->bumpimage      =
            txt->reflectimage   = NULL;
            txt->additiveRate   = 0 ;
			txt->typebits = TBit_MIPMap ; // By default 
//            txt->transparency.val = 0; // can mean no transp.
//            txt->transparency.lwEnvlope = NULL ;
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
///-------- Read_VMAP_LWO2  reads UVs and morphs by vertexes....
lwo2_error Read_VMAP_LWO2(sLwoObject *lwo,
                   sLwoLayer *curlayer,
                    int offset,int endOffset,
                    int prepareis0  )
{
    int iiv,ii,ofs,id2;
    //re? struct  MorphSet    *newmset=NULL; // can stay NULL.
    int     startofs= offset;
    id2 = readInt( lwo , offset );
#ifdef  LWDBG
    printf("    type:%c%c%c%c\n",lwo->mainchunk[offset],lwo->mainchunk[offset+1],lwo->mainchunk[offset+2],lwo->mainchunk[offset+3]   );
#endif
    /*----- excape if type not suported ---------*/
    if(
        id2 == 0x5049434B   ||  // PICK
        id2 == 0x57474854   ||  // WGHT
        id2 == 0x4D4E5657   ||  // MNVW
        //id2 == 0x54585556 ||  // TXUV
        //id2 == 0x52474241 ||  // RGB, RGBA (?)
        id2 == 0x4D4F5246   ||  // MORF vertex displacement delta
        id2 == 0x53504F54       // SPOT
    ) return(lwoerr_Ok); // OK but not supported...
    ii= readShort( lwo , offset + 4 ) ;
    //---- open new base with the name:
	if(id2 == 0x52474241 ) //RGB !
	{	// layer use color:
		curlayer->m_ColorFlag |=1;
	}
    // UBYTE   *AllocRmbStackID( int size, struct  MemStackRoot *Stack , char *idname )
    if( prepareis0 == 0 )
    {
       // rmbchunk = (struct RememberChunk  *) (*khi->KHI_AllocRmbStackID)( sizeof(struct RememberChunk) , &(curlayer->txuvVector_vmap), readLWOString( lwo, offset + 6  )  );
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
    /*--- by polygons -----*/
    while( ofs < endOffset )
    {
        // read vertex
        iiv =  readLWOVX( lwo , ofs);
        ofs += passLWOVX( lwo , ofs);
        switch( id2 )
        {
            case 0x54585556: // TXUV
                //uv[nbdimsuivie(uvrgb=5)][nbV]
                // VMAP affect vertexes.
                /*---- read OK for OGL UV ----*/
                curlayer->LWO_Vertex[iiv].U = readFloat( lwo, ofs );
                curlayer->LWO_Vertex[iiv].V = readFloat( lwo, ofs + 4 );
            break;
            case 0x52474241 : // RGBA				
                curlayer->LWO_Vertex[iiv].r = readFloat( lwo, ofs );
                curlayer->LWO_Vertex[iiv].g = readFloat( lwo, ofs+4 );
                curlayer->LWO_Vertex[iiv].b = readFloat( lwo, ofs+8 );
            break;
/* re?
            case    (('S'<<24) | ('P'<<16) |('O'<<8)| 'T' ) :
            case    (('M'<<24) | ('O'<<16) |('R'<<8)| 'F' ) :
                newmset->vertex[iiv].X =  readFloat( lwo, ofs );
                newmset->vertex[iiv].Y =  readFloat( lwo, ofs + 4 );
                newmset->vertex[iiv].Z =  readFloat( lwo, ofs + 8 );
            break;
*/
        }
        ofs += ( sizeof(float)*ii); // float size.
    }
    return(lwoerr_Ok);
}
///-------- Read_VMAD_LWO2
lwo2_error Read_VMAD_LWO2(sLwoObject *lwo,
                   sLwoLayer *curlayer,
                    unsigned int offset,unsigned int endOffset,
                    struct LwTexture *texturetoapply )
{
    // Discontinuous Vertex Mapping (on polys)
    //VMAD { type[ID4], dimension[U2], name[S0],
    //( vert[VX], poly[VX], value[F4] # dimension )* }
/*
Some common type codes are
PICK
Selection set. This is a VMAP of dimension 0 that marks points for quick selection by name during modeling. It has no effect on the geometry of the object.
WGHT
Weight maps have a dimension of 1 and are generally used to alter the influence of deformers such as bones. Weights can be positive or negative, and the default weight for unmapped vertices is 0.0.
MNVW
Subpatch weight maps affect the shape of geometry created by subdivision patching.
TXUV
UV texture maps have a dimension of 2.
RGB, RGBA
Color maps, with a dimension of 3 or 4.
MORF
These contain vertex displacement deltas.
SPOT
These contain absolute vertex displacements (alternative vertex positions).
*/
//    int iip,mm;
//    fixflt[][]   uv;
    int startofs = offset ;
    unsigned int     id2,iiv,iip,ii,ofs,mm,jj,numvv;
    sLwoPolyToVertex     *nextlink;
    float   tuu,tvv;
    id2 = readInt( lwo, offset );
    //          System.out.println("    VMADTYPE:"+new String(mainchunk,offset+8,4)+" "+id2 );
    /*----- escape if not suported ---------*/
    if(
        id2 == 0x5049434B   ||  // PICK
        id2 == 0x57474854   ||  // WGHT
        id2 == 0x4D4E5657   ||  // MNVW
        //id2 == 0x54585556 ||  // TXUV
// note: disontinuous RGBA in OGL would separe again vertexes.
        // id2 == 0x52474241 ||  // RGB, RGBA (?)
        id2 == 0x4D4F5246   ||  // MORF vertex displacement delta
        id2 == 0x53504F54       // SPOT
    ) return(lwoerr_Ok);
	ii= readShort( lwo, offset + 4 ) ;
	if( texturetoapply == NULL )
    {
        //(*KHardInfo->KHI_outprint)(readLWOString( lwo, offset + 6  ) );
        //(*KHardInfo->KHI_outprint)(":vmad name\n");
		// rmbchunk = (struct RememberChunk  *) (*khi->KHI_AllocRmbStackID)( sizeof(struct RememberChunk) , &(curlayer->txuvVector_vmad), readLWOString( lwo, offset + 6  )  );
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

    /*--- by polygons -----*/
    while( ofs < endOffset )
    {
        // read vertex
        iiv =  readLWOVX( lwo, ofs);
        ofs += passLWOVX( lwo, ofs);
        // read poly
        iip =  readLWOVX(lwo,ofs);
        ofs += passLWOVX(lwo,ofs);
        //old curlayer->LWO_Polygon[ iip ].Flags |= KFaceC1.vmadovervmap ;
        // search vertex place in the existing poly..
        //uv = NULL;
        jj = -1 ; // vertex index found.
        if( texturetoapply == curlayer->LWO_Polygon[ iip ].surface  )
        {
            for( mm=0 ; mm< curlayer->LWO_Polygon[ iip ].m_NumberOfVertex   ; mm++)
            {
                // test if this is the vertex.
                if(
                    // test if same pointer.
                    curlayer->LWO_Polygon[ iip ].V[mm].V
                    ==
                    iiv
                   )
                {
                    //uv = m_LWO_Layer[nbl].LWO_Polygon[iip].uv;
                    // quit loop !
                    jj = mm;
                    mm = curlayer->LWO_Polygon[ iip ].m_NumberOfVertex;
                }
            }
        }
        if( jj!= -1 )
        {
            switch( id2 )
            {
                case 0x54585556: // TXUV
                tuu =  readFloat( lwo, ofs );
                tvv =  readFloat( lwo, ofs+4 );
                curlayer->LWO_Polygon[ iip ].V[jj].tU = tuu ;
                curlayer->LWO_Polygon[ iip ].V[jj].tV = tvv ;
                // test if ==0 in case of there would be >1 UV map layer.
                if( curlayer->LWO_Polygon[ iip ].V[jj].DiscontinuousMappingOffset == 0 )
                {
                    // if we are the first dicontinuous for this vertex, ->OK, new one.
                    if( curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse == NULL )
                    {
                      // link
                      curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse = &(curlayer->LWO_Polygon[ iip ].V[jj]);
                      // ->NextDiscontinuousUse should be and rest NULL.
                      // declare point to create.
                      curlayer->LWO_Polygon[ iip ].V[jj].DiscontinuousMappingOffset = curlayer->NumberOfDiscontinousVertexToAdd+1 ;
                      curlayer->LWO_Polygon[ iip ].V[jj].tU = tuu ;
                      curlayer->LWO_Polygon[ iip ].V[jj].tV = tvv ;
                      curlayer->NumberOfDiscontinousVertexToAdd++;
                    } else
                    {
                      // if any other discontinuous point
                      // for the same vertex has the same UV, don't need new one: use brother.
                      numvv= -1 ; // not found.
                      nextlink = curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse ;
                      while( nextlink != NULL )
                      {
                            if( tuu == nextlink->tU  &&
                                tvv == nextlink->tV
                                )
                            {
                                // THIS DISCONTINUITY ALREADY EXIST.
                                numvv = nextlink->DiscontinuousMappingOffset ;
                            }
                            nextlink = nextlink->NextDiscontinuousUse ;
                      }
                      if( numvv == -1 )
                      {
                        // new discontinuous vertex to create:
                        // link
                        nextlink = curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse ;
                        curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse = &(curlayer->LWO_Polygon[ iip ].V[jj]);
                        curlayer->LWO_Polygon[ iip ].V[jj].NextDiscontinuousUse = nextlink ;

                        // declare point to create.
                        curlayer->LWO_Polygon[ iip ].V[jj].DiscontinuousMappingOffset = curlayer->NumberOfDiscontinousVertexToAdd+1 ;
                        curlayer->LWO_Polygon[ iip ].V[jj].tU = tuu ;
                        curlayer->LWO_Polygon[ iip ].V[jj].tV = tvv ;
                        curlayer->NumberOfDiscontinousVertexToAdd++;
                      } else
                      {
                        // else, reuse previous new discontinuous point:
						// link
                        nextlink = curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse ;
                        curlayer->LWO_Vertex[ iiv ].FirstDiscontinuousUse = &(curlayer->LWO_Polygon[ iip ].V[jj]);
                        curlayer->LWO_Polygon[ iip ].V[jj].NextDiscontinuousUse = nextlink ;
						// reuse previous:
                        curlayer->LWO_Polygon[ iip ].V[jj].DiscontinuousMappingOffset = numvv;
                       //re? curlayer->LWO_Polygon[ iip ].V[jj].DontRecreateMeDiscontinuous =1;
                      }
                    }
                }
                break;
            /* case 0x52474241 : // RGBA
                    uv[ 2 ][ jj ] = readFloat( ofs );
                    uv[ 3 ][ jj ] = readFloat( ofs + 4 );
                    uv[ 4 ][ jj ] = readFloat( ofs + 8 );
                break;
            */
            }
        }
        ofs += (sizeof(float)*ii); 
    }
    return(lwoerr_Ok);
}
///
///-------- Read_CLIP_LWO2
lwo2_error Read_CLIP_LWO2( sLwoObject *lwo,
                    int offset,
                    int endofs,
                    struct  LwoClip *lwc
                    )
{
    int endOffsetL2;
    char *path=NULL;
    /*------------- find clip index -----------*/
    lwc->LWClipindex = readInt(lwo,offset);
    offset+=4;
    while ( offset < endofs )
    {
        int ii,id;
        id=readInt(lwo,offset);
        endOffsetL2 = offset +  6 + readShort(lwo,offset+4);
        //switch(id)
        if(id == 0x5354494C )
        {
            //case    0x5354494C: // STIL
                /* get image path and cut last name.  */
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
        //------ next chunk.-----
        offset=endOffsetL2;
    }  
    lwc->imagename   = path ; // can be NULL
    return(lwoerr_Ok);
}
///
///-------- Read_SURF_LWO2
lwo2_error Read_SURF_LWO2( sLwoObject *lwo,
                    int ofs,
                    int endoffset
                    )
{
	unsigned int     ii,jj,kk;
	char  *name,*father;
	struct LwTexture  *buildme,*srfather;

	name = readLWOString( lwo, ofs );
	ofs = passLWOString( lwo, ofs);
	father = readLWOString(lwo, ofs );
	ofs = passLWOString(lwo, ofs);

#ifdef DEBUG3D
    /*{ char strlog[64];
        sprintf(strlog,"    name:%s: father:%s:\n",name,father );
        (*KHardInfo->KHI_outprint)(strlog); }*/
#endif
	//---- find surface object from TAGS: ------
	jj=-1;
	kk=-1;
	for(ii=0 ; ii<lwo->nbsurf ; ii++)
	{
		if( strcmp(name, lwo->surfaces[ii].name ) == 0 ) jj = ii;
		if( strcmp(father, lwo->surfaces[ii].name ) ==0 ) kk = ii;
	}
	if(jj == -1 ) { return(lwoerr_Ok);  } // not found !! (should not happen.)
	buildme = &(lwo->surfaces[jj]) ;

	srfather =  NULL;
	if( kk != -1) srfather = &(lwo->surfaces[kk]) ;
	/*----- read all subchunks. -----*/
	Read_SURF_LWO2sub(lwo,  ofs ,endoffset  , buildme , srfather );

	/*------ now we now everything about the texture data, find the correct renderer; -----*/
	//  was a java stuff: buildme.findRenderer();
    return(lwoerr_Ok);
}
///
///-------- Read_SURF_LWO2sub
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
/*#ifdef DEBUG3D
    { char strlog[64];
        sprintf(strlog, "        surfsub:%c%c%c%c: length:%d\n", lwo->mainchunk[offset],lwo->mainchunk[offset+1],lwo->mainchunk[offset+2],lwo->mainchunk[offset+3],
                             readShort(lwo,offset+4)        );
#endif*/
        switch(id)
        {
            case 0x434F4C52: // COLR 4f x3 + vx
                // find color in int format
				buildme->RGBAcolor[0] = readFloat( lwo, offset +6);
				buildme->RGBAcolor[1] = readFloat( lwo, offset +10);
				buildme->RGBAcolor[2] = readFloat( lwo, offset +14);
				buildme->RGBAcolor[3] = 1.0f;
                break;
                case 0x52494D47: //RIMG { image[VX] } reflection image (spherical ?)
                    rr =  readLWOVX(lwo, offset +6);
                    //ofs += passLWOVX(ofs);
                    buildme->reflectimage = searchClip( lwo, rr );
#ifdef  LWDBG
    printf("    RIMG:%08x:\n", (int) buildme->reflectimage );
#endif
                break;
                // BLOK { header[SUB-CHUNK], attributes[SUB-CHUNK] * }
                case 0x424C4F4B:
                    Read_BLOK_LWO2SURF( lwo, offset+6,endOffsetL2,buildme, father );
                break;
                case 0x5452414E:  // TRAN
                    // intensity[FP4], envelope[VX]
                        buildme->transparencyRate = readFloat(lwo, offset +6 );
//re                    buildme->transparency.val =  readFloat( lwo, offset +6);
//re                    buildme->transparency.lwEnvlope = searchEnvl( lwo , readLWOVX(lwo, offset +10 ) );
                break;
                case  (('A'<<24)|('D'<<16)|('T'<<8)|'R' ):
                    buildme->additiveRate = readFloat(lwo, offset +6 );
                break;

            }
            //------ next chunk.-----
            offset=endOffsetL2;
        }
    return(lwoerr_Ok);
}
///
lwo2_error Read_BLOK_LWO2SURF(  sLwoObject *lwo,
                        int offset ,
                        int endofs,
                        struct LwTexture *buildme ,
                        struct LwTexture *father )
{
    int ii;
    unsigned int    chan=0;
    int endOffsetL2;
    int id;

/*
    1st chunk are BLOK type: it can be:
IMAP - an image map texture
PROC - a procedural texture
GRAD - a gradient texture
SHDR - a shader plug-in
    block-header { ordinal[S0], block-attributes[SUB-CHUNK] * }
*/
        id=readInt(lwo, offset);
        endOffsetL2 = offset +  6 + readShort( lwo, offset+4);
       
#ifdef DEBUG3D
/*    { char strlog[64];
        sprintf(strlog,"    BLOK ID:%c%c%c%c: %08x\n",id>>24,id>>16,id>>8,id,id );
        (*KHardInfo->KHI_outprint)(strlog);    }*/
#endif
        // BLOK HEADER:
        {
            int end2;
            int ofsblok = offset + 6 ;
            // jump ordinal string:
            ofsblok = passLWOString(lwo,ofsblok);
            while( ofsblok < endOffsetL2  )
            {
                id=readInt( lwo, ofsblok);
                end2 = ofsblok + 6 + readShort( lwo, ofsblok+4);
                if( id == ((('C'<<24)|('H'<<16)|('A'<<8)|'N' ))  )
                {
                    chan = readInt( lwo, ofsblok+6 );
/*#ifdef DEBUG3D
    { char strlog[64];
        sprintf(strlog,"    BLOK HEADER CHAN:%c%c%c%c:\n",chan>>24,chan>>16,chan>>8,chan );
        (*KHardInfo->KHI_outprint)(strlog);    }
#endif*/
                }
                ofsblok += end2 ;
            }
        }
        offset=endOffsetL2;
        while ( offset < endofs )
        {
            id=readInt( lwo, offset);
            endOffsetL2 = offset +  6 + readShort( lwo, offset+4);
/*#ifdef DEBUG3D
    { char strlog[64];
        sprintf(strlog,"        :%c%c%c%c: %08x\n",id>>24,id>>16,id>>8,id,id );   }
#endif*/
            switch(id)
            {
                case 0x50524F4A: // "PROJ" define if UV or planar, or sphe, or cyl, mapping.
                    //  PROJ { projection-mode[U2] }
                   /* ii = readShort(lwo, offset+6);
                    switch( ii)
                    {
                        case 5:
// not the exact meaning->     buildme.typebits |= LWTexture.tb_mapimage ;
#ifdef  LWDBG
    printf("        PROJ: UV\n", buildme->reflectimage );
#endif
                        break;
                    }*/
/*
0 - Planar
The image is projected on a plane along the major axis (specified in the AXIS subchunk). r and s map to the other two axes.
1 - Cylindrical
The image is wrapped cylindrically around the major axis. r maps to longitude (angle around the major axis).
2 - Spherical
The image is wrapped spherically around the major axis. r and s map to longitude and latitude.
3 - Cubic
Like Planar, but projected along all three axes. The dominant axis of the geometric normal selects the projection axis for a given surface spot.
4 - Front Projection
The image is projected on the current camera's viewplane. r and s map to points on the viewplane.
5 - UV
r and s map to points (u, v) defined for the geometry using a vertex map (identified in the BLOK's VMAP subchunk).
*/
                break;
				case ('W'<<24) |('R'<<16)|('A'<<8)|('P'):  // WRAP
                    ii = readShort(lwo, offset+6);
					if(ii == 3 ) buildme->typebits |= TBit_ClampWidth ;
                    ii = readShort(lwo, offset+8);
					if(ii == 3 ) buildme->typebits |= TBit_ClampHeight ;
                break;
                case    0x564D4150: // SURF/BLOK/VMAP
                 // System.out.println("\t\t\tmapname:"+ readLWOString(offset+6)  );
                    buildme->txuv_used = readLWOString( lwo,offset+6) ;
                break;
                case    0x494D4147: // SURF/BLOK/IMAG
                    if( chan == (('C'<<24)|('O'<<16)|('L'<<8)|'R' )  )
                        buildme->mapimage = searchClip( lwo, readLWOVX(lwo, offset+6 ) );
                    else
                    if( chan == (('B'<<24)|('U'<<16)|('M'<<8)|'P' )  )
                        buildme->bumpimage = searchClip( lwo, readLWOVX(lwo, offset+6 ) );
/*#ifdef  LWDBG
    printf("        IMAG:%x\n", buildme->mapimage );
#endif*/
                break;
            } // switch end
/*
UV Vertex Map
VMAP { txuv-map-name[S0] }
For UV projection, which depends on texture coordinates at each vertex, this selects the name of the TXUV vertex map that contains those coordinates.
*/
/*      :IMAP: size:48
        :TMAP: size:110
        :PROJ: size:8
        :AXIS: size:8
        :IMAG: size:8
        :WRAP: size:10
        :WRPW: size:12
        :WRPH: size:12
        :VMAP: size:14
        :AAST: size:12
        :PIXB: size:8
*/
            //------ next chunk.-----
            offset=endOffsetL2;
        }
    return(lwoerr_Ok);
}
///
///-------- searchClip
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
///

/*!
	\brief	Close all allocs from ReadLwo().
	\param	_pLwoObjectlwo pointer to struct computed with ReadLwo.
	\param _layerIndex
	\return a sLwoLayer layer shape description.
*/
const sLwoLayer *LwoGetLayer(const sLwoObject *_pLwoObject, const unsigned int _layerIndex)
{
	unsigned int ii;
	for(ii=0 ; ii<_pLwoObject->m_NumberOfLayers ; ii++)
		if( _pLwoObject->m_LWO_Layer[ii].m_LayerNumber == _layerIndex ) return 
				&(_pLwoObject->m_LWO_Layer[ii]);
	
	// failed:
	return 0L;
}
