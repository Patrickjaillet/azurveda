// SPDX-License-Identifier: LGPL-2.1-only

#include "ImplicitEquationImage.h"
#include "VirtualEquation.h"
#include "PackFloat.h"

BASEOBJECT_DECLARE_CLASS( "Equ", ImplicitEquationImage, VirtualImage,"Image By Equation","EquationImg","Define a grey or color image with a (X,Y,time) equation." );

ImplicitEquationImage::ImplicitEquationImage() : VirtualImage()
{

	REGISTER_MEMBER_FLAG(mSer_EquaFlags,"Flags",0,"timedynamic")
	REGISTER_MEMBER_ENUMS(mSer_ResultTypeEnum,"ResultType",0,"X To Grey.XYZD To RGBA")
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_PixelWidth ,"Pixel Width",256,1,512 );
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_PixelHeight,"Pixel Height",256,1,512 );
	REGISTER_MEMBER_REFERENCE( mSerRef_Equation ,"Equation", VirtualEquation::m_Description );
}

unsigned int ImplicitEquationImage::GetByteDepth()
{
	switch(mSer_ResultTypeEnum.Get())
	{
		case etype_XToGrey:
			return 1;
		default:
			return 4;
	}
}

bool ImplicitEquationImage::CreateInternal(void)
{

	if( !VirtualImage::CreateInternal()) return(false);

	m_IsTimeDynamic = mSer_EquaFlags.TestFlags(eflag_TimeDynamic);
	ProtectedDraw(0.0f);

	return(true);
}

unsigned int	ImplicitEquationImage::GetPixelWidth()
{
	return( mSer_PixelWidth.Get() );
}

unsigned int	ImplicitEquationImage::GetPixelHeight()
{
	return( mSer_PixelHeight.Get() );
}

void	ImplicitEquationImage::UpdateToFrame(float _framedate)
{
	if(!m_IsTimeDynamic) return;
	if(m_CurrentFrameDate == _framedate) return;
		ProtectedDraw(_framedate);
	m_CurrentFrameDate = _framedate ;

}

void	ImplicitEquationImage::ProtectedDraw(float _framedate)
{
	double fx,fy,fxd,fyd;
	unsigned int xx,yy,xl,yl;
	unsigned char *pbuffer = m_pBuffer ;
	fx = fy = 0.0;
	fxd = fyd = 1.0;
	xl = GetPixelWidth();
	yl = GetPixelHeight();
	fxd /= (double) xl;
	fyd /= (double) yl;

	VirtualEquation	*pEqu = (VirtualEquation *)mSerRef_Equation.GetObjectPointer();
	if( !pEqu ) return;
	float	paramTable[4];
	paramTable[2]=0.0f;
	paramTable[3]=_framedate ;

	float	resultTable[4];

	resultTable[0] =
	resultTable[1] =
	resultTable[2] = 0.0f;
	resultTable[3] = 1.0f;

	for(yy=0 ; yy < yl ; yy++ )
	{
		paramTable[1] = (float)fy;
		fx = 0.0;

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
		} else
		{
			int intval;
			const float v0 = PackFloat::m_0p0;
			const float v1 = PackFloat::m_1p0;
			const float v255 = PackFloat::m_255p0;
			float fval;
			for(xx=0 ; xx < xl ; xx++ )
			{

				paramTable[0] =(float) fx;

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

		fy += fyd;
	}
}
