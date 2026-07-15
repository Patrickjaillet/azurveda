// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackList_H
#define COM_M4NKIND_PackList_H

#include "PackStruct.h"

class PackList : public PackStruct
{

public:

	PackList( BaseTypeCreatorCallBackFunction _func);
#ifdef _ENGINE_EDITABLE_

	virtual ~PackList(void);
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);
#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif

#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackList"; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual	Cell * AddElement( int _indexWhereToInsert=-1,BaseType *_pObjectToAdd=0L);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void DeleteElement( unsigned int _index);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void DeleteElement( BaseType *_pObWhichCellIsToDestroy);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual  const unsigned char * MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_pendName=0L);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual  void	SwapElements( unsigned int _indexOfElementToSwapWithNext);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual  void	CloneElement( unsigned int _indexOfElementToClone);
#endif

protected:

	PackULong	mSerUlong_ChunkLength;

	BaseTypeCreatorCallBackFunction		m_CreatorFunc;

};
#endif
