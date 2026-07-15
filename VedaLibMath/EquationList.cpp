/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "EquationList.h"
#include <math.h>

#ifndef	sinf
#define	sinf	sin
#endif

#ifndef	cosf
#define	cosf	cos
#endif

#ifndef	sqrtf
#define	sqrtf	sqrt
#endif

BASEOBJECT_DECLARE_CLASS( "Ls", EquationList, VirtualEquation,"Equation by list","ListEqu.","Build a \"value =Func(X,Y,Z,Time)\" equation with operators." );

EquationList::EquationList() : VirtualEquation()
	,mSer_ValueList(NewPackFloat)
	,mSer_OperationList(NewOperator)
{
	REGISTER_MEMBER( mSer_ValueList,"VarValues" );
	REGISTER_MEMBER( mSer_OperationList,"Equation" );
	
}
/*!
	\brief	static method passed to the list to create new elements.
	\return a new MathOperator object.
*/
BaseType *EquationList::NewPackFloat()
{ 
	PackFloat	*pFloat = new PackFloat(); 
#ifdef _ENGINE_EDITABLE_
	pFloat->SetInfoString("Value");
#endif
	return pFloat;
}

/*!
	\brief	 Compute a scalar value according to a parameter list.
	\param	TODO		
*/
void EquationList::Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )
{
	float	vv[16];	// 8 var +8 additional variables known as v1,v2,...

	int	ii=0;
	// set default out to zero:
	while(ii<4)
	{	
		vv[ii] = 0.0f; // def result.
		ii++;
	}
	// copy def in:
	const float	*pIn = &(_IncomingParameterTable[0]);
	while(ii<8)
	{	
		vv[ii] = *(pIn++);
		ii++;
	}
	// give start values to additional variables if needed...
	PackList::Cell	*pCell = mSer_ValueList.GetFirstCell();
	while(pCell && ii<16)
	{
		vv[ii] = ((PackFloat *) pCell->GetManagedObject())->Get();
		pCell = pCell->GetNextBrotherCell();
		ii++;
	}
	// execute lines of the equation:
	pCell = mSer_OperationList.GetFirstCell();
	EquationListLine	*pEquationList;
	float	ff,uu;
	while(pCell)
	{
		pEquationList =  (EquationListLine *)pCell->GetManagedObject();
		
		ff = vv[ pEquationList->mSer_AffectedVarEnum.Get()	];
		uu = vv[ pEquationList->mSer_UsedVarEnum.Get()		];
		vv[ pEquationList->mSer_AffectedVarEnum.Get() ] = 
			m_Op_FunctionTable[ pEquationList->mSer_OperatorEnum.Get() ]( ff, uu );

		pCell = pCell->GetNextBrotherCell();
	}
	// copy result:
	// set default out to zero:
	for(ii=0 ;ii<4 ; ii++)
	{	
		_OutgoingParameterTable[ii] = vv[ii];
	}

}

float	EquationList::Op_Equal(float _Affect,float _With)
{
	return(_With);
}
float	EquationList::Op_Add(float _Affect,float _With)
{
	return(_Affect+_With);
}
float	EquationList::Op_Sub(float _Affect,float _With)
{
	return(_Affect-_With);
}
float	EquationList::Op_Mul(float _Affect,float _With)
{
	return(_Affect*_With);
}
float	EquationList::Op_Div(float _Affect,float _With)
{
	return(_Affect/_With);
}
float	EquationList::Op_Sin(float _Affect,float _With)
{
	return(sinf(_With * PackFloat::m_Pi));
}
float	EquationList::Op_Cos(float _Affect,float _With)
{
	return(cosf(_With * PackFloat::m_Pi ));
}
float	EquationList::Op_Sqrt(float _Affect,float _With)
{
	return(sqrtf(_With));
}

float (*EquationList::m_Op_FunctionTable[])(float _Affect,float _With)=
{
	&EquationList::Op_Equal, //need 0 subcells
	&EquationList::Op_Add, //need 0 subcells 
	&EquationList::Op_Sub, 
	&EquationList::Op_Mul,
	&EquationList::Op_Div, // need 0 subcell
	&EquationList::Op_Sin, // need 2: min,max
	&EquationList::Op_Cos, // need 1
	&EquationList::Op_Sqrt
};
