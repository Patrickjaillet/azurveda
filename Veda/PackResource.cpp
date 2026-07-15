// SPDX-License-Identifier: LGPL-2.1-only

#include "PackResource.h"
#include "PackULong.h"

#include <stdio.h>

PackResource::PackResource(void): PackStruct()
	,m_pBinary(0L)
	,m_isLinkWith(false)
	,m_CurrentChunkSize(0L)
{
	REGISTER_MEMBER( mSer_FilePath,"FilePath");

	REGISTER_MEMBER(mSer_BinarySerializedLength,0L);

}
#ifdef	_ENGINE_EDITABLE_
PackResource::~PackResource(void)
{
	this->FreeResource();
}
#endif

const unsigned char * PackResource::Serialize_In(const unsigned char * _pDescriptionChunk)
{
#ifdef _ENGINE_EDITABLE_
	this->FreeResource();
#endif

	_pDescriptionChunk = PackStruct::Serialize_In(_pDescriptionChunk);

	unsigned int binlength = mSer_BinarySerializedLength.Get();
	if( binlength>0)
	{
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

unsigned int PackResource:: GetSerializedDescriptionSize(void)
{
	unsigned int size = PackStruct::GetSerializedDescriptionSize();
	if(  m_isLinkWith)
	{
		size +=  mSer_BinarySerializedLength.Get();
	}
	return( size );
}
#endif
#ifdef _ENGINE_EDITABLE_

unsigned char * PackResource::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{

	_pDescriptionChunkToFill = PackStruct::Serialize_Out(_pDescriptionChunkToFill);
	unsigned int binlength = mSer_BinarySerializedLength.Get();
	if(	m_isLinkWith )
	{
		unsigned int	ll=0;
		unsigned char *pbuffer = m_pBinary ;
		for(ll=0 ; ll<binlength ; ll++ ) *(_pDescriptionChunkToFill++) = *(pbuffer++) ;
	}
	return( _pDescriptionChunkToFill  );
}
#endif
#ifdef _ENGINE_EDITABLE_

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

unsigned int PackResource::GetByteLength()
{

	if( m_isLinkWith ) return mSer_BinarySerializedLength.Get();

	if(mSer_FilePath.length() == 0) return 0;
	FILE *ff = fopen( mSer_FilePath.Get() , "rb" );
	if( !ff ) return(0);

		fseek( ff, 0 , SEEK_END );
		unsigned int filesize = ftell( ff );
	fclose(ff);

	return filesize;
}

void PackResource::GetBinaryAtOffset( const unsigned char *&_pBinary , unsigned int &_BinaryLengthReturned,
				unsigned int _byteOffset,const unsigned int _maximumLengthToGet

	)
{
	_pBinary = 0L;
	_BinaryLengthReturned = 0L;
	unsigned int sersize = mSer_BinarySerializedLength.Get();
	if( m_isLinkWith )
	{
		if( _byteOffset >= sersize )return;
		_pBinary = m_pBinary +_byteOffset ;
		sersize -= _byteOffset;
		if( sersize>_maximumLengthToGet ) sersize=_maximumLengthToGet;
		_BinaryLengthReturned = sersize ;
		return;
	}

	if(mSer_FilePath.length() == 0) return ;
	FILE *ff = fopen( mSer_FilePath.Get() , "rb" );
	if( !ff ) return;

	fseek( ff, 0 , SEEK_END );
	unsigned int filesize = ftell( ff );
	if(_byteOffset>=filesize ) return ;

	filesize -= _byteOffset;
	if(_maximumLengthToGet< filesize)
	{

		filesize = _maximumLengthToGet;
	}
#ifdef _ENGINE_EDITABLE_
	this->FreeResource();
#endif
	m_pBinary = new unsigned char[ filesize ];
	if(!m_pBinary)
	{
		fclose(ff);
		return;
	}

	fseek( ff, _byteOffset , SEEK_SET );

	fread( m_pBinary , filesize , 1 , ff );
	fclose( ff ) ;
	m_CurrentChunkSize = filesize;
	_pBinary = m_pBinary;
	_BinaryLengthReturned = filesize;

}
#ifdef _ENGINE_EDITABLE_

void	PackResource::SetFile(const char *_pFilePath)
{

	if(  PackString::Compare(mSer_FilePath.Get(),_pFilePath) == 0 ) return;

	mSer_FilePath.Set( _pFilePath);

}
#endif
#ifdef _ENGINE_EDITABLE_

void	PackResource::SetLinkMode(bool _isLinked)
{

	if(_isLinked)
	{
		if( m_isLinkWith ) return;

		unsigned int lengthread;

		this->FreeResource();
		GetBinaryAtOffset((const unsigned char * &) m_pBinary,lengthread);
		if(lengthread==0)
		{
			m_isLinkWith = false;
			return;
		}
		BeginChangeNotification();

			mSer_BinarySerializedLength.Set( lengthread );
			m_isLinkWith = true;
		EndChangeNotification();
	} else
	{
		if( !m_isLinkWith ) return;
		BeginChangeNotification();

			this->FreeResource();
			mSer_BinarySerializedLength.Set(0);
			m_isLinkWith = false;
		EndChangeNotification();
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

bool	PackResource::GetLinkMode()
{
	return (m_isLinkWith);
}
#endif
#ifdef _ENGINE_EDITABLE_

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
		{
			unsigned int newlength = m_CurrentChunkSize+_BinaryLength;
			unsigned char	*pNewChunk = new unsigned char[ newlength ];
			if(!pNewChunk) return false;

			for(ii=0; ii<m_CurrentChunkSize ; ii++ )pNewChunk[ii]=m_pBinary[ii];

			for(ii=0; ii<_BinaryLength ; ii++ )pNewChunk[m_CurrentChunkSize+ii]=_pBinaryToSet[ii];
			m_CurrentChunkSize += _BinaryLength;
			delete m_pBinary;
			m_pBinary = pNewChunk;
		}
	}else
	{

		if(_append)
		{

			FILE *ff = fopen( mSer_FilePath.Get() , "ab" );
			if( ff == 0L ) return false;

			fwrite( _pBinaryToSet, 1, _BinaryLength , ff );
			fclose( ff );
		} else
		{

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

void	PackResource::SetFileExtension(const char *_pFileExtenbsion)
{
	m_FileExtension.Set(_pFileExtenbsion);
}
#endif
