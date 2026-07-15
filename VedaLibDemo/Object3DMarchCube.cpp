#include "Object3DMarchCube.h"
#include "MarchinCubeSpace.h"
#include "LightModel.h"
#include "Equ_XYZD_Virtual.h"
#include "VirtualMachine.h"
#include "PackFloat.h"
#include <math.h>
#ifndef sqrtf
#define sqrtf sqrt
#endif
#ifdef _ENGINE_EDITABLE_
	#include <time.h>
#endif

//#include <kos.h>
BASEOBJECT_DECLARE_CLASS( "mcb", Object3DMarchCube, Object3DVirtualDynamic,"3D Object MarchingCube","Obj3DMeta","Object defined by space(X,Y,Z) equations. Threshold is the value that stands for the surface.");

Object3DMarchCube::Object3DMarchCube() : Object3DVirtualDynamic()
	,mSer_EquationTermList(Object3DMarchCube::NewEquationTerm)
	,m_pPass4EdgeCubeIndex(0L)
	,m_pVertexLightTerm(0L)
#ifdef _ENGINE_EDITABLE_
//, m_CurrentNumberOfVertex(0L)
#endif
{
	// patch Object3DVirtual::mSer_Flags to specify extra bit preferences:
	// it is already registered in super constructor by:
	// REGISTER_MEMBER_FLAG( mSer_Flags ,"Flags",0,"Static.LookCam" );
#ifdef _ENGINE_EDITABLE_
	// default is : one color
	mSer_Flags.AddFlags(OB3DFlag_OneColor);
	//old mSer_Flags.SetBitFieldInfo( "Static.LookCam.Rend32.Rend128.OneColor" );
	// now we manage color model with colorPackType.
	// change default value for Marchincube, because it does generate more than 1000...
	mSer_MaximumNumberOfVertex.Set(4000);
	mSer_MaximumNumberOfTriangle.Set(8000);
#endif
	REGISTER_MEMBER_REFERENCE(	mSer_MarchinCubeSpaceRef,"Cube Space",MarchinCubeSpace::m_Description );
	//! a special lightning model can be used or not:
	REGISTER_MEMBER_REFERENCE( mSer_LightModel,"LightModel",LightModel::m_Description );
	REGISTER_MEMBER_PACKFLOATBYTE_0_1( mSer_Thresold ,"Thresold", 0.3f );
	REGISTER_MEMBER( mSer_EquationTermList ,"Terms" );
}

#define FP_BITS(fp) (*(DWORD *)&(fp))
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS 0x3F800000
inline float rsqrtf(float v){
	float v_half = v * 0.5f;
	int i = *(int *) &v;
	i = 0x5f3759df - (i >> 1);
	v = *(float *) &i;
	return v * (1.5f - v_half * v * v);
}
// r = 1/p
#define FP_INV(r,p)                                                          \
{                                                                            \
    int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                 \
    r = *(float *)&_i;                                                       \
    r = r * (2.0f - (p) * r);                                                \
}

//! \brief constructor 
Object3DMarchCube::EquationTerm::EquationTerm() : PackStruct()
	,mSer_Color(PackFloat::vd_XYZD) // this float will manage RGBA.
{
	REGISTER_MEMBER_REFERENCE( mSer_Equation ,"Equation", Equ_XYZD_Virtual::m_Description );
	REGISTER_MEMBER_PACKFLOATBYTE_0_1( mSer_Color ,"Color", 1.0f );
}
inline void    Object3DMarchCube::CreateOneVertex_X_RGBA( const MarchinCubeSpace::MarchCubeExtraValue *_pCubeExtraValue,
							VirtualMachine::InternalVertex *_pvtx,
							float _rate )
{
	const unsigned short  iinv = (unsigned short)(_rate *255.0f);
	const unsigned short irate = 255 - iinv ;
	_pvtx->m_ColorRGBA[0] = m_ColorConversion[(unsigned char)(
		(((unsigned short)_pCubeExtraValue->u_RGBA[0])*irate +
		((unsigned short)_pCubeExtraValue[1].u_RGBA[0])*iinv)>>8)] ;
	_pvtx->m_ColorRGBA[1] = m_ColorConversion[(unsigned char)(
		(((unsigned short)_pCubeExtraValue->u_RGBA[1])*irate +
		((unsigned short)_pCubeExtraValue[1].u_RGBA[1])*iinv)>>8)] ;
	_pvtx->m_ColorRGBA[2] = m_ColorConversion[(unsigned char)(
		(((unsigned short)_pCubeExtraValue->u_RGBA[2])*irate +
		((unsigned short)_pCubeExtraValue[1].u_RGBA[2])*iinv)>>8)] ;
	_pvtx->m_ColorRGBA[3] = 255;
}
inline void    Object3DMarchCube::CreateOneVertex_Y_RGBA( const MarchinCubeSpace::MarchCubeExtraValue *_pCubeExtraValue,
							VirtualMachine::InternalVertex *_pvtx,
							float _rate )
{
	const unsigned short  iinv = (unsigned short)(_rate *255.0f);
	const unsigned short irate = 255 - iinv ;
	_pvtx->m_ColorRGBA[0] = m_ColorConversion[(unsigned char)(
		(((unsigned short)_pCubeExtraValue->u_RGBA[0])*irate +
		((unsigned short)_pCubeExtraValue[m_nxt].u_RGBA[0])*iinv)>>8)] ;
	_pvtx->m_ColorRGBA[1] = m_ColorConversion[(unsigned char)(
		(((unsigned short)_pCubeExtraValue->u_RGBA[1])*irate +
		((unsigned short)_pCubeExtraValue[m_nxt].u_RGBA[1])*iinv)>>8)] ;
	_pvtx->m_ColorRGBA[2] = m_ColorConversion[(unsigned char)(
		(((unsigned short)_pCubeExtraValue->u_RGBA[2])*irate +
		((unsigned short)_pCubeExtraValue[m_nxt].u_RGBA[2])*iinv)>>8)] ;
	_pvtx->m_ColorRGBA[3] = 255;
}
inline void    Object3DMarchCube::CreateOneVertex_Z_RGBA( const MarchinCubeSpace::MarchCubeExtraValue *_pCubeExtraValue,
							VirtualMachine::InternalVertex *_pvtx,
							float _rate )
{
	const unsigned short  iinv = (unsigned short)(_rate *255.0f);
	const unsigned short irate = 255 - iinv ;
	_pvtx->m_ColorRGBA[0] = m_ColorConversion[(unsigned char)(
		(((unsigned short)_pCubeExtraValue->u_RGBA[0])*irate +
		((unsigned short)_pCubeExtraValue[m_nxtnyt].u_RGBA[0])*iinv)>>8)] ;
	_pvtx->m_ColorRGBA[1] = m_ColorConversion[(unsigned char)(
		(((unsigned short)_pCubeExtraValue->u_RGBA[1])*irate +
		((unsigned short)_pCubeExtraValue[m_nxtnyt].u_RGBA[1])*iinv)>>8)] ;
	_pvtx->m_ColorRGBA[2] = m_ColorConversion[(unsigned char)(
		(((unsigned short)_pCubeExtraValue->u_RGBA[2])*irate +
		((unsigned short)_pCubeExtraValue[m_nxtnyt].u_RGBA[2])*iinv)>>8)] ;
	_pvtx->m_ColorRGBA[3] = 255;
}
inline void    Object3DMarchCube::CreateOneVertex_X_Normal( const float *_pCubeValue,
							VirtualMachine::InternalVertex *_pvtx,
							float _rate )
{
	// get diff vector from this:
	float vv,vv_x,nx1,ny1,nz1,nx2,ny2,nz2;
	vv		= *_pCubeValue;
	vv_x	= _pCubeValue[1];
	// find normal of the cube:
	nx1 = vv - vv_x;
	ny1 = vv - _pCubeValue[m_nxt];
	nz1 = vv - _pCubeValue[m_nxtnyt];
	// find normal of the next cube:
	 nx2 = vv_x - _pCubeValue[2];
	ny2 = vv_x - _pCubeValue[m_nxt+1];
	nz2 = vv_x - _pCubeValue[m_nxtnyt+1];
	// find average normal:	
	 nx1 += _rate*( nx2-nx1 );
	ny1 += _rate*( ny2-ny1 );
	nz1 += _rate*( nz2-nz1 );
	// normalize:

//	vv = 0.5f / sqrtf( nx1*nx1 + ny1*ny1 + nz1*nz1 );
	vv = 0.4f * rsqrtf(nx1*nx1 + ny1*ny1 + nz1*nz1);

	_pvtx->m_nx = nx1 * vv ;
	_pvtx->m_ny = ny1 * vv ;
	_pvtx->m_nz = nz1 * vv ;
	
}
inline void    Object3DMarchCube::CreateOneVertex_Y_Normal( const float *_pCubeValue,
							VirtualMachine::InternalVertex *_pvtx,
							float _rate )
{
	// get diff vector from this:
	float vv,vv_y,nx1,ny1,nz1,nx2,ny2,nz2;

	vv		= *_pCubeValue;
	vv_y	= _pCubeValue[m_nxt];
	// find normal of the cube:
	nx1 = vv - _pCubeValue[1];
	ny1 = vv - vv_y; 
	nz1 = vv - _pCubeValue[m_nxtnyt];
	// find normal of the next cube:
	nx2 = vv_y - _pCubeValue[m_nxt+1];
	 ny2 = vv_y - _pCubeValue[m_nxt+m_nxt];
	nz2 = vv_y - _pCubeValue[m_nxt+m_nxtnyt];
	// find average normal:
	nx1 += _rate*( nx2-nx1 );
	 ny1 += _rate*( ny2-ny1 );
	nz1 += _rate*( nz2-nz1 );
	// normalize:

	vv = 0.4f * rsqrtf(nx1*nx1 + ny1*ny1 + nz1*nz1);

	_pvtx->m_nx = nx1 * vv ;
	_pvtx->m_ny = ny1 * vv ;
	_pvtx->m_nz = nz1 * vv ;

}
inline void    Object3DMarchCube::CreateOneVertex_Z_Normal( const float *_pCubeValue,
							VirtualMachine::InternalVertex *_pvtx,
							float _rate )
{
	// get diff vector from this:
	float vv,vv_z,nx1,ny1,nz1,nx2,ny2,nz2;

	vv		= *_pCubeValue;
	vv_z	= _pCubeValue[m_nxtnyt];
	// find normal of the cube:
	nx1 = vv - _pCubeValue[1];
	ny1 = vv - _pCubeValue[m_nxt];
	nz1 = vv - vv_z ;
	// find normal of the next cube:
	nx2 = vv_z - _pCubeValue[m_nxtnyt+1];
	ny2 = vv_z - _pCubeValue[m_nxtnyt+m_nxt];
	 nz2 = vv_z - _pCubeValue[m_nxtnyt+m_nxtnyt];
	// find average normal:
	nx1 += _rate*( nx2-nx1 );
	ny1 += _rate*( ny2-ny1 );
	 nz1 += _rate*( nz2-nz1 );
	// normalize:

//	vv =  0.5f / sqrtf( nx1*nx1 + ny1*ny1 + nz1*nz1 );
	vv = 0.4f * rsqrtf(nx1*nx1 + ny1*ny1 + nz1*nz1);
	_pvtx->m_nx = nx1 * vv ;
	_pvtx->m_ny = ny1 * vv ;
	_pvtx->m_nz = nz1 * vv ;
}
/*!
	\brief	update the shape of the object for this date, during
			the lifetime the object, when it is created.
			by defdault, does nothing, so static object can be created
			with CreateInternal() only.
	\param	_newShapeTime the new date.
	\param	_shapeIndex the index of the shape to modify.
*/
void Object3DMarchCube::CreateShape(float _newShapeTime, unsigned int _shapeIndex)
{ 
	MarchinCubeSpace *pcubespace = (MarchinCubeSpace *) mSer_MarchinCubeSpaceRef.GetObjectPointer() ;
	// this test can't return because these states are secured by CreateInternal()
	//if( !pcubespace || !m_pObject3DBufferTable ) return; // no CubeSpace linked.
	VirtualMachine::InternalObject3DBuffer *pObject3DVirtualBuffer = m_pObject3DBufferTable->m_pObject3DBuffer;
	if( pObject3DVirtualBuffer->IsCompiledAsStatic() ) return;

#ifdef _ENGINE_EDITABLE_
	clock_t		functionStartTime=clock();
#endif
	/*---- pass 1: find scalar value at corners ----------*/
    /*---- pass 2: create vertex on the cubes edges & normals ------*/
    /*---- pass 3: create triangles refering vertex (now with 2) -----*/

	const int nx = pcubespace->GetLengthX() ;
    const int ny = pcubespace->GetLengthY() ;
    const int nz = pcubespace->GetLengthZ() ;
	const int nxt = nx+1;
	const int nyt = ny+1;
//	const int nzt = nz+1;

	m_nxt = nxt;
	m_nxtnyt = nxt*nyt;

	const float fxs = -1.0f;
	const float fxd = 2.0f/ (float)nx; 
	const float fyd = 2.0f/ (float)ny; 
	const float fzd = 2.0f/ (float)nz;

	const float	edgerate = mSer_Thresold.Get();

    /*------------ ---------------------------------------*/
    /*---- pass 1: find scalar value at corners ----------*/
    /*------------ ---------------------------------------*/
	float	boundCube[6];
	PackList::Cell *pCell = mSer_EquationTermList.GetFirstCell();
	// get a checkdate ticket from the cube space: if a subcube
	// has a different value, it has not been processed yet, and its value is false,
	// it means it is 1.0f.
	const unsigned int checkdate_pass1	= pcubespace->GetNextCheckDate();
	m_checkdate_pass1 = checkdate_pass1;
	while( pCell)
	{	// loop by equation object
		Equ_XYZD_Virtual	*pEquationObject = (Equ_XYZD_Virtual	*)
			((EquationTerm *) pCell->GetManagedObject())->mSer_Equation.GetObjectPointer();
		const float v255 = PackFloat::m_255p0;
		unsigned char elementColor[4];
		{			
			PackFloatByte_Color	*pElementColor = 
				&(((EquationTerm *) pCell->GetManagedObject())->mSer_Color);
			elementColor[0] = (unsigned char)(pElementColor->Get(0) * v255);
			elementColor[1] = (unsigned char)(pElementColor->Get(1) * v255);
			elementColor[2] = (unsigned char)(pElementColor->Get(2) * v255);
				elementColor[3] =255;//test (unsigned char)(pElementColor->Get(3) * v255);		
		}
		pCell = pCell->GetNextBrotherCell() ; // next
		if( !pEquationObject ) continue;

		int	TermIndex=0;
		// the x[2] y[2] z[z] inside bounds where term != 1.0

		while( pEquationObject->SetFrameDate((float)_newShapeTime,TermIndex,boundCube) )
		{
			float	fy,fz;
			/*for(zz=0 ; zz<6 ; zz++)
			{
				yy = boundCube[zz];
				if( yy<0 ) yy=0;
				boundIndex[zz]= yy;
			}*/
			// bound index start/end
			signed char	bisx,binx;
			signed char		bisy,biny;
			signed char		bisz,binz;
			bisx	=	(signed char)((boundCube[0]-fxs)/fxd);
				if( bisx<0 ) bisx =0;
				else if( bisx>nx ) bisx=nx;
			binx	=	(signed char)((boundCube[1]-fxs)/fxd)+1;
				if( binx<0 ) binx =0;
				else if( binx>nx ) binx=nx;
			bisy	=	(signed char)((boundCube[2]-fxs)/fyd);
				if( bisy<0 ) bisy =0;
				else if( bisy>ny ) bisy=ny;
			biny	=	(signed char)((boundCube[3]-fxs)/fyd)+1;
				if( biny<0 ) biny =0;
				else if( biny>ny ) biny=ny;	
			bisz	=	(signed char)((boundCube[4]-fxs)/fzd);
				if( bisz<0 ) bisz =0;
				else if( bisz>nz ) bisz=nz;
			binz	=	(signed char)((boundCube[5]-fxs)/fzd)+1;
				if( binz<0 ) binz =0;
				else if( binz>nz ) binz=nz;
						
			fz = fxs + (float)bisz * fzd;
 
			float			*pmc_value_s = pcubespace->GetCubeTable_Values();
			unsigned char	*pmc_flags_s = pcubespace->GetCubeTable_Flags();
			unsigned int	*pmc_checkDate_s = pcubespace->GetCubeTable_CheckDate();
			const float startfy = fxs + (float)bisy * fyd;
			const float startfx = fxs  +(float)bisx * fxd;
			// switch to color mix version if need.
			if( !mSer_Flags.TestFlags(OB3DFlag_OneColor) )
			{
				MarchinCubeSpace::MarchCubeExtraValue *pmc_ColorValue_s =
					pcubespace->GetCubeTable_ExtraValues();
				//MarchinCubeSpace::MarchCubeExtraValue elementColor;
				for(signed char zz=bisz ; zz<=binz ; zz++ )
				{
					fy = startfy;
					int index_yz= (zz*m_nxtnyt) + (bisy*nxt) + bisx ;
					for(signed char yy=bisy ; yy<=biny ; yy++ )
					{
						unsigned int	*pmc_checkDate = &pmc_checkDate_s[ index_yz ];
						float			*pmc_value = &pmc_value_s[ index_yz ] ;
						unsigned char	*pmc_flags = &pmc_flags_s[ index_yz ];
						MarchinCubeSpace::MarchCubeExtraValue *pmc_ColorValue = &pmc_ColorValue_s[ index_yz ];
						if( !pEquationObject->SetYZConstant(fy,fz) )
						{	// false means all the line is 1.0f
							for( signed char xx=bisx ; xx<=binx ; xx++ )
							{	// all are 1.0f
								if( *(pmc_checkDate) != checkdate_pass1 )
								{	// first value given to this cube:
									*(pmc_value)	 = 1.0f ;
									pmc_ColorValue->u_WholeBitField = 0xffffffff ;
									*(pmc_checkDate) = checkdate_pass1 ;
									*(pmc_flags) = 0;
								} 
								pmc_value++;
								pmc_ColorValue++;
								pmc_checkDate++;
								pmc_flags++;
							}
						} else
						{
							float fx = startfx;
							for(signed char xx=bisx ; xx<=binx ; xx++ )
							{ 
								register float fv = pEquationObject->ComputeByX(fx) ;
								if( *(pmc_checkDate) != checkdate_pass1 )
								{	// first value given to this cube corner:
									*(pmc_value)	 = fv ;
									unsigned short val = (unsigned short)(fv*v255);
									unsigned short inv = 255-val;
									val *=255; // default start val=255.
									pmc_ColorValue->u_RGBA[0] = 									
						(val + ((unsigned short)elementColor[0])*inv)>>8;
									pmc_ColorValue->u_RGBA[1] = 	
						(val + ((unsigned short)elementColor[1])*inv)>>8;
									pmc_ColorValue->u_RGBA[2] = 	
						(val + ((unsigned short)elementColor[2])*inv)>>8;
	//((unsigned short)(*(pmc_ColorValue).u_RGBA[3])*val + ((unsigned short)elementColor[3])*inv)>>8;
									*(pmc_checkDate) = checkdate_pass1 ;

								} else
								{	// "value" has already been processed in this pass.
									// so we multiply:
									unsigned short val = (signed short)(fv*v255);
									unsigned short inv = 255-val;
									pmc_ColorValue->u_RGBA[0] = 									
	((unsigned short)(pmc_ColorValue->u_RGBA[0])*val + ((unsigned short)elementColor[0])*inv)>>8;
									pmc_ColorValue->u_RGBA[1] = 	
	((unsigned short)(pmc_ColorValue->u_RGBA[1])*val + ((unsigned short)elementColor[1])*inv)>>8;
									pmc_ColorValue->u_RGBA[2] = 	
	((unsigned short)(pmc_ColorValue->u_RGBA[2])*val + ((unsigned short)elementColor[2])*inv)>>8;

									fv *= *(pmc_value) ;
									*(pmc_value) = fv;
								}
								*(pmc_flags++) = (unsigned char) ( fv < edgerate ); // MUST BE "0x01" if OK, and not -1, for pass 3.
								pmc_value++;
								pmc_ColorValue++;
								pmc_checkDate++;
								fx+=fxd;
							}
						}
						index_yz+= nxt;
						fy+=fyd;
					}
					fz+=fzd;
				} // end z loop
			} else // end of test "color mix version"
			{	// no-colormix version which is faster:
				for(signed char zz=bisz ; zz<=binz ; zz++ )
				{
					fy = startfy;
					int index_yz= (zz*m_nxtnyt) + (bisy*nxt) + bisx ;
					for(signed char yy=bisy ; yy<=biny ; yy++ )
					{
						unsigned int	*pmc_checkDate = &pmc_checkDate_s[ index_yz ];
						float			*pmc_value = &pmc_value_s[ index_yz ] ;
						unsigned char	*pmc_flags = &pmc_flags_s[ index_yz ];
						if( !pEquationObject->SetYZConstant(fy,fz) )
						{	// false means all the line is 1.0f
							for(signed char xx=bisx ; xx<=binx ; xx++ )
							{	// all are 1.0f
								if( *(pmc_checkDate) != checkdate_pass1 )
								{	// first value given to this cube:
									*(pmc_value)	 = 1.0f ;
									*(pmc_checkDate) = checkdate_pass1 ;
									*(pmc_flags) = 0;
								} 
								pmc_value++;
								pmc_checkDate++;
								pmc_flags++;
							}
						} else
						{
							float fx = startfx;
							for(signed char xx=bisx ; xx<=binx ; xx++ )
							{ 
								register float fv = pEquationObject->ComputeByX(fx) ;
								if( *(pmc_checkDate) != checkdate_pass1 )
								{	// first value given to this cube corner:
									*(pmc_value)	 = fv ;
									*(pmc_checkDate) = checkdate_pass1 ;
								} else
								{	// "value" has already been processed in this pass.
									// so we multiply:
									fv *= *(pmc_value) ;
									*(pmc_value) = fv;
								}
								*(pmc_flags++) = (unsigned char) ( fv < edgerate ); // MUST BE "0x01" if OK, and not -1, for pass 3.
								pmc_value++;
								pmc_checkDate++;
								fx+=fxd;
							}
						}
						index_yz+= nxt;
						fy+=fyd;
					}
					fz+=fzd;
				} // end z loop
			} // end of else test "no color mix version"
			TermIndex++;
		} // end loop by term of the equation.

	} // next equation object

    /*------------ ---------------------------------------*/
    /*---- pass 2: create vertex on the cubes edges ------*/
    /*    and find normals of the created vertex     ----*/
	/*    ->and create the cube polygons for the cube with offset index (0,-1,-1)
			(because this one has all its vertex created!) */
    /*------------ ---------------------------------------*/

	float			*pmc_value_s = pcubespace->GetCubeTable_Values();
	const unsigned char	*pmc_flags_s = pcubespace->GetCubeTable_Flags();
	MarchinCubeSpace::MarchCubeIndex	*pmc_Index_s = pcubespace->GetCubeTable_Index();
	unsigned int	*pmc_checkDate_s = pcubespace->GetCubeTable_CheckDate();
	// VirtualMachine::InternalTriangle are int[3]
	int	*pnref = (int *) (pObject3DVirtualBuffer->GetFirstTriangle());

	const int	maxref = mSer_MaximumNumberOfTriangle.Get()-5 ; //-5 because 5 tr can ba created per loop.
	const unsigned int checkdate_pass2 = checkdate_pass1+1; // pass 2. pass3=+8
	const unsigned int checkdate_pass3 = checkdate_pass1+1+8; // pass 2. pass3=+8
	const unsigned int *polyTable = pcubespace->GetPolyIndexTable() ;
	VirtualMachine::InternalVertex *pvtx =  pObject3DVirtualBuffer->GetFirstVertex();// &(g_ClientVertexBase[ nbVertexCreated ]);
	const int	nbvmax = mSer_MaximumNumberOfVertex.Get()-3 ;
	if(nbvmax<3) return;
	int	nbVertexCreated = 0;
	int	nbPolyCreated=0;
	unsigned char *pPass4EdgeCubeIndex = m_pPass4EdgeCubeIndex;
	const bool colormix = ! mSer_Flags.TestFlags(OB3DFlag_OneColor);
	LightModel *pLightModel = (LightModel *) mSer_LightModel.GetObjectPointer();

	const bool preparePass4CreatorsIndex = (pLightModel != 0L) && pLightModel->IsAmbientOcclusionActive();

pCell = mSer_EquationTermList.GetFirstCell();
while( pCell)
{
	Equ_XYZD_Virtual	*pEquationObject = (Equ_XYZD_Virtual *)
		((EquationTerm *) pCell->GetManagedObject())->mSer_Equation.GetObjectPointer();
	pCell = pCell->GetNextBrotherCell() ;
	if( !pEquationObject ) continue;// a cell can be empty.
	int	TermIndex=0;

	// the x[2] y[2] z[z] inside bounds where term != 1.0
	while( pEquationObject->SetFrameDate((float)_newShapeTime,TermIndex,boundCube) )
	{
		float	fx,fy,fz;

		// bound index start/end
		signed char		bisx,binx;
		signed char		bisy,biny;
		signed char		bisz,binz;
			bisx	=	(signed char)((boundCube[0]-fxs)/fxd);
				if( bisx<0 ) bisx =0;
				else if( bisx>nx ) bisx=nx;
			binx	=	(signed char)((boundCube[1]-fxs)/fxd);
				if( binx<0 ) binx =0;
				else if( binx>nx ) binx=nx;
			bisy	=	(signed char)((boundCube[2]-fxs)/fyd);
				if( bisy<0 ) bisy =0;
				else if( bisy>ny ) bisy=ny;
			biny	=	(signed char)((boundCube[3]-fxs)/fyd);
				if( biny<0 ) biny =0;
				else if( biny>ny ) biny=ny;
			bisz	=	(signed char)((boundCube[4]-fxs)/fzd);
				if( bisz<0 ) bisz =0;
				else if( bisz>nz ) bisz=nz;
			binz	=	(signed char)((boundCube[5]-fxs)/fzd);
				if( binz<0 ) binz =0;
				else if( binz>nz ) binz=nz;

		fz = fxs + ((float)bisz) * fzd;
		const float startfy = fxs + (float)bisy * fyd;
		const float startfx = fxs  +(float)bisx * fxd;
		for(signed char zz=bisz ; zz<=binz ; zz++ )
		{
			fy = startfy;
			int index_yz= (zz*m_nxtnyt) + (bisy*nxt) + bisx ;
			for(signed char yy=bisy ; yy<=biny ; yy++ )
			{
				unsigned char	kk;
				const float			*pmc_value = &pmc_value_s[ index_yz ] ;
				const unsigned char	*pmc_flags = &pmc_flags_s[ index_yz ];
				unsigned int	*pmc_checkDate = &pmc_checkDate_s[ index_yz ];
MarchinCubeSpace::MarchCubeIndex	*pmc_Index = &pmc_Index_s[ index_yz ];
				const unsigned char	*pmc_fl2;
				// prepare 4 bit for the cube octo-configuration (the 4 lowest bit are the 4 upper of the next.)
				const bool checkPolygon = ( (zz>bisz) && (yy>bisy) && (zz != nz) && (yy != ny) );
				if( checkPolygon ) 
				{
					pmc_fl2 = pmc_flags-nxt-m_nxtnyt ; 
					kk = pmc_flags[0]<<3; // == pmc_fl2[nxt*nyt+nxt]
					kk |= pmc_fl2[ nxt ]<<2;
					kk |= pmc_fl2[ m_nxtnyt ]<<1;
					kk |= *pmc_fl2;
					pmc_fl2++;
				}
				fx = startfx; // fxs  +((float)bisx) * fxd;
				const unsigned char edgecheckYZ = // constant test term for YZ during X loop. used as a bit mask.
							(((unsigned char)( yy != ny ))<<1) // ==2 if edge Y
							|(((unsigned char)( zz != nz))<<2); // ==4 if edge Z
				MarchinCubeSpace::MarchCubeExtraValue *pmc_ExtraValue;
				pmc_ExtraValue = 0L;
				if(colormix)
					pmc_ExtraValue= &((pcubespace->GetCubeTable_ExtraValues())[index_yz]);
				for(signed char xx=bisx ; xx<=binx ; xx++ )
				{
					// true if still not passed in pass 2
					if( *(pmc_checkDate) < checkdate_pass2 ) 
					{
						unsigned char ii = *pmc_flags;
					//--------------------------------------
					// first vertex edge creation check
					/*unsigned char edgecheck = 
							((unsigned char)( yy != ny && (ii != (pmc_flags[nxt]))  )<<1) // ==2 if edge Y
							|((unsigned char)( zz != nz  &&  (ii != (pmc_flags[m_nxtnyt])) )<<2) // ==4 if edge Z
							|(unsigned char)( xx != nx  && (ii !=(pmc_flags[1]))  ); // ==1 if edge X
					*/
					unsigned char edgecheck = (
							 ((ii^pmc_flags[nxt])<<1) // ==2 if edge Y
							|((ii^pmc_flags[m_nxtnyt])<<2) // ==4 if edge Z
							) & edgecheckYZ ;
					edgecheck|=((unsigned char)( xx != nx)) & (ii^pmc_flags[1]);// ==1 if edge X
											
					if(edgecheck!=0) // at least one vertex to create. One test is better than 3 tests.
					{	// here, we know at least one, two or tree vertexes will be created.
						if( nbVertexCreated >= nbvmax  )
						{   // errorcase: not enough vertex in base:
							//pObject3DVirtualBuffer->SetNumberOfActiveTriangle(0);
							//return;
							nbPolyCreated = 0;
							goto endcase;
						}
						// if pass4 is needed, we keep an index of the cube that created vertexes.
						if(preparePass4CreatorsIndex)
						{
							*(pPass4EdgeCubeIndex++) = (unsigned char)xx;
							*(pPass4EdgeCubeIndex++) = (unsigned char)yy;
							*(pPass4EdgeCubeIndex++) = (unsigned char)zz;
						}
					//------------------- check line-> X
					if( edgecheck & 1)
					{
						//declare new vertex to grid ---
						pmc_Index->m_vx = nbVertexCreated ;
						float divv,ddivp = (pmc_value[1] -pmc_value[0]);
						FP_INV(divv,ddivp);
						float rate=  (edgerate-pmc_value[0] ) *divv ;
						//float rate=  (edgerate-fvv ) / ( pmc_value[1]-fvv) ;
						pvtx->m_x = pvtx->m_u = fx+ fxd*rate ;
						pvtx->m_y = pvtx->m_v = fy;
						pvtx->m_z = fz ;
						CreateOneVertex_X_Normal(pmc_value, pvtx, rate );
						if(colormix) CreateOneVertex_X_RGBA(pmc_ExtraValue, pvtx, rate);
						pvtx++;
						nbVertexCreated++;
					}
					//--------------------------------------
					//------------------- check line-> Y
					if(edgecheck & 2 )
					{
						//declare new vertex to grid ---
						pmc_Index->m_vy = nbVertexCreated ;
						pvtx->m_x = pvtx->m_u = fx;
						float divv,ddivp = ( pmc_value[nxt]-pmc_value[0]);
						FP_INV(divv,ddivp);
						float rate=  (edgerate-pmc_value[0] ) *divv ;
						pvtx->m_y = pvtx->m_v = fy+  fyd*rate ;
						pvtx->m_z = fz ;	
						CreateOneVertex_Y_Normal(pmc_value, pvtx, rate );
						if(colormix) CreateOneVertex_Y_RGBA(pmc_ExtraValue, pvtx, rate);
						pvtx++;
						nbVertexCreated++;
					}
					//--------------------------------------
					//------------------- check line-> Z
					if( edgecheck & 4 )
					{
						//declare new vertex to grid ---
						pmc_Index->m_vz = nbVertexCreated ;
						pvtx->m_x = pvtx->m_u = fx;
						pvtx->m_y = pvtx->m_v = fy;
						float divv,ddivp = ( pmc_value[m_nxtnyt]-pmc_value[0]);
						FP_INV(divv,ddivp);
						float rate=  (edgerate-pmc_value[0] ) *divv ;
						//float rate=  (edgerate-fvv ) / ( pmc_value[m_nxtnyt]-fvv) ;
						pvtx->m_z = fz +  fzd*rate;					
						CreateOneVertex_Z_Normal(pmc_value, pvtx, rate );
						if(colormix) CreateOneVertex_Z_RGBA(pmc_ExtraValue, pvtx, rate);
						pvtx++;
						nbVertexCreated++;
					}
					} // end of main edgecheck test.
					 *(pmc_checkDate) = checkdate_pass2+edgecheck;
					} // end test checkdate if still not passed in pass 2
					pmc_flags++;
					//------ create polygons for the cube of index 0,-1,-1:
					if( checkPolygon && xx != binx )
					{
						//----- compute cube okt value:
						kk<<=4;
						kk |= (*pmc_flags) <<3; // == pmc_fl2[nxt*nyt+nxt]
						kk |= pmc_fl2[ nxt ]<<2;
						kk |= pmc_fl2[ m_nxtnyt ]<<1;
						kk |= *pmc_fl2;
						pmc_fl2++; // _flags, -y-z lines before.
						//test pmc_checkDate2++;
						// here kk [0,255]
						//---- search polygon creation pattern on table:
						if(kk !=0 && kk !=255 && *(pmc_checkDate) < checkdate_pass3 )
						{
							// for the polygons:
							const unsigned int    *poctcase;
							// each cube configuration sizes: 16*4
							int	memoryindex =(int)( ((pmc_Index-nxt)-m_nxtnyt) );
							poctcase = &polyTable[kk<<4];
							int ll = *(poctcase++);
							if( nbPolyCreated >= maxref )
							{ // error case: too much polygons
								//pObject3DVirtualBuffer->SetNumberOfActiveTriangle(0);
								//return;
								nbPolyCreated = 0;
								goto endcase;
							}
							nbPolyCreated+=ll;
							int ii;
							for(ii=0 ; ii<ll ; ii++)
							{
								//--- 3 vertexes by triangle added:
								//  printf(" :%d:", *(	(unsigned int *)( ((int)mc2)+	*(octcase)))		);
// we do some warning test in edition mode... this tests shuldn't happen.
#ifdef _ENGINE_EDITABLE_
								int tt1,tt2,tt3;
#endif
								*(pnref++) = 
#ifdef _ENGINE_EDITABLE_
									tt1 =
#endif
									(int)
									*((unsigned short *)( memoryindex+
												*(poctcase++))) ;
									//printf("%d:", *((unsigned int *)( ((int)mc2)+*(octcase)))	);
								*(pnref++) = 
#ifdef _ENGINE_EDITABLE_									
									tt2 =
#endif									
									(int)
									*((unsigned short *)( memoryindex+
												*(poctcase++))) ;
									//printf("%d: ", *((unsigned int *)( ((int)mc2)+*(octcase))));
								*(pnref++) = 
#ifdef _ENGINE_EDITABLE_
									tt3 =
#endif									
									(int)
									*((unsigned short *)( memoryindex+
												*(poctcase++))) ;
#ifdef _ENGINE_EDITABLE_
								if(tt1<0 || tt1>=nbVertexCreated ||
									tt2<0 || tt2>=nbVertexCreated ||
									tt3<0 || tt3>=nbVertexCreated 
									)
								{
									// SHOULDNT HAPPEN !!!
									nbPolyCreated = 0;
									goto endcase;								
								}
#endif
							} // end loop by triangle
							//old *(pmc_checkDate) = checkdate_pass2+1;
							*(pmc_checkDate) += 8; // keep trace of the vertex created on the cubes.
						} // end check cube

					}
					//--- next cube ---
					pmc_Index++;
					pmc_value++;
					pmc_ExtraValue++;
					pmc_checkDate++;
					fx+=fxd;
				} // end loop X
				index_yz+= m_nxt;
				fy+=fyd;
			}
			fz+=fzd;
		} // end loop by Z.
		//end loop by term of the equation:
		TermIndex++;
	}
	// next equation object:
}
    //printf("\n");
    //printf("total ref created:%d:\n", nbPolyRefCreated );
endcase:
	m_NbVertexCreated = nbVertexCreated;
	pObject3DVirtualBuffer->SetNumberOfActiveTriangle(nbPolyCreated);

	Pass4_Diffuse(_newShapeTime);
	// light model affinage pass:
	if(preparePass4CreatorsIndex)
	{
		m_NumberOfCreativeCubes = ((unsigned int)pPass4EdgeCubeIndex-(unsigned int)m_pPass4EdgeCubeIndex)/3 ;
		Pass4_AmbientOcclusion();
	}

	// if asked, compile as static shape:
	if( mSer_Flags.TestFlags(OB3DFlag_Static) ) pObject3DVirtualBuffer->CompileAsStatic();

#ifdef _ENGINE_EDITABLE_
	clock_t		functionEndTime=clock();

	m_ObjectInfoLine.Set("NbV:");
	m_ObjectInfoLine.AddInt(nbVertexCreated);
	m_ObjectInfoLine.AddString("  nbTr:");
	m_ObjectInfoLine.AddInt(nbPolyCreated);
	m_ObjectInfoLine.AddString("  built:");
	m_ObjectInfoLine.AddInt(functionEndTime-functionStartTime);
#endif	

}

/*!
	\brief	just use CreateInternal() to return an error in some case...
	no use in other cases...
*/
bool Object3DMarchCube::CreateInternal(void)
{
	unsigned int objectFlags = 				
				VirtualMachine::bOb3D_VertexNormal|
				VirtualMachine::bOb3D_VertexUVMapping;
	// if we have no lightning model, we don't manage vertex colors, and the texture does.
	if( mSer_LightModel.GetObjectPointer()!=0L ||
		!(mSer_Flags.TestFlags( OB3DFlag_OneColor ))
		) objectFlags |= VirtualMachine::bOb3D_VertexRGBA ;
	// this alloc open a 3D buffer with maximum set by mSer_MaximumNumberOfVertex/Triangle:
	bool result = Object3DVirtualDynamic::CreateInternal_Alloc1Internal3DBuffer(objectFlags);
	if(!result) return result;
	MarchinCubeSpace *pcubespace = (MarchinCubeSpace *) mSer_MarchinCubeSpaceRef.GetObjectPointer() ;
	if(!pcubespace)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR("a Cube Space must be linked.");
		return false;
	}
	PackList::Cell *pCell = mSer_EquationTermList.GetFirstCell();
	if( !pCell) // empty list
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR("The equation term list is empty.");
		return false;		
	}
	m_pPass4EdgeCubeIndex = new unsigned char[mSer_MaximumNumberOfVertex.Get()*3 ];
	if(!m_pPass4EdgeCubeIndex)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
		return false;	
	}
	m_pVertexLightTerm = new float[mSer_MaximumNumberOfVertex.Get()*3];
	if(!m_pVertexLightTerm)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
		return false;	
	}

	float	vv=0.0f; 
	for(int ii=0 ; ii<256 ; ii++)
	{
		m_ColorConversion[ii]=vv;
		vv+=(1.0f/256.0f);
	}
	// at last, set a first shape:
	CreateShape(PackFloat::m_0p0);
	return true;
}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void	Object3DMarchCube::CloseInternal(void)
{
	Object3DVirtualDynamic::CloseInternal();
	if(m_pVertexLightTerm)
	{
		delete [] m_pVertexLightTerm;
		m_pVertexLightTerm=0L;
	}
	if(m_pPass4EdgeCubeIndex)
	{
		delete [] m_pPass4EdgeCubeIndex;
		m_pPass4EdgeCubeIndex = 0L;
	}
}
#endif
/*
		\brief	do pass 4.
		pass1: scalar values computation
		pass2: vertex creation
		pass3: polygon creation (done with pass2)
		pass4: light model affinage, if needed.
*/
void	Object3DMarchCube::Pass4_Diffuse(float _framedate)
{
	LightModel *pLightModel = (LightModel *) mSer_LightModel.GetObjectPointer();
	if(!pLightModel) return;
	VirtualMachine::InternalObject3DBuffer *pObject3DVirtualBuffer = m_pObject3DBufferTable->m_pObject3DBuffer;
	float	inparam[4];
	inparam[0] = inparam[3] =_framedate;
	float	*pvlight;

	PackList::Cell *pLinearCell	= pLightModel->GetFirstLinearLight();
	const PackList::Cell *pKeepFirst=pLinearCell;
	while(pLinearCell)
	{
		int ii;
		LightModel::LinearLight *pLinearLight =  (LightModel::LinearLight *)pLinearCell->GetManagedObject();

		float	lightcolor[4];
		float	lightVector[4];

		lightcolor[0]=lightcolor[1]=lightcolor[2]=
		lightVector[1] =
		lightVector[0]=lightVector[2]=0.0f;
		pLinearLight->mSer_Color.Compute(lightcolor,inparam);
		pLinearLight->mSer_Vector.Compute(lightVector,inparam);
		// normalize light vector:
		float invnorm=1.0f/sqrtf(lightVector[0]*lightVector[0]+
			lightVector[1]*lightVector[1]+
			lightVector[2]*lightVector[2]);
		lightVector[0]*=invnorm;
		lightVector[1]*=invnorm;
		lightVector[2]*=invnorm;
		VirtualMachine::InternalVertex *pVertex = pObject3DVirtualBuffer->GetFirstVertex();
		pvlight = m_pVertexLightTerm;
		if(pKeepFirst == pLinearCell)
		for(ii=0 ;ii<m_NbVertexCreated ; ii++)
		{
			float	vectordot=
				pVertex->m_nx *lightVector[0] +
				pVertex->m_ny *lightVector[1] +
				pVertex->m_nz *lightVector[2] ;
			if(vectordot<0.0f)vectordot=0.0f; // back to ambient.
			*pvlight++ = lightcolor[0]*vectordot;
			*pvlight++ = lightcolor[1]*vectordot;
			*pvlight++ = lightcolor[2]*vectordot;
			//if(vectordot>0.0f)vectordot=0.0f;
			pVertex++;
		}else
		for(ii=0 ;ii<m_NbVertexCreated ; ii++)
		{
			float	vectordot=
				pVertex->m_nx *lightVector[0] +
				pVertex->m_ny *lightVector[1] +
				pVertex->m_nz *lightVector[2] ;
			if(vectordot<0.0f)vectordot=0.0f; // back to ambient.
			*pvlight++ += lightcolor[0]*vectordot;
			*pvlight++ += lightcolor[1]*vectordot;
			*pvlight++ += lightcolor[2]*vectordot;

			//if(vectordot>0.0f)vectordot=0.0f;
			pVertex++;
		}
		pLinearCell = pLinearCell->GetNextBrotherCell();
	}
	// calibrate diffuse with ambient and apply to material color:
	float	AmbientColor[4];
	AmbientColor[0] = AmbientColor[1] = AmbientColor[2]=0.0f;
	pLightModel->mSer_AmbientColor.Compute(AmbientColor,inparam);
	VirtualMachine::InternalVertex *pVertex = pObject3DVirtualBuffer->GetFirstVertex();
	pvlight = m_pVertexLightTerm;
	int ii;
	if( ! mSer_Flags.TestFlags(OB3DFlag_OneColor) )
	for(ii=0 ;ii<m_NbVertexCreated ; ii++)
	{
		float lrr = *pvlight++;
		float lgg = *pvlight++;
		float lbb = *pvlight++;
		pVertex->m_ColorRGBA[0] *= (AmbientColor[0]+(1.0f-AmbientColor[0])*lrr);
		pVertex->m_ColorRGBA[1] *= (AmbientColor[1]+(1.0f-AmbientColor[1])*lgg);
		pVertex->m_ColorRGBA[2] *= (AmbientColor[2]+(1.0f-AmbientColor[2])*lbb);
		pVertex++;
	} else
	for(ii=0 ;ii<m_NbVertexCreated ; ii++) // without colormix, def color is white.
	{
		float lrr = *pvlight++;
		float lgg = *pvlight++;
		float lbb = *pvlight++;
		pVertex->m_ColorRGBA[0] = (AmbientColor[0]+(1.0f-AmbientColor[0])*lrr);
		pVertex->m_ColorRGBA[1] = (AmbientColor[1]+(1.0f-AmbientColor[1])*lgg);
		pVertex->m_ColorRGBA[2] = (AmbientColor[2]+(1.0f-AmbientColor[2])*lbb);
		pVertex++;
	}
	
}
/*
		\brief	do pass 4.
		pass1: scalar values computation
		pass2: vertex creation
		pass3: polygon creation (done with pass2)
		pass4: light model affinage, if needed.
*/
void	Object3DMarchCube::Pass4_AmbientOcclusion()
{	// if a lightniong object was set, do difuse lights:
	LightModel *pLightModel = (LightModel *) mSer_LightModel.GetObjectPointer();
	if(!pLightModel) return;
	// if pass4 launch, pcubespace must be OK:
	MarchinCubeSpace *pcubespace = (MarchinCubeSpace *) mSer_MarchinCubeSpaceRef.GetObjectPointer() ;
	const int nxt = pcubespace->GetLengthX()+1 ;
    const int nyt = pcubespace->GetLengthY()+1 ;
	//const int nz = pcubespace->GetLengthZ() ;
	const unsigned char *pPass4EdgeCubeIndex = m_pPass4EdgeCubeIndex;
	unsigned int	*pmc_checkDate_s = pcubespace->GetCubeTable_CheckDate();
	const float	*pmc_value_s = pcubespace->GetCubeTable_Values();

	const float fxs = -1.0f;
	const int maxXn = pcubespace->GetLengthX();
	const int maxYn = pcubespace->GetLengthY();
	const int maxZn = pcubespace->GetLengthZ();
	const float fxd = 2.0f/ (float)(pcubespace->GetLengthX()); 
	const float fyd = 2.0f/ (float)(pcubespace->GetLengthY()); 
	const float fzd = 2.0f/ (float)(pcubespace->GetLengthZ()); 
	float occlusionstrength = pLightModel->mSer_OcclusionStrength.Get();
	int xlength	=	(int)((occlusionstrength)/fxd);
	int ylength	=	(int)((occlusionstrength)/fyd);
	int zlength	=	(int)((occlusionstrength)/fzd);

	unsigned int checkdate_pass3 = m_checkdate_pass1+9;
	for(unsigned int ii=0 ; ii<m_NumberOfCreativeCubes ; ii++)
	{
		unsigned char xx = *(pPass4EdgeCubeIndex++);
		unsigned char yy = *(pPass4EdgeCubeIndex++);
		unsigned char zz = *(pPass4EdgeCubeIndex++);
		unsigned int cubeIndex = (zz*m_nxtnyt)+(yy*nxt)+xx;
		// find average normal for that cube:
		float nx,ny,nz;
		float vv = pmc_value_s[cubeIndex];
		nx = vv - pmc_value_s[cubeIndex+1] ;
		ny = vv - pmc_value_s[cubeIndex+m_nxt] ;
		nz = vv - pmc_value_s[cubeIndex+m_nxtnyt] ;
		vv = 1.0f * rsqrtf(nx*nx + ny*ny + nz*nz);
		nx *=vv;
		ny *=vv;
		nz *=vv;
		// scan the cubes around
		int iz,zstart,zend;
		zstart = zz - zlength;
		zend = zz+zlength;
		if( zstart<0 ) zstart =0;
		if( zend>maxZn ) zend=maxZn;
			int ystart,yend;
			ystart = yy - ylength;
			yend = yy+ylength;
			if( ystart<0 ) ystart =0;
			if( yend>maxYn ) yend=maxYn;
				int xstart,xend;
				xstart = xx - xlength;
				xend = xx+xlength;
				if( xstart<0 ) xstart =0;
				if( xend>maxXn ) xend=maxXn;
		for(iz=zstart ; iz<zend ; iz++)
		{
			int iy;
			for(iy=ystart ; iy<yend ; iy++)
			{
				unsigned int subcubeIndex = (iz*m_nxtnyt)+(iy*nxt)+xstart;
				unsigned int	*pmc_checkDate = pmc_checkDate_s + subcubeIndex ;
				int ix;
				for(ix=xstart ; ix<xend ; ix++)
				{
					unsigned int checkdate = *pmc_checkDate ;
					if( checkdate>checkdate_pass3 ) // not >= because == means check with no vertex.
					{
						checkdate-=checkdate_pass3; // gives 1->7 values for edge creation.
						// get the distance and vector from that cube
						//if(checkdate&1)
						
					}
					pmc_checkDate++;
					subcubeIndex++;
				} // end loop scancube X		
			}	// end loop scancube Y	
		} // end loop scancube Z

	} // end loop by creative cube.

}
