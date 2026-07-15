/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "BubbleGreyImage.h"

#include <math.h>
#ifndef sqrtf
#define sqrtf sqrt
#endif

BASEOBJECT_DECLARE_CLASS( "Bub", BubbleGreyImage, ProceduralImage,"Image Grey Bubble","BubbleImg","Defines a grey image using a bubble algo, with a 2PowerN square size." );

BubbleGreyImage::BubbleGreyImage() : ProceduralImage()
{
	REGISTER_MEMBER_FLAG( mSer_Flags ,"Flags",0,"ByRegion.Multiply" );
	//TODO ! it is declared serialized to finalize the serialization shape:
	REGISTER_MEMBER_REFERENCE(mSer_SubImage,0L,VirtualImage::m_Description);	
}

/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
			It uses a faaast fractal-building perlin noise algorithm: all pixels
			are written Once, and 1 bitmap is used.
		\return false if creation failed.
*/
bool BubbleGreyImage::CreateInternal(void)
{
	if( !ProceduralImage::CreateInternal() ) return(false); // alloc and reset random seed.

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
    mask = width_height = GetPixelWidth(); // == pixel_Height
    mask--; // 256 ->255

    // "recursion" is used as the number of subdivided squares:

	subSquareLength = mSer_2nPixelSize.Get() -
						mSer_Recursion.Get() ;  // 1->10 ->can be negative if recursion stronger than size.

	if( subSquareLength<0) subSquareLength=0;	// bug found by TEX/MANKIND 08/12/2005 .


	recurseforce = mSer_Recursion.Get() - mSer_2nPixelSize.Get() +8;

	subSquareLength = subSquareMask = (1<< subSquareLength);
	subSquareMask--;

	// mix one time the random seed: then a constant random number
	// will be find for all region of the image, using that randomroot.
	 mSer_RandomSeed.GetRandomNumber();
	m_RandImageSeed = (int)( mSer_RandomSeed.GetRandomNumber() );
//--------------------------------------
	// shift half the image size, to center a circle when recurse=0 (1 bubble)
	width_height>>=1;
	for(yy=-width_height ; yy<width_height ; yy++ )
	{
		for(xx=-width_height ; xx<width_height ; xx++)
		{	
			// for each pixels test distance to the 9 neighbours squares centers:
			distA = 255; // search minimum distance
			regioncolor=0; // also search region color.
            for(ofy=-subSquareLength ; ofy<=subSquareLength ; ofy+=subSquareLength )
            {
				for(ofx=-subSquareLength ; ofx<=subSquareLength ; ofx+=subSquareLength )
				{		// for each subquarters, the center got an unique x/y scroll offset.
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
					{	// get nearest point:

						if( distB<distA )
						{
							distA= distB ;
							regioncolor = vv2;
						}
					}
				}

			}
			// if flag on, print an unique color by region:
			if(  (flag & BUBFLAG_OneColorByRegion) )
				*(chunkbase++)= (unsigned char)regioncolor ;
			else // shade normal version:
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

// note: Close() is defined in VirtualImage.
/*
int		BubbleGreyImage::FindDistance( int distx, int disty )
{
	return(( (int)sqrtf( distx*distx + disty*disty  ) ));
}
*/
unsigned int		BubbleGreyImage::FindShiftX( int distx, int disty )
{
	return( (((distx*	m_RandImageSeed ) + (7935456*disty))>>16) );
}
unsigned int		BubbleGreyImage::FindShiftY( int distx, int disty )
{
	return( (((disty*8946567) + (m_RandImageSeed*distx))>>16) );

}
