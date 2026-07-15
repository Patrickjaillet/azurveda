// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackDynamicType_H
#define COM_M4NKIND_PackDynamicType_H

#include "PackStruct.h"
#include "PackULong_Enums.h"
class BaseObject;

class PackDynamicType : public PackStruct
{

public:

	PackDynamicType(void);

#ifdef _ENGINE_EDITABLE_

	virtual ~PackDynamicType(void);
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackDynamicType"; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char	*ValueToString();
#endif

#ifdef _ENGINE_EDITABLE_

	void Set(unsigned int _shapeEnum );
#endif
#ifdef _ENGINE_EDITABLE_

	inline BaseType *Get(){ return m_pSelectedType; };
#endif

	inline unsigned int GetSelectedIndex(){ return mSer_TypeEnum.Get(); };

protected:

	PackULong_Enums		mSer_TypeEnum;

	BaseType	*m_pSelectedType;

};

#endif
