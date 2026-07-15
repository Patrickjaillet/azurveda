#include "Object3DVirtualDynamic.h"
#include "PackFloat.h"

BASEOBJECT_DECLARE_VIRTUALCLASS( "D", Object3DVirtualDynamic, Object3DMeshVirtual);

Object3DVirtualDynamic::Object3DVirtualDynamic() : Object3DMeshVirtual()
{
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_MaximumNumberOfVertex,"MaxNbVertex",1024,1,65535 );
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_MaximumNumberOfTriangle,"MaxNbTriangle",1024,1,65535 );
}

bool Object3DVirtualDynamic::CreateInternal_Alloc1Internal3DBuffer(unsigned int _i3DBufferFlag)
{

	m_pObject3DBufferTable = new Object3DBufferHandler[1];

	m_pObject3DBufferTable->m_pRenderTexture = 0L;
	if( !m_pObject3DBufferTable )
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
		return false;
	}
	GetMachine()->NewObject3DBuffer( &(m_pObject3DBufferTable->m_pObject3DBuffer) ,
		mSer_MaximumNumberOfVertex.Get(),
		mSer_MaximumNumberOfTriangle.Get(),
		_i3DBufferFlag
		);
	if(!(m_pObject3DBufferTable->m_pObject3DBuffer))
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
		return false;
	}
	m_LastShapeTime = PackFloat::m_Max;
	m_CurrentNumberOf3DBuffer = 1;
	return(true);

}
