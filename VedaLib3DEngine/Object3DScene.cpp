/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "Object3DScene.h"
#include "VirtualEquation.h"
#include "PackFloat.h"
BASEOBJECT_DECLARE_CLASS( "scn", Object3DScene, Object3DVirtual,"3D Object Scene","Scene","a scene consists of a hierarchy of 3D objects and their movements." );

Object3DScene::Object3DReference::Object3DReference() 
	: PackStruct()
	,mSer_ChannelModifierList(NewChannelModifier)
{
	REGISTER_MEMBER_REFERENCE(mSer_Object3DReference ,"Object", Object3DVirtual::m_Description );
	REGISTER_MEMBER_PACKULONG(mSer_Object3DLayerIndex ,"LayerIndex", 0);
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_BeginAtNumber ,"BeginAtNumber", 0,0,127);
	REGISTER_MEMBER_WITHLIMITS_SETMINMAX(mSer_RecursionNumber ,"RecursionNumber", 0,0,127);
	REGISTER_MEMBER(mSer_ChannelModifierList ,"ChannelModifiers" );
	REGISTER_MEMBER(mSer_ShapeModifierList ,"ShapeModifiers" );
};
//! \brief constructor . It registers the members.
Object3DScene::ChannelModifier::ChannelModifier(void) : PackStruct()
	,mSer_Value(PackFloat::vd_XYZ)
{
	REGISTER_MEMBER_ENUMS( mSer_ChannelID,"Channel",0,"Position.Rotation.Scale.Color.Transparency");
	REGISTER_MEMBER(mSer_Value ,"Value" );
}

/*!
	\brief	static method passed to the list to create new elements.
	\return a new PartToPlay object.
*/
BaseType *Object3DScene::NewObject3DReference()
{ 
	return new Object3DReference();
};

Object3DScene::Object3DScene() : Object3DVirtual()
	,mSer_ObjectTree(NewObject3DReference)
{
	// we register this one from Scene3DVirtual
	REGISTER_MEMBER( mSer_ObjectTree,"Object Tree" );
}
/*!
	\brief	static method passed to the list to create new elements.
	\return a new PartToPlay object.
*/
BaseType *Object3DScene::NewChannelModifier()
{
	return new ChannelModifier();
}
/*!
	\brief	Process a real time computing and drawing. Can be extended.
	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pViewPort	where to render.
	\param	_shapeIndex	optional index for multiple shape objects. def to zero.
	\param _pModifierList optional ShapeModifierList pointer, to modify objects.
*/
void	Object3DScene::RenderObject( double _frameDate, 
								   VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex,
									ShapeModifierList *_pModifierList )
{
	// note: using RenderObject(), the matrix has a current state in _pViewPort,
	// including camera position and anything.
	ProcessObject( &mSer_ObjectTree,_pViewPort,_rgba,_frameDate );
}

/*!
	\brief	Recursive processing of the tree.
	\param	_pCell tree cell.
	\param	_pVp viewport
	\param	_rgba shadable color state.
	\param	_frameDate a date, in second, which defines the effect cinematic.
*/
void	Object3DScene::ProcessObject( PackTreeCell* _pCell,VirtualMachine::InternalViewPort *_pVp
							,const float _rgba[4]
							,double _frameDate )
{
	float	rgba[4];
	rgba[0] = _rgba[0];
	rgba[1] = _rgba[1];
	rgba[2] = _rgba[2];
	rgba[3] = _rgba[3];
	_pVp->Matrix_Push();
	Object3DReference *pObRef = (Object3DReference *)_pCell->GetObject();
	Object3DVirtual	*pOb3D = (Object3DVirtual *) (pObRef->mSer_Object3DReference.GetObjectPointer());
	const float v_0 = PackFloat::m_0p0 ;
	const float v_1 = PackFloat::m_1p0;
	const float vm1 = -v_1;
	// note: matix is applied even if no object were link:

	unsigned int recurseIndex = 0;
	const unsigned int nbrecurse = pObRef->mSer_RecursionNumber.Get() + 1;
	while(recurseIndex<nbrecurse)
	{
		// thius way we can manage NULL object in the tree and use these nodes
		// for matrix tricks.
		// scan the list of channel modifier for the object:
		PackList::Cell *pCell = pObRef->mSer_ChannelModifierList.GetFirstCell();
		while(pCell)
		{
			// apply matrix position, rotation,scale
			float	paramEqu[4];
			float	resultEqu[4];
			// default, result is added:
			resultEqu[0] = resultEqu[1] = resultEqu[2] =  resultEqu[3] = v_0; 
			paramEqu[0] = paramEqu[3]= (float)_frameDate;
			ChannelModifier *pChanModifier = (ChannelModifier *) pCell->GetManagedObject();
			// compute the equation:
			pChanModifier->mSer_Value.Compute(resultEqu,paramEqu);
			// ugly switch:
			switch( pChanModifier->mSer_ChannelID.Get() )
			{
				case eOb3DChannel_Position:
					// translate:
					_pVp->Matrix_Translate( resultEqu[0], resultEqu[1], resultEqu[2]);
					break;
				case eOb3DChannel_Rotation:
					// rotate:
					_pVp->Matrix_Rotate( resultEqu[0],v_0,vm1,v_0);
					_pVp->Matrix_Rotate( resultEqu[1],vm1,v_0,v_0);
					_pVp->Matrix_Rotate( resultEqu[2],v_0,v_0,v_1);
					break;
				case eOb3DChannel_Scale:
					// if value is not selected, do noting:
					if( pChanModifier->mSer_Value.GetSelectedIndex() != 0 )
						_pVp->Matrix_Scale(resultEqu[0],resultEqu[1],resultEqu[2]);
					break;
				case eOb3DChannel_Color:
					rgba[0] *= resultEqu[0];
					rgba[1] *= resultEqu[1];
					rgba[2] *= resultEqu[2];
					break;
				case eOb3DChannel_Transparency:
					rgba[3] *= resultEqu[0];
					break;
				default:
				break;
			};
			pCell = pCell->GetNextBrotherCell();
		}
		
		// real time dynamic process for the object, and render.		
		// optim: no modifier, no list. 
		Object3DVirtual::ShapeModifierList *pModifier=0L;
		if(pObRef->mSer_ShapeModifierList.GetNumberOfCell()>0) 
				pModifier = &(pObRef->mSer_ShapeModifierList);

		if(pOb3D) pOb3D->RenderObject( _frameDate, _pVp, rgba,
			pObRef->mSer_Object3DLayerIndex.Get(),
			pModifier );

		//process sons
		PackTreeCell *_pSubCell = _pCell->GetFirstSubCell() ;
		while(_pSubCell)
		{
			// only apply sons that ask to be applied at this index:
			Object3DReference *pSubObRef = (Object3DReference *)_pSubCell->GetObject();
			if( recurseIndex>=pSubObRef->mSer_BeginAtNumber.Get() )
			{
				ProcessObject(_pSubCell,_pVp,rgba,_frameDate); //recursion
			}
			_pSubCell = _pSubCell->GetNextBrotherCell();
		}
		recurseIndex++;
	} // end while recurse.

	_pVp->Matrix_Pop();
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
void Object3DScene::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	float v_0 = PackFloat::m_0p0 ;
	float v_1 =PackFloat::m_1p0;

	float xx,yy,zz,rx,ry,rz;
	_pPreviewConfiguration->GetPreviewPosition(xx,yy,zz);
	yy -= 0.5f ;
	zz += 1.0f;
	_pPreviewConfiguration->GetPreviewRotation(rx,ry,rz);

	// set a camera viewport:
	_pPreviewViewPort->Matrix_LoadID();
	// first: if needed apply flip: Z axe rotation:
	_pPreviewViewPort->Matrix_Rotate( rz,v_0,v_0,-v_1);
	//old: find angles to target with the vector target-camera:
	//AtanO1O2(resultEqu,resultEqu[0]-c_x,resultEqu[1]-c_y,resultEqu[2]-c_z);
	_pPreviewViewPort->Matrix_Rotate(ry,v_1,v_0,v_0);
	_pPreviewViewPort->Matrix_Rotate( rx,v_0,v_1,v_0);
	//translate camera:
	_pPreviewViewPort->Matrix_Translate( -xx, -yy, -zz);
	_pPreviewViewPort->SetFOVLength(0.5f); // important. static angle. is multiplied by viewport scale.
	// ------- draw grid:
	ProcessPreview_DrawGrid(_pPreviewViewPort);

	// ------- then draw object, as normal scene, using preview camera:
	const float rgba[4]={1.0f,1.0f,1.0f,1.0f};
	RenderObject(_frameDate,_pPreviewViewPort,rgba);


}
#endif