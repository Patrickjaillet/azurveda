/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "Texture3D.h"

#include "VirtualImage.h"
#include "VirtualMachine.h"

#ifdef _ENGINE_EDITABLE_
// for preview.
#include <math.h>
#endif

BASEOBJECT_DECLARE_CLASS( "t3d", Texture3D, BaseObject,"3D Texture","Texture","Defines a render type for 3D Objects." );

Texture3D::Texture3D() : BaseObject()
	,m_pInternalTexture(0L)
	,mSer_BaseColor(PackFloat::vd_XYZD)
#ifdef _ENGINE_EDITABLE_
	// in edition mode, create a private 3D object buffer to preview the texture.
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
		// in dedition mode, set default value for BaseColor:
	mSer_BaseColor.SetConstant(1.0f,0);
	mSer_BaseColor.SetConstant(1.0f,1);
	mSer_BaseColor.SetConstant(1.0f,2);
	mSer_BaseColor.SetConstant(1.0f,3);
#endif
	REGISTER_MEMBER_REFERENCE( mSerRef_MappingImage ,"Map Image", VirtualImage::m_Description );
	REGISTER_MEMBER_REFERENCE( mSerRef_EnvImage ,"Env Image", VirtualImage::m_Description );
}

/*!
	\brief	Method that really build the object using the serializable parameters.
			For the virtual VirtualImage class, it allocs the bitmap memory.
			Close() should close everything opened by CreateInternal().
*/
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
	// in edition mode, create a private 3D object buffer to preview the texture.
	GetMachine()->NewObject3DBuffer( &m_pPreviewObject3D, 
		(m_MaxNumberOfGridDivisionInWidth+1)*(m_MaxNumberOfGridDivisionInHeight+1),
		(m_MaxNumberOfGridDivisionInWidth)*(m_MaxNumberOfGridDivisionInHeight)*2,
		VirtualMachine::bOb3D_VertexNormal|VirtualMachine::bOb3D_VertexUVMapping
		/*|VirtualMachine::bOb3D_VertexRGBA*/);
#endif
	m_LastUpdateDate = PackFloat::m_Max;
	UpdateToFrame(PackFloat::m_0p0);
	return(true);
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void Texture3D::CloseInternal(void)
{
	GetMachine()->DeleteTexture( &m_pInternalTexture ); // it does the checking.
#ifdef _ENGINE_EDITABLE_
	// in edition mode, create a private 3D object buffer to preview the texture.
	// we delete it on close of course:
	GetMachine()->DeleteObject3DBuffer( &m_pPreviewObject3D );
#endif
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. This is done with this method. sub classes can implement it (or not) in
			any way, to explicit current shape of an object.<br>

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
	\param	_pPreviewConfiguration
*/
void Texture3D::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	if(!m_pPreviewObject3D ) return;
	// change object shape.
	VirtualMachine::InternalVertex *pVertexFill;
	VirtualMachine::InternalVertex *pVertex = pVertexFill = m_pPreviewObject3D->GetFirstVertex();
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
	// place matrix:
	_pPreviewViewPort->Matrix_LoadID();
	_pPreviewViewPort->Matrix_Translate(opx,opy,opz-0.75f); // camera move
	_pPreviewViewPort->Matrix_Rotate( (objectRotAxeX)*2.4f,0.0f,-1.0f,0.0f);
	_pPreviewViewPort->Matrix_Rotate( (objectRotAxeY)*2.4f,-1.0f,0.0f,0.0f);
	//m_pPreviewMatrixStack->Rotate((float)_frameDate*0.1f ,0.0f,0.0f,1.0f);
	// draw rendering:
	// we don't use Mesh3D:: RenderObject() so we have to mix the basecolor ourself:
	UpdateToFrame(_frameDate);
	/*float	inparam[4],outparam[4];
	inparam[3]=_frameDate; // time as parameter.
	outparam[0]=outparam[1]=outparam[2]=outparam[3]=1.0f; // if no constant, use white:
	if( mSer_BaseColor.GetSelectedIndex() != 0) // optim
		mSer_BaseColor.Compute(outparam,inparam);
	m_pPreviewObject3D->SetColor(outparam[0],outparam[1],outparam[2],outparam[3]);*/
	_pPreviewViewPort->SetFOVLength(0.75f);
	_pPreviewViewPort->RenderMesh( 	m_pPreviewObject3D, m_pInternalTexture );
}
#endif
/*!
	\brief	In case of a time-dynamic texture, an update had to be done:
	\param	_framedate date in seconds
*/
void	Texture3D::UpdateToFrame(float _framedate)
{
	if(m_LastUpdateDate == _framedate) return;

	//m_RGBA[4]
	float	inparam[4],outparam[4];
	// update base color:
	inparam[0]=PackFloat::m_0p0;
	inparam[3]=_framedate; // time as parameter.
	outparam[0]=outparam[1]=outparam[2]=outparam[3]=1.0f;
	if(mSer_BaseColor.GetSelectedIndex() != 0)
	{
		outparam[0]=outparam[1]=outparam[2]=outparam[3]=PackFloat::m_0p0;
		mSer_BaseColor.Compute(outparam,inparam);
	}
	m_pInternalTexture->SetBaseColor(outparam);
	// update mapping image:
	VirtualImage *pimage;
	// test ok:
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
	// update environment image:
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
	// now, the texture is updated for date:	
	m_LastUpdateDate = _framedate;
}
