// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackULong_H
#define COM_M4NKIND_PackULong_H

#include "BaseType.h"

class PackULong : public BaseType
{

public:
#ifdef _ENGINE_EDITABLE_

	PackULong(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual ~PackULong(void);
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return("PackULong"); };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char	*ValueToString();
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void	Set(unsigned int _value);
#endif

	inline unsigned int	Get() const { return m_value ; };

	inline void	operator +=(unsigned int _value){ m_value +=  _value; };

protected:

	unsigned int	m_value;
};

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKULONG(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.Set(_DefaultVal);
#else
#define		REGISTER_MEMBER_PACKULONG(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);
#endif

#endif
