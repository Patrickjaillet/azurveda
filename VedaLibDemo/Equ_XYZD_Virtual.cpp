#include "Equ_XYZD_Virtual.h"
#include "VirtualMachine.h"
BASEOBJECT_DECLARE_VIRTUALCLASS( "xyzt", Equ_XYZD_Virtual, VirtualEquation );

/*!
	\brief	Constructor. There should only be members initialisation there.
*/
Equ_XYZD_Virtual::Equ_XYZD_Virtual() : VirtualEquation()
	,m_LastComputedDate(PackFloat::m_Max)
{

}

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. This is done with this method. sub classes can implement it (or not) in
			any way, to explicit current shape of an object.<br>

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
	\param	_pPreviewConfiguration	
*/
/*void Equ_XYZD_Virtual::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

}*/
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	protected extention for ProcessPreview() that allows to draw the equation on a bitmap.
	\param	_frameDate a date, in second, which defines the effect cinematic.
*/
/*void	Equ_XYZD_Virtual::ProcessPreview_Equation(double _frameDate)
{


}*/
#endif
/*!
	\brief	 Compute a scalar value according to a parameter list (X,Y,Z,D).
	\param	_OutgoingParameterTable result table. elements are writen or not according to the equation.
	\param _IncomingParameterTable. float table, the (X,Y,Z,D) parameters.
	\return	scalar value in space.
*/
float Equ_XYZD_Virtual::Compute( float _x,float _y,float _z )
{
	SetYZConstant(_y,_z);
	return ComputeByX(_x);
}
/*!
	\brief	VirtualEquation 's virtual method redirection
	\param	_OutgoingParameterTable result table. elements are writen or not according to the equation.
	\param _IncomingParameterTable. float table, the (X,Y,Z,D) parameters.
*/
void Equ_XYZD_Virtual::Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )
{
	if(m_LastComputedDate==_IncomingParameterTable[3])
	{
		float cubelimits[6]; // we don't care.
		SetFrameDate(_IncomingParameterTable[3],0,cubelimits);
		m_LastComputedDate = _IncomingParameterTable[3];
	}
	SetYZConstant(_IncomingParameterTable[1],_IncomingParameterTable[2]);
	_OutgoingParameterTable[0] += ComputeByX(_IncomingParameterTable[0]);
}
