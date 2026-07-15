/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackDynamicType.h"
#include "PackString.h"

PackDynamicType::PackDynamicType(void): PackStruct()
	,m_pSelectedType(0L)
{
}

#ifdef _ENGINE_EDITABLE_
PackDynamicType::~PackDynamicType(void)
{

}
#endif
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char *PackDynamicType::Serialize_In( const unsigned char * _pDescriptionChunk )
{	
	// read enum value:
	_pDescriptionChunk = mSer_TypeEnum.Serialize_In( _pDescriptionChunk );
	// select shape according to enum:
	m_pSelectedType = 0L;
	PackStruct::Cell	*pCell = GetFirstCell();
	unsigned int	ii=1; // 0 means no selection.
	while(pCell)
	{
		if(ii == mSer_TypeEnum.Get() )  m_pSelectedType=pCell->GetManagedObject();
		ii++;
		pCell = pCell->GetNextBrotherCell() ;
	}
	// serialize shape if needed.
	if( m_pSelectedType ) 
	{	
		_pDescriptionChunk = m_pSelectedType->Serialize_In( _pDescriptionChunk );
	}

	return _pDescriptionChunk;
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.(1,2,3, or4)
*/
unsigned int  PackDynamicType::GetSerializedDescriptionSize(void)
{
	register unsigned int	size = mSer_TypeEnum.GetSerializedDescriptionSize();
	// add shape size if needed.
	if( m_pSelectedType ) 
	{	
		size += m_pSelectedType->GetSerializedDescriptionSize();
	}

	return size;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * PackDynamicType::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	// read enum value:
	_pDescriptionChunkToFill = mSer_TypeEnum.Serialize_Out( _pDescriptionChunkToFill );
	//  deserialize shape if needed.
	if( m_pSelectedType ) 
	{	
		_pDescriptionChunkToFill = m_pSelectedType->Serialize_Out(_pDescriptionChunkToFill);
	}

	return _pDescriptionChunkToFill;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	convert the value of this object to an explicit string. The object manages the string privately,
		so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			Note: this is not virtual, but each class can manage m_pValueString or not.
	\return	the value as a const string. 
*/
const char	*PackDynamicType::ValueToString()
{
	const char *pDefaultReturn = "-";
	if(mSer_TypeEnum.Get() ==0 ) return pDefaultReturn; // default no choice.
	//if( !m_pValueString ) m_pValueString = new PackString;
	//if( m_pValueString )
	{
		//old false m_pValueString->Set( mSer_TypeEnum.ValueToString() );
		unsigned int	ii=1; // 0 means no selection.
		PackStruct::Cell	*pCell = GetFirstCell();
		while(pCell)
		{
			if(ii == mSer_TypeEnum.Get() ) return
				pCell->GetManagedObject()->ValueToString();
			ii++;
			pCell = pCell->GetNextBrotherCell() ;
		}
	}
	return pDefaultReturn;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Change the shape of this dynamic Type.
	\param	_shapeEnum an enum value, its meaning vary with registered types.
*/
void PackDynamicType::Set(unsigned int _shapeEnum )
{
	// of course do nothing if same.
	if( _shapeEnum == mSer_TypeEnum.Get() ) return;
	BaseType *pSelectObj = 0L;
	PackStruct::Cell	*pCell = GetFirstCell();
	unsigned int	ii=1;
	while(pCell)
	{
		if(ii ==_shapeEnum )  pSelectObj=pCell->GetManagedObject();
		ii++;
		pCell = pCell->GetNextBrotherCell() ;
	}
	if(ii<_shapeEnum ) return;
	BeginChangeNotification();

		// this set doesn't occur an update because it has not been ->SetManaged():
		// still, it is serialized and so undoed by BeginChangeNotification()
		mSer_TypeEnum.Set(_shapeEnum);
		m_pSelectedType =pSelectObj; // can be 0L.
		
	EndChangeNotification();	
}
#endif
