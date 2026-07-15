/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_RenderTargetVirtual_H
#define COM_M4NKIND_RenderTargetVirtual_H

#include "BaseObject.h"
#include "PackObjectReference.h"
#include "VirtualMachine.h"
/*!
	\class	RenderTargetVirtual
	\ingroup BaseObjectInherited_VedaLib3DEngine
	\brief	Define a screen rectangle where to draw.
			It is the virtual version.

*/
class RenderTargetVirtual : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	RenderTargetVirtual(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(RenderTargetVirtual);
	/*!
		\brief	Access to the hardware abstract texture for rendering.
		\return the hardware abstract texture.
	*/
	inline  VirtualMachine::InternalViewPort	*GetViewPort(){ return m_pInternalViewPort; };

/*==================================================================
								PROTECTED
==================================================================*/
protected:

	//! the hardware abstract texture for rendering.
	VirtualMachine::InternalViewPort	*m_pInternalViewPort;

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by Create().
		\return false if creation failed.
	*/
//	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	virtual void CloseInternal(void);
#endif

};
#endif
