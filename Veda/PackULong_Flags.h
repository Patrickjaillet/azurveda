// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackULong_Flags_H
#define COM_M4NKIND_PackULong_Flags_H

#include "PackULong.h"

class PackULong_Flags : public PackULong
{

public:

	PackULong_Flags(void);
#ifdef _ENGINE_EDITABLE_

	virtual ~PackULong_Flags(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackULong::PackULong_Flags"; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char	*ValueToString();
#endif

	inline bool TestFlags( unsigned int _bitFlagsToTest ) const {return ((m_value&_bitFlagsToTest)!=0);};

#ifdef _ENGINE_EDITABLE_

	inline void AddFlags( unsigned int _FlagsToAdd ){ Set(Get()|_FlagsToAdd); };
#endif
#ifdef _ENGINE_EDITABLE_

	inline void RemoveFlags( unsigned int _FlagsToRemove ){Set( Get() &(~_FlagsToRemove) );};
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void SetBitFieldInfo( const char *_meaning );
#endif
#ifdef _ENGINE_EDITABLE_

	const char *GetBitFieldInfo( ){ return m_Bitfield_Meaning_InfoString; };
#endif

protected:
#ifdef _ENGINE_EDITABLE_

	const char	*m_Bitfield_Meaning_InfoString;
#endif

};
#ifdef _ENGINE_EDITABLE_

#define		REGISTER_MEMBER_FLAG(_object,_InfoString,_DefaultVal,_InfoBitFiled) \
REGISTER_MEMBER(_object,_InfoString)\
_object.Set(_DefaultVal);\
_object.SetBitFieldInfo( _InfoBitFiled );

#else

#define		REGISTER_MEMBER_FLAG(_object,_InfoString,_DefaultVal,_InfoBitFiled) \
REGISTER_MEMBER(_object,_InfoString)

#endif

#endif
