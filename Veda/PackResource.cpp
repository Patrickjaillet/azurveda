/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackResource.h"
#include "PackULong.h"
//#ifdef _ENGINE_EDITABLE_
#include <stdio.h>
//#endif
PackResource::PackResource(void): PackStruct()
	,m_pBinary(0L)
	,m_isLinkWith(false)
	,m_CurrentChunkSize(0L)
{
	REGISTER_MEMBER( mSer_FilePath,"FilePath");
	// binary length is then serialized, but can't be edited or undoed (no info string):
	REGISTER_MEMBER(mSer_BinarySerializedLength,0L);

}
#ifdef	_ENGINE_EDITABLE_
PackResource::~PackResource(void)
{
	this->FreeResource();
}
#endif
/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char * PackResource::Serialize_In(const unsigned char * _pDescriptionChunk)
{
#ifdef _ENGINE_EDITABLE_
	this->FreeResource();
#endif
	// classic serialization for mSer_FilePath  and mSer_BinaryLength:
	_pDescriptionChunk = PackStruct::Serialize_In(_pDescriptionChunk);

	unsigned int binlength = mSer_BinarySerializedLength.Get();
	if( binlength>0)	// if link mode
	{	// then load the binary serialized:
		m_pBinary = new unsigned char[binlength];
		if(m_pBinary)
		{
			unsigned int	ll;
			unsigned char *pbuffer = m_pBinary ;
			for(ll=0 ; ll<binlength ; ll++ ) *(pbuffer++) = *(_pDescriptionChunk++) ;
			m_CurrentChunkSize = binlength;
		}
		m_isLinkWith = true;
	} else
	{
		m_isLinkWith = false;
	}
	return _pDescriptionChunk;
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.(1,2,3, or4)
*/
unsigned int PackResource:: GetSerializedDescriptionSize(void)
{
	unsigned int size = PackStruct::GetSerializedDescriptionSize();
	if(  m_isLinkWith)// if linked with.
	{	
		size +=  mSer_BinarySerializedLength.Get();
	}
	return( size );
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * PackResource::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	// classic serialization for mSer_FilePath ,mSer_Flags and mSer_BinaryLength:
	_pDescriptionChunkToFill = PackStruct::Serialize_Out(_pDescriptionChunkToFill);
	unsigned int binlength = mSer_BinarySerializedLength.Get();
	if(	m_isLinkWith )	// if link mode.
	{	// save binary:
		unsigned int	ll=0;
		unsigned char *pbuffer = m_pBinary ;
		for(ll=0 ; ll<binlength ; ll++ ) *(_pDescriptionChunkToFill++) = *(pbuffer++) ;
	}
	return( _pDescriptionChunkToFill  );
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief delete if was allocated.
*/
void PackResource::FreeResource()
{
	if( m_pBinary )
	{
		delete [] m_pBinary;
		m_pBinary = 0L;
#ifdef _ENGINE_EDITABLE_
		mSer_BinarySerializedLength.Set(0);
#endif
		m_CurrentChunkSize = 0;
	}
}
#endif

/*!
	\brief Get the image of the file as a binary. if _pBinary is 0L, no binary.
			This is the main Get accessor method that should be used for a PackResource.
	\param _pBinary return a pointer to the binary, as a char table.
	\param _BinaryLength return the exact byte length of the binary.
*/
unsigned int PackResource::GetByteLength()
{
	// if link within:
	if( m_isLinkWith ) return mSer_BinarySerializedLength.Get();
	// else: it's the file file:
	if(mSer_FilePath.length() == 0) return 0; // test necessary on windows !!
	FILE *ff = fopen( mSer_FilePath.Get() , "rb" );
	if( !ff ) return(0); // open trouble: size is 0.
		// find file size:
		fseek( ff, 0 , SEEK_END );
		unsigned int filesize = ftell( ff );
	fclose(ff);

	return filesize;
}

/*!
	\brief Get the image of the file or linked chunk as a binary.
			if _pBinary is 0L, no binary, or file problem.
			This is the main Get accessor method that should be used for a PackResource.
			You can then read that binary chunk within your function, but don't store a
			pointer to it: It could be destroyed in the future.
			This class allows streaming with parameters _ByteOffset and _maximumLengthToGet, in File mode or in Linked mode.

	\param _pBinary return a pointer to the binary, as a char table.
	\param _BinaryLength return the exact byte length of the binary in _pBinary.
	\param	_ByteOffset
*/
void PackResource::GetBinaryAtOffset( const unsigned char *&_pBinary , unsigned int &_BinaryLengthReturned, 
				unsigned int _byteOffset,const unsigned int _maximumLengthToGet
	
	)
{
	_pBinary = 0L; // default: failure.
	_BinaryLengthReturned = 0L;
	unsigned int sersize = mSer_BinarySerializedLength.Get();
	if( m_isLinkWith ) 
	{	// if linked within:
		if( _byteOffset >= sersize )return; // error! offset after end !
		_pBinary = m_pBinary +_byteOffset ;
		sersize -= _byteOffset; // bytes left.
		if( sersize>_maximumLengthToGet ) sersize=_maximumLengthToGet;
		_BinaryLengthReturned = sersize ;	
		return;
	}
	// external file:
	if(mSer_FilePath.length() == 0) return ;
	FILE *ff = fopen( mSer_FilePath.Get() , "rb" );
	if( !ff ) return; // open trouble

	// find file size:
	fseek( ff, 0 , SEEK_END );
	unsigned int filesize = ftell( ff );
	if(_byteOffset>=filesize ) return ; // offset after file.
	// compute the maximum size that will be load:
	filesize -= _byteOffset; 
	if(_maximumLengthToGet< filesize)
	{
		// if we ask only n bytes, we load just n bytes, that's all. Why would we load more, hu ? Why ? You tell me that, I don't know.
		filesize = _maximumLengthToGet;
	}
#ifdef _ENGINE_EDITABLE_
	this->FreeResource();
#endif
	m_pBinary = new unsigned char[ filesize ];
	if(!m_pBinary) 
	{	
		fclose(ff);
		return; // mem error !
	}
	// set at offset
	fseek( ff, _byteOffset , SEEK_SET );
	// read file:
	fread( m_pBinary , filesize , 1 , ff );
	fclose( ff ) ;
	m_CurrentChunkSize = filesize;
	_pBinary = m_pBinary;
	_BinaryLengthReturned = filesize;

}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief  set the origin file.
	\param	_pFilePath
*/
void	PackResource::SetFile(const char *_pFilePath)
{
	// do nothing if same file.
	if(  PackString::Compare(mSer_FilePath.Get(),_pFilePath) == 0 ) return;	
//	BeginChangeNotification(); <- automatic, mSer_FilePath is registered in this PackStruct.
	//bool wasLink = GetLinkMode();
	//SetLinkMode(false); // unlink previous/ free buffer.
	mSer_FilePath.Set( _pFilePath);
	//SetLinkMode(wasLink);

//	EndChangeNotification();

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief  Load and link the file.
	\param	_isLink true: is linked, false, is in file.
*/
void	PackResource::SetLinkMode(bool _isLinked)
{
	//unsigned int sersize = mSer_BinarySerializedLength.Get();
	if(_isLinked)
	{
		if( m_isLinkWith ) return; // already done if >0.
		// relink.
		unsigned int lengthread;
		// read inside:
		// Note: the const cast is needed by GCC, not by dotnet .
		this->FreeResource();
		GetBinaryAtOffset((const unsigned char * &) m_pBinary,lengthread);
		if(lengthread==0)
		{	
			m_isLinkWith = false;
			return; // dos problem occured: unlinkable.
		}
		BeginChangeNotification(); 
			// declare link inside object:
			mSer_BinarySerializedLength.Set( lengthread );
			m_isLinkWith = true;
		EndChangeNotification(); 
	} else
	{
		if( !m_isLinkWith ) return; // already done !
		BeginChangeNotification(); 
			// now it's in file:
			this->FreeResource();
			mSer_BinarySerializedLength.Set(0);
			m_isLinkWith = false;
		EndChangeNotification(); 
	}	
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief  get Link mode.
	\return	true is linked, false, is in file.
*/
bool	PackResource::GetLinkMode()
{
	return (m_isLinkWith);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief Give a binary Form: in file mode, it is saved, in link mode, it is kept.
	\param _pBinaryToSet
	\param _BinaryLength byte size of _pBinaryToSet
	\param	_append if true, chunk is added to the existing chunk or file.
	\return	true if OK, false if file mode write error.
*/
bool PackResource::SetBinary( const char *_pBinaryToSet , unsigned int _BinaryLength, bool _append )
{
	unsigned int	ii;
	
	if( m_isLinkWith)
	{
		if(m_CurrentChunkSize == 0) _append=false;
		if(!_append)
		{
			this->FreeResource();
			if(_BinaryLength ==0) return false;
			m_pBinary = new unsigned char[ _BinaryLength ];
			if(!m_pBinary) return false;
			for(ii=0; ii<_BinaryLength ; ii++ )m_pBinary[ii]=_pBinaryToSet[ii];
			m_CurrentChunkSize = _BinaryLength ;
		} else
		{ // append:
			unsigned int newlength = m_CurrentChunkSize+_BinaryLength;
			unsigned char	*pNewChunk = new unsigned char[ newlength ];
			if(!pNewChunk) return false;
			// copy old:
			for(ii=0; ii<m_CurrentChunkSize ; ii++ )pNewChunk[ii]=m_pBinary[ii];
			// copy next.
			for(ii=0; ii<_BinaryLength ; ii++ )pNewChunk[m_CurrentChunkSize+ii]=_pBinaryToSet[ii];
			m_CurrentChunkSize += _BinaryLength;
			delete m_pBinary;
			m_pBinary = pNewChunk;
		}
	}else
	{
		//this->FreeResource();
		if(_append)
		{
			// re-write file:
			FILE *ff = fopen( mSer_FilePath.Get() , "ab" );
			if( ff == 0L ) return false;	
			//fseek( ff, 0 , SEEK_END );
			fwrite( _pBinaryToSet, 1, _BinaryLength , ff );
			fclose( ff );
		} else // append:
		{
			// re-write file:
			FILE *ff = fopen( mSer_FilePath.Get() , "wb" );
			if( ff == 0L ) return false;	
			fwrite( _pBinaryToSet, 1, _BinaryLength , ff );
			fclose( ff );		
		}
	}
	return true;
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief  At init in edition mode, propose the file extension that this resource object manage.
	\param	_pFileExtenbsion without the dot, "mpg" , "jpg" , ...
*/
void	PackResource::SetFileExtension(const char *_pFileExtenbsion)
{
	m_FileExtension.Set(_pFileExtenbsion);
}
#endif
