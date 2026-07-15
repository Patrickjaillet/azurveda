/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "RenderTargetVirtual.h"

BASEOBJECT_DECLARE_VIRTUALCLASS( "reta", RenderTargetVirtual, BaseObject );

RenderTargetVirtual::RenderTargetVirtual() : BaseObject()
	,m_pInternalViewPort(0L)
{

}

/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
		\return false if creation failed.
*/
/*
bool RenderTargetVirtual::CreateInternal(void)
{	
	GetMachine()->NewInternalViewPort( &m_pInternalViewPort );
	return(true);
}
*/
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void	RenderTargetVirtual::CloseInternal(void)
{
	GetMachine()->DeleteViewPort( &m_pInternalViewPort );
}
#endif
