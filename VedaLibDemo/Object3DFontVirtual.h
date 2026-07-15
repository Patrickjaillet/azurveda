#ifndef	COM_M4NKIND_Object3DFontVirtual_H
#define COM_M4NKIND_Object3DFontVirtual_H

#include "Object3DMeshVirtual.h"
#include "PackFloatByte_Limits.h"
#include "PackObjectReference.h"
#include "PackList.h"

class Object3DFontVirtual : public Object3DMeshVirtual
{

public:

	Object3DFontVirtual(void);

	BASEOBJECT_DEFINE_VIRTUALCLASS(Object3DFontVirtual);

	virtual bool GetShapeIndexForCharacter( const char *_pString,
											int &_rShapeIndex,
											float &_rCharWidth )=0;

protected:
	class Shadow : public PackStruct
	{ public:

		Shadow();

		PackObjectReference		mSer_TextureReference;
		PackFloatByte_Limits	mSer_DecalX;
		PackFloatByte_Limits	mSer_DecalY;
	};

	static BaseType *NewShadow();

	PackList	mSer_ShadowList;

};

#endif
