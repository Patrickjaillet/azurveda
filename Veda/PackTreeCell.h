// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackTreeCell_H
#define COM_M4NKIND_PackTreeCell_H

#include "BaseType.h"
#include "PackULong.h"

class PackTreeCell : public BaseType
{

public:

	PackTreeCell( BaseTypeCreatorCallBackFunction _func);
#ifdef _ENGINE_EDITABLE_

	virtual ~PackTreeCell(void);
#endif

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackTreeCell"; };
#endif

	virtual	void	SetObjectThatManagesThis(BaseObject *_pManager);

	PackTreeCell * AddSubCellToThisCell( int _indexWhereToInsert=-1 );

	inline BaseType * GetObject(){ return m_pObjectManaged;  };

	inline PackTreeCell * GetNextBrotherCell(){ return m_pNextSerializedBrotherCell;  };

	inline PackTreeCell * GetFirstSubCell(){ return m_pFirstSerializedSubCell;  };

#ifdef _ENGINE_EDITABLE_

	void DeleteSubCells( PackTreeCell *_pSubCellToDelete);
#endif
#ifdef _ENGINE_EDITABLE_

#endif
#ifdef _ENGINE_EDITABLE_

#endif
#ifdef _ENGINE_EDITABLE_

#endif

protected:

	BaseTypeCreatorCallBackFunction	m_CreatorFunction;

	BaseType					*m_pObjectManaged;

	PackULong		mSerUlong_JumpToNextChunk;

	PackTreeCell	*m_pFirstSerializedSubCell;

	PackTreeCell	*m_pNextSerializedBrotherCell;

	PackTreeCell	**m_ppPointerToAttachNextSerializedSubCellAtEnd;

#ifdef _ENGINE_EDITABLE_

	void DeleteAllSubCells( void );
#endif
#ifdef _ENGINE_EDITABLE_

	void DeleteSubCellsProtectedRecursive( PackTreeCell *_pSubCellToDelete);
#endif
};

#endif
