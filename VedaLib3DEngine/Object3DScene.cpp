// SPDX-License-Identifier: LGPL-2.1-only

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

Object3DScene::ChannelModifier::ChannelModifier(void) : PackStruct()
	,mSer_Value(PackFloat::vd_XYZ)
{
	REGISTER_MEMBER_ENUMS( mSer_ChannelID,"Channel",0,"Position.Rotation.Scale.Color.Transparency");
	REGISTER_MEMBER(mSer_Value ,"Value" );
}

BaseType *Object3DScene::NewObject3DReference()
{
	return new Object3DReference();
};

Object3DScene::Object3DScene() : Object3DVirtual()
	,mSer_ObjectTree(NewObject3DReference)
{

	REGISTER_MEMBER( mSer_ObjectTree,"Object Tree" );
}

BaseType *Object3DScene::NewChannelModifier()
{
	return new ChannelModifier();
}

void	Object3DScene::RenderObject( double _frameDate,
								   VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int ,
									ShapeModifierList * )
{

	ProcessObject( &mSer_ObjectTree,_pViewPort,_rgba,_frameDate );
}

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

	unsigned int recurseIndex = 0;
	const unsigned int nbrecurse = pObRef->mSer_RecursionNumber.Get() + 1;
	while(recurseIndex<nbrecurse)
	{

		PackList::Cell *pCell = pObRef->mSer_ChannelModifierList.GetFirstCell();
		while(pCell)
		{

			float	paramEqu[4];
			float	resultEqu[4];

			resultEqu[0] = resultEqu[1] = resultEqu[2] =  resultEqu[3] = v_0;
			paramEqu[0] = paramEqu[3]= (float)_frameDate;
			ChannelModifier *pChanModifier = (ChannelModifier *) pCell->GetManagedObject();

			pChanModifier->mSer_Value.Compute(resultEqu,paramEqu);

			switch( pChanModifier->mSer_ChannelID.Get() )
			{
				case eOb3DChannel_Position:

					_pVp->Matrix_Translate( resultEqu[0], resultEqu[1], resultEqu[2]);
					break;
				case eOb3DChannel_Rotation:

					_pVp->Matrix_Rotate( resultEqu[0],v_0,vm1,v_0);
					_pVp->Matrix_Rotate( resultEqu[1],vm1,v_0,v_0);
					_pVp->Matrix_Rotate( resultEqu[2],v_0,v_0,v_1);
					break;
				case eOb3DChannel_Scale:

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

		Object3DVirtual::ShapeModifierList *pModifier=0L;
		if(pObRef->mSer_ShapeModifierList.GetNumberOfCell()>0)
				pModifier = &(pObRef->mSer_ShapeModifierList);

		if(pOb3D) pOb3D->RenderObject( _frameDate, _pVp, rgba,
			pObRef->mSer_Object3DLayerIndex.Get(),
			pModifier );

		PackTreeCell *_pSubCell = _pCell->GetFirstSubCell() ;
		while(_pSubCell)
		{

			Object3DReference *pSubObRef = (Object3DReference *)_pSubCell->GetObject();
			if( recurseIndex>=(unsigned int)pSubObRef->mSer_BeginAtNumber.Get() )
			{
				ProcessObject(_pSubCell,_pVp,rgba,_frameDate);
			}
			_pSubCell = _pSubCell->GetNextBrotherCell();
		}
		recurseIndex++;
	}

	_pVp->Matrix_Pop();
}
#ifdef _ENGINE_EDITABLE_

void Object3DScene::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	float v_0 = PackFloat::m_0p0 ;
	float v_1 =PackFloat::m_1p0;

	float xx,yy,zz,rx,ry,rz;
	_pPreviewConfiguration->GetPreviewPosition(xx,yy,zz);
	yy -= 0.5f ;
	zz += 1.0f;
	_pPreviewConfiguration->GetPreviewRotation(rx,ry,rz);

	_pPreviewViewPort->Matrix_LoadID();

	_pPreviewViewPort->Matrix_Rotate( rz,v_0,v_0,-v_1);

	_pPreviewViewPort->Matrix_Rotate(ry,v_1,v_0,v_0);
	_pPreviewViewPort->Matrix_Rotate( rx,v_0,v_1,v_0);

	_pPreviewViewPort->Matrix_Translate( -xx, -yy, -zz);
	_pPreviewViewPort->SetFOVLength(0.5f);

	ProcessPreview_DrawGrid(_pPreviewViewPort);

	const float rgba[4]={1.0f,1.0f,1.0f,1.0f};
	RenderObject(_frameDate,_pPreviewViewPort,rgba);

}
#endif
