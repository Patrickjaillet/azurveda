/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "BaseType.h"

#ifdef	_ENGINE_EDITABLE_
#include "BaseObject.h"
#include "BaseContext.h"
#include "PackString.h"
#include <stdio.h>
#endif

BaseType::BaseType(void) :
	m_pObjectThatManagesThis(0L)	// we can assume it could be not initialized in non-ed mode.
#ifdef _ENGINE_EDITABLE_
	,m_pInfoString(0L)
	,m_pPreviousValueShape(0L)
	,m_pValueString(NULL)
#endif
{
}
/*!
	\brief	The Object this one belong to as a member. Should only be used at init.
	\param _pManager BaseObject
*/
void	BaseType::SetObjectThatManagesThis(BaseObject *_pManager)
{
	m_pObjectThatManagesThis = _pManager;  
}

#ifdef _ENGINE_EDITABLE_
BaseType::~BaseType(void)
{	// accidentally, this temp memory could have to be flushed (it shouldn't happen).
	if( m_pPreviousValueShape )
	{
		delete [] m_pPreviousValueShape;
		m_pPreviousValueShape = 0L;
	}
	// the string object that manages method ValueToString() die with the object:
	if( m_pValueString ) 
	{
		delete m_pValueString ;
		m_pValueString =NULL; 
	}
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write a serialized form, using GetSerializedDescriptionSize() and Serialize_Out().
	\param	_pFileName the file path. 
	\return 0 if OK.
*/
unsigned int BaseType::WriteFile(const char * _pFileName)
{
	// save the chunk.
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
	int vtest = (int)pserializedFormAfter - (int)pserializedForm;
	if( vtest != serializedFormSize )
	{
		vtest++; // if goes here, serialization has size bugs.
	}
	delete [] pserializedForm ;
	if( ff == 0L ) return(3);
	return(0);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Try to read a serialized form, using Serialize_In().
	\param	_pFileName the file path.
	\return 0 if OK.
*/
unsigned int BaseType::ReadFile(const char * _pFileName)
{
	FILE *ff = fopen( _pFileName , "rb" );
	if( !ff ) return(1);
	// find file size:
	fseek( ff, 0 , SEEK_END );
	unsigned int serializedFormSize = ftell( ff );
	fseek( ff, 0 , SEEK_SET );
	// alloc a chunk
	unsigned char *pserializedForm = new unsigned char[ serializedFormSize ];
	if( pserializedForm == 0L ){ fclose(ff); return(2); }
	// read file:
	fread( pserializedForm , serializedFormSize , 1 , ff );
	fclose( ff ) ;
	// re-make object:
	Serialize_In( pserializedForm );
	// delete chunk used:
	delete [] pserializedForm;
	//OK:
	return(0);
}
#endif


#ifdef _ENGINE_EDITABLE_
/*!
	\brief	explicit the use of an object when used as a class member, compiled only in editable mode.
			you can just not use that. use REGISTER_MEMBER macro instead.
	\param	_InfoString a short string explaining what's the object for. 
*/
void	BaseType::SetInfoString(const char * _InfoString)
{
	m_pInfoString = _InfoString;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief  Tool that makes an object take the value(s) of another. Use with care:
			You got to clone a PackUlong with a PackULong, a PackString with a PackString:
			Clone Object of the same type or it will crash .				
	\param _ModelObject the object took as a model.
*/
void	BaseType::MakeCloneOf( BaseType &_ModelObject )
{
	// check that both had  the same type.
	if( PackString::Compare( GetClassID(), _ModelObject.GetClassID() ) != 0) return;

	// use serialization to clone:
	unsigned int serByteSize = 	_ModelObject.GetSerializedDescriptionSize();
	if( serByteSize == 0 ) return; // shouldn't happen. serialized size always >0.

	unsigned char *pChunk = new unsigned char[serByteSize];
	if( !pChunk ) return;

	_ModelObject.Serialize_Out(pChunk);
	Serialize_In( pChunk );

	delete [] pChunk;
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	In edition mode, when a member of an object is changed (like in a Set()), 
	you got to use BeginChangeNotification(), make your changes, and then 
	EndChangeNotification(), so that the edition of this object and member
	is known by the context, and allows it to manage a GUI update and the undo stack.
*/
void	BaseType::BeginChangeNotification()
{	
	// we verify that this object IS a member managed by an object, then managed by a context:
	if( !m_pObjectThatManagesThis ) return;

	// to trace previous value for undo management, we 
	// must be an editable member, not a hidden member:
	if( !GetInfoString() ) return;
	// create a serialized shape of the previous value, to send the previous 
	// value to the context for undo management:
	// (first destroy shape if there was already one: it shouldnt happen as it is destroyed in EndChangeNotification().)
	if( m_pPreviousValueShape )
	{
		delete [] m_pPreviousValueShape;
		m_pPreviousValueShape = 0L;
	}
	// if object is managed itself by a context, send a notification
	// (it is used to lock the context in case of use by other thread)
	// it has to be done BEFORE setting the new value to keep object coherence.
	if( m_pObjectThatManagesThis->GetContext() )
		m_pObjectThatManagesThis->GetContext()
			->NotifyObjectChange( BaseContext::eVedaUpdate_MemberIsGoingToChange, 
							m_pObjectThatManagesThis,this );
	
	// keep a description of the previous value by serializing an image of it:
	m_PreviousValueShapeLength = GetSerializedDescriptionSize();
	m_pPreviousValueShape = new unsigned char[m_PreviousValueShapeLength];

	if( !m_pPreviousValueShape ) return; // no memory left ?
	Serialize_Out( m_pPreviousValueShape );

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	In edition mode, when a member of an object is changed (like in a Set()), 
	you got to use BeginChangeNotification(), make your changes, and then 
	EndChangeNotification(), so that the edition of this object and member
	is known by the context, and allows it to manage a GUI update and the undo stack.
*/
void	BaseType::EndChangeNotification()
{
	// we verify that this object IS a member managed by an object, then managed by a context:
	if( !m_pObjectThatManagesThis ) return;
	// and we must be an editable member, not a hidden member:
	if( !GetInfoString() ) return;
	// get an image of the member after the change:
	unsigned int AfterShapeLength = GetSerializedDescriptionSize();
	unsigned char *pAfterValueShape = new unsigned char[AfterShapeLength];
	if( pAfterValueShape ) 
	{
		Serialize_Out( pAfterValueShape );

		// notify the object change to the context, for GUI update and undo management:
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
	// finally destroy both PreviousValueShape and aftervalueShape:
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
/*!
	\brief	convert the value of this object to an explicit string. The object manages the string privately,
		so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
	\return	the value as a const string. 
*/
const char	*BaseType::ValueToString()
{
	if( m_pValueString )	return 	m_pValueString->Get() ;
	else return("");
}
#endif