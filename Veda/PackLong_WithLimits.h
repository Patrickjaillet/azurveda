/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_PackLong_WithLimits_H
#define COM_M4NKIND_PackLong_WithLimits_H

#include "PackLong.h"

/*!
	\class	PackLong_WithLimits
	\ingroup	BaseSerializableClass
	\brief	 PackLong_WithLimits specialize PackLong to define a min and max value
			possible for this signed integer value. Specify these min,max after
			serialisation member registration. When compiled as non-editable,
			this class become a classic PackLong, nothing more is compiled.
*/

#ifdef _ENGINE_EDITABLE_
// if we are in editable mode:
class PackLong_WithLimits : public PackLong
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. 
	*/
	PackLong_WithLimits(void);
	/*!
		\brief	Destructor. 
	*/
	virtual ~PackLong_WithLimits(void);

	/*!
		\brief  Each BaseType's inherited classes must explicit an ID for their
				class, or let use one of the super class at least through this virtual method.
				This is needed by GUIs to detect the types of each sub-members, and shape
				an interface for each Object according to their member list.
		\return a const character string, that must be unique and unchanged for all serializable base type.
	*/
	virtual const char *GetClassID() const { return "PackLong_WithLimits"; };

	/*!
		\brief set the value of this type, which will be cut to teh defined limits.
		\param _value the value to set, which will be cut to teh defined limits.
	*/
	virtual void Set( signed int _value );

	/*!
		\brief set the minimum and maximum values possible for this type.
		 It has to be used at init. Doon't use it directly, use macro PACKLONG_WITHLIMITS_SETMINMAX()
		 to be compatible with non editable compilation mode.
		\param _min the minimum value possible for this object.
		\param _max the maximum value possible for this object.
	*/
	void	SetMinMax( signed int _min, signed int _max );

	/*!
		\brief get the minimum 
	*/
	inline int GetMin(){return(m_Minimum);};	
	/*!
		\brief set the maximum values possible for this type.
	*/
	inline int GetMax(){return(m_Maximum);};	

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	signed int	m_Minimum;
	signed int	m_Maximum;
};
/*!
	\def	REGISTER_MEMBER_WITHLIMITS_SETMINMAX

	\brief	This macro is used for member registration to specify the limits possible for this member:
*/
#define		REGISTER_MEMBER_WITHLIMITS_SETMINMAX(_object,_InfoString,_DefaultVal,_min,_max) \
REGISTER_MEMBER(_object,_InfoString)\
_object.SetMinMax( _min, _max );\
_object.Set(_DefaultVal);
#else
/*==================================================================
								WHEN NON EDITABLE:
==================================================================*/
// if we are in non-editable mode:
typedef PackLong PackLong_WithLimits;
/*!
	\def	REGISTER_MEMBER_WITHLIMITS_SETMINMAX

	\brief	This macro is used after member registration to specify the limits possible for this member.
			In non-editable mode, it does nothing.
*/

#define		REGISTER_MEMBER_WITHLIMITS_SETMINMAX(_object,_InfoString,_DefaultVal,_min,_max) \
REGISTER_MEMBER(_object,_InfoString)

// end of editable mode test :
#endif

// end of file:
#endif
