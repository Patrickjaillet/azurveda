// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_NamedObject_H
#define COM_M4NKIND_NamedObject_H

#include "PackStruct.h"

#include "PackString.h"

class NamedObject : public PackStruct
{

public:

	NamedObject(void);
#ifdef _ENGINE_EDITABLE_

	virtual ~NamedObject(void);
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif

	inline unsigned int GetChunkLength(){ return(mSerUlong_ChunkLength.Get() );  };

	inline const char *  GetName(void) const{ return( mSerStr_ObjectName.Get() ); };

#ifdef _ENGINE_EDITABLE_

	void  SetName( const char * _pname );
#endif

protected:

	PackULong		mSerUlong_ChunkLength;

	PackString		mSerStr_ObjectName;

	BaseContext				*m_pBaseContext;

};

#endif
