/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "ImplicitEquationImage.h"
#include "VirtualEquation.h"
#include "PackFloat.h"

BASEOBJECT_DECLARE_CLASS( "Equ", ImplicitEquationImage, VirtualImage,"Image By Equation","EquationImg","Define a grey or color image with a (X,Y,time) equation." );

ImplicitEquationImage::ImplicitEquationImage() : VirtualImage()
{
	//! preference flags about the way the quation is used.
	REGISTER_MEMBER_FLAG(mSer_EquaFlags,"Flags",0,"timedynamic")
	REGISTER_MEMBER_ENUMS(mSer_ResultTypeEnum,"ResultType",0,"X To Grey.XYZD To RGBA")
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_PixelWidth ,"Pixel Width",256,1,512 );
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_PixelHeight,"Pixel Height",256,1,512 );
	REGISTER_MEMBER_REFERENCE( mSerRef_Equation ,"Equation", VirtualEquation::m_Description );
}
/*!
	\brief	Return the number of byte component for one pixel.
		This one is no more a virtual method.
*/
unsigned int ImplicitEquationImage::GetByteDepth()
{
	switch(mSer_ResultTypeEnum.Get())
	{
		case etype_XToGrey:
			return 1;
		default: //shouldn't happen
			return 4; // all other are RGBA.	
	}
}
/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
			Create the whole bitmap using one equation.
		\return false if creation failed.
*/
bool ImplicitEquationImage::CreateInternal(void)
{
	// do super call, that allocs the bitmap:
	// note that GetByteDepth() if it was allocated for grey or RGBA:
	if( !VirtualImage::CreateInternal()) return(false);
	//register xx
	// declare we can be time dynamic.
	m_IsTimeDynamic = mSer_EquaFlags.TestFlags(eflag_TimeDynamic);
	ProtectedDraw(0.0f);

	return(true);
}

// note: Close() is defined in VirtualImage.
/*!
	\brief	Return the pixel width of the image
*/
unsigned int	ImplicitEquationImage::GetPixelWidth()
{
	return( mSer_PixelWidth.Get() );
}
/*!
	\brief	Return the pixel height of the image
*/
unsigned int	ImplicitEquationImage::GetPixelHeight()
{
	return( mSer_PixelHeight.Get() );
}
/*
	\brief	If IsTimeDynamic() is true, this image can change with time.
			You have to use UpdateToFrame()to make the image change to a given date.
*/
void	ImplicitEquationImage::UpdateToFrame(float _framedate)
{
	if(!m_IsTimeDynamic) return;
	if(m_CurrentFrameDate == _framedate) return; // already updated.
		ProtectedDraw(_framedate);
	m_CurrentFrameDate = _framedate ;

}
/*
	\brief the real draw method is kept protected, because used by construction and update:
*/
void	ImplicitEquationImage::ProtectedDraw(float _framedate)
{
	register double fx,fy,fxd,fyd;
	register unsigned int xx,yy,xl,yl;
	register unsigned char *pbuffer = m_pBuffer ; // alloc done by superclass.
	fx = fy = 0.0;
	fxd = fyd = 1.0;
	xl = GetPixelWidth();
	yl = GetPixelHeight();
	fxd /= (double) xl;
	fyd /= (double) yl;

	VirtualEquation	*pEqu = (VirtualEquation *)mSerRef_Equation.GetObjectPointer();
	if( !pEqu ) return; // an object can just not be linked.
	float	paramTable[4];
	paramTable[2]=0.0f; // Z is not used, we are 2D.
	paramTable[3]=_framedate ; // we can be time dynamic:

	float	resultTable[4];
	// default if equation let untouch:
	resultTable[0] =  // RGB to zero.
	resultTable[1] =  
	resultTable[2] = 0.0f; 
	resultTable[3] = 1.0f; // alpha channel default!

	for(yy=0 ; yy < yl ; yy++ )
	{
		paramTable[1] = (float)fy;
		fx = 0.0;
		// we do a pixel format switch at this level:
		if(mSer_ResultTypeEnum.Get()== etype_XToGrey)
		{
			for(xx=0 ; xx < xl ; xx++ )
			{
				paramTable[0] =(float) fx;
				pEqu->Compute( resultTable , paramTable );		
				int intval = (int) ( resultTable[0] * PackFloat::m_255p0 ) ;
				if(intval<0) intval=0;
				else if(intval>255) intval=255;
				*(pbuffer++) =  (unsigned char) intval ;
				fx += fxd;
			}			
		} else // all other format is RGBA.
		{
			register int intval;
			const float v0 = PackFloat::m_0p0;
			const float v1 = PackFloat::m_1p0;
			const float v255 = PackFloat::m_255p0;
			register float fval;
			for(xx=0 ; xx < xl ; xx++ )
			{
				// OMFG It needs for OPTIMISATION here !
				paramTable[0] =(float) fx;
				// virtual call to any type of equation !!
				pEqu->Compute( resultTable , paramTable );		
				
				fval = resultTable[0] ;
				if(fval<=v0) *(pbuffer++) = 0;
				else if(fval>=v1) *(pbuffer++) = 255;
				else{
					 *(pbuffer++) = (int) ( resultTable[0] * v255 ) ;
				}
				fval = resultTable[1] ;
				if(fval<=v0) *(pbuffer++) = 0;
				else if(fval>=v1) *(pbuffer++) = 255;
				else{
					 *(pbuffer++) = (int) ( resultTable[0] * v255 ) ;
				}
				fval = resultTable[2] ;
				if(fval<=v0) *(pbuffer++) = 0;
				else if(fval>=v1) *(pbuffer++) = 255;
				else{
					 *(pbuffer++) = (int) ( resultTable[0] * v255 ) ;
				}
				fval = resultTable[3] ;
				if(fval<=v0) *(pbuffer++) = 0;
				else if(fval>=v1) *(pbuffer++) = 255;
				else{
					 *(pbuffer++) = (int) ( resultTable[0] * v255 ) ;
				}
				fx += fxd;
			}			
		}
		// next line
		fy += fyd;
	} // end of Y loop
}

