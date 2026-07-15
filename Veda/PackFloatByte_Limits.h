/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackFloatByte_Limits_H
#define COM_M4NKIND_PackFloatByte_Limits_H

#include "PackFloat.h"

/*!
	\class	PackFloatByte_Limits
	\ingroup	BaseSerializableClass
	\brief	 manage a float value, but serialize it in 1 byte, where the 256 value domain
			is scaled between 2 float given at inition. default ends are:[ 0.0f , 1.0f [

*/

class PackFloatByte_Limits : public PackFloat
{
/*==================================================================
								PUBLIC
==================================================================*/
public:

	/*!
		\brief	Constructor. 
	*/
	PackFloatByte_Limits(void);
	/*!
		\brief	Constructor To initialize PackFloat as a multi dimentional vector. 
		\param _DimensionEnum on of the 3 VectorDimension enum.
	*/
	PackFloatByte_Limits( VectorDimension _DimensionEnum );

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackFloatByte_Limits(void);
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
	virtual const char *GetClassID() const { return "PackFloat::PackFloatByte_Limits"; };
#endif		
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	change the value:
		\param	
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
	/*!
		\brief	Initialize the variation domain with 2 floats, then the 256 possible value will be defined inside.
		\param	_min minimum
		\param _max maximum.
	*/
	void	SetMinMax(float _min,float _max); 

	/*!
		\brief get the minimum 
	*/
	inline float GetMin(){return(m_MinValue);};	

	/*!
		\brief set the maximum values possible for this type.
	*/
	inline float GetMax(){return(m_MinValue+m_DomainLength);};	

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! minimum value of the variation domain: (this is inited, not serialized)
	float	m_MinValue;
	//! max-Min value of the variation domain: (this is inited, not serialized)
	float	m_DomainLength;

};

/*!
	\def	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS

	\brief	This macro is used to register a serializable member in a class constructor.
			For editable mode, it uses _InfoString to explicit the use of the member, in order
			to display it in a GUI for example.
		
*/
#ifdef _ENGINE_EDITABLE_
#define		REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(_object,_InfoString,_DefaultVal,_min,_max) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.SetMinMax( _min , _max );\
_object.Set(_DefaultVal);
#define		REGISTER_MEMBER_PACKFLOATBYTE_0_1(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);\
_object.SetInfoString( _InfoString );\
_object.Set(_DefaultVal);
#else

#define		REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(_object,_InfoString,_DefaultVal,_min,_max) \
RegisterSerializableMember(_object);\
_object.SetMinMax( _min , _max );
#define		REGISTER_MEMBER_PACKFLOATBYTE_0_1(_object,_InfoString,_DefaultVal) \
RegisterSerializableMember(_object);
// end if engine not editable:
#endif

// end of .h:
#endif
