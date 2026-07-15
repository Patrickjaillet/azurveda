/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackStruct_H
#define COM_M4NKIND_PackStruct_H

#include "BaseType.h"
#include "PackULong.h"
/*!
	\class	PackStruct
	\ingroup	BaseSerializableClass
	\brief	 Base type managing a suit of any PackSerializable (or inherited) Objects,
			which types are differents but can't be changed.
			To use a PackStruct, register a list of PackSerializable objects in construction.
			This objects must be already allocated. PackStruct doesn't manage the existance
			of the listed objects. It just allows to re-serialize their values.
			This list must not vary in the life of the object, so that it can be 
			serialized in and out again and again.
			For a dynamic List handling objects from the same class, look: PackList
*/

class PackStruct : public BaseType
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
	*/
	PackStruct();
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackStruct(void);
#endif
	/*!
		\class	Cell
		\brief	nested class to PackStruct , stands for an element cell in the list.
	*/
	class Cell 
	{
		public:
			/*!
				\brief	Constructor. 
			*/
			Cell(): m_pNextCell(0L)
				,m_pPreviousCell(0L)
				,m_pObjectManaged(0L) {};

			/*!
				\brief	get next cell in the list, NULL if last.
				\return  the cell element or 0L.
			*/
			inline Cell * GetPreviousBrotherCell(){ return m_pPreviousCell;  };

			/*!
				\brief	get next cell in the list, NULL if last.
				\return  the cell element or 0L.
			*/
			inline Cell * GetNextBrotherCell(){ return m_pNextCell;  };
			/*!
				\brief	set object that this cell manage
				\param  _ob the serializable object
			*/
			inline void SetManagedObject(BaseType	*_ob){ m_pObjectManaged = _ob ; };
			/*!
				\brief	set object that this cell manage
				\return  the object or NULL if none.
			*/
			inline BaseType *GetManagedObject(){ return m_pObjectManaged;  };
#ifdef _ENGINE_EDITABLE_
			/*!
				\brief	Set Previous cell in the list.
				\param  _pPrevCell the new cell element.
			*/
			inline void SetPreviousBrotherCell(Cell *_pPrevCell){ m_pPreviousCell = _pPrevCell;  };
#endif
			/*!
				\brief	Set next cell in the list.
				\param  _pNewCell the new cell element.
			*/
			inline void SetNextBrotherCell(Cell *_pNewCell){ m_pNextCell = _pNewCell;  };

		protected:
			//!  previous cell
			Cell	*m_pPreviousCell;
			//!  next cell
			Cell	*m_pNextCell;
			//! the object serialized by this cell:
			BaseType	*m_pObjectManaged;
	};
	/*!
		\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
		\param	_pDescriptionChunk the objet description chunk. 
		\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
	*/
	virtual  const unsigned char * Serialize_In( const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
		\return	byte size of the serialisation to do.
	*/
	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	write the Current object definition to a Chunk using private packed types, recursively.
		\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
		\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
	*/
	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif

	/*!
		\brief	The Object this one belong to as a member. Should only be used at init.
		\param _pManager BaseObject
	*/
	virtual	void	SetObjectThatManagesThis(BaseObject *_pManager);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "PackStruct"; };
#endif		
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	convert the value of this object to an explicit string. The object manages the string privately,
			so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			 Note: this is not virtual, but each class can manage m_pValueString or not.
		\return	the value as a const string. 
	*/
	virtual const char	*ValueToString();
#endif
	/*!
		\brief	Add a new element, at the given index.
		\param	_indexWhereToInsert the index where to insert. if -1, at the end.
		\param _pObjectToAdd object to manage, or NULL to set it after on the returned cell.
		\return  the element created.
	*/
	virtual	Cell * AddElement( int _indexWhereToInsert=-1,BaseType *_pObjectToAdd=0L);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief get an element at a given index.
		\param	_index the index of the element
		\return the element at the index or NULL if failed.
	*/
	BaseType * Get( unsigned int _index);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief delete an element at a given object of the cell to destroy. 
			does not destroy the object.
		\param	_index index of the object to destroy.
	*/
	virtual void DeleteElement( unsigned int _index);

#endif
	/*!
		\brief	FirstCell or null if empty
		\return the cell
	*/
	inline Cell *GetFirstCell() const{ return(m_pFirstCell );  };
	/*!
		\brief	the	Last Cell or null if empty
		\return the cell
	*/
	inline Cell *GetLastCell() const{ return(m_pLastCell );  };
	/*!
		\brief	return the number of cell.
		\return the number of cell.
	*/
	inline unsigned int GetNumberOfCell() const{ return(m_NumberOfCell );  };
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief close and destroy the whole list.
	*/
	void DeleteAllElements( void );
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! member that manages the list of members to serialize automatically.
	Cell	*m_pFirstCell;
	//! another member that manages the list of members to serialize automatically, and allow to grow the list by the end.
	Cell	*m_pLastCell;
	//! the number of cell.
	unsigned int	m_NumberOfCell;
	/*!
		\brief Register any serializable object to serialize for read or write operation,
				Use it in objects constructors.
				The object are serialized in the file in the same order as they are given by this method.
		\param	_object reference to the member, which must inherit from BaseType.
	*/
	void RegisterSerializableMember( BaseType &_object );


};

#endif
