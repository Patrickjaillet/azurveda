// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackStruct_H
#define COM_M4NKIND_PackStruct_H

#include "BaseType.h"
#include "PackULong.h"

class PackStruct : public BaseType
{

public:

	PackStruct();
#ifdef _ENGINE_EDITABLE_

	virtual ~PackStruct(void);
#endif

	class Cell
	{
		public:

			Cell(): m_pNextCell(0L)
				,m_pPreviousCell(0L)
				,m_pObjectManaged(0L) {};

			inline Cell * GetPreviousBrotherCell(){ return m_pPreviousCell;  };

			inline Cell * GetNextBrotherCell(){ return m_pNextCell;  };

			inline void SetManagedObject(BaseType	*_ob){ m_pObjectManaged = _ob ; };

			inline BaseType *GetManagedObject(){ return m_pObjectManaged;  };
#ifdef _ENGINE_EDITABLE_

			inline void SetPreviousBrotherCell(Cell *_pPrevCell){ m_pPreviousCell = _pPrevCell;  };
#endif

			inline void SetNextBrotherCell(Cell *_pNewCell){ m_pNextCell = _pNewCell;  };

		protected:

			Cell	*m_pPreviousCell;

			Cell	*m_pNextCell;

			BaseType	*m_pObjectManaged;
	};

	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif

	virtual	void	SetObjectThatManagesThis(BaseObject *_pManager);

#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackStruct"; };
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char	*ValueToString();
#endif

	virtual	Cell * AddElement( int _indexWhereToInsert=-1,BaseType *_pObjectToAdd=0L);
#ifdef _ENGINE_EDITABLE_

	BaseType * Get( unsigned int _index);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void DeleteElement( unsigned int _index);

#endif

	inline Cell *GetFirstCell() const{ return(m_pFirstCell );  };

	inline Cell *GetLastCell() const{ return(m_pLastCell );  };

	inline unsigned int GetNumberOfCell() const{ return(m_NumberOfCell );  };
#ifdef _ENGINE_EDITABLE_

	void DeleteAllElements( void );
#endif

protected:

	Cell	*m_pFirstCell;

	Cell	*m_pLastCell;

	unsigned int	m_NumberOfCell;

	void RegisterSerializableMember( BaseType &_object );

};

#endif
