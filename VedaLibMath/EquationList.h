// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_EquationList_H
#define COM_M4NKIND_EquationList_H

#include "VirtualEquation.h"
#include "PackFloat.h"
#include "PackStruct.h"
#include "PackULong_Enums.h"
#include "PackList.h"

class EquationList : public VirtualEquation
{

public:

	EquationList(void);

	BASEOBJECT_DEFINE_CLASS(EquationList);

	virtual void Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] );

	enum  {
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

	enum  {
		eEquOp_Equal=0,
		eEquOp_Add,
		eEquOp_Sub,
		eEquOp_Mul,
		eEquOp_Div,
		eEquOp_Sin,
		eEquOp_Cos,
		eEquOp_Sqrt
	};

	class EquationListLine : public PackStruct
	{
	public:

		PackULong_Enums	mSer_AffectedVarEnum;

		PackULong_Enums	mSer_OperatorEnum;

		PackULong_Enums	mSer_UsedVarEnum;

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

	static BaseType *NewOperator(){ return new EquationListLine(); };

	PackList 	mSer_OperationList;

	static BaseType *NewPackFloat();

	PackList 	mSer_ValueList;

	static	float	Op_Equal(float _Affect,float _With);
	static	float	Op_Add(float _Affect,float _With);
	static	float	Op_Sub(float _Affect,float _With);
	static	float	Op_Mul(float _Affect,float _With);
	static	float	Op_Div(float _Affect,float _With);
	static	float	Op_Sin(float _Affect,float _With);
	static	float	Op_Cos(float _Affect,float _With);
	static	float	Op_Sqrt(float _Affect,float _With);

	static float (*m_Op_FunctionTable[])(float _Affect,float _With);

protected:

};
#endif
