#include <cstdint>
#include "Object3DMarchCube.h"
#include "MarchinCubeSpace.h"
#include "LightModel.h"
#include "Equ_XYZD_Virtual.h"
#include "VirtualMachine.h"
#include "PackFloat.h"
#include <math.h>
#ifdef _ENGINE_EDITABLE_
	#include <time.h>
#endif

BASEOBJECT_DECLARE_CLASS( "mcb", Object3DMarchCube, Object3DVirtualDynamic,"3D Object MarchingCube","Obj3DMeta","Object defined by space(X,Y,Z) equations. Threshold is the value that stands for the surface.");

Object3DMarchCube::Object3DMarchCube() : Object3DVirtualDynamic()
	,mSer_EquationTermList(Object3DMarchCube::NewEquationTerm)
	,m_pPass4EdgeCubeIndex(0L)
	,m_pVertexLightTerm(0L)
#ifdef _ENGINE_EDITABLE_

#endif
{

#ifdef _ENGINE_EDITABLE_

	mSer_Flags.AddFlags(OB3DFlag_OneColor);

	mSer_MaximumNumberOfVertex.Set(4000);
	mSer_MaximumNumberOfTriangle.Set(8000);
#endif
	REGISTER_MEMBER_REFERENCE(	mSer_MarchinCubeSpaceRef,"Cube Space",MarchinCubeSpace::m_Description );

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

#define FP_INV(r,p)                                                          \
{                                                                            \
    int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                 \
    r = *(float *)&_i;                                                       \
    r = r * (2.0f - (p) * r);                                                \
}

Object3DMarchCube::EquationTerm::EquationTerm() : PackStruct()
	,mSer_Color(PackFloat::vd_XYZD)
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

	float vv,vv_x,nx1,ny1,nz1,nx2,ny2,nz2;
	vv		= *_pCubeValue;
	vv_x	= _pCubeValue[1];

	nx1 = vv - vv_x;
	ny1 = vv - _pCubeValue[m_nxt];
	nz1 = vv - _pCubeValue[m_nxtnyt];

	 nx2 = vv_x - _pCubeValue[2];
	ny2 = vv_x - _pCubeValue[m_nxt+1];
	nz2 = vv_x - _pCubeValue[m_nxtnyt+1];

	 nx1 += _rate*( nx2-nx1 );
	ny1 += _rate*( ny2-ny1 );
	nz1 += _rate*( nz2-nz1 );

	vv = 0.4f * rsqrtf(nx1*nx1 + ny1*ny1 + nz1*nz1);

	_pvtx->m_nx = nx1 * vv ;
	_pvtx->m_ny = ny1 * vv ;
	_pvtx->m_nz = nz1 * vv ;

}
inline void    Object3DMarchCube::CreateOneVertex_Y_Normal( const float *_pCubeValue,
							VirtualMachine::InternalVertex *_pvtx,
							float _rate )
{

	float vv,vv_y,nx1,ny1,nz1,nx2,ny2,nz2;

	vv		= *_pCubeValue;
	vv_y	= _pCubeValue[m_nxt];

	nx1 = vv - _pCubeValue[1];
	ny1 = vv - vv_y;
	nz1 = vv - _pCubeValue[m_nxtnyt];

	nx2 = vv_y - _pCubeValue[m_nxt+1];
	 ny2 = vv_y - _pCubeValue[m_nxt+m_nxt];
	nz2 = vv_y - _pCubeValue[m_nxt+m_nxtnyt];

	nx1 += _rate*( nx2-nx1 );
	 ny1 += _rate*( ny2-ny1 );
	nz1 += _rate*( nz2-nz1 );

	vv = 0.4f * rsqrtf(nx1*nx1 + ny1*ny1 + nz1*nz1);

	_pvtx->m_nx = nx1 * vv ;
	_pvtx->m_ny = ny1 * vv ;
	_pvtx->m_nz = nz1 * vv ;

}
inline void    Object3DMarchCube::CreateOneVertex_Z_Normal( const float *_pCubeValue,
							VirtualMachine::InternalVertex *_pvtx,
							float _rate )
{

	float vv,vv_z,nx1,ny1,nz1,nx2,ny2,nz2;

	vv		= *_pCubeValue;
	vv_z	= _pCubeValue[m_nxtnyt];

	nx1 = vv - _pCubeValue[1];
	ny1 = vv - _pCubeValue[m_nxt];
	nz1 = vv - vv_z ;

	nx2 = vv_z - _pCubeValue[m_nxtnyt+1];
	ny2 = vv_z - _pCubeValue[m_nxtnyt+m_nxt];
	 nz2 = vv_z - _pCubeValue[m_nxtnyt+m_nxtnyt];

	nx1 += _rate*( nx2-nx1 );
	ny1 += _rate*( ny2-ny1 );
	 nz1 += _rate*( nz2-nz1 );

	vv = 0.4f * rsqrtf(nx1*nx1 + ny1*ny1 + nz1*nz1);
	_pvtx->m_nx = nx1 * vv ;
	_pvtx->m_ny = ny1 * vv ;
	_pvtx->m_nz = nz1 * vv ;
}

void Object3DMarchCube::CreateShape(float _newShapeTime, unsigned int )
{
	MarchinCubeSpace *pcubespace = (MarchinCubeSpace *) mSer_MarchinCubeSpaceRef.GetObjectPointer() ;

	VirtualMachine::InternalObject3DBuffer *pObject3DVirtualBuffer = m_pObject3DBufferTable->m_pObject3DBuffer;
	if( pObject3DVirtualBuffer->IsCompiledAsStatic() ) return;

#ifdef _ENGINE_EDITABLE_
	clock_t		functionStartTime=clock();
#endif

	const int nx = pcubespace->GetLengthX() ;
    const int ny = pcubespace->GetLengthY() ;
    const int nz = pcubespace->GetLengthZ() ;
	const int nxt = nx+1;
	const int nyt = ny+1;

	m_nxt = nxt;
	m_nxtnyt = nxt*nyt;

	const float fxs = -1.0f;
	const float fxd = 2.0f/ (float)nx;
	const float fyd = 2.0f/ (float)ny;
	const float fzd = 2.0f/ (float)nz;

	const float	edgerate = mSer_Thresold.Get();

	float	boundCube[6];
	PackList::Cell *pCell = mSer_EquationTermList.GetFirstCell();

	const unsigned int checkdate_pass1	= pcubespace->GetNextCheckDate();
	m_checkdate_pass1 = checkdate_pass1;
	while( pCell)
	{
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
				elementColor[3] =255;
		}
		pCell = pCell->GetNextBrotherCell() ;
		if( !pEquationObject ) continue;

		int	TermIndex=0;

		while( pEquationObject->SetFrameDate((float)_newShapeTime,TermIndex,boundCube) )
		{
			float	fy,fz;

			signed char	bisx,binx;
			signed char		bisy,biny;
			signed char		bisz,binz;
			bisx	=	(signed char)((boundCube[0]-fxs)/fxd);
				if( bisx<0 ) bisx =0;
				else if( bisx>nx ) bisx=(signed char)nx;
			binx	=	(signed char)((boundCube[1]-fxs)/fxd)+1;
				if( binx<0 ) binx =0;
				else if( binx>nx ) binx=(signed char)nx;
			bisy	=	(signed char)((boundCube[2]-fxs)/fyd);
				if( bisy<0 ) bisy =0;
				else if( bisy>ny ) bisy=(signed char)ny;
			biny	=	(signed char)((boundCube[3]-fxs)/fyd)+1;
				if( biny<0 ) biny =0;
				else if( biny>ny ) biny=(signed char)ny;
			bisz	=	(signed char)((boundCube[4]-fxs)/fzd);
				if( bisz<0 ) bisz =0;
				else if( bisz>nz ) bisz=(signed char)nz;
			binz	=	(signed char)((boundCube[5]-fxs)/fzd)+1;
				if( binz<0 ) binz =0;
				else if( binz>nz ) binz=(signed char)nz;

			fz = fxs + (float)bisz * fzd;

			float			*pmc_value_s = pcubespace->GetCubeTable_Values();
			unsigned char	*pmc_flags_s = pcubespace->GetCubeTable_Flags();
			unsigned int	*pmc_checkDate_s = pcubespace->GetCubeTable_CheckDate();
			const float startfy = fxs + (float)bisy * fyd;
			const float startfx = fxs  +(float)bisx * fxd;

			if( !mSer_Flags.TestFlags(OB3DFlag_OneColor) )
			{
				MarchinCubeSpace::MarchCubeExtraValue *pmc_ColorValue_s =
					pcubespace->GetCubeTable_ExtraValues();

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
						{
							for( signed char xx=bisx ; xx<=binx ; xx++ )
							{
								if( *(pmc_checkDate) != checkdate_pass1 )
								{
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
								float fv = pEquationObject->ComputeByX(fx) ;
								if( *(pmc_checkDate) != checkdate_pass1 )
								{
									*(pmc_value)	 = fv ;
									unsigned short val = (unsigned short)(fv*v255);
									unsigned short inv = 255-val;
									val *=255;
									pmc_ColorValue->u_RGBA[0] =
						(val + ((unsigned short)elementColor[0])*inv)>>8;
									pmc_ColorValue->u_RGBA[1] =
						(val + ((unsigned short)elementColor[1])*inv)>>8;
									pmc_ColorValue->u_RGBA[2] =
						(val + ((unsigned short)elementColor[2])*inv)>>8;

									*(pmc_checkDate) = checkdate_pass1 ;

								} else
								{

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
								*(pmc_flags++) = (unsigned char) ( fv < edgerate );
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
				}
			} else
			{
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
						{
							for(signed char xx=bisx ; xx<=binx ; xx++ )
							{
								if( *(pmc_checkDate) != checkdate_pass1 )
								{
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
								float fv = pEquationObject->ComputeByX(fx) ;
								if( *(pmc_checkDate) != checkdate_pass1 )
								{
									*(pmc_value)	 = fv ;
									*(pmc_checkDate) = checkdate_pass1 ;
								} else
								{

									fv *= *(pmc_value) ;
									*(pmc_value) = fv;
								}
								*(pmc_flags++) = (unsigned char) ( fv < edgerate );
								pmc_value++;
								pmc_checkDate++;
								fx+=fxd;
							}
						}
						index_yz+= nxt;
						fy+=fyd;
					}
					fz+=fzd;
				}
			}
			TermIndex++;
		}

	}

	float			*pmc_value_s = pcubespace->GetCubeTable_Values();
	const unsigned char	*pmc_flags_s = pcubespace->GetCubeTable_Flags();
	MarchinCubeSpace::MarchCubeIndex	*pmc_Index_s = pcubespace->GetCubeTable_Index();
	unsigned int	*pmc_checkDate_s = pcubespace->GetCubeTable_CheckDate();

	int	*pnref = (int *) (pObject3DVirtualBuffer->GetFirstTriangle());

	const int	maxref = mSer_MaximumNumberOfTriangle.Get()-5 ;
	const unsigned int checkdate_pass2 = checkdate_pass1+1;
	const unsigned int checkdate_pass3 = checkdate_pass1+1+8;
	const unsigned int *polyTable = pcubespace->GetPolyIndexTable() ;
	VirtualMachine::InternalVertex *pvtx =  pObject3DVirtualBuffer->GetFirstVertex();
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
	if( !pEquationObject ) continue;
	int	TermIndex=0;

	while( pEquationObject->SetFrameDate((float)_newShapeTime,TermIndex,boundCube) )
	{
		float	fx,fy,fz;

		signed char		bisx,binx;
		signed char		bisy,biny;
		signed char		bisz,binz;
			bisx	=	(signed char)((boundCube[0]-fxs)/fxd);
				if( bisx<0 ) bisx =0;
				else if( bisx>nx ) bisx=(signed char)nx;
			binx	=	(signed char)((boundCube[1]-fxs)/fxd);
				if( binx<0 ) binx =0;
				else if( binx>nx ) binx=(signed char)nx;
			bisy	=	(signed char)((boundCube[2]-fxs)/fyd);
				if( bisy<0 ) bisy =0;
				else if( bisy>ny ) bisy=(signed char)ny;
			biny	=	(signed char)((boundCube[3]-fxs)/fyd);
				if( biny<0 ) biny =0;
				else if( biny>ny ) biny=(signed char)ny;
			bisz	=	(signed char)((boundCube[4]-fxs)/fzd);
				if( bisz<0 ) bisz =0;
				else if( bisz>nz ) bisz=(signed char)nz;
			binz	=	(signed char)((boundCube[5]-fxs)/fzd);
				if( binz<0 ) binz =0;
				else if( binz>nz ) binz=(signed char)nz;

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

				const bool checkPolygon = ( (zz>bisz) && (yy>bisy) && (zz != nz) && (yy != ny) );
				if( checkPolygon )
				{
					pmc_fl2 = pmc_flags-nxt-m_nxtnyt ;
					kk = pmc_flags[0]<<3;
					kk |= pmc_fl2[ nxt ]<<2;
					kk |= pmc_fl2[ m_nxtnyt ]<<1;
					kk |= *pmc_fl2;
					pmc_fl2++;
				}
				fx = startfx;
				const unsigned char edgecheckYZ =
							(((unsigned char)( yy != ny ))<<1)
							|(((unsigned char)( zz != nz))<<2);
				MarchinCubeSpace::MarchCubeExtraValue *pmc_ExtraValue;
				pmc_ExtraValue = 0L;
				if(colormix)
					pmc_ExtraValue= &((pcubespace->GetCubeTable_ExtraValues())[index_yz]);
				for(signed char xx=bisx ; xx<=binx ; xx++ )
				{

					if( *(pmc_checkDate) < checkdate_pass2 )
					{
						unsigned char ii = *pmc_flags;

					unsigned char edgecheck = (
							 ((ii^pmc_flags[nxt])<<1)
							|((ii^pmc_flags[m_nxtnyt])<<2)
							) & edgecheckYZ ;
					edgecheck|=((unsigned char)( xx != nx)) & (ii^pmc_flags[1]);

					if(edgecheck!=0)
					{
						if( nbVertexCreated >= nbvmax  )
						{

							nbPolyCreated = 0;
							goto endcase;
						}

						if(preparePass4CreatorsIndex)
						{
							*(pPass4EdgeCubeIndex++) = (unsigned char)xx;
							*(pPass4EdgeCubeIndex++) = (unsigned char)yy;
							*(pPass4EdgeCubeIndex++) = (unsigned char)zz;
						}

					if( edgecheck & 1)
					{

						pmc_Index->m_vx = (unsigned short)nbVertexCreated ;
						float divv,ddivp = (pmc_value[1] -pmc_value[0]);
						FP_INV(divv,ddivp);
						float rate=  (edgerate-pmc_value[0] ) *divv ;

						pvtx->m_x = pvtx->m_u = fx+ fxd*rate ;
						pvtx->m_y = pvtx->m_v = fy;
						pvtx->m_z = fz ;
						CreateOneVertex_X_Normal(pmc_value, pvtx, rate );
						if(colormix) CreateOneVertex_X_RGBA(pmc_ExtraValue, pvtx, rate);
						pvtx++;
						nbVertexCreated++;
					}

					if(edgecheck & 2 )
					{

						pmc_Index->m_vy = (unsigned short)nbVertexCreated ;
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

					if( edgecheck & 4 )
					{

						pmc_Index->m_vz = (unsigned short)nbVertexCreated ;
						pvtx->m_x = pvtx->m_u = fx;
						pvtx->m_y = pvtx->m_v = fy;
						float divv,ddivp = ( pmc_value[m_nxtnyt]-pmc_value[0]);
						FP_INV(divv,ddivp);
						float rate=  (edgerate-pmc_value[0] ) *divv ;

						pvtx->m_z = fz +  fzd*rate;
						CreateOneVertex_Z_Normal(pmc_value, pvtx, rate );
						if(colormix) CreateOneVertex_Z_RGBA(pmc_ExtraValue, pvtx, rate);
						pvtx++;
						nbVertexCreated++;
					}
					}
					 *(pmc_checkDate) = checkdate_pass2+edgecheck;
					}
					pmc_flags++;

					if( checkPolygon && xx != binx )
					{

						kk<<=4;
						kk |= (*pmc_flags) <<3;
						kk |= pmc_fl2[ nxt ]<<2;
						kk |= pmc_fl2[ m_nxtnyt ]<<1;
						kk |= *pmc_fl2;
						pmc_fl2++;

						if(kk !=0 && kk !=255 && *(pmc_checkDate) < checkdate_pass3 )
						{

							const unsigned int    *poctcase;

							intptr_t	memoryindex =(intptr_t)( ((pmc_Index-nxt)-m_nxtnyt) );
							poctcase = &polyTable[kk<<4];
							int ll = *(poctcase++);
							if( nbPolyCreated >= maxref )
							{

								nbPolyCreated = 0;
								goto endcase;
							}
							nbPolyCreated+=ll;
							int ii;
							for(ii=0 ; ii<ll ; ii++)
							{

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

								*(pnref++) =
#ifdef _ENGINE_EDITABLE_
									tt2 =
#endif
									(int)
									*((unsigned short *)( memoryindex+
												*(poctcase++))) ;

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

									nbPolyCreated = 0;
									goto endcase;
								}
#endif
							}

							*(pmc_checkDate) += 8;
						}

					}

					pmc_Index++;
					pmc_value++;
					pmc_ExtraValue++;
					pmc_checkDate++;
					fx+=fxd;
				}
				index_yz+= m_nxt;
				fy+=fyd;
			}
			fz+=fzd;
		}

		TermIndex++;
	}

}

endcase:
	m_NbVertexCreated = nbVertexCreated;
	pObject3DVirtualBuffer->SetNumberOfActiveTriangle(nbPolyCreated);

	Pass4_Diffuse(_newShapeTime);

	if(preparePass4CreatorsIndex)
	{
		m_NumberOfCreativeCubes = static_cast<unsigned int>((pPass4EdgeCubeIndex-m_pPass4EdgeCubeIndex)/3) ;
		Pass4_AmbientOcclusion();
	}

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

bool Object3DMarchCube::CreateInternal(void)
{
	unsigned int objectFlags =
				VirtualMachine::bOb3D_VertexNormal|
				VirtualMachine::bOb3D_VertexUVMapping;

	if( mSer_LightModel.GetObjectPointer()!=0L ||
		!(mSer_Flags.TestFlags( OB3DFlag_OneColor ))
		) objectFlags |= VirtualMachine::bOb3D_VertexRGBA ;

	bool result = Object3DVirtualDynamic::CreateInternal_Alloc1Internal3DBuffer(objectFlags);
	if(!result) return result;
	MarchinCubeSpace *pcubespace = (MarchinCubeSpace *) mSer_MarchinCubeSpaceRef.GetObjectPointer() ;
	if(!pcubespace)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR("a Cube Space must be linked.");
		return false;
	}
	PackList::Cell *pCell = mSer_EquationTermList.GetFirstCell();
	if( !pCell)
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

	CreateShape(PackFloat::m_0p0);
	return true;
}
#ifdef _ENGINE_EDITABLE_

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
		unsigned int ii;
		LightModel::LinearLight *pLinearLight =  (LightModel::LinearLight *)pLinearCell->GetManagedObject();

		float	lightcolor[4];
		float	lightVector[4];

		lightcolor[0]=lightcolor[1]=lightcolor[2]=
		lightVector[1] =
		lightVector[0]=lightVector[2]=0.0f;
		pLinearLight->mSer_Color.Compute(lightcolor,inparam);
		pLinearLight->mSer_Vector.Compute(lightVector,inparam);

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
			if(vectordot<0.0f)vectordot=0.0f;
			*pvlight++ = lightcolor[0]*vectordot;
			*pvlight++ = lightcolor[1]*vectordot;
			*pvlight++ = lightcolor[2]*vectordot;

			pVertex++;
		}else
		for(ii=0 ;ii<m_NbVertexCreated ; ii++)
		{
			float	vectordot=
				pVertex->m_nx *lightVector[0] +
				pVertex->m_ny *lightVector[1] +
				pVertex->m_nz *lightVector[2] ;
			if(vectordot<0.0f)vectordot=0.0f;
			*pvlight++ += lightcolor[0]*vectordot;
			*pvlight++ += lightcolor[1]*vectordot;
			*pvlight++ += lightcolor[2]*vectordot;

			pVertex++;
		}
		pLinearCell = pLinearCell->GetNextBrotherCell();
	}

	float	AmbientColor[4];
	AmbientColor[0] = AmbientColor[1] = AmbientColor[2]=0.0f;
	pLightModel->mSer_AmbientColor.Compute(AmbientColor,inparam);
	VirtualMachine::InternalVertex *pVertex = pObject3DVirtualBuffer->GetFirstVertex();
	pvlight = m_pVertexLightTerm;
	unsigned int ii;
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
	for(ii=0 ;ii<m_NbVertexCreated ; ii++)
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

void	Object3DMarchCube::Pass4_AmbientOcclusion()
{
	LightModel *pLightModel = (LightModel *) mSer_LightModel.GetObjectPointer();
	if(!pLightModel) return;

	MarchinCubeSpace *pcubespace = (MarchinCubeSpace *) mSer_MarchinCubeSpaceRef.GetObjectPointer() ;
	const int nxt = pcubespace->GetLengthX()+1 ;

	const unsigned char *pPass4EdgeCubeIndex = m_pPass4EdgeCubeIndex;
	unsigned int	*pmc_checkDate_s = pcubespace->GetCubeTable_CheckDate();
	const float	*pmc_value_s = pcubespace->GetCubeTable_Values();

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

		float nx,ny,nz;
		float vv = pmc_value_s[cubeIndex];
		nx = vv - pmc_value_s[cubeIndex+1] ;
		ny = vv - pmc_value_s[cubeIndex+m_nxt] ;
		nz = vv - pmc_value_s[cubeIndex+m_nxtnyt] ;
		vv = 1.0f * rsqrtf(nx*nx + ny*ny + nz*nz);
		nx *=vv;
		ny *=vv;
		nz *=vv;

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
					if( checkdate>checkdate_pass3 )
					{
						checkdate-=checkdate_pass3;

					}
					pmc_checkDate++;
					subcubeIndex++;
				}
			}
		}

	}

}
