/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_Texture3D_H
#define COM_M4NKIND_Texture3D_H

#include "BaseObject.h"
#include "PackObjectReference.h"
#include "PackULong_Flags.h"
#include "VirtualMachine.h"
#include "PackDynamicFloat.h"
/*!
	\class	Texture3D
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Manages a texture in order to render 3D objects.
*/
class Texture3D : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Texture3D(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Texture3D);
	/*!
		\brief	Access to the hardware abstract texture for rendering.
		\return the hardware abstract texture.
	*/
	inline  VirtualMachine::InternalTexture	*GetVirtualMachineTexture(){ return m_pInternalTexture; };
	/*!
		\brief	In case of a time-dynamic texture, an update had to be done:
		\param	_framedate date in seconds
	*/
	virtual  void	UpdateToFrame(float _framedate);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
				created object. This is done with this method. sub classes can implement it (or not) in
				any way, to explicit current shape of an object.<br>

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration
	*/
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! property flags
	PackULong_Flags	mSer_Flags;
	//! the base RGBA color, which modulates the component to vertex and object base color.
	PackDynamicFloat	mSer_BaseColor;
	//! the image for texture mapping:
	PackObjectReference	mSerRef_MappingImage;
	//! the image for environment mapping:
	PackObjectReference	mSerRef_EnvImage;
	//! the hardware abstract texture for rendering.
	VirtualMachine::InternalTexture		*m_pInternalTexture;
	//! the last time the texture was updated.
	float	m_LastUpdateDate;
#ifdef _ENGINE_EDITABLE_
	//! in edition mode, create a private 3D object buffer to preview the texture.
	VirtualMachine::InternalObject3DBuffer *m_pPreviewObject3D;
	//! constant for preview 
	static const unsigned int		m_MaxNumberOfGridDivisionInWidth=14;
	//! constant for preview 
	static const unsigned int		m_MaxNumberOfGridDivisionInHeight=14;
#endif
		/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	virtual void	CloseInternal(void);
#endif

};

#endif
