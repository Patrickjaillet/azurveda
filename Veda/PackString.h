// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackString_H
#define COM_M4NKIND_PackString_H

#include "BaseType.h"

class PackString : public BaseType
{

public:
#ifdef	_ENGINE_EDITABLE_

	PackString(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual ~PackString(void);
#endif

	virtual const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackString"; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char	*ValueToString();
#endif
#ifdef _ENGINE_EDITABLE_

	void AddInt( int _value);
#endif
#ifdef _ENGINE_EDITABLE_

	void AddString( const char * _pstr);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual	void Set( const char *  _pCharacterString );
#endif

	inline const char *  Get() const {return( (char * ) m_StringBuffer );};

	static int		Compare( const char *  _pStringToCompare, const char *  _pOtherStringToCompare );

	inline unsigned int length(){ return length(Get())-1; };

protected:

	char	*m_StringBuffer;

#ifdef _ENGINE_EDITABLE_

	static const unsigned int	m_GrowByteLength=64;

	unsigned int	m_UsedByteLength;

	unsigned int	m_TotalByteLength;
#endif

#ifdef _ENGINE_EDITABLE_

	virtual void Free();
#endif

	static unsigned int length( const char *_pString);

	static  const char * strcopy( char *_pStringToFill, const char *_pSourceString);
};

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKSTRING(_object,_InfoString) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );
#else
#define		REGISTER_MEMBER_PACKSTRING(_object,_InfoString) \
RegisterSerializableMember(_object);
#endif

#endif
