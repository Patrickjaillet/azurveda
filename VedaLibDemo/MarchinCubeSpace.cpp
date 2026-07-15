#include <cstdint>
#include "MarchinCubeSpace.h"
#include "PackFloat.h"
#include "Texture3D.h"
BASEOBJECT_DECLARE_CLASS( "mcsp3d", MarchinCubeSpace, BaseObject,"3DMarchinCube Space","CubeSpace","Define a cube grid used by Marchin Cube 3D Object to build themselves. Don't use many big ones, they take a lot of memory!!!" );

MarchinCubeSpace::MarchinCubeSpace() : BaseObject()
	,m_pMainTableAlloc(0L)

	,m_CheckDate(0L)
{

  REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_LengthX,"X Length",30,2,96);
  REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_LengthY,"Y Length",30,2,96);
  REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_LengthZ,"Z Length",30,2,96);
}

bool MarchinCubeSpace::CreateInternal(void)
{
	return( PreGenerateTriangleTables() );
}
#ifdef _ENGINE_EDITABLE_

void MarchinCubeSpace::CloseInternal(void)
{

	if( m_pMainTableAlloc) delete [] m_pMainTableAlloc ;
	if(m_polyIndexTable) delete [] m_polyIndexTable;
	m_polyIndexTable = 0L;
	m_pMainTableAlloc = 0L;

}
#endif

typedef struct _Edge2D16Case{
    unsigned char   ed2d_edge1;
    unsigned char   ed2d_edge2;
} Edge2D16Case ;

#define setedge(a1,a2)     ( a2<<4 | a1 )

const Edge2D16Case g_Edge2D16Case[16]=
{
    {0,             0           },
    {setedge(0,2),  0           },
    {setedge(1,0),  0           },
    {setedge(1,2),  0           },
    {setedge(2,3),  0           },
    {setedge(0,3),  0           },
    {setedge(1,3),setedge(2,0)  },
    {setedge(1,3),  0           },
    {setedge(3,1),  0           },
    {setedge(0,1),setedge(3,2)  },
    {setedge(3,0),  0           },
    {setedge(3,2),  0           },
    {setedge(2,1),  0           },
    {setedge(0,1),  0           },
    {setedge(2,0),  0           },
    {0,             0           }
};

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

const struct one4charline g_RemapCubeVertexTo6Squares[6]=
{

    {   m_3, m_2,m_1,m_0    },
    {   m_6, m_7,m_4,m_5    },
    {   m_1, m_0,m_5,m_4    },
    {   m_2, m_6,m_0,m_4    },
    {   m_7, m_3,m_5,m_1    },
    {   m_7, m_6,m_3,m_2    }
};

const struct one4charline g_Remap6SquareEdgesToCube[6]=
{
    {0,1,2,3},
    {4,6,5,7},
    {4,9,8,0},
    {8,2,6,10},
    {9,5,1,11},
    {3,11,10,7}
};

bool    MarchinCubeSpace::PreGenerateTriangleTables( void )
{
    unsigned int	octoconfig;
    unsigned char   *allocTable1,*PreTable1,*PreTable2,*acc,*ptv,vv;
	const unsigned char *cptv;

	allocTable1 = PreTable1 = new  unsigned char[
									  (256 * ((12*2)+1))
									+ (256 * ((12*2)+1))
									+ (256*16)
								] ;
	if( !PreTable1 ) return false;

    PreTable2 = allocTable1 + (((12*2)+1)*256 );

    for( octoconfig = 0 ; octoconfig<256 ; octoconfig++ )
    {

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
            }

            cptv = &g_Edge2D16Case[ii].ed2d_edge1 ;

			for(jj=0 ; jj<2 ; jj++)
            {
                vv= *(cptv++);
				if (vv != 0  )
                {
                    *(acc++) = edgeconv[ ((unsigned int ) vv) & 0x0000000f ];
                    *(acc++) = edgeconv[ ((unsigned int ) vv)>>4 ];
                    nbv++;
                }
            }

            edgeconv+=4;
        }

        *(PreTable1) = nbv;
        PreTable1+= ((12*2)+1);
    }

    PreTable1 = allocTable1;
    ptv = PreTable2 ;

    for( octoconfig = 0 ; octoconfig<256 ; octoconfig++ )
    {
		unsigned int	nextsearch,premier,ii,nbv;
        *(ptv) = vv = *(PreTable1) ;
        ptv++;
        premier = nextsearch = PreTable1[1] ;

		for( nbv=0 ; nbv <vv ; nbv++ )
        {

            *(ptv++) = nextsearch ;

            acc = PreTable1 +1;
            for(ii=0 ; ii<vv ; ii++)
            {
                if( nextsearch == *(acc) )
                {

                    nextsearch = acc[1];

                    *(acc) = 255;

                    ii= vv;
                }
                acc+=2;
            }

            *(ptv++) = nextsearch ;

            if( nextsearch == premier )
            {

                acc = PreTable1 +1;
                for(ii=0 ; ii<vv ; ii++)
                {
                    if( *(acc) != 255 )
                    {
                        nextsearch = premier =  *(acc) ;

                        ii= vv;
                    }
                    acc+=2;
                }
            }

        }
        PreTable1+= ((12*2)+1);
    }

    ptv = allocTable1 + (512 * ((12*2)+1)) ;

    for( octoconfig = 0 ; octoconfig<256 ; octoconfig++ )
    {
		unsigned int premier,ii,nbedges;
        nbedges = (unsigned int) *(PreTable2);
        PreTable2++;

        premier = *(PreTable2);

        ii = 0 ;
        acc = ptv + 1 ;

		while( nbedges >0 )
        {

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

        }

        *(ptv)=ii;
        ptv+=16;
    }

	{

		int nbX =	mSer_LengthX.Get() +1;
		int nbY =	mSer_LengthY.Get() +1;
		int nbZ =	mSer_LengthZ.Get() +2;

		float       f0p5= PackFloat::m_0p5;
		unsigned int    *polyTable,ii,jj;
		unsigned char   *edgetable;

		const unsigned int totalnumberofcube =  nbX * nbY * nbZ ;

		unsigned char *allocptr;
		m_pMainTableAlloc = allocptr =new unsigned char[ 32 +
							totalnumberofcube * (
							sizeof(float)+
							sizeof(MarchCubeExtraValue)+
							sizeof(unsigned int)+
							sizeof(MarchCubeIndex)+
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
		allocptr = (unsigned char *)(((uintptr_t)(allocptr+31)) & ~(uintptr_t)0x1F) ;
		m_pCubeTable_Values = (float *)allocptr;
		m_pCubeTable_ExtraValue = (MarchCubeExtraValue *)&(m_pCubeTable_Values[totalnumberofcube]);
		m_pCubeTable_CheckDate = (unsigned int *)&(m_pCubeTable_ExtraValue[totalnumberofcube]);
		m_pCubeTable_Index = (MarchCubeIndex *)&(m_pCubeTable_CheckDate[ totalnumberofcube ]);
		m_pCubeTable_Flags = (unsigned char *)&(m_pCubeTable_Index[totalnumberofcube]);

		nbZ--;

		for(ii=0; ii<totalnumberofcube ; ii++)
		{

			m_pCubeTable_CheckDate[ii]=0;

		}

#define pX  1
#define pY  nbX
#define pZ  (nbX*nbY)
		edgetable = allocTable1 + (512 * ((12*2)+1)) ;

		unsigned int    tredge[12];
		unsigned int	*pfill = &(tredge[0]);

		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pX		+ ofs_z	;
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*(pX+pZ)	+ ofs_y	;
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pX		+ ofs_y	;
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*(pX+pY)	+ ofs_z	;

		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*0		+ ofs_z	;
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pZ		+ ofs_y	;
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*0		+ ofs_y	;
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pY		+ ofs_z	;

		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*0		+ ofs_x	;
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pZ		+ ofs_x	;
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*pY		+ ofs_x	;
		*(pfill++)	=	sizeof(MarchinCubeSpace::MarchCubeIndex)*(pY+pZ)	+ ofs_x	;

		for(ii=0 ; ii<256 ; ii++)
		{

			*(polyTable++) = *(edgetable++) ;
			for( jj=0 ; jj<15 ; jj++ )
			{
				*(polyTable++) = tredge[ *(edgetable++)  ] ;
			}
		}

	}

	delete allocTable1;
	return true;
}
