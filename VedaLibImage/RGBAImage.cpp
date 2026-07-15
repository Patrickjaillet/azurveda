/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "RGBAImage.h"

#include "PackFloat.h"
#include "GreyImage.h"
#include "VirtualEquation.h"
BASEOBJECT_DECLARE_CLASS("rgba", RGBAImage, VirtualImage,"Image RGBA Mixer","ColorImage","Creates a Red-Green-Blue-Alpha layered color image by mixing other images." );

RGBAImage::RGBAImage() : VirtualImage()
	,mSer_LayerList(NewImageLayer)
{ 
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_PixelWidth ,"Pixel Width",256,1,512 );
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_PixelHeight,"Pixel Height",256,1,512 );

	REGISTER_MEMBER( mSer_LayerList,"Layer List" );

}
RGBAImage::ImageLayer::ImageLayer() : PackStruct()
{
	REGISTER_MEMBER_FLAG(mSer_LayerFlag,"Layer",0,"Red.Green.Blue.Alpha.ToHSV.Multiply.AlphaOnLayer");
	REGISTER_MEMBER_REFERENCE(mSerRef_ComponentImage ,"Image", VirtualImage::m_Description );
	REGISTER_MEMBER_REFERENCE(mSerRef_Curve ,"Curve", VirtualEquation::m_Description );
}

/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
			Mix one or more GreyImage into a RGBA bitmap.
		\return false if creation failed.
*/
bool RGBAImage::CreateInternal(void)
{		
	// alloc bitmap, and exit if already constructed:
	if( !VirtualImage::CreateInternal()) return(false);

	int	xx,yy;
	int width = GetPixelWidth() ;
	int height = GetPixelHeight();

	register unsigned char *pbuffer= m_pBuffer ;
	// first, set 0,0,0,255 everywhere:
	yy = width*height /* *GetByteDepth() */;
	for(xx =0 ; xx<yy ; xx++) 
	{
		*(pbuffer++) =0;
		*(pbuffer++) =0;
		*(pbuffer++) =0;
		*(pbuffer++) = 255;
	}
	PackList::Cell *pcell = mSer_LayerList.GetFirstCell();

	//unsigned char *pCurveMatrix; // recalibration curve.
	//pCurveMatrix = new unsigned char[256];
	//if( !pCurveMatrix ) return false;
	unsigned char tCurveMatrix[256];
	// loop by layer
	while(pcell)
	{
		unsigned int			rr,nr,alpha;
			int	zz;
		unsigned char	*pSourceImageBuffer=0L,*pSourceImageLineBuffer,*pSourcePixel;
		ImageLayer *pLayer = (ImageLayer *)pcell->GetManagedObject();
		unsigned int flag				=	pLayer->mSer_LayerFlag.Get() ;
		VirtualImage	*pSourceImage		=	(VirtualImage *)pLayer->mSerRef_ComponentImage.GetObjectPointer();
		VirtualEquation *pSourceCurve	=	(VirtualEquation *)pLayer->mSerRef_Curve.GetObjectPointer();

		int	sourceWidth,sourceDepth; 
		int	vecAddVertical,vecAddHorizontal,vecHorizontal,vecVertical;

		char componentIndex[4]={0,1,2,3};
		sourceWidth = vecAddVertical = vecAddHorizontal = vecHorizontal = vecVertical = sourceDepth = 0 ;
		//an image can be linked or not
		if( pSourceImage )
		{
			sourceDepth =  pSourceImage->GetByteDepth() ;
			sourceWidth = pSourceImage->GetPixelWidth() ;
			vecAddHorizontal = (sourceWidth<<16) / width ;
			sourceWidth *=sourceDepth;
			vecAddVertical = (pSourceImage->GetPixelHeight()<<16) / height ;
			pSourceImageBuffer = pSourceImage->GetBuffer();
			if(sourceDepth == 1 )
			{
				componentIndex[1] =
				componentIndex[2] =
				componentIndex[3] = 0;
			}
		}
		// a recalibration curve can be linked or not:
		float	vv[4];
		float   result[4];
		vv[0] = vv[3] = PackFloat::m_0p0;
		for( xx=0 ; xx < 256 ; xx++ )
		{	
			yy = xx;
			if(pSourceCurve )
			{
				result[0] = PackFloat::m_0p0;
				pSourceCurve->Compute(result, vv );
				yy =(int)( result[0] * PackFloat::m_255p0 );
				if( yy>255) yy=255;
				if( yy<0) yy=0;
			}
			tCurveMatrix[xx] = yy;
			vv[0] += PackFloat::m_1Div255 ;
			vv[3] = vv[0];
		}

		pbuffer = m_pBuffer;
		for(yy =0 ; yy<height ; yy++)
		{	// non sense if pSourceImageBuffer == NULL.
			pSourceImageLineBuffer = pSourceImageBuffer + ((vecVertical>>16)* sourceWidth )  ;
			vecHorizontal = 0;
			// by pixels:
			for(xx =0 ; xx<width; xx++)
			{	
				if( pSourceImageBuffer ) 
				{	// find scaled source pixel location:
					pSourcePixel = pSourceImageLineBuffer + (vecHorizontal>>16)*sourceDepth ;
					// find alpha value:
					alpha =  pSourcePixel[ componentIndex[3] ] ;
					alpha = tCurveMatrix[ alpha ];
				}
				// loop by component

				for(zz=3 ; zz>=0 ; zz--) // do alpha first.
				{
					if(flag & (1<<zz)) // if it applies to this component:
					{
						rr = pbuffer[zz];
						nr = 0;
						if( pSourceImageBuffer ) // if an image source is linked for this layer:
						{					
							nr =  pSourcePixel[ componentIndex[zz] ] ;
							nr = tCurveMatrix[ nr ];
							//
						} else //if no image and a curve, apply curve to previous layers:
							rr = tCurveMatrix[ rr ];
						//if( nr )
						//{
							if( flag & LayerFlag_MulAlpha )
							{
								rr = (((rr*(255-alpha))+ (alpha*nr))>>8);
							} else
								if(  flag & LayerFlag_AddMul )
									rr = (rr * (nr+1))>>8;
								else rr += nr;
						//} 
						
						if( rr>255 ) rr=255;
						pbuffer[zz] = rr;	
					}
				}
				// if RGB to hsv:
				if(flag & LayerFlag_HSV) TslToRgb( pbuffer[0], pbuffer[1], pbuffer[2], pbuffer);
	
				pbuffer+=4;
				vecHorizontal	+= vecAddHorizontal ;
	
			} // end width loop
			vecVertical += vecAddVertical;
		} // end height loop
		pcell = pcell->GetNextBrotherCell();
	} // end loop by layer

//	delete [] pCurveMatrix;

	return(true);
}

// note: Close() is defined in VirtualImage.
/*!
	\brief	Return the pixel width of the image
*/
unsigned int	RGBAImage::GetPixelWidth()
{
	return( mSer_PixelWidth.Get() );
}
/*!
	\brief	Return the pixel height of the image
*/
unsigned int	RGBAImage::GetPixelHeight()
{
	return( mSer_PixelHeight.Get() );
}
void    RGBAImage::TslToRgb(  unsigned int t,unsigned int  s,unsigned int  l, unsigned char *R )
{
/* t [0,511]<<2  s[0,255]  l[0,255]   */

register int     rr,gg,bb;
register int     ii,f,p,q,x;

        if ( s == 0 )   /* ACHROMATIC case */
        {
            rr = l;
            gg = l;
            bb = l;
        } else      /* CHROMATIC case */
        {
            /* t[0,63] -> [0,185] 185=31*6 */
            /* t[0,511] ->[0,1530] 1530=255*6 */
            //t2 = (t*185) /63 ;
            t = ((t & 255)*1530) /255 ;     /* t2 [0,1530]  */

            ii = t>>8;  /* [0,5]*/

            f = t - (ii<<8); /* f [0,255]  partie fractionnelle */

            p = l * (255 - s) /255;
            q = l * ((255*255)- (s*f)) / (255*255) ;   /* 930=30*31  */
            x = l * ((255*255)- (s*(255-f)))/(255*255) ;

                rr = l;
                gg = l;
                bb = l;

            switch(ii) {
                case 0:     /* Rouge -> Jaune */
                    rr =l;
                    gg =x;
                    bb =p;
                    break;
                case 1:     /* Jaune -> Vert A CHIER */
                    rr =q;
                    gg =l;
                    bb =p;
                    break;
                case 2:     /* Vert -> Cyan */
                    rr =p;
                    gg =l;
                    bb =x;
                    break;
                case 3:     /* Cyan -> Bleu */
                    rr =p;
                    gg =q;
                    bb =l;
                    break;
                case 4:     /* Bleu -> Magenta */
                    rr =x;
                    gg =p;
                    bb =l;
                    break;
                case 5:     /* Magenta -> Rouge */
                    rr =l;
                    gg =p;
                    bb =q;
                    break;
                }
        } /* end case chromatic */

        *(R++)= rr;
        *(R++)= gg;
        *(R++)= bb;

}
///

