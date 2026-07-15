// SPDX-License-Identifier: LGPL-2.1-only

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

const unsigned char *PackDynamicType::Serialize_In( const unsigned char * _pDescriptionChunk )
{

	_pDescriptionChunk = mSer_TypeEnum.Serialize_In( _pDescriptionChunk );

	m_pSelectedType = 0L;
	PackStruct::Cell	*pCell = GetFirstCell();
	unsigned int	ii=1;
	while(pCell)
	{
		if(ii == mSer_TypeEnum.Get() )  m_pSelectedType=pCell->GetManagedObject();
		ii++;
		pCell = pCell->GetNextBrotherCell() ;
	}

	if( m_pSelectedType )
	{
		_pDescriptionChunk = m_pSelectedType->Serialize_In( _pDescriptionChunk );
	}

	return _pDescriptionChunk;
}
#ifdef _ENGINE_EDITABLE_

unsigned int  PackDynamicType::GetSerializedDescriptionSize(void)
{
	register unsigned int	size = mSer_TypeEnum.GetSerializedDescriptionSize();

	if( m_pSelectedType )
	{
		size += m_pSelectedType->GetSerializedDescriptionSize();
	}

	return size;
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * PackDynamicType::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{

	_pDescriptionChunkToFill = mSer_TypeEnum.Serialize_Out( _pDescriptionChunkToFill );

	if( m_pSelectedType )
	{
		_pDescriptionChunkToFill = m_pSelectedType->Serialize_Out(_pDescriptionChunkToFill);
	}

	return _pDescriptionChunkToFill;
}
#endif
#ifdef _ENGINE_EDITABLE_

const char	*PackDynamicType::ValueToString()
{
	const char *pDefaultReturn = "-";
	if(mSer_TypeEnum.Get() ==0 ) return pDefaultReturn;

	{

		unsigned int	ii=1;
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

void PackDynamicType::Set(unsigned int _shapeEnum )
{

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

		mSer_TypeEnum.Set(_shapeEnum);
		m_pSelectedType =pSelectObj;

	EndChangeNotification();
}
#endif
