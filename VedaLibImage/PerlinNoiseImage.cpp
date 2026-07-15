// SPDX-License-Identifier: LGPL-2.1-only

#include "PerlinNoiseImage.h"

BASEOBJECT_DECLARE_CLASS( "Noise", PerlinNoiseImage, ProceduralImage,"Image Grey Noise","NoiseImg","Defines a grey image using a noise algorythm, with a 2PowerN square size." );

PerlinNoiseImage::PerlinNoiseImage() : ProceduralImage()
{

}
unsigned char PerlinNoiseImage::GetPerlinMergeRandom( int vv, unsigned char uforce  )
{

    if(uforce >=8)
        return( vv>>2  );
    else
        return(
                (unsigned char)(
                (
				(( mSer_RandomSeed.GetRandomNumber() & 255) * (8-uforce))+
                ((vv>>2) * (uforce))
                )>>3
                )
              );
}

bool PerlinNoiseImage::CreateInternal(void)
{

	if( !ProceduralImage::CreateInternal() ) return(false);

	unsigned char *pchunkbase=m_pBuffer;
	unsigned int    mask,offset=1,middle,ofx,ofy;
    int             width_height,ll,vv,vv2,force,xx,yy;

    *(pchunkbase) = 128 ;
	ll =  mask = width_height = GetPixelWidth();
    mask--;

	force = mSer_Recursion.Get() ;

    while( ll>1 )
    {
        middle = ll>>1;

        for( yy=0 ; yy<width_height ; yy+=ll)
        {
            for( xx=0 ; xx<width_height ; xx+=ll)
            {

                    ofx = ((xx+ll )& mask ) ;
                    ofy = ((yy+ll )& mask ) * width_height ;
                    vv  = pchunkbase[ ofx +ofy ] ;
                    vv += pchunkbase[ xx  +ofy ] ;

                    ofy = yy * width_height ;
                    vv += pchunkbase[ ofx +ofy ] ;
                    vv += pchunkbase[  xx +ofy ] ;

                ofx = (xx+middle ) ;
                ofy = (yy+middle ) *width_height ;
                pchunkbase[ ofx + ofy ] = GetPerlinMergeRandom(vv,force);
            }
        }

        for( yy=0 ; yy<width_height ; yy+=ll)
        {
            for( xx=0 ; xx<width_height ; xx+=ll)
            {

                    ofy =  yy * width_height ;
                vv2 = pchunkbase[ xx + ofy ];
                    ofy =  (yy+middle) * width_height ;
                vv2 += pchunkbase[ xx + middle + ofy ];

                    ofx = ((xx+ll )& mask ) ;
                    ofy =  yy * width_height ;
                    vv= vv2 + pchunkbase[ ofx +ofy ] ;

                    ofx = xx + middle ;
                    ofy =  ((yy-middle)& mask) * width_height ;
                    vv += pchunkbase[ ofx +ofy ] ;

                ofy = yy *width_height ;
                pchunkbase[ ofx  + ofy ] = GetPerlinMergeRandom(vv,force);

                    ofy =  ((yy+ll)& mask) * width_height ;
                    vv2+= pchunkbase[ xx +ofy ] ;

                    ofx =  ((xx-middle)& mask) ;
                    ofy =  (yy+middle) * width_height ;
                    vv2+=  pchunkbase[ ofx +ofy ] ;

                pchunkbase[ xx + ofy ] =  GetPerlinMergeRandom(vv2,force);
            }
        }

        ll = middle ;
        offset = offset + offset ;
        force++;
    }

	return(true);
}
