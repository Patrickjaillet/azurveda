// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PackResource_H
#define COM_M4NKIND_PackResource_H

#include "PackStruct.h"
#include "PackULong_Flags.h"
#include "PackString.h"

class PackResource : public PackStruct
{

public:

	PackResource(void);

#ifdef _ENGINE_EDITABLE_

	virtual ~PackResource(void);
#endif

	virtual const unsigned char * Serialize_In(const unsigned char * _pDescriptionChunk);

#ifdef _ENGINE_EDITABLE_

	virtual unsigned int  GetSerializedDescriptionSize(void);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual unsigned char * Serialize_Out(unsigned char * _pDescriptionChunkToFill);
#endif
#ifdef _ENGINE_EDITABLE_

	virtual const char *GetClassID() const { return "PackStruct::PackResource"; };
#endif

	unsigned int GetByteLength();

	void GetBinaryAtOffset( const unsigned char *&_pBinary , unsigned int &_BinaryLengthReturned,
					unsigned int _byteOffset=0UL,const unsigned int _maximumLengthToGet=0xffffffffUL
		);

#ifdef _ENGINE_EDITABLE_

	void	SetFileExtension(const char *_pFileExtenbsion);
#endif
#ifdef _ENGINE_EDITABLE_

	inline const char *GetFileExtension() const { return m_FileExtension.Get(); };
#endif
#ifdef _ENGINE_EDITABLE_

	void	SetFile(const char *_pFilePath);
#endif
#ifdef _ENGINE_EDITABLE_

	inline const char *GetFilePath() const { return mSer_FilePath.Get(); };
#endif
#ifdef _ENGINE_EDITABLE_

	void	SetLinkMode(bool _isLink);
#endif
#ifdef _ENGINE_EDITABLE_

	bool	GetLinkMode();
#endif
#ifdef _ENGINE_EDITABLE_

	bool SetBinary( const char *_pBinaryToSet , unsigned int _BinaryLength, bool _append=false );
#endif
#ifdef _ENGINE_EDITABLE_

	inline bool SetLE_UnsignedShort( unsigned short _short, bool _append=true )
	{
		char bb;
		bb = (char)_short;
		if(!SetBinary( &bb , 1, _append)) return false;
		bb = (char)(_short>>8);
		if(!SetBinary( &bb , 1, true)) return false;
	}
#endif
#ifdef _ENGINE_EDITABLE_

	inline bool SetLE_UnsignedInt( unsigned int _value, bool _append=true )
	{
		char bb;

		bb = (char)_value;
		if(!SetBinary( &bb , 1, _append)) return false;
		bb = (char)(_value>>8);
		SetBinary( &bb , 1, true);
		bb = (char)(_value>>16);
		SetBinary( &bb , 1, true);
		bb = (char)(_value>>24);
		if(!SetBinary( &bb , 1, true)) return false;
	}
#endif

	inline	bool	GetLE_UnsignedShort( unsigned short &_short,unsigned int &_fileOffset)
	{
		const unsigned char *pBinary;
		unsigned int	lengthReturned;
		GetBinaryAtOffset(pBinary,lengthReturned,_fileOffset,2);
		if(lengthReturned==2)
		{
			_fileOffset+=2;
			_short = (pBinary[1]<<8)|pBinary[0];
			return true;
		} else return false;
	};

	inline	bool	GetLE_SignedShort( signed short &_short,unsigned int &_fileOffset)
		{ return GetLE_UnsignedShort(((unsigned short &)_short),_fileOffset); };

	inline	bool	GetLE_UnsignedInt( unsigned int &_int,unsigned int &_fileOffset)
	{
		const unsigned char *pBinary;
		unsigned int	lengthReturned;
		GetBinaryAtOffset(pBinary,lengthReturned,_fileOffset,4);
		if(lengthReturned==4)
		{
			_fileOffset+=4;
			_int = (pBinary[3]<<24)|(pBinary[2]<<16)|(pBinary[1]<<8)|pBinary[0];
			return true;
		} else return false;
	};

	inline	bool	GetLE_SignedInt( signed int &_int,unsigned int &_fileOffset)
		{ return GetLE_UnsignedInt(((unsigned int &)_int),_fileOffset); };

protected:

	PackString			mSer_FilePath;

	PackULong			mSer_BinarySerializedLength;

	unsigned char	*m_pBinary;

	unsigned int	m_CurrentChunkSize;

	bool			m_isLinkWith;
#ifdef _ENGINE_EDITABLE_

	PackString			m_FileExtension;
#endif
#ifdef _ENGINE_EDITABLE_

	virtual void FreeResource();
#endif
};

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_RESOURCE(_object,_InfoString,_FileExtension) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.SetFileExtension(_FileExtension);
#else
#define		REGISTER_MEMBER_RESOURCE(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);
#endif

#endif
