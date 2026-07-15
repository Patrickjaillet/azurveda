/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackFloat_FixedPoint_H
#define COM_M4NKIND_PackFloat_FixedPoint_H

#include "PackFloat.h"

/*!
	\class	PackFloat_FixedPoint
	\ingroup	BaseSerializableClass
	\brief	 Base type managing a float, but serialized like a PackLong.
			You provide a const number at construction that fixes a number
			of bit after the point. For example, 1 bit after the point will
			make possible: 0.0f , 0.5f, 1.0f, 1.5f,... 2 bits after the point
			will manage: 0.0f,0.25f,0.5f,0.75f,1.0f,1.25f,...
*/

class PackFloat_FixedPoint : public PackFloat
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor by default number of bit after the point is 6: (_ValueToMultiplyFromIntToFloat=1/64.0)
	*/
	PackFloat_FixedPoint();
	/*!
		\brief	Constructor. 
		\param _ValueToMultiplyFromIntToFloat fixes how many bit are used after the point.
	*/
	PackFloat_FixedPoint(const float _ValueToMultiplyFromIntToFloat );
	/*!
		\brief	Constructor To initialize PackFloat as a multi dimentional vector. 
		\param _DimensionEnum on of the 3 VectorDimension enum.
		\param _ValueToMultiplyFromIntToFloat fixes how many bit are used after the point.
	*/
	PackFloat_FixedPoint( VectorDimension _DimensionEnum,const float _ValueToMultiplyFromIntToFloat );

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackFloat_FixedPoint(void);
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
		\return	byte size of the serialisation to do.(1,2,3, or4)
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
	virtual const char *GetClassID() const { return "PackFloat::PackFloat_FixedPoint"; };
#endif

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the value that give the ratio for serialization:
	float	m_ValueToMultiplyFromIntToFloat;
};


// end of file:
#endif
