// SPDX-License-Identifier: LGPL-2.1-only

#include "BubbleGreyImage.h"

#include <math.h>
#ifndef sqrtf
#define sqrtf sqrt
#endif

BASEOBJECT_DECLARE_CLASS( "Bub", BubbleGreyImage, ProceduralImage,"Image Grey Bubble","BubbleImg","Defines a grey image using a bubble algo, with a 2PowerN square size." );

BubbleGreyImage::BubbleGreyImage() : ProceduralImage()
{
	REGISTER_MEMBER_FLAG( mSer_Flags ,"Flags",0,"ByRegion.Multiply" );

	REGISTER_MEMBER_REFERENCE(mSer_SubImage,0L,VirtualImage::m_Description);
}

bool BubbleGreyImage::CreateInternal(void)
{
	if( !ProceduralImage::CreateInternal() ) return(false);

	unsigned int    mask;
	int				distA,distB;
	int				xx,yy,erx,ery,xm,ym;
    int             regioncolor,vv2;
	int				subSquareLength,subSquareMask;
    int				ofx,ofy;
	int				width_height;
	int			recurseforce;
	unsigned char	*chunkbase = m_pBuffer;
	int				flag = mSer_Flags.Get();
    mask = width_height = GetPixelWidth();
    mask--;

	subSquareLength = mSer_2nPixelSize.Get() -
						mSer_Recursion.Get() ;

	if( subSquareLength<0) subSquareLength=0;

	recurseforce = mSer_Recursion.Get() - mSer_2nPixelSize.Get() +8;

	subSquareLength = subSquareMask = (1<< subSquareLength);
	subSquareMask--;

	 mSer_RandomSeed.GetRandomNumber();
	m_RandImageSeed = (int)( mSer_RandomSeed.GetRandomNumber() );

	width_height>>=1;
	for(yy=-width_height ; yy<width_height ; yy++ )
	{
		for(xx=-width_height ; xx<width_height ; xx++)
		{

			distA = 255;
			regioncolor=0;
            for(ofy=-subSquareLength ; ofy<=subSquareLength ; ofy+=subSquareLength )
            {
				for(ofx=-subSquareLength ; ofx<=subSquareLength ; ofx+=subSquareLength )
				{
        	        		xm = xx-(xx & subSquareMask)+ofx;
	        	        	ym = yy-(yy & subSquareMask)+ofy;
                            vv2 =FindShiftX(xm & mask,ym & mask);
							erx = xx - xm -((vv2)&subSquareMask );
                            ery = yy - ym -((FindShiftY(xm & mask,ym & mask))&subSquareMask );
					distB = (int)sqrtf( (float)(erx*erx + ery*ery)  );
					if( flag & BUBFLAG_Multiply )
					{
						distB <<= recurseforce ;
						if( distB>256 ) distB = 256;
						distA *= distB;
						distA>>=8;
					} else
					{

						if( distB<distA )
						{
							distA= distB ;
							regioncolor = vv2;
						}
					}
				}

			}

			if(  (flag & BUBFLAG_OneColorByRegion) )
				*(chunkbase++)= (unsigned char)regioncolor ;
			else
			if( !( flag & BUBFLAG_Multiply ))
			{
				distA <<= recurseforce ;
				if(distA>255) distA= 255;
				*(chunkbase++)= (unsigned char)distA;
			}
			else *(chunkbase++)= (unsigned char)distA;
		}
	}
	return(true);
}

unsigned int		BubbleGreyImage::FindShiftX( int distx, int disty )
{
	return( (((distx*	m_RandImageSeed ) + (7935456*disty))>>16) );
}
unsigned int		BubbleGreyImage::FindShiftY( int distx, int disty )
{
	return( (((disty*8946567) + (m_RandImageSeed*distx))>>16) );

}
