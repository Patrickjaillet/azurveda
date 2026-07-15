/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "Object3DGrid.h"
#include "PackFloat.h"
BASEOBJECT_DECLARE_CLASS( "grd", Object3DGrid, Object3DMeshVirtual,"3D Object Grid","Grid3D","construct a subdivided grid plane.");

Object3DGrid::Object3DGrid() : Object3DMeshVirtual() 
	,mSer_DimensionXY(PackFloat::vd_XY)
	,mSer_MappingUV1(PackFloat::vd_XY)
	,mSer_MappingUV2(PackFloat::vd_XY)
{
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_SubdivisionX,"SubdivX",1,1,256);
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_SubdivisionY,"SubdivY",1,1,256);
	REGISTER_MEMBER_PACKFLOAT_XY(mSer_DimensionXY,"Scale",1.0f,1.0f);
	REGISTER_MEMBER_PACKFLOAT_XY(mSer_MappingUV1,"MapXY0",0.0f,0.0f);
	REGISTER_MEMBER_PACKFLOAT_XY(mSer_MappingUV2,"MapXY1",1.0f,1.0f);
}

/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by CreateInternal().
*/
bool Object3DGrid::CreateInternal(void)
{
	const unsigned int subdivX = mSer_SubdivisionX.Get();
	const unsigned int subdivY = mSer_SubdivisionY.Get();

	m_pObject3DBufferTable = new Object3DBufferHandler[1];

	unsigned int meshflag = 
			VirtualMachine::bOb3D_VertexNormal|
			VirtualMachine::bOb3D_VertexUVMapping;
	if( !mSer_Flags.TestFlags(OB3DFlag_OneColor) )
		meshflag |= VirtualMachine::bOb3D_VertexRGBA ;

	if( !m_pObject3DBufferTable ) return false;
	GetMachine()->NewObject3DBuffer(  &(m_pObject3DBufferTable->m_pObject3DBuffer),
		(subdivX+1)*(subdivY+1),
		(subdivX)*(subdivY)*2,
		meshflag);
	VirtualMachine::InternalObject3DBuffer *p3DBuffer =  m_pObject3DBufferTable->m_pObject3DBuffer;
	if(!p3DBuffer) return false;	
	m_CurrentNumberOf3DBuffer = 1;
	// --- trinagles are static for this object:
	VirtualMachine::InternalTriangle *pTriangleFill = p3DBuffer->GetFirstTriangle();
	const unsigned int nbw = subdivX+1;
	unsigned int xx,yy;
	for(yy=0;yy<subdivY; yy++)
	{
		for(xx=0;xx<subdivX ; xx++)
		{
			pTriangleFill->m_p0 = yy*nbw + xx ;
			pTriangleFill->m_p1 =  yy*nbw + xx+1;
			pTriangleFill->m_p2 = (yy+1)*nbw + xx ;
			pTriangleFill++;
			pTriangleFill->m_p0 = (yy+1)*nbw + xx ;
			pTriangleFill->m_p1 = yy*nbw + xx+1;
			pTriangleFill->m_p2 = (yy+1)*nbw + xx +1;
			pTriangleFill++;
		}
	}
	p3DBuffer->SetNumberOfActiveTriangle(
		(subdivY)*(subdivX)*2
		);	
	// --- vertex are dynamic for this object:
	CreateShape(PackFloat::m_0p0);
	return true;
}
/*!
	\brief	update the shape of the object for this date, during
			the lifetime the object, when it is created.
			by defdault, does nothing, so static object can be created
			with CreateInternal() only.
	\param	_newShapeTime the new date.
	\param	_shapeIndex the index of the shape to modify.
*/
void Object3DGrid::CreateShape(float _newShapeTime, unsigned int _shapeIndex )
{
	VirtualMachine::InternalObject3DBuffer *p3DBuffer =  m_pObject3DBufferTable->m_pObject3DBuffer;
	m_pObject3DBufferTable->m_ActiveNumberOfVertex = p3DBuffer->GetMaximumNumberOfVertexes();
	// ----- 
	const unsigned int subdivX = mSer_SubdivisionX.Get();
	const unsigned int subdivY = mSer_SubdivisionY.Get();

	VirtualMachine::InternalVertex *pVertex,*pVertexFill;
	pVertex	= pVertexFill = p3DBuffer->GetFirstVertex();

	float	fx,fxd,fy,fyd,fxc,fdivx,fdivy,fu,fv;
	const float vm0p5=-0.5f;
	const float	v0p0 = PackFloat::m_0p0;
	const float v1 = PackFloat::m_1p0;
	fxd = mSer_DimensionXY.Get(0) ;
	fxc = fxd*vm0p5;
	fdivx = v1/(float)subdivX;
	fxd*=fdivx;

	fyd = mSer_DimensionXY.Get(1) ;
	fy = fyd*vm0p5;
	fdivy = v1/(float)subdivY;
	fyd*=fdivy;
	unsigned int xx,yy;
	fv = mSer_MappingUV1.Get(1);
	fdivx *=( mSer_MappingUV2.Get(0)- mSer_MappingUV1.Get(0));
	fdivy *=( mSer_MappingUV2.Get(1)- mSer_MappingUV1.Get(1));
	for(yy=0;yy<=subdivY; yy++)
	{
		fx = fxc ;
		fu = mSer_MappingUV1.Get(0);
		for(xx=0;xx<=subdivX ; xx++)
		{
			pVertexFill->m_ColorRGBA[0]=v1;
			pVertexFill->m_ColorRGBA[1]=v1;
			pVertexFill->m_ColorRGBA[2]=v1;
			pVertexFill->m_ColorRGBA[3]=v1;
			pVertexFill->m_x = fx ;
			pVertexFill->m_y = fy ;
			pVertexFill->m_z =v0p0;
			pVertexFill->m_u = fu  ;
			pVertexFill->m_v =  fv;
			//float nx,ny,nz,nn;
			pVertexFill->m_nx = v0p0 ;
			pVertexFill->m_ny = v0p0 ;
			pVertexFill->m_nz = PackFloat::m_0p5 ;
			pVertexFill++;
			fu +=fdivx;
			fx +=fxd ;
		}
		fv += fdivy ;
		fy +=fyd;
	}
}
