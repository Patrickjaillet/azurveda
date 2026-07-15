/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "RenderTargetTexture.h"
#include "Texture3D.h"

BASEOBJECT_DECLARE_CLASS( "t", RenderTargetTexture, RenderTargetVirtual,"2D RenderTarget Texture","TextRect","Rectangle to render, but It will be done on a 3D Texture." );

RenderTargetTexture::RenderTargetTexture() : RenderTargetVirtual()
	,m_pInternalViewPort(0L)
{
	REGISTER_MEMBER_REFERENCE(	mSer_TextureToRender,"3DTexture",Texture3D::m_Description );
	// Use enum: VirtualMachine::InternalTexture::TextureImageLayerEnum
	REGISTER_MEMBER_ENUMS(mSer_TargetLayer,"TargetLayer",0,"Mapping.Environment");
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_2nPixelWidth , "2N Width",8,1,9 );
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX( mSer_2nPixelHeight, "2N Height",8,1,9 );
}

/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
		\return false if creation failed.
*/
bool RenderTargetTexture::CreateInternal(void)
{	
	Texture3D	*pT3dOb = (Texture3D *)mSer_TextureToRender.GetObjectPointer();
	if(!pT3dOb) return true; // no texture pointed.
	VirtualMachine::InternalTexture	*ptexture =  pT3dOb->GetVirtualMachineTexture();
	if(!ptexture) return true; // shouldn't happen.
	GetMachine()->NewViewPortOnTexture( &m_pInternalViewPort,ptexture,
		mSer_2nPixelWidth.Get(),
		mSer_2nPixelHeight.Get(),
		( VirtualMachine::InternalTexture::TextureImageLayerEnum ) mSer_TargetLayer.Get()
		);

	//TODO GetMachine()->NewInternalViewPort( &m_pInternalViewPort );
	return(true);
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
/* DONE BY RenderTargetVirtual.
void	RenderTargetTexture::CloseInternal(void)
{
	GetMachine()->DeleteInternalViewPort( &m_pInternalViewPort );
}*/
#endif
