/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_EquationList_H
#define COM_M4NKIND_EquationList_H

#include "VirtualEquation.h"
#include "PackFloat.h"
#include "PackStruct.h"
#include "PackULong_Enums.h"
#include "PackList.h"
/*!
	\class	EquationList
	\ingroup BaseObjectInherited_VedaLibMath BaseObjectInheritedDocGroup
	\brief	Able to compute an equation according to a tree definition.
*/
class EquationList : public VirtualEquation
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	EquationList(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(EquationList);

	/*!
		\brief	 Compute a scalar value according to a parameter list (X,Y,Z,D).
		\param	_OutgoingParameterTable result table. elements are writen or not according to the equation.
		\param _IncomingParameterTable. float table, the (X,Y,Z,D) parameters.
	*/
	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

	/*!
		\brief	 enum of the available Variables	
	*/
	typedef enum  {
		eEqu_Var_ResultX=0,
		eEqu_Var_ResultY,
		eEqu_Var_ResultZ,
		eEqu_Var_ResultTime,
		eEqu_Var_InputX,
		eEqu_Var_InputY,
		eEqu_Var_InputZ,
		eEqu_Var_InputTime,
		eEqu_Var_v1,
		eEqu_Var_v2,
		eEqu_Var_v3,
		eEqu_Var_v4,
		eEqu_Var_v5,
		eEqu_Var_v6,
		eEqu_Var_v7,
		eEqu_Var_v8
	};
	/*!
		\brief	 enum of the available operators	
	*/
	typedef enum  {
		eEquOp_Equal=0,
		eEquOp_Add,
		eEquOp_Sub,
		eEquOp_Mul,
		eEquOp_Div,
		eEquOp_Sin,
		eEquOp_Cos,
		eEquOp_Sqrt
	};

	/*!
		\brief	 Nested class that describes a reference to an object3D, used as element in the list.
	*/
	class EquationListLine : public PackStruct
	{
	public:
		//! the variable touch:
		PackULong_Enums	mSer_AffectedVarEnum;
		//! the operator identifier:
		PackULong_Enums	mSer_OperatorEnum;
		//! the variable used:
		PackULong_Enums	mSer_UsedVarEnum;
	
		//! \brief constructor . It registers the members:
		EquationListLine(void) : PackStruct()
		{
			REGISTER_MEMBER_ENUMS(	mSer_AffectedVarEnum ,"Affect", eEqu_Var_ResultX,
				"ResultX.ResultY.ResultZ.ResultTime.InputX.InputY.InputZ.InputTime.v0.v1.v2.v3.v4.v5.v6.v7" );
			REGISTER_MEMBER_ENUMS(	mSer_OperatorEnum,"Operator",eEquOp_Equal,
				"=.+=.-=.*=./=.sinus().cosinus().sqrt()");
			REGISTER_MEMBER_ENUMS(	mSer_UsedVarEnum ,"With", eEqu_Var_ResultX,
				"ResultX.ResultY.ResultZ.ResultTime.InputX.InputY.InputZ.InputTime.v0.v1.v2.v3.v4.v5.v6.v7" );

		}
	};
	
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new MathOperator object.
	*/
	static BaseType *NewOperator(){ return new EquationListLine(); };

	//! the equation line list:
	PackList 	mSer_OperationList;

	/*!
		\brief	static method passed to the list to create new elements.
		\return a new MathOperator object.
	*/
	static BaseType *NewPackFloat();

	//! the start value list:
	PackList 	mSer_ValueList;

	static	float	Op_Equal(float _Affect,float _With);
	static	float	Op_Add(float _Affect,float _With);
	static	float	Op_Sub(float _Affect,float _With);
	static	float	Op_Mul(float _Affect,float _With);
	static	float	Op_Div(float _Affect,float _With);
	static	float	Op_Sin(float _Affect,float _With);
	static	float	Op_Cos(float _Affect,float _With);
	static	float	Op_Sqrt(float _Affect,float _With);

	//! function table to execute the operators by index.
	static float (*m_Op_FunctionTable[])(float _Affect,float _With);


/*==================================================================
								PROTECTED
==================================================================*/
protected:

};
#endif
