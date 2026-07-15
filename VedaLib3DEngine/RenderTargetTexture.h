/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_RenderTargetTexture_H
#define COM_M4NKIND_RenderTargetTexture_H

#include "RenderTargetVirtual.h"
#include "PackObjectReference.h"
#include "PackLong_WithLimits.h"
#include "PackULong_Enums.h"
#include "VirtualMachine.h"
/*!
	\class	RenderTargetTexture
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Define a screen rectangle to draw on a texture, for
			multipass rendering.
*/
class RenderTargetTexture : public RenderTargetVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	RenderTargetTexture(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(RenderTargetTexture);

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the Texture where to render
	PackObjectReference	mSer_TextureToRender;
	//! enum: tell which texture layer to render. Use enum: VirtualMachine::InternalTexture::TextureImageLayerEnum
	PackULong_Enums	mSer_TargetLayer;
	//! the render texture width (1,2,4,8,16,32,64,128,256,512)
	PackLong_WithLimits	mSer_2nPixelWidth;
	//! the render texture height (1,2,4,8,16,32,64,128,256,512)
	PackLong_WithLimits	mSer_2nPixelHeight;



	//! the hardware abstract texture for rendering.
	VirtualMachine::InternalViewPort	*m_pInternalViewPort;

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by Create().
		\return false if creation failed.
	*/
	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	//DONE BY RenderTargetVirtual. virtual void CloseInternal(void);
#endif

};
#endif
