/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
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
        return(     //GetRnd()
                (unsigned char)(
                (
				(( mSer_RandomSeed.GetRandomNumber() & 255) * (8-uforce))+
                ((vv>>2) * (uforce))
                )>>3
                )
              );
}
/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
			It uses a faaast fractal-building perlin noise algorithm: all pixels
			are written Once, and 1 bitmap is used.
		\return false if creation failed.
*/
bool PerlinNoiseImage::CreateInternal(void)
{
	// important: do that for every objects:
	if( !ProceduralImage::CreateInternal() ) return(false); // alloc and reset random seed.

	register	unsigned char *pchunkbase=m_pBuffer;
	register	unsigned int    mask,offset=1,middle,ofx,ofy;
    register	int             width_height,ll,vv,vv2,force,xx,yy;

    *(pchunkbase) = 128 ;  // always start with middle value at corner.
	ll =  mask = width_height = GetPixelWidth();
    mask--; // 256 ->255

    // check random force by depth:
	force = mSer_Recursion.Get() ;
 
    while( ll>1 )
    {
        middle = ll>>1;
        //--------- cross "X" 1 pixel/square.
        for( yy=0 ; yy<width_height ; yy+=ll)
        {
            for( xx=0 ; xx<width_height ; xx+=ll)
            {
                // -> SHORTER !

                    // short version:
                    ofx = ((xx+ll )& mask ) ;
                    ofy = ((yy+ll )& mask ) * width_height ;
                    vv  = pchunkbase[ ofx +ofy ] ;
                    vv += pchunkbase[ xx  +ofy ] ;

                    ofy = yy * width_height ;
                    vv += pchunkbase[ ofx +ofy ] ;
                    vv += pchunkbase[  xx +ofy ] ;
                    
                // write pix:
                ofx = (xx+middle ) ;
                ofy = (yy+middle ) *width_height ;
                pchunkbase[ ofx + ofy ] = GetPerlinMergeRandom(vv,force); //vv>>2;
            }
        }
        //--------- cross "+" 2 pixel/square.
        for( yy=0 ; yy<width_height ; yy+=ll)
        {
            for( xx=0 ; xx<width_height ; xx+=ll)
            {
                //--- 2 value common: (no and needed.)
                    ofy =  yy * width_height ;
                vv2 = pchunkbase[ xx + ofy ];
                    ofy =  (yy+middle) * width_height ;
                vv2 += pchunkbase[ xx + middle + ofy ];
                //------------- left
                    ofx = ((xx+ll )& mask ) ;
                    ofy =  yy * width_height ;
                    vv= vv2 + pchunkbase[ ofx +ofy ] ; // more left

                    ofx = xx + middle ;
                    ofy =  ((yy-middle)& mask) * width_height ;
                    vv += pchunkbase[ ofx +ofy ] ; // up.

                // write "+" pix 1: left
                ofy = yy *width_height ;
                pchunkbase[ ofx /*xx+middle*/ + ofy ] = GetPerlinMergeRandom(vv,force); //vv>>2;
                //------------- point down
                    ofy =  ((yy+ll)& mask) * width_height ;
                    vv2+= pchunkbase[ xx +ofy ] ; // more down

                    ofx =  ((xx-middle)& mask) ;
                    ofy =  (yy+middle) * width_height ;
                    vv2+=  pchunkbase[ ofx +ofy ] ; // more right.
                // write "+" pix 1: down
                // done. ofy = (yy+middle ) *width_height ;
                pchunkbase[ xx + ofy ] =  GetPerlinMergeRandom(vv2,force); //vv2>>2;
            }
        }
        //---- next dimension
        ll = middle ;   // going >>1
        offset = offset + offset ;  // going <<1
        force++;    // less random.
    }
	

	return(true);
}

// note: Close() is defined in VirtualImage.
