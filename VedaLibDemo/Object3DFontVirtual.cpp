#include "Object3DFontVirtual.h"
#include "Texture3D.h"
BASEOBJECT_DECLARE_VIRTUALCLASS( "ftv", Object3DFontVirtual, Object3DMeshVirtual);

Object3DFontVirtual::Object3DFontVirtual() : Object3DMeshVirtual()
	,mSer_ShadowList(NewShadow)
{

}

Object3DFontVirtual::Shadow::Shadow()
{
	REGISTER_MEMBER_REFERENCE(mSer_TextureReference,"Texture",Texture3D::m_Description );
	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(mSer_DecalX,"Shift X",0.05f,-1.0f,1.0f);
	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(mSer_DecalY,"Shift Y",0.05f,-1.0f,1.0f);
}

BaseType *Object3DFontVirtual::NewShadow()
{
	return new Shadow();
}
