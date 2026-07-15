/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "Object3DMeshVirtual.h"
#include "Texture3D.h"
#include "ModifierObject3DVirtual.h"

BASEOBJECT_DECLARE_VIRTUALCLASS( "msh", Object3DMeshVirtual, Object3DVirtual);

Object3DMeshVirtual::Object3DMeshVirtual() : Object3DVirtual() 
	,m_pObject3DBufferTable(0L)
	,m_pLastShapeModifierList(0L)
	,m_CurrentNumberOf3DBuffer(0L)
	,m_LastShapeTime(PackFloat::m_Max) // default: we create object for a given date.
{
	REGISTER_MEMBER_FLAG( mSer_Flags ,"Flags",0,"Static.LookCam.Rend32.Rend128.OneColor" );
	REGISTER_MEMBER_REFERENCE(	mSer_TextureRef,"Texture",Texture3D::m_Description );
}
//! constructor that init pointers to null:
Object3DMeshVirtual::Object3DBufferHandler::Object3DBufferHandler()
: m_pObject3DBuffer(0L)
,m_pRenderTexture(0L)
{

}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
void Object3DMeshVirtual::CloseInternal(void)
{
	//GetMachine()->DeleteObject3DBuffer(&m_pObject3DBuffer);
	Object3DBufferHandler *p3dBuff = m_pObject3DBufferTable ;
	if( !p3dBuff ) return;
	for(unsigned int ii=0 ; ii<m_CurrentNumberOf3DBuffer ; ii++)
	{
		GetMachine()->DeleteObject3DBuffer(&(p3dBuff->m_pObject3DBuffer));
		GetMachine()->DeleteTexture( &(p3dBuff->m_pRenderTexture) );
		p3dBuff++;
	}
	delete [] m_pObject3DBufferTable;
	m_pObject3DBufferTable = 0L;
	m_CurrentNumberOf3DBuffer = 0;
	// force CreateShape() after next built:
	m_LastShapeTime = PackFloat::m_Max;
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. This is done with this method. sub classes can implement it (or not) in
			any way, to explicit current shape of an object.<br>
			Extended to draw.
	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
	\param	_pPreviewConfiguration
*/
void Object3DMeshVirtual::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

	// super call to set the matrix in the viewport:
	Object3DVirtual::ProcessPreview(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);
	// then draw our objects:
	if(m_CurrentNumberOf3DBuffer==0) return;
	unsigned int shapeIndex = ((unsigned int)_frameDate) % m_CurrentNumberOf3DBuffer;
	const float white[4]={1.0f,1.0f,1.0f,1.0f};
	RenderObject(_frameDate,_pPreviewViewPort,white,shapeIndex);

}
#endif
/*!
	\brief	Process a real time computing and drawing. Can be extended.
	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pViewPort	where to render.
	\param	_shapeIndex	optional index for multiple shape objects. def to zero.
*//*old
void	Object3DMeshVirtual::RenderObject( double _frameDate, VirtualMachine::InternalViewPort *_pViewPort, unsigned int _shapeIndex )
{	
	static const float rgbadefault[4]={1.0f,1.0f,1.0f,1.0f};
	RenderObject(_frameDate,_pViewPort,_shapeIndex,rgbadefault);
}*/

/*!
	\brief	Process a real time computing and drawing. Can be extended.
	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pViewPort	where to render.
	\param	_shapeIndex	optional index for multiple shape objects. def to zero.
	\param _pModifierList optional ShapeModifierList pointer, to modify objects.
*/
void	Object3DMeshVirtual::RenderObject( double _frameDate, 
								   VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex,
									ShapeModifierList *_pModifierList )
{
	float fdate = (float)_frameDate;
	if( _shapeIndex>=m_CurrentNumberOf3DBuffer) return;
	// if needed, reshape the object:
	VirtualMachine::InternalObject3DBuffer *p3DBuf = m_pObject3DBufferTable[ _shapeIndex ].m_pObject3DBuffer;
	// if(!p3DBuf) return; should be not created in that case.

	if( (fdate != m_LastShapeTime) ||
		m_pLastShapeModifierList != _pModifierList)
	{
		// this would create/reset the whole object if it is dynamic.
		CreateShape(fdate,_shapeIndex);
		// now apply modifier list:
		if(_pModifierList)
			_pModifierList->ModifyVertexList(p3DBuf->GetFirstVertex(),
					m_pObject3DBufferTable[ _shapeIndex ].m_ActiveNumberOfVertex,
					fdate);
		// keep trace of the current date/modifier.
		m_LastShapeTime = fdate;
		m_pLastShapeModifierList = _pModifierList ;
	}
	VirtualMachine::InternalTexture *pTexture =  m_pObject3DBufferTable[ _shapeIndex ].m_pRenderTexture ;
	if(!pTexture) // if not pre rendered, use classic texture link:
	{
		Texture3D *pTextureObject =(Texture3D *) mSer_TextureRef.GetObjectPointer();
		if(pTextureObject)
		{
			pTextureObject->UpdateToFrame(fdate);
			pTexture = pTextureObject->GetVirtualMachineTexture();
		}
	}
	p3DBuf->SetColor(_rgba[0], _rgba[1],_rgba[2],_rgba[3]);

	if( mSer_Flags.TestFlags(OB3DFlag_LookCam))
	{	// 
		_pViewPort->Matrix_Push();
			_pViewPort->Matrix_LoadID( VirtualMachine::InternalViewPort::eLoadId_Rotation );
			_pViewPort->RenderMesh( p3DBuf, pTexture );
		_pViewPort->Matrix_Pop();
	} else // classic
		_pViewPort->RenderMesh( p3DBuf, pTexture ); 
}
/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by CreateInternal().
*/
bool Object3DMeshVirtual::CreateInternal_RenderObjectToTexture(void)
{
 if(!(mSer_Flags.TestFlags(OB3DFlag_PreRender32) | mSer_Flags.TestFlags(OB3DFlag_PreRender128)) ) return true;
	int texturewidth = 6;
	if( mSer_Flags.TestFlags(OB3DFlag_PreRender128) ) texturewidth=7;

	VirtualMachine::InternalTexture *pVMTexture =0L;
	Texture3D *pTextureObject =(Texture3D *) mSer_TextureRef.GetObjectPointer();	
	if(pTextureObject) pVMTexture = pTextureObject->GetVirtualMachineTexture();

	unsigned int ii;
	for(ii=0 ; ii<m_CurrentNumberOf3DBuffer ; ii++)
	{
		VirtualMachine::InternalTexture *pRenderTexture;
		VirtualMachine::InternalViewPort *pRenderViewport;
		// create a new texture to render the object on:
		GetMachine()->NewTexture( &pRenderTexture );
		m_pObject3DBufferTable[ii].m_pRenderTexture = pRenderTexture;
		if(!pRenderTexture) return false;
		//we re-use a few bits from the original texture:
		unsigned int flags=0;
			if(pVMTexture) flags |= pVMTexture->GetRenderFlags() & 
			(
			VirtualMachine::InternalTexture::itf_Filtered|
			VirtualMachine::InternalTexture::itf_Additive|
			VirtualMachine::InternalTexture::itf_substractive|
			VirtualMachine::InternalTexture::itf_doublesided
			);

		pRenderTexture->SetRenderFlags(flags|
					VirtualMachine::InternalTexture::itf_ClampTxtU|
					VirtualMachine::InternalTexture::itf_ClampTxtV
					//|VirtualMachine::InternalTexture::itf_depthwrite| itf_Additive
					//VirtualMachine::InternalTexture::itf_depthtest //itf_substractive
					);

		// create a viewport on the texture:
		GetMachine()->NewViewPortOnTexture(&pRenderViewport,pRenderTexture,
													texturewidth,
													texturewidth,
							VirtualMachine::InternalTexture::itile_TextureMapping
													);
		if(!pRenderViewport) return false;
		// get bounding box of the object:
		//float bdx1,bdx2,bdy1,bdy2,bdz1,bdz2;
		BoundingBox	sBoundingBox;
		sBoundingBox.FindBoundingBox(m_pObject3DBufferTable[ii].m_pObject3DBuffer);
		float cenx=(sBoundingBox.m_x2+sBoundingBox.m_x1)*PackFloat::m_0p5 ;
		float ceny=(sBoundingBox.m_y2+sBoundingBox.m_y1)*PackFloat::m_0p5 ;
		// make object rendering fit a square:
		pRenderViewport->Matrix_LoadID();
		pRenderViewport->Matrix_Scale(1.0f/(sBoundingBox.m_x2-sBoundingBox.m_x1),
									1.0f/(sBoundingBox.m_y2-sBoundingBox.m_y1),1.0f );
		pRenderViewport->Matrix_Translate(-cenx,-ceny,-0.5f);
		pRenderViewport->SetFOVLength(0.5f);

		// render the object:
		pRenderViewport->Clear(0.0f,0.0f,0.0f);
		pRenderViewport->RenderMesh(m_pObject3DBufferTable[ii].m_pObject3DBuffer,pVMTexture);
		pRenderViewport->SwapBuffer();
		// delete the dummy viewport. (the texture is still there.)
		GetMachine()->DeleteViewPort( &pRenderViewport );
		// delete the old object:
		GetMachine()->DeleteObject3DBuffer(  &(m_pObject3DBufferTable[ii].m_pObject3DBuffer) );

		// create the new billboard object:
		VirtualMachine::InternalObject3DBuffer	*pbillboard;
		GetMachine()->NewObject3DBuffer(&pbillboard,4,2,/*bOb3D_VertexNormal|*/VirtualMachine::bOb3D_VertexUVMapping);
		if(!pbillboard) return false;
		VirtualMachine::InternalVertex *pVert = pbillboard->GetFirstVertex();
		const float	v0 = PackFloat::m_0p0;
		const float	v1 = PackFloat::m_1p0;
		pVert->m_x = sBoundingBox.m_x1;
		pVert->m_y = sBoundingBox.m_y1;
		pVert->m_z = v0;
		pVert->m_u = v0;
		pVert->m_v = v1;
		pVert++;
		pVert->m_x = sBoundingBox.m_x2;
		pVert->m_y = sBoundingBox.m_y1;
		pVert->m_z = v0;
		pVert->m_u = v1;
		pVert->m_v = v1;
		pVert++;		
		pVert->m_x = sBoundingBox.m_x2;
		pVert->m_y = sBoundingBox.m_y2;
		pVert->m_z = v0;
		pVert->m_u = v1;
		pVert->m_v = v0;
		pVert++;
		pVert->m_x = sBoundingBox.m_x1;
		pVert->m_y = sBoundingBox.m_y2;
		pVert->m_z = v0;
		pVert->m_u = v0;
		pVert->m_v = v0;

		VirtualMachine::InternalTriangle *pTriangle = pbillboard->GetFirstTriangle();
		pTriangle->m_p0 = 1;
		pTriangle->m_p1 = 2;
		pTriangle->m_p2 = 0;
		pTriangle++;
		pTriangle->m_p0 = 0;
		pTriangle->m_p1 = 2;
		pTriangle->m_p2 = 3;
		pbillboard->SetNumberOfActiveTriangle(2);
		pbillboard->CompileAsStatic();
		m_pObject3DBufferTable[ii].m_pObject3DBuffer = pbillboard;
	}

	return true;
}
//! \brief init box.
void	Object3DMeshVirtual::BoundingBox::FindBoundingBox( VirtualMachine::InternalObject3DBuffer *_pOb3d )
{
	unsigned int ii,nbtr = _pOb3d->GetNumberOfActiveTriangle() ;
	VirtualMachine::InternalVertex *pVertv,*pVert = _pOb3d->GetFirstVertex();
	VirtualMachine::InternalTriangle *pTri = _pOb3d->GetFirstTriangle();
	m_x1=m_y1=m_z1 = PackFloat::m_Max;
	m_x2=m_y2=m_z2 = -m_x1;
	float xx,yy,zz;
	nbtr *= 3;
	unsigned int *pIndex= (unsigned int *)pTri; // wa assure ti objects are int[3] lists.
	for(ii=0 ; ii<nbtr ; ii++)
	{
		pVertv = pVert+(*pIndex++);
		xx = pVertv->m_x ;
		if(xx>m_x2) m_x2 =xx;
		if(xx<m_x1) m_x1 =xx;
		yy = pVertv->m_y ;
		if(yy>m_y2) m_y2 =yy;
		if(yy<m_y1) m_y1 =yy;
		zz = pVertv->m_z ;
		if(zz>m_z2) m_z2 =zz;
		if(zz<m_z1) m_z1 =zz;
	}
}
/*!
	\brief	tool for CreateInternal(). If mSer_Flags ask it,(OB3DFlag_Static)
			once the object shape is created (to date 0 seconds), the object shape
			is frozen and changes will be ignored.
			It allows some optilizations...
*/
void Object3DMeshVirtual::CreateInternal_FreezeStaticShape(void)
{
	unsigned int  ii;
	for(ii=0 ; ii<m_CurrentNumberOf3DBuffer ; ii++)
	{
		m_pObject3DBufferTable[ii].m_pObject3DBuffer->CompileAsStatic();
	}
}

/*!
	\brief	update the shape of the object for this date, during
			the lifetime the object, when it is created.
			by defdault, does nothing, so static object can be created
			with CreateInternal() only.
	\param	_newShapeTime the new date.
	\param	_shapeIndex the index of the shape to modify.
*/
void Object3DMeshVirtual::CreateShape(float _newShapeTime, unsigned int _shapeIndex )
{
	// do nothing, but prevents other class from adding more useless overrides. 
}
