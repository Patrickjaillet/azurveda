// SPDX-License-Identifier: LGPL-2.1-only

#include "Media3DView.h"
#include "RenderTargetVirtual.h"
#include "Object3DVirtual.h"
#include "PackFloat.h"
#include <math.h>
BASEOBJECT_DECLARE_CLASS( "3dv", Media3DView, VirtualMedia,"Media 3D View","3DView","Render a camera view of a 3D Object or Scene." );

Media3DView::Media3DView() : VirtualMedia()
	,mSer_PositionRef(PackFloat::vd_XYZ)
	,mSer_TargetPositionRef(PackFloat::vd_XYZ)
	,mSer_FlipRot(PackFloat::vd_X)
	,mSer_FOV(PackFloat::vd_X)
{
	REGISTER_MEMBER_FLAG( mSer_Flags ,"Flag",scfl_NoClear,"NoClear" );
	REGISTER_MEMBER_PACKFLOAT( mSer_TimeLength ,"TimeLength",16.0f );

	REGISTER_MEMBER_REFERENCE(mSer_MainObject ,"MainObject", Object3DVirtual::m_Description );

	REGISTER_MEMBER_REFERENCE(mSer_RenderTargetRef ,"RenderTo", RenderTargetVirtual::m_Description );

	REGISTER_MEMBER(mSer_PositionRef ,"CamPos" );
	REGISTER_MEMBER(mSer_TargetPositionRef ,"CamTarget" );
	REGISTER_MEMBER(mSer_FlipRot ,"CamFlip" );
	REGISTER_MEMBER(mSer_FOV ,"CamFOV" );

#ifdef _ENGINE_EDITABLE_
	mSer_FOV.SetConstant(0.75f);
	mSer_PositionRef.SetConstant(1.5f,2);
#endif

}

bool Media3DView::CreateInternal(void)
{
	m_MediaTimeLength = mSer_TimeLength.Get();
	return true;
}

#ifdef _ENGINE_EDITABLE_

#endif

void	Media3DView::AtanO1O2( float *po1, float xd,float yd,float zd )
{
	float ffi,nz;
	zd=-zd;
	ffi = (float)atan2f(xd,zd);
        *(po1++) = ffi ;

        nz = (zd *cosf(ffi)) + (xd * sinf(ffi));
         *(po1) = -(float)atan2f( yd, nz ) ;

}

void Media3DView::ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort )
{
	float	paramEqu[4];
	float	resultEqu[4];

	Object3DVirtual *p3DObject = (Object3DVirtual *)mSer_MainObject.GetObjectPointer();
	if(!p3DObject) return;

	unsigned int flags = mSer_Flags.Get() ;

	_pViewPort->Matrix_LoadID();

	{
		float v_0 = PackFloat::m_0p0 ;
		float v_1 =PackFloat::m_1p0;

		paramEqu[0] = paramEqu[1] = paramEqu[2] = v_0;
		paramEqu[3]= (float)_frameDate;

		resultEqu[0]=v_0;
		mSer_FlipRot.Compute( resultEqu,paramEqu);
		 _pViewPort->Matrix_Rotate( resultEqu[0],v_0,v_0,-v_1);

		float c_x,c_y,c_z;
		resultEqu[0]=resultEqu[1]=v_0;
		resultEqu[2]=-1.0f;
		mSer_PositionRef.Compute( resultEqu,paramEqu);
		c_x = resultEqu[0];
		c_y = resultEqu[1];
		c_z = resultEqu[2];

		resultEqu[0]=resultEqu[1]=resultEqu[2]=v_0;
		mSer_TargetPositionRef.Compute( resultEqu,paramEqu);

		AtanO1O2(resultEqu,resultEqu[0]-c_x,resultEqu[1]-c_y,resultEqu[2]-c_z);
		_pViewPort->Matrix_Rotate( resultEqu[1],v_1,v_0,v_0);
		_pViewPort->Matrix_Rotate( resultEqu[0],v_0,v_1,v_0);

		_pViewPort->Matrix_Translate( -c_x, -c_y, -c_z);
	}

	RenderTargetVirtual *pRender = (RenderTargetVirtual *)
			mSer_RenderTargetRef.GetObjectPointer();
	if(pRender)
	{
		_pViewPort = pRender->GetViewPort();
		if(!_pViewPort) return;
	}
	resultEqu[0]=0.0f;
	mSer_FOV.Compute(resultEqu,paramEqu);
	if( resultEqu[0] == 0.0f ) resultEqu[0] = 0.75f;
	_pViewPort->SetFOVLength( resultEqu[0] );

	if( !(flags & scfl_NoClear) ) _pViewPort->Clear();
	const float white[4]={1.0f,1.0f,1.0f,1.0f};
	p3DObject->RenderObject( _frameDate, _pViewPort, white );

}
