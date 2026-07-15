// SPDX-License-Identifier: LGPL-2.1-only

#include "BaseType.h"

#ifdef	_ENGINE_EDITABLE_
#include "BaseObject.h"
#include "BaseContext.h"
#include "PackString.h"
#include <cstddef>
#include <stdio.h>
#endif

BaseType::BaseType(void) :
	m_pObjectThatManagesThis(0L)
#ifdef _ENGINE_EDITABLE_
	,m_pInfoString(0L)
	,m_pPreviousValueShape(0L)
	,m_pValueString(NULL)
#endif
{
}

void	BaseType::SetObjectThatManagesThis(BaseObject *_pManager)
{
	m_pObjectThatManagesThis = _pManager;
}

#ifdef _ENGINE_EDITABLE_
BaseType::~BaseType(void)
{
	if( m_pPreviousValueShape )
	{
		delete [] m_pPreviousValueShape;
		m_pPreviousValueShape = 0L;
	}

	if( m_pValueString )
	{
		delete m_pValueString ;
		m_pValueString =NULL;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned int BaseType::WriteFile(const char * _pFileName)
{

	unsigned int serializedFormSize = GetSerializedDescriptionSize();
	if( serializedFormSize == 0 ) return(1);
	unsigned char *pserializedForm = new unsigned char[ serializedFormSize ];
	unsigned char *pserializedFormAfter;
	if( pserializedForm == 0L ) return(2);

	FILE *ff = fopen( _pFileName , "wb+" );
	if( ff != 0L )
	{
		pserializedFormAfter =
			Serialize_Out( pserializedForm );
		fwrite( pserializedForm, 1, serializedFormSize , ff );
		fclose( ff );
	}
	ptrdiff_t vtest = pserializedFormAfter - pserializedForm;
	if( static_cast<size_t>(vtest) != serializedFormSize )
	{
		vtest++;
	}
	delete [] pserializedForm ;
	if( ff == 0L ) return(3);
	return(0);
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned int BaseType::ReadFile(const char * _pFileName)
{
	FILE *ff = fopen( _pFileName , "rb" );
	if( !ff ) return(1);

	fseek( ff, 0 , SEEK_END );
	unsigned int serializedFormSize = ftell( ff );
	fseek( ff, 0 , SEEK_SET );

	unsigned char *pserializedForm = new unsigned char[ serializedFormSize ];
	if( pserializedForm == 0L ){ fclose(ff); return(2); }

	fread( pserializedForm , serializedFormSize , 1 , ff );
	fclose( ff ) ;

	Serialize_In( pserializedForm );

	delete [] pserializedForm;

	return(0);
}
#endif

#ifdef _ENGINE_EDITABLE_

void	BaseType::SetInfoString(const char * _InfoString)
{
	m_pInfoString = _InfoString;
}
#endif
#ifdef _ENGINE_EDITABLE_

void	BaseType::MakeCloneOf( BaseType &_ModelObject )
{

	if( PackString::Compare( GetClassID(), _ModelObject.GetClassID() ) != 0) return;

	unsigned int serByteSize = 	_ModelObject.GetSerializedDescriptionSize();
	if( serByteSize == 0 ) return;

	unsigned char *pChunk = new unsigned char[serByteSize];
	if( !pChunk ) return;

	_ModelObject.Serialize_Out(pChunk);
	Serialize_In( pChunk );

	delete [] pChunk;
}
#endif

#ifdef _ENGINE_EDITABLE_

void	BaseType::BeginChangeNotification()
{

	if( !m_pObjectThatManagesThis ) return;

	if( !GetInfoString() ) return;

	if( m_pPreviousValueShape )
	{
		delete [] m_pPreviousValueShape;
		m_pPreviousValueShape = 0L;
	}

	if( m_pObjectThatManagesThis->GetContext() )
		m_pObjectThatManagesThis->GetContext()
			->NotifyObjectChange( BaseContext::eVedaUpdate_MemberIsGoingToChange,
							m_pObjectThatManagesThis,this );

	m_PreviousValueShapeLength = GetSerializedDescriptionSize();
	m_pPreviousValueShape = new unsigned char[m_PreviousValueShapeLength];

	if( !m_pPreviousValueShape ) return;
	Serialize_Out( m_pPreviousValueShape );

}
#endif
#ifdef _ENGINE_EDITABLE_

void	BaseType::EndChangeNotification()
{

	if( !m_pObjectThatManagesThis ) return;

	if( !GetInfoString() ) return;

	unsigned int AfterShapeLength = GetSerializedDescriptionSize();
	unsigned char *pAfterValueShape = new unsigned char[AfterShapeLength];
	if( pAfterValueShape )
	{
		Serialize_Out( pAfterValueShape );

		if(m_pObjectThatManagesThis->GetContext())
			m_pObjectThatManagesThis->GetContext()
			->NotifyObjectChange( BaseType::eVedaUpdate_MemberChanged,
									m_pObjectThatManagesThis,this,
									m_pPreviousValueShape,
									m_PreviousValueShapeLength,
									pAfterValueShape,
									AfterShapeLength
									);
	}

	if( m_pPreviousValueShape )
	{
		delete [] m_pPreviousValueShape;
		m_pPreviousValueShape = 0L;
	}
	if( pAfterValueShape )
	{
		delete [] pAfterValueShape;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

const char	*BaseType::ValueToString()
{
	if( m_pValueString )	return 	m_pValueString->Get() ;
	else return("");
}
#endif
