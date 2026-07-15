/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackFloat_H
#define COM_M4NKIND_PackFloat_H

#include "BaseType.h"

/*!
	\class	PackFloat
	\ingroup	BaseSerializableClass
	\brief	 Base type managing a serializable float value. It is serialized to 3 bytes,
			(or 1 byte if value is 0.0f). the 4th byte is always set to 0, so that the mantissa
			is only 15 bit long, not 23 as the usual "IEEE float".

*/

class PackFloat : public BaseType
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Default Constructor. 
	*/
	PackFloat(void);

	/*!
		\brief	VectorDimensions are enum used to initialize a PackFloat to a X,Y or a X,Y,Z vector. 
	*/
	typedef	enum {
		vd_X=1,
		vd_XY=2,
		vd_XYZ=3,
		vd_XYZD=4
	}	VectorDimension	;
	/*!
		\brief	Constructor To initialize PackFloat as a multi dimentional vector. 
		\param _DimensionEnum on of the 3 VectorDimension enum.
	*/
	PackFloat( VectorDimension _DimensionEnum );

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackFloat(void);
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
	virtual const char *GetClassID() const { return "PackFloat"; };
#endif		
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	convert the value of this object to an explicit string. The object manages the string privately,
			so just read it or copy it. the string would be destroyed with the objects, and changed when using Set() methods.
			 Note: this is not virtual, but each class can manage m_pValueString or not.
		\return	the value as a const string. 
	*/
	virtual const char	*ValueToString();
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	change the value:
		\param	_value  value
	*/
	virtual void	Set(float _value); 
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	change the value of a given dimension:
		\param _dimensionIndex	0 for X, 1 for Y, 2 for Z
		\param	_value  value
	*/
	virtual void	Set(unsigned int _dimensionIndex,float _value); 
#endif
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	change values of X,Y and Z.
		\param	_x  value
		\param	_y  value
		\param	_z  value
	*/
	inline void	Set3f(float _x, float _y, float _z){ Set(_x); Set(1,_y); Set(2,_z); };
#endif
	/*!
		\brief	Get the value
		\return	 the float
	*/
	inline float	Get() const { return m_value[0] ; };
	/*!
		\brief	Get the value. WILL CRASH IF _dimensionIndex>2.
		\param _dimensionIndex	0 for X, 1 for Y, 2 for Z
		\return	 the float
	*/
	inline float	Get(unsigned int _dimensionIndex) const { return m_value[_dimensionIndex] ; };

	/*!
		\brief	return Dimension Managed.
				it should be 

		\return	 the float
	*/
	inline VectorDimension	GetVectorDimension(){return (VectorDimension)m_NumberOfDimensionManaged ; };

	//! public const float value in order to optimize size access for equations:
	static const float	m_0p0;
	static const float	m_0p01; // 1/100
	static const float	m_0p00001;
	static const float	m_0p25;
	static const float	m_0p5;
	static const float	m_1p0;
	static const float	m_2p0;
	static const float	m_255p0;
	static const float	m_256p0;
	static const float	m_0p0625; // 1/16
	static const float	m_100p0;
	static const float	m_1Div255; // 1/255
	static const float	m_1Div256; // 1/255
	static const float  m_1Div64;
	static const float	m_Pi;
	static const float	m_2Pi;
	static const float	m_180DivPi;
	static const float	m_32767p0; //(1<<15)-1 , to make float-1,1 to 16bit conversion.
	static const float	m_Max; //= 3.402823466e+38F; // this is max positive.

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//!		the value of this class: 
	float	m_value[vd_XYZD];

	//! the number of Dimension managed if PackFloat was initialized as a vector
	const unsigned int	m_NumberOfDimensionManaged;
};

/*!
	\def	REGISTER_MEMBER_PACKFLOAT

	\brief	This macro is used to register a serializable member in a class constructor.
			For editable mode, it uses _InfoString to explicit the use of the member, in order
			to display it in a GUI for example.
		
*/
#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKFLOAT(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.Set(_DefaultVal);
#else
#define		REGISTER_MEMBER_PACKFLOAT(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);
#endif

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKFLOAT_XY(_object,_InfoString,_DefaultValX,_DefaultValY) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.Set(0,_DefaultValX);\
_object.Set(1,_DefaultValY);
#else
#define		REGISTER_MEMBER_PACKFLOAT_XY(_object,_InfoString,_DefaultValX,_DefaultValY) \
RegisterSerializableMember(_object);
#endif

#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKFLOAT_XYZ(_object,_InfoString,_DefaultValX,_DefaultValY,_DefaultValZ) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.Set(0,_DefaultValX);\
_object.Set(1,_DefaultValY);\
_object.Set(2,_DefaultValZ);
#else
#define		REGISTER_MEMBER_PACKFLOAT_XYZ(_object,_InfoString,_DefaultValX,_DefaultValY,_DefaultValZ) \
RegisterSerializableMember(_object);
#endif

// end of file:
#endif
