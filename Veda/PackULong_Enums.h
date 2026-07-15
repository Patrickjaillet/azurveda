// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackULong_Enums_H
#define COM_M4NKIND_PackULong_Enums_H

#include "PackULong.h"

#ifdef _ENGINE_EDITABLE_

class PackULong_Enums : public PackULong
{

public:

	PackULong_Enums(void);

	virtual ~PackULong_Enums(void);

	virtual const char *GetClassID() const { return "PackULong::PackULong_Enums"; };

	virtual const char	*ValueToString();

	virtual void SetEnumsInfo( const char *_meaning );

	const char *GetEnumsInfo( ){ return m_Enums_Meaning_InfoString; };

protected:

	const char	*m_Enums_Meaning_InfoString;

};

#define		REGISTER_MEMBER_ENUMS(_object,_InfoString,_DefaultVal,_InfoEnumsList) \
REGISTER_MEMBER(_object,_InfoString)\
_object.Set(_DefaultVal);\
_object.SetEnumsInfo( _InfoEnumsList );

#else

typedef PackULong PackULong_Enums;

#define		REGISTER_MEMBER_ENUMS(_object,_InfoString,_DefaultVal,_InfoEnumsList) \
REGISTER_MEMBER(_object,_InfoString)

#endif

#endif
