/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackFloat_FixedPoint.h"
#include "PackLong.h"
#include "PackString.h"
	
/*!
	\brief	Constructor by default number of bit after the point is 6: (_ValueToMultiplyFromIntToFloat=1/64.0)
*/
PackFloat_FixedPoint::PackFloat_FixedPoint() : PackFloat()
	,m_ValueToMultiplyFromIntToFloat(PackFloat::m_1Div64)
{

}
PackFloat_FixedPoint::PackFloat_FixedPoint(const float _ValueToMultiplyFromIntToFloat): PackFloat()	
	,m_ValueToMultiplyFromIntToFloat(_ValueToMultiplyFromIntToFloat)
{

}
/*!
	\brief	Constructor To initialize PackFloat as a multi dimentional vector. 
	\param _DimensionEnum on of the 3 VectorDimension enum.
	\param _ValueToMultiplyFromIntToFloat fixes how many bit are used after the point.
*/
PackFloat_FixedPoint::PackFloat_FixedPoint( VectorDimension _DimensionEnum,const float _ValueToMultiplyFromIntToFloat ) : PackFloat(_DimensionEnum)
	,m_ValueToMultiplyFromIntToFloat(_ValueToMultiplyFromIntToFloat)
{

}
#ifdef _ENGINE_EDITABLE_
PackFloat_FixedPoint::~PackFloat_FixedPoint(void)
{

}
#endif
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char * PackFloat_FixedPoint::Serialize_In( const unsigned char * _pDescriptionChunk)
{
	PackLong serlong;
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		_pDescriptionChunk = serlong.Serialize_In(_pDescriptionChunk);
		m_value[ii] = ((float)(serlong.Get())) * m_ValueToMultiplyFromIntToFloat ;
	}
	return(_pDescriptionChunk);
}


#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.(1,2,3, or4)
*/
unsigned int PackFloat_FixedPoint::GetSerializedDescriptionSize(void)
{
	unsigned int size=0;
	PackLong serlong;
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		serlong.Set( (int)(m_value[ii] / m_ValueToMultiplyFromIntToFloat) ); 
		size += serlong.GetSerializedDescriptionSize();
	}
	return( size ) ;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * PackFloat_FixedPoint::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	PackLong serlong;
	for(unsigned int ii=0 ; ii<m_NumberOfDimensionManaged ; ii++ )
	{
		serlong.Set( (int)(m_value[ii] / m_ValueToMultiplyFromIntToFloat) ); 
		_pDescriptionChunkToFill =
				serlong.Serialize_Out( _pDescriptionChunkToFill );
	}
	return( _pDescriptionChunkToFill ) ;
}
#endif
