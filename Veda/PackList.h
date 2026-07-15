/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackList_H
#define COM_M4NKIND_PackList_H

#include "PackStruct.h"

/*!
	\class	PackList
	\ingroup	BaseSerializableClass
	\brief	 Base type managing a list of any PackSerializable (or inherited) Objects
			from the same class. In edition mode, you can grow or shorten the list.
			GetFirstCell() give you a first 'PackListCell', then you can scan the list element
			with PackListCell::GetNextBrotherCell() and PackListCell::GetManagedObject().
*/

class PackList : public PackStruct
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
		\param _func a function that return a new object to manage as an element of the list.
	*/
	PackList( BaseTypeCreatorCallBackFunction _func);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackList(void);
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
	virtual const char *GetClassID() const { return "PackList"; };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Add a new element, at the given index, in edition context.
				in non-editable, it shouldnt be used, and serialization recreate the elements.
		\param	_indexWhereToInsert the index where to insert. if -1, at the end.
		\param _pObjectToAdd object to manage, or NULL to set it after on the cell.
		\return  the element created.
	*/
	virtual	Cell * AddElement( int _indexWhereToInsert=-1,BaseType *_pObjectToAdd=0L);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief delete an element at a given index. It closes and destroys the object.
		\param	_index index of the object to destroy.
	*/
	virtual void DeleteElement( unsigned int _index);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief delete an element at a given index. It closes and destroys the object.
		\param _pObWhichCellIsToDestroy ob WhichCellIsToDestroy.
	*/
	virtual void DeleteElement( BaseType *_pObWhichCellIsToDestroy);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Acts like Serialize_In() but keep all previous objects.
				So you can load and mix 2 or more context this way.

		\param	_pDescriptionChunk the binary image of a context.
		\param _pendName specify a name to add at the end of each new object.
	*/
	virtual  const unsigned char * MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_pendName=0L);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Edition tool: Exchange 2 neighbour elements in the list, if there is a next element.
				if index is 0, 0 will be 1 and 1 will be 0.
		\param	_indexOfElementToSwapWithNext any row, from zero to GetNumberOfCell()-2
	*/
	virtual  void	SwapElements( unsigned int _indexOfElementToSwapWithNext);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Insert a new element after _indexOfElementToClone,
			and clone it with the previous element shape. Note this
			can exclude some data members, like Object reference.
		\param	_indexOfElementToClone any row, from zero to GetNumberOfCell()-1
	*/
	virtual  void	CloneElement( unsigned int _indexOfElementToClone);
#endif
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! thechunk length of the list:
	PackULong	mSerUlong_ChunkLength;
	//! Static Method Pointer to the NewObject function used to create the object that this node manage.
	BaseTypeCreatorCallBackFunction		m_CreatorFunc;

};
#endif
