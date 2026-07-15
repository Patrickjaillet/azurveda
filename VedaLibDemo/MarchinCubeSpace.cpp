#include "MarchinCubeSpace.h"
#include "PackFloat.h"
#include "Texture3D.h"
BASEOBJECT_DECLARE_CLASS( "mcsp3d", MarchinCubeSpace, BaseObject,"3DMarchinCube Space","CubeSpace","Define a cube grid used by Marchin Cube 3D Object to build themselves. Don't use many big ones, they take a lot of memory!!!" );

MarchinCubeSpace::MarchinCubeSpace() : BaseObject()
	,m_pMainTableAlloc(0L)
/*	, m_pCubeTable_Index(0L)
	, m_pCubeTable_Flags(0L)
	, m_pCubeTable_Values(0L)
	, m_pCubeTable_ExtraValue(0L)
	, m_pCubeTable_CheckDate(0L)
	, m_polyIndexTable(0L)
	*/
	,m_CheckDate(0L)
{

  REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_LengthX,"X Length",30,2,96);
  REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_LengthY,"Y Length",30,2,96);
  REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_LengthZ,"Z Length",30,2,96);
}

/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by CreateInternal().
*/
bool MarchinCubeSpace::CreateInternal(void)
{
	return( PreGenerateTriangleTables() );
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void MarchinCubeSpace::CloseInternal(void)
{
		/*old
	if(m_pCubeTable_Index) delete [] m_pCubeTable_Index;
	if(m_pCubeTable_Flags) delete [] m_pCubeTable_Flags;
	if(m_pCubeTable_Values) delete [] m_pCubeTable_Values;
	if(m_pCubeTable_ExtraValue) delete [] m_pCubeTable_ExtraValue;
	if(m_pCubeTable_CheckDate) delete [] m_pCubeTable_CheckDate;
	*/
	if( m_pMainTableAlloc) delete [] m_pMainTableAlloc ;
	if(m_polyIndexTable) delete [] m_polyIndexTable;
	m_polyIndexTable = 0L;
	m_pMainTableAlloc = 0L;
/*
	m_pCubeTable_Index = 0L;
	m_pCubeTable_Flags = 0L;
	m_pCubeTable_Values	= 0L;
	m_pCubeTable_ExtraValue = 0L;
	m_polyIndexTable= 0L;
	m_pCubeTable_CheckDate = 0L;
	*/
}
#endif

/*///---------------------- static global mini tables ------------*/
/*---- minitable giving 2D edges for x16 cases ----*/
/*
 1-2   +0+
 | |   2 1
 4-8   +3+
out
--->
in
*/
typedef struct _Edge2D16Case{
    unsigned char   ed2d_edge1;     // 4 low -> 4 high
    unsigned char   ed2d_edge2;     // 4 low -> 4 high
} Edge2D16Case ;
// note: if any of these 2 uchar are != 0, the edge is valid.

#define setedge(a1,a2)     ( a2<<4 | a1 )
// (2) note: calm down on static tables please: always const
const Edge2D16Case g_Edge2D16Case[16]=    // size = 15*2 = 30 bytes.
{
    {0,             0           },      // 0
    {setedge(0,2),  0           },      // 1
    {setedge(1,0),  0           },      // 2
    {setedge(1,2),  0           },      // 3  1 + 2
    {setedge(2,3),  0           },      // 4
    {setedge(0,3),  0           },      // 5  4 + 1
    {setedge(1,3),setedge(2,0)  },      // 6  4 + 2 OTHER POSIBLE.
    {setedge(1,3),  0           },      // 7  4 + 2 + 1
    {setedge(3,1),  0           },      // 8
    {setedge(0,1),setedge(3,2)  },      // 9  8 + 1 OTHER POSSIBLE
    {setedge(3,0),  0           },      // 10 8 + 2
    {setedge(3,2),  0           },      // 11 8 + 2 + 1
    {setedge(2,1),  0           },      // 12 8 + 4
    {setedge(0,1),  0           },  // 13 8 + 4 + 1
    {setedge(2,0),  0           },   // 14 8 + 4 + 2
    {0,             0           }
};
/*

z+
 \
  +->X+
  |
  Y+
OLD!
 4----5
 | 0----1
 | |  | |
 | |  | |
 6-|--7 |
   2----3
NEW:
 5----1
 | 4----0
 | |  | |
 | |  | |
 7-|--3 |
   6----2
0 1 2 3 4 5 6 7
a b c d e f g h
4 0 6 2 5 1 7 3

1
 +----+ 
 |\  2|\
 | +----+
5| |0 | |
 | |  | |
 +-|--+ |<-3
  \|   \|
   +----+
      4
 1-2   +0+
 | |   2 1
 4-8   +3+
      
      */
//--- give for each 6 faces of cube, a square front vertex order (1)

#define m_0     1<<0
#define m_1     1<<1
#define m_2     1<<2
#define m_3     1<<3
#define m_4     1<<4
#define m_5     1<<5
#define m_6     1<<6
#define m_7     1<<7

struct one4charline {
    unsigned char   cc[4];
};

const struct one4charline g_RemapCubeVertexTo6Squares[6]= // sizeof: 24 bytes
{
    // note: reversed order <->
    {   m_3, m_2,m_1,m_0    },  // front    >remplacé par le masque
    {   m_6, m_7,m_4,m_5    },  // back
    {   m_1, m_0,m_5,m_4    },  // up
    {   m_2, m_6,m_0,m_4    },  // left
    {   m_7, m_3,m_5,m_1    },  // right
    {   m_7, m_6,m_3,m_2    }   // down
};


// remap (squareEdge(0,3)xface) -> cubeEdge(0,11)  (3)
const struct one4charline g_Remap6SquareEdgesToCube[6]= // sizeof: 24 bytes
{
    {0,1,2,3},  // front
    {4,6,5,7},  // back
    {4,9,8,0},  // up
    {8,2,6,10}, // left
    {9,5,1,11}, // right
    {3,11,10,7} // down.
};

//---- struct for 0,255 cube creation index.
/*///-------------------------  PreGenerateTriangleTables ----*/
bool    MarchinCubeSpace::PreGenerateTriangleTables( void )
{
    unsigned int	octoconfig;
    unsigned char   *allocTable1,*PreTable1,*PreTable2,*acc,*ptv,vv;
	const unsigned char *cptv;
    //CubeEspace **pce;
    //---- alloc tables:
 //old    PreTable1 = calloc( 512 , (12*2)+1  ); // 12 vector max per cube.
	allocTable1 = PreTable1 = new  unsigned char[ 
									  (256 * ((12*2)+1)) // pass 1
									+ (256 * ((12*2)+1)) // pass 2
									+ (256*16)			 // pass 3
								] ;
	if( !PreTable1 ) return false;
	// note: for 1 cube we have: (1char=nb vector) + 12 possible 2char vector.
    PreTable2 = allocTable1 + (((12*2)+1)*256 );

    //---- pass1: find all edges for 1 cube config:
    for( octoconfig = 0 ; octoconfig<256 ; octoconfig++ )
    {
		// the first byte tells the number of vertex used:
        acc = PreTable1 +1;
        unsigned int nbv=0;
		unsigned char *vertconv = (unsigned char *)g_RemapCubeVertexTo6Squares ;
        unsigned char *edgeconv = (unsigned char *)g_Remap6SquareEdgesToCube ;
		unsigned int	ii,jj,cubface;
        for(cubface=0 ; cubface<6 ; cubface++)
        {
            ii = 0;
            for(jj=0 ; jj<4 ; jj++)
            {
                ii<<=1;
                if(  (  ((unsigned int) *(vertconv))  & octoconfig) != 0L ) ii|=1;
                vertconv++;
            } // here, ii [0,15]

			//--- get 0 , 1 or 2 vector(s):
            //  edge vector: low[0,3]->high[0,3].
            cptv = &g_Edge2D16Case[ii].ed2d_edge1 ;

			for(jj=0 ; jj<2 ; jj++)
            {
                vv= *(cptv++);
				if (vv != 0  )
                {	// 2 bytes indexe one edge:
                    *(acc++) = edgeconv[ ((unsigned int ) vv) & 0x0000000f ];
                    *(acc++) = edgeconv[ ((unsigned int ) vv)>>4 ]; // vv unsigned.
                    nbv++;
                }
            }
            // ed2d_edge1
            edgeconv+=4;
        }
		// the first byte tells the number of edges on the cube configuration:
        *(PreTable1) = nbv;
        PreTable1+= ((12*2)+1);
    }
    //----- pass 2: by cubes, sort edges in logic convex polygon order
/*
    faux car:
    si on "efface" les vecteurs pris,
    (et si 2 poly fermé)
    à la fin du premier poly, 
    nextsearch reprend la valeur du depart
    -> la condition de fin n'est pas qu'on a pas trouvé:
    on trouve toujours.
    la cond. de fin d'un poly:
        si nextsearch a repris la valeur de depart.
    condition de fin de tout le cub: pas besoin (boucle for)
    comment trouver le depart du 2eme poly ?

*/

    PreTable1 = allocTable1; // restart.
    ptv = PreTable2 ;   // keep var Pretable2 for pass3 (?)
/*
 attention:
 PreTable2 is of the form:
 (1 char= nb total edges of cub0)+
 (nb*2)
 then just after again:
 (1 char= nb total edges of cub1 )+
 (nb*2)
  ... (size for a cub vary.)

*/
    for( octoconfig = 0 ; octoconfig<256 ; octoconfig++ )
    {
		unsigned int	nextsearch,premier,ii,nbv;
        *(ptv) = vv = *(PreTable1) ;
        ptv++;
        premier = nextsearch = PreTable1[1] ;   // start to search... the 1st vector.

		for( nbv=0 ; nbv <vv ; nbv++ )
        {
            //--- search the next vector:
            //nomore nextfound = (0L); // mean not found.

            //--- write sorted edge (part1)
            *(ptv++) = nextsearch ;

            acc = PreTable1 +1;
            for(ii=0 ; ii<vv ; ii++)
            {
                if( nextsearch == *(acc) )
                {   // found vector !
                    //--- declare vector found. There is always one.
                    //nomore nextfound = acc ;
                    nextsearch = acc[1];
                    //--- has been found->erase.
                    *(acc) = 255;
                    // no need to erase vector end: acc[1] = 255 ;
                    //break-like:
                    ii= vv; // quit loop.
                }
                acc+=2;
            }
            //--- write sorted edge (part2)
            *(ptv++) = nextsearch ;         
            //--- if polygon finished !
            if( nextsearch == premier )
            {
                // there MUST exist some start vectors with
                // value not erased (!= 255)
                acc = PreTable1 +1; 
                for(ii=0 ; ii<vv ; ii++)
                {
                    if( *(acc) != 255 )
                    {   // found start of new poly:
                        nextsearch = premier =  *(acc) ;
                        //break-like:
                        ii= vv; // quit loop.
                    }
                    acc+=2;
                }
            }
            //--- 1 vector has been moved.
        }       
        PreTable1+= ((12*2)+1); // read     
    }
    //----- pass 3: compute easily final triangle table.
    // final table of the form:
    // char1= nb of triangle[0,5] + 5*3 index on (0,11) cube edge.
    // if less than 5 poly, rest if filled with 0 to have 16b by cube
    // configuration:
    ptv = allocTable1 + (512 * ((12*2)+1)) ; 

    // PreTable2: sorted edges that give convex polygon orders...
    for( octoconfig = 0 ; octoconfig<256 ; octoconfig++ )
    {
		unsigned int premier,ii,nbedges;
        nbedges = (unsigned int) *(PreTable2);
        PreTable2++;

        premier = *(PreTable2);

        ii = 0 ;    // nb triangle
        acc = ptv + 1 ;

		while( nbedges >0 )
        {
	    // get the 1st triangle:
            while( PreTable2[3] != premier )
            {
				*(acc++) = premier ;
				*(acc++) = PreTable2[2] ;
				*(acc++) = PreTable2[3] ;

                ii++;
                nbedges--;
                PreTable2+=2;
            }
            nbedges-=2;
            PreTable2+=4;
            premier = PreTable2[0] ;
            //nbedges--;
            //PreTable2+=2;
        }

        *(ptv)=ii;
        ptv+=16;
    }
    /*-------- pass4: here generate cubespaces 3D buffer with precalc index, using buffer3:  -----*/
	{
		// nbX is the number of SUBDIVISION. total x= nbx+1
		int nbX =	mSer_LengthX.Get() +1;
		int nbY =	mSer_LengthY.Get() +1;
		int nbZ =	mSer_LengthZ.Get() +2; //NOTE: NOW WE GET (Z+1) SQUARE BECAUSE OF A CASE WHERE TANGENT ARE FOUND WITH DIFFENT WITH NEXT Z's CUBE VALUE. 

		float       f0p5= PackFloat::m_0p5;
		unsigned int    *polyTable,ii,jj;
		unsigned char   *edgetable;

		// NOTE: NOW WE GET (Z+1) SQUARE BECAUSE OF A CASE
		// WHERE TANGENT ARE FOUND WITH NEXT Z's CUBE VALUE. 
		const unsigned int totalnumberofcube =  nbX * nbY * nbZ ;
/*old		m_pCubeTable_Index = new MarchCubeIndex[ totalnumberofcube ];
		m_pCubeTable_Flags = new unsigned char[ totalnumberofcube ];
		m_pCubeTable_Values = new float[ totalnumberofcube ];		
		m_pCubeTable_ExtraValue = new MarchCubeExtraValue[totalnumberofcube];
		m_pCubeTable_CheckDate = new unsigned int[ totalnumberofcube ];

		if( !polyTable || 
			!m_pCubeTable_Index || 
			!m_pCubeTable_Flags || 
			!m_pCubeTable_Values ||
			!m_pCubeTable_ExtraValue ||
			!m_pCubeTable_CheckDate )
		{
			delete [] allocTable1;
			return false;
		}
*/	
		unsigned char *allocptr;
		m_pMainTableAlloc = allocptr =new unsigned char[ 32 + // for a 32 byte cache align.
							totalnumberofcube * ( 
							sizeof(float)+ // m_pCubeTable_Values GOT TO BE ALIGNED.
							sizeof(MarchCubeExtraValue)+ //m_pCubeTable_ExtraValue
							sizeof(unsigned int)+ // m_pCubeTable_CheckDate
							sizeof(MarchCubeIndex)+ //
							sizeof(unsigned char)
							)
							];
		m_polyIndexTable = polyTable = new unsigned int[ 16*256 ] ;
		if( !polyTable || 
			!m_pMainTableAlloc )
		{
			delete [] allocTable1;
			return false;
		}
		allocptr = (unsigned char *)((int)(allocptr+31) & 0xffffffe0 ) ;
		m_pCubeTable_Values = (float *)allocptr;
		m_pCubeTable_ExtraValue = (MarchCubeExtraValue *)&(m_pCubeTable_Values[totalnumberofcube]);
		m_pCubeTable_CheckDate = (unsigned int *)&(m_pCubeTable_ExtraValue[totalnumberofcube]);			
		m_pCubeTable_Index = (MarchCubeIndex *)&(m_pCubeTable_CheckDate[ totalnumberofcube ]);
		m_pCubeTable_Flags = (unsigned char *)&(m_pCubeTable_Index[totalnumberofcube]);

		nbZ--; // sEE COMMENT UP.
		
		// set the checkdate to zero:
		for(ii=0; ii<totalnumberofcube ; ii++) 
		{	
			//no need m_pCubeTable_Flags[ii]=0;
			//m_pCubeTable_Index[ii].m_vx=0;
			//m_pCubeTable_Index[ii].m_vy=0;
			//no need m_pCubeTable_Index[ii].m_vz=0;
			m_pCubeTable_CheckDate[ii]=0; // the only one really need.
			//no need m_pCubeTable_Values[ii]=1.0f;
		}

		/*---- associates a polygon marchin table with correct edge index -----*/
		// this table gives (int *) poiting v_x edges where vertex numbers are.
		// add bytes to (MarchCube *) 

#define pX  1
#define pY  nbX
#define pZ  (nbX*nbY)
		edgetable = allocTable1 + (512 * ((12*2)+1)) ;  // buffer 3
		// translate edges numbers (0-11) to memory offsets from 
		// a cube cell to a (int *) pointing edge vertex on another
		// or the same cube:

		unsigned int    tredge[12];
		unsigned int	*pfill = &(tredge[0]);

		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pX		+ ofs_z	; //0
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*(pX+pZ)	+ ofs_y	; //1
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pX		+ ofs_y	; //2
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*(pX+pY)	+ ofs_z	; //3

		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*0		+ ofs_z	; //4
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pZ		+ ofs_y	; //5
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*0		+ ofs_y	; //6
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pY		+ ofs_z	; //7

		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*0		+ ofs_x	; //8
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pZ		+ ofs_x	; //9
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pY		+ ofs_x	; //10
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*(pY+pZ)	+ ofs_x	; //11

		for(ii=0 ; ii<256 ; ii++)
		{
			// copy nb of triangles.
			*(polyTable++) = *(edgetable++) ;
			for( jj=0 ; jj<15 ; jj++ )
			{
				*(polyTable++) = tredge[ *(edgetable++)  ] ;
			}
		}

	}


	/*----- delete temp cube index precalc buffer -----*/	
	delete allocTable1;
	return true;
}
