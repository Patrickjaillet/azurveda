// SPDX-License-Identifier: LGPL-2.1-only

#include "Texture3D.h"

#include "VirtualImage.h"
#include "VirtualMachine.h"

#ifdef _ENGINE_EDITABLE_

#include <math.h>
#endif

BASEOBJECT_DECLARE_CLASS( "t3d", Texture3D, BaseObject,"3D Texture","Texture","Defines a render type for 3D Objects." );

Texture3D::Texture3D() : BaseObject()
	,m_pInternalTexture(0L)
	,mSer_BaseColor(PackFloat::vd_XYZD)
#ifdef _ENGINE_EDITABLE_

	,m_pPreviewObject3D(0L)
#endif
{

	REGISTER_MEMBER_FLAG( mSer_Flags ,"Properties",
				VirtualMachine::InternalTexture::itf_Filtered |
				VirtualMachine::InternalTexture::itf_depthwrite|
				VirtualMachine::InternalTexture::itf_depthtest,
		"Filtered.Additive.EnvAdd.ClipU.ClipV.Z Write.Z Test.Substractive.doubleSide" );
	REGISTER_MEMBER(mSer_BaseColor,"BaseColor");
#ifdef _ENGINE_EDITABLE_

	mSer_BaseColor.SetConstant(1.0f,0);
	mSer_BaseColor.SetConstant(1.0f,1);
	mSer_BaseColor.SetConstant(1.0f,2);
	mSer_BaseColor.SetConstant(1.0f,3);
#endif
	REGISTER_MEMBER_REFERENCE( mSerRef_MappingImage ,"Map Image", VirtualImage::m_Description );
	REGISTER_MEMBER_REFERENCE( mSerRef_EnvImage ,"Env Image", VirtualImage::m_Description );
}

bool Texture3D::CreateInternal(void)
{
	GetMachine()->NewTexture( &m_pInternalTexture );
	if( !m_pInternalTexture ) return false;

	VirtualImage *pImg = (VirtualImage *)mSerRef_MappingImage.GetObjectPointer();
	if( pImg ) m_pInternalTexture->SetImageBitmap( VirtualMachine::InternalTexture::itile_TextureMapping,
				pImg->GetBuffer(),
				pImg->GetByteDepth(),
				pImg->GetPixelWidth(),
				pImg->GetPixelHeight()
				);
	pImg = (VirtualImage *)mSerRef_EnvImage.GetObjectPointer();
	if( pImg ) m_pInternalTexture->SetImageBitmap( VirtualMachine::InternalTexture::itile_Environment,
				pImg->GetBuffer(),
				pImg->GetByteDepth(),
				pImg->GetPixelWidth(),
				pImg->GetPixelHeight()
				);

	m_pInternalTexture->SetRenderFlags( mSer_Flags.Get() );

#ifdef _ENGINE_EDITABLE_

	GetMachine()->NewObject3DBuffer( &m_pPreviewObject3D,
		(m_MaxNumberOfGridDivisionInWidth+1)*(m_MaxNumberOfGridDivisionInHeight+1),
		(m_MaxNumberOfGridDivisionInWidth)*(m_MaxNumberOfGridDivisionInHeight)*2,
		VirtualMachine::bOb3D_VertexNormal|VirtualMachine::bOb3D_VertexUVMapping
		);
#endif
	m_LastUpdateDate = PackFloat::m_Max;
	UpdateToFrame(PackFloat::m_0p0);
	return(true);
}
#ifdef _ENGINE_EDITABLE_

void Texture3D::CloseInternal(void)
{
	GetMachine()->DeleteTexture( &m_pInternalTexture );
#ifdef _ENGINE_EDITABLE_

	GetMachine()->DeleteObject3DBuffer( &m_pPreviewObject3D );
#endif
}
#endif
#ifdef _ENGINE_EDITABLE_

void Texture3D::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	if(!m_pPreviewObject3D ) return;

	VirtualMachine::InternalVertex *pVertexFill = m_pPreviewObject3D->GetFirstVertex();
	float opx,opy,opz;
	float objectRotAxeX,objectRotAxeY,objectRotAxeZ;
	_pPreviewConfiguration->GetPreviewPosition( opx,opy,opz);
	if(opx<-1.0f)opx= -1.0f;
	if(opx>1.0f) opx= 1.0f;
	if(opy<-1.0f)opy= -1.0f;
	if(opy>1.0f) opy= 1.0f;
	if(opz<-2.0f)opz= -2.0f;
	if(opz>0.5f) opz= 0.5f;

	_pPreviewConfiguration->GetPreviewRotation( objectRotAxeX,objectRotAxeY,objectRotAxeZ);
	unsigned int xx,yy;
	const float v1 = PackFloat::m_1p0;
	float	idiv = v1/65536.0f;
	for(yy=0;yy<=m_MaxNumberOfGridDivisionInHeight; yy++)
	{
		for(xx=0;xx<=m_MaxNumberOfGridDivisionInWidth ; xx++)
		{
			pVertexFill->m_ColorRGBA[0]=v1;
			pVertexFill->m_ColorRGBA[1]=v1;
			pVertexFill->m_ColorRGBA[2]=v1;
			pVertexFill->m_ColorRGBA[3]=v1;
			pVertexFill->m_x = -0.5f + ((float)((xx<<16)/m_MaxNumberOfGridDivisionInWidth))*idiv ;
			pVertexFill->m_y = -0.5f + ((float)((yy<<16)/m_MaxNumberOfGridDivisionInHeight))*idiv ;
			pVertexFill->m_z =
				sin( (pVertexFill->m_x+_frameDate*0.2f)*8.0f)*
				cos( (pVertexFill->m_y-_frameDate*0.3f)*8.0f )*0.3f*objectRotAxeZ;
			pVertexFill->m_u = -1.0f + ((float)((xx<<16)/m_MaxNumberOfGridDivisionInWidth))*idiv*3.0f ;
			pVertexFill->m_v = -1.0f + ((float)((yy<<16)/m_MaxNumberOfGridDivisionInHeight))*idiv*3.0f ;
			float nx,ny,nz,nn;
			nx = -sin( (pVertexFill->m_x-0.01f+_frameDate*0.2f)*8.0f)*
				cos( (pVertexFill->m_y-_frameDate*0.3f)*8.0f )*0.3f*objectRotAxeZ;
			nx += pVertexFill->m_z;
			ny = -sin( (pVertexFill->m_x+_frameDate*0.2f)*8.0f)*
				cos( (pVertexFill->m_y-0.01f-_frameDate*0.3f)*8.0f )*0.3f*objectRotAxeZ;
			ny += pVertexFill->m_z;
			nz = 0.04f;
			nn = 1.0f/sqrt( nx*nx + ny*ny + nz*nz );

			pVertexFill->m_nx = nx*nn ;
			pVertexFill->m_ny = ny*nn ;
			pVertexFill->m_nz = nz*nn ;
			pVertexFill++;
		}
	}
	VirtualMachine::InternalTriangle *pTriangleFill = m_pPreviewObject3D->GetFirstTriangle();
	unsigned int nbw = m_MaxNumberOfGridDivisionInWidth+1;
	for(yy=0;yy<m_MaxNumberOfGridDivisionInHeight; yy++)
	{
		for(xx=0;xx<m_MaxNumberOfGridDivisionInWidth ; xx++)
		{
			pTriangleFill->m_p0 = yy*nbw + xx ;
			pTriangleFill->m_p1 =  yy*nbw + xx+1;
			pTriangleFill->m_p2 = (yy+1)*nbw + xx ;
			pTriangleFill++;
			pTriangleFill->m_p0 = (yy+1)*nbw + xx ;
			pTriangleFill->m_p1 = yy*nbw + xx+1;
			pTriangleFill->m_p2 = (yy+1)*nbw + xx +1;
			pTriangleFill++;
		}
	}
	m_pPreviewObject3D->SetNumberOfActiveTriangle(
		(m_MaxNumberOfGridDivisionInWidth)*(m_MaxNumberOfGridDivisionInHeight)*2
		);

	_pPreviewViewPort->Matrix_LoadID();
	_pPreviewViewPort->Matrix_Translate(opx,opy,opz-0.75f);
	_pPreviewViewPort->Matrix_Rotate( (objectRotAxeX)*2.4f,0.0f,-1.0f,0.0f);
	_pPreviewViewPort->Matrix_Rotate( (objectRotAxeY)*2.4f,-1.0f,0.0f,0.0f);

	UpdateToFrame(_frameDate);

	_pPreviewViewPort->SetFOVLength(0.75f);
	_pPreviewViewPort->RenderMesh( 	m_pPreviewObject3D, m_pInternalTexture );
}
#endif

void	Texture3D::UpdateToFrame(float _framedate)
{
	if(m_LastUpdateDate == _framedate) return;

	float	inparam[4],outparam[4];

	inparam[0]=PackFloat::m_0p0;
	inparam[3]=_framedate;
	outparam[0]=outparam[1]=outparam[2]=outparam[3]=1.0f;
	if(mSer_BaseColor.GetSelectedIndex() != 0)
	{
		outparam[0]=outparam[1]=outparam[2]=outparam[3]=PackFloat::m_0p0;
		mSer_BaseColor.Compute(outparam,inparam);
	}
	m_pInternalTexture->SetBaseColor(outparam);

	VirtualImage *pimage;

	pimage = (VirtualImage *) mSerRef_MappingImage.GetObjectPointer();
	 if(pimage && pimage->IsTimeDynamic() )
	 {
		pimage->UpdateToFrame(_framedate);
		m_pInternalTexture->SetImageBitmap( VirtualMachine::InternalTexture::itile_TextureMapping,
					pimage->GetBuffer(),
					pimage->GetByteDepth(),
					pimage->GetPixelWidth(),
					pimage->GetPixelHeight()
					);
	 }

	 pimage = (VirtualImage *) mSerRef_EnvImage.GetObjectPointer();
	 if(pimage && pimage->IsTimeDynamic() )
	 {
		pimage->UpdateToFrame(_framedate);
		m_pInternalTexture->SetImageBitmap( VirtualMachine::InternalTexture::itile_Environment,
					pimage->GetBuffer(),
					pimage->GetByteDepth(),
					pimage->GetPixelWidth(),
					pimage->GetPixelHeight()
					);
	 }

	m_LastUpdateDate = _framedate;
}
