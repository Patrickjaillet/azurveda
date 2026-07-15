#include "Object3DVirtualDynamic.h"
#include "PackFloat.h"

BASEOBJECT_DECLARE_VIRTUALCLASS( "D", Object3DVirtualDynamic, Object3DMeshVirtual);

Object3DVirtualDynamic::Object3DVirtualDynamic() : Object3DMeshVirtual()
{
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_MaximumNumberOfVertex,"MaxNbVertex",1024,1,65535 );
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_MaximumNumberOfTriangle,"MaxNbTriangle",1024,1,65535 );
}
/*!
	\brief	Alloc one VirtualMachine::Internal3DBuffer with the number of vertex and triangle defined.
		this sould be used by CreateInternal or CreateShape().
	\param	_i3DBufferFlag the flags used to declare the vertex members.
	\return true if OK, false if error. 
*/
bool Object3DVirtualDynamic::CreateInternal_Alloc1Internal3DBuffer(unsigned int _i3DBufferFlag)
{
	// closed by  Object3DVirtual::CloseInternal()
	m_pObject3DBufferTable = new Object3DBufferHandler[1];

	m_pObject3DBufferTable->m_pRenderTexture = 0L; // this is the texture if object was pre-rendered.
	if( !m_pObject3DBufferTable )
	{	
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
		return false;
	}
	GetMachine()->NewObject3DBuffer( &(m_pObject3DBufferTable->m_pObject3DBuffer) ,
		mSer_MaximumNumberOfVertex.Get(), // can't be zero , thanks veda !
		mSer_MaximumNumberOfTriangle.Get(),
		_i3DBufferFlag
		);
	if(!(m_pObject3DBufferTable->m_pObject3DBuffer))
	{	
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
		return false;
	}
	m_LastShapeTime = PackFloat::m_Max; // default: we create object for a given date.
	m_CurrentNumberOf3DBuffer = 1;
	return(true);
	// close internal done by Virtual3DObject
}

