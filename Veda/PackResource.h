/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackResource_H
#define COM_M4NKIND_PackResource_H

#include "PackStruct.h"
#include "PackULong_Flags.h"
#include "PackString.h"

/*!
	\class	PackResource
	\ingroup	BaseSerializableClass
	\brief	 Base type managing a file as a resource. the file could be external, on the disk,
			or internal, which means linked into the serialized context.
			In all compilation modes, You get the resource with GetBinary(). In editable mode,
			you can set a binary file 
			You Set() and Get() a file name relative to the executable path,
			and choose if it should be linked while serialized, or just read from the file at inition.
			In all cases, you get the binary with GetBinary() 
			It is used for the automated serialization.

*/

class PackResource : public PackStruct
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
	*/
	PackResource(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackResource(void);
#endif
	/*!
		\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
		\param	_pDescriptionChunk the objet description chunk. 
		\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
	*/
	virtual const unsigned char * Serialize_In(const unsigned char * _pDescriptionChunk);

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
	virtual const char *GetClassID() const { return "PackStruct::PackResource"; };
#endif		
	/*!
		\brief Get the image of the file as a binary. if _pBinary is 0L, no binary.
				This is the main Get accessor method that should be used for a PackResource.
		\param _pBinary return a pointer to the binary, as a char table.
		\param _BinaryLength return the exact byte length of the binary.
	*/
	unsigned int GetByteLength();

	/*!
		\brief Get the image of the file or linked chunk as a binary.
				if _pBinary is 0L, no binary, or file problem.
				This is the main Get accessor method that should be used for a PackResource.
				You can then read that binary chunk within your function, but don't store a
				pointer to it: It could be destroyed in the future.
				(Note it is destroyed by a next call.)
				This class allows streaming with parameters _ByteOffset and _maximumLengthToGet, in File mode or in Linked mode.

		\param _pBinary return a pointer to the binary, as a char table.
		\param _BinaryLength return the exact byte length of the binary in _pBinary.
		\param	_ByteOffset
	*/
	void GetBinaryAtOffset( const unsigned char *&_pBinary , unsigned int &_BinaryLengthReturned, 
					unsigned int _byteOffset=0UL,const unsigned int _maximumLengthToGet=0xffffffffUL
		);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  At init in edition mode, propose the file extension that this resource object manage.
		\param	_pFileExtenbsion without the dot, "mpg" , "jpg" , ...
	*/
	void	SetFileExtension(const char *_pFileExtenbsion);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  get the file extension that this resource object manage.
				can return null (0L) if not specified.
		\return 	_pFileExtenbsion
	*/
	inline const char *GetFileExtension() const { return m_FileExtension.Get(); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  set the origin file.
		\param	_pFilePath
	*/
	void	SetFile(const char *_pFilePath);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  access to the origin file path, even in link mode.
				can return 0L or invalid path.
		\return File Path.
	*/
	inline const char *GetFilePath() const { return mSer_FilePath.Get(); };
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  Load and link the file if true,
				declare the resource is a file outside the object if false.
		\param	_isLink true: is linked, false, is in file.
	*/
	void	SetLinkMode(bool _isLink);
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  get Link mode.
		\return	true is linked, false, is in file.
	*/
	bool	GetLinkMode();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief Give a binary Form: in external file mode, it is saved, in link mode, it is kept in memory.
		\param	_pBinaryToSet
		\param	_BinaryLength byte size of _pBinaryToSet
		\param	_append if true, chunk is added to the existing chunk or file.
		\return	true if OK, false if file mode write error.
	*/
	bool SetBinary( const char *_pBinaryToSet , unsigned int _BinaryLength, bool _append=false );
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  set or append a 2 bytes unsigned integer in Little endian.
				By default, it is added at the end of the file.
		\param	_short 2 bytes unsigned integer
		\param	_append if true, chunk is added to the existing chunk or file.
		\return	true if OK, false if file mode write error.
	*/
	inline bool SetLE_UnsignedShort( unsigned short _short, bool _append=true )
	{
		char bb;
		bb = _short;
		if(!SetBinary( &bb , 1, _append)) return false;
		bb = _short>>8;
		if(!SetBinary( &bb , 1, true)) return false;
	}
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief  set or append a 4 bytes unsigned integer in Little endian.
				By default, it is added at the end of the file.
		\param	_value 4 bytes unsigned integer
		\param	_append if true, chunk is added to the existing chunk or file.
		\return	true if OK, false if file mode write error.
	*/
	inline bool SetLE_UnsignedInt( unsigned int _value, bool _append=true )
	{
		char bb;
		//bool res;
		bb = _value;
		if(!SetBinary( &bb , 1, _append)) return false;
		bb = _value>>8;
		SetBinary( &bb , 1, true);
		bb = _value>>16;
		SetBinary( &bb , 1, true);
		bb = _value>>24;
		if(!SetBinary( &bb , 1, true)) return false;
	}
#endif

	/*
		\brief read Little Endian Unsigned short at fileOffset, and increment fileOffset after it.
		\param	_short the value read and returned.
		\return if false, _short invalid.
	*/
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
	/*
		\brief read Little Endian Signed short at fileOffset, and increment fileOffset after it.
		\param	_short the value read and returned.
		\return if false, _short invalid.
	*/
	inline	bool	GetLE_SignedShort( signed short &_short,unsigned int &_fileOffset)
		{ return GetLE_UnsignedShort(((unsigned short &)_short),_fileOffset); };
	/*
		\brief read Little Endian Unsigned 4 byte int at fileOffset, and increment fileOffset after it.
		\param	_short the value read and returned.
		\return if false, _short invalid.
	*/
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
	/*
		\brief read Little Endian Unsigned 4 byte int at fileOffset, and increment fileOffset after it.
		\param	_short the value read and returned.
		\return if false, _short invalid.
	*/
	inline	bool	GetLE_SignedInt( signed int &_int,unsigned int &_fileOffset)
		{ return GetLE_UnsignedInt(((unsigned int &)_int),_fileOffset); };

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the file path in file mode, empty or origin file path in linked mode. Editable.
	PackString			mSer_FilePath;
	//! the total linked length of the binary currently open.0 means filemode. computed, serialized but not editable.
	PackULong			mSer_BinarySerializedLength;
	//! the binary currently opened. The chunk is managed by the object. In Linked mode, the whole binary start here. In File mode, it start a chunk that can be any part (or the whole) of the file.
	unsigned char	*m_pBinary;
	//! means the current byte size of m_pBinary. Can be different from the resource byte size.
	unsigned int	m_CurrentChunkSize;
	//! true if link, false is in file.
	bool			m_isLinkWith;
#ifdef _ENGINE_EDITABLE_
	//! propose a file extension for the resource managed.
	PackString			m_FileExtension;
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief delete if was allocated.
	*/
	virtual void FreeResource();
#endif
};

/*!
	\def	REGISTER_MEMBER_RESOURCE

	\brief	This macro is used to register a serializable member in a class constructor.
			For editable mode, it uses _InfoString to explicit the use of the member, in order
			to display it in a GUI for example.	
	\param	_object
	\param	_InfoString	string to display on editor, 0L mean serialized but not edited.
	\param	_FileExtension
*/
#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_RESOURCE(_object,_InfoString,_FileExtension) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.SetFileExtension(_FileExtension);
#else
#define		REGISTER_MEMBER_RESOURCE(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);
#endif

// end of file.
#endif
