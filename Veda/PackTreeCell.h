/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackTreeCell_H
#define COM_M4NKIND_PackTreeCell_H

#include "BaseType.h"
#include "PackULong.h"
/*!
	\class	PackTreeCell
	\ingroup	BaseSerializableClass
	\brief	 base type managing a Tree database of any PackSerializable (or inherited) Objects.
			the object type managed is unique and defined at construction, by passing
			a function that do return a new object.

*/

class PackTreeCell : public BaseType
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
		\param _func a function that return a new object to manage as an element of the list.
	*/
	PackTreeCell( BaseTypeCreatorCallBackFunction _func);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackTreeCell(void);
#endif
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
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "PackTreeCell"; };
#endif		
	/*!
		\brief	The Object this one belong to as a member. Should only be used at init.
		\param _pManager BaseObject
	*/
	virtual	void	SetObjectThatManagesThis(BaseObject *_pManager);

	/*!
		\brief	Add a new sub cell element, at the given index, to this Cell.
		\param	_indexWhereToInsert the index where to insert. if -1, at the end.
		\return  the element created.
	*/
	PackTreeCell * AddSubCellToThisCell( int _indexWhereToInsert=-1 );
	/*!
		\brief	Return a pointer on the BaseType managed by this cell:
		\return  the object. should never be zero.
	*/
	inline BaseType * GetObject(){ return m_pObjectManaged;  };

	/*!
		\brief	get next brother cell in the hierarchy, or null if last of the brotherhood.
				It used to scan the tree.
		\return  the cell element or 0L.
	*/
	inline PackTreeCell * GetNextBrotherCell(){ return m_pNextSerializedBrotherCell;  };
	/*!
		\brief	get first subCell if there is any, or null if no subcell at all.
				It used to scan the tree.
		\return  the cell element or 0L.
	*/
	inline PackTreeCell * GetFirstSubCell(){ return m_pFirstSerializedSubCell;  };

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief delete an element and its whole sub elements. the first to delete can be at any level.
		\param	_pSubCellToDelete pointer to subcell.
	*/
	void DeleteSubCells( PackTreeCell *_pSubCellToDelete);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief delete an element at a given index. It closes and destroys the object.
		\param	_index index of the object to destroy.
	*/
	//old  not validated void DeleteSubCell( unsigned int _index);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Edition tool: Exchange 2 cells in the tree.
				You can't exchange the root cell. if one of the 2 cells
				is the root, nothing happens. If method is thrown 
		\param_pFirstSubCell 
		\param _pSecondSubCell
	*/
	//TODO virtual  void	SwapElements( PackTreeCell *_pFirstSubCell,PackTreeCell *_pSecondSubCell );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Edition tool: Insert a new element after _indexOfElementToClone,
			and clone it with the previous element shape. Note this
			can exclude some data members, like Object reference.
		\param	_pSubCellToClone the cell element to clone.
	*/
	//TODO virtual  void	CloneElementAsBrother( PackTreeCell *_pSubCellToClone);
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! a function pointer that return a new object of the type managed by the Tree.
	BaseTypeCreatorCallBackFunction	m_CreatorFunction;

	//! the object serialized by this cell:
	BaseType					*m_pObjectManaged;

	//! chunk length:
	PackULong		mSerUlong_JumpToNextChunk;

	//! member that manages the list of sub cell to serialize automatically. NULL if no subcellss.
	PackTreeCell	*m_pFirstSerializedSubCell;

	//! member that manages the list of cell to serialize automatically. NULL if no subcells.
	PackTreeCell	*m_pNextSerializedBrotherCell;

	//! another member that manages the list of members to serialize automatically, and allow to grow the list by the end.
	PackTreeCell	**m_ppPointerToAttachNextSerializedSubCellAtEnd;

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief close and destroy the whole list.
	*/
	void DeleteAllSubCells( void );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief delete an element and its whole sub elements. the first to delete can be at any level.
		\param	_pSubCellToDelete pointer to subcell.
	*/
	void DeleteSubCellsProtectedRecursive( PackTreeCell *_pSubCellToDelete);
#endif
};

#endif
