// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackLong_H
#define COM_M4NKIND_PackLong_H

#include "BaseType.h"

class PackLong : public BaseType
{

public:
#ifdef _ENGINE_EDITABLE_

	PackLong(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual ~PackLong(void);
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackLong"; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void Set( signed int _value );
#endif

	inline int Get( ){ return m_value; };
#ifdef _ENGINE_EDITABLE_

	virtual const char	*ValueToString();
#endif

protected:

	signed int	m_value;
};

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKLONG(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.Set(_DefaultVal);
#else
#define		REGISTER_MEMBER_PACKLONG(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);
#endif

#endif
