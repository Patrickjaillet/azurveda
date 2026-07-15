#ifndef	COM_M4NKIND_Object3DFontArial_H
#define COM_M4NKIND_Object3DFontArial_H

#include "Object3DFontVirtual.h"
#include "PackFloatByte_Limits.h"
#include "PackULong_Flags.h"
#include "PackList.h"

class Object3DFontArial : public Object3DFontVirtual
{

public:

	Object3DFontArial(void);

	BASEOBJECT_DEFINE_CLASS(Object3DFontArial);

	virtual bool GetShapeIndexForCharacter( const char *_pString,
											int &_rShapeIndex,
											float &_rCharWidth );

protected:

	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif

	static const signed char m_fontc[];

	typedef struct {
		signed int	m_ShapeIndex;
		float		m_CharWidth;
	} FontBase ;

	FontBase	*m_pCharTable;

};

#endif
