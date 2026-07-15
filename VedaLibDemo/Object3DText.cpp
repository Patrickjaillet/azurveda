#include "Object3DText.h"
#include "Object3DFontVirtual.h"
#include "Object3DText.h"
#include "VirtualEquation.h"
#include "TextLocalized.h"
#include <math.h>
BASEOBJECT_DECLARE_CLASS( "Text", Object3DText, Object3DVirtual,"3D Object Text","Text3D"," 3D Scene where Objects are defined by a text and a 3D Object Font." );

Object3DText::Object3DText() : Object3DVirtual()
	,m_pCharacterBase(0L)

{
	REGISTER_MEMBER_FLAG(mSer_Flags,"Flags",0,"OneLine.MoveWords.RandomOrder");
	REGISTER_MEMBER_REFERENCE( mSer_TextRef,"Text",TextLocalized::m_Description );
	REGISTER_MEMBER_REFERENCE(	mSer_AlphabetRef,"3D Font",Object3DFontVirtual::m_Description );

	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(mSer_InterSpace,"InterSpace",0.0f,0.0f,2.0f);
	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(mSer_SpaceWidth,"SpaceWidth",0.203125f,0.0f,2.0f);

	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(mSer_HeightSpace,"HeightSpace",0.5f,0.0f,2.0f);
	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(mSer_HeightScale,"HeightScale",1.0f,0.0f,8.0f);

	REGISTER_MEMBER(mSer_ShadeIn,"ShadeIn");
	REGISTER_MEMBER(mSer_ShadeOut,"ShadeOut");
	REGISTER_MEMBER(mSer_Shade,"Shade");
}

Object3DText::TextShadeComplex::TextShadeComplex() : PackStruct()

	,mSer_EquForPosition(PackFloat::vd_XYZ)
	,mSer_EquForRotation(PackFloat::vd_XYZ)
	,mSer_EquForScale(PackFloat::vd_XYZ)
	,mSer_EquForColor(PackFloat::vd_XYZ)
{
	REGISTER_MEMBER_FLAG( mSer_Flags,"Flags",bTSF_ShadeAlpha,"ShadeAlpha.Relative");

	REGISTER_MEMBER_PACKFLOAT(mSer_BeginAt,"BeginAt",0.0f);
	REGISTER_MEMBER_PACKFLOAT(mSer_EndAt,"EndAt",2.0f);

	REGISTER_MEMBER_PACKFLOATBYTE_0_1(mSer_OutGoingLetterRatio,"LetterRatio",0.75f);
	REGISTER_MEMBER(mSer_EquForPosition,"Position");
	REGISTER_MEMBER(mSer_EquForRotation,"Rotation");
	REGISTER_MEMBER(mSer_EquForScale,"Scale");
	REGISTER_MEMBER(mSer_EquForColor,"Color");
}

Object3DText::PackDynamicTextShadeIn::PackDynamicTextShadeIn()
{

	REGISTER_MEMBER(mSer_TextShade,"Complex");
}

Object3DText::PackDynamicTextShadeOut::PackDynamicTextShadeOut() : PackDynamicTextShadeIn()
{
#ifdef _ENGINE_EDITABLE_

	mSer_TextShade.mSer_BeginAt.Set(6.0f);
	mSer_TextShade.mSer_EndAt.Set(8.0f);
#endif
}

bool Object3DText::CreateInternal(void)
{
	TextLocalized			*pText = (TextLocalized *) mSer_TextRef.GetObjectPointer() ;
	Object3DFontVirtual	*pFont = (Object3DFontVirtual *)mSer_AlphabetRef.GetObjectPointer();
	if(!pText || !pFont )
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR("Please link a text and a font");
		return false;
	}
	unsigned int llength = pText->GetLength();
	if(llength == 0)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR("text length is zero");
		return false;
	}
	m_pCharacterBase = new CharacterBase[llength];
	if(!m_pCharacterBase)
	{	BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
		return false;
	}

	unsigned int ii;
	unsigned int nbUsed=0;
	float	xx=0.0f;
	float	yy=0.0f;
	float	zz=0.0f;
	float	timeDelta=0.0f;
	float	maxWidth=0.0f;

	for(ii=0 ; ii<llength ; ii++)
	{
		float	scalex=1.0f;
		float	scalez=1.0f;
		int		shapeIndex;
		float	shapeWidth;
		const char *pChar = pText->GetChar(ii);
		switch(*pChar)
		{
			case '\n':
					if(mSer_Flags.TestFlags(eFlag_OneLine))break;
					if( xx>maxWidth ) maxWidth = xx;
					xx = 0.0f;
					yy += mSer_HeightSpace.Get();

					if(mSer_Flags.TestFlags(eFlag_MoveWords)) timeDelta +=1.0f;
					continue;
				break;
			case ' ':
					xx+= mSer_SpaceWidth.Get() ;
					if(mSer_Flags.TestFlags(eFlag_MoveWords)) timeDelta +=1.0f;
					continue;
				break;
			default:
				break;
		}
		bool result = pFont->GetShapeIndexForCharacter( pChar,shapeIndex,shapeWidth );
		if(result)
		{
			m_pCharacterBase[nbUsed].m_ShapeIndex = shapeIndex;
			m_pCharacterBase[nbUsed].m_xx = xx;
			m_pCharacterBase[nbUsed].m_yy = yy;
			m_pCharacterBase[nbUsed].m_zz = zz;
			m_pCharacterBase[nbUsed].m_sx = scalex;
			m_pCharacterBase[nbUsed].m_sy = mSer_HeightScale.Get();
			m_pCharacterBase[nbUsed].m_sz = scalez;
			m_pCharacterBase[nbUsed].m_TimeDelta = timeDelta;

			if(! mSer_Flags.TestFlags(eFlag_MoveWords)) timeDelta +=1.0f;
			xx += shapeWidth ;
			xx += mSer_InterSpace.Get();
			nbUsed++;
		} else xx+= mSer_SpaceWidth.Get() ;
	}
	if( xx>maxWidth ) maxWidth = xx;
	m_WidthMax = maxWidth;
	m_HeightMax = yy + mSer_HeightSpace.Get();
	m_CharacterUsed = nbUsed;

	float invTotalTime = 1.0f / m_pCharacterBase[nbUsed-1].m_TimeDelta;
	float v0p5 = PackFloat::m_0p5 ;
	for(ii=0 ; ii<nbUsed ;ii++)
	{
		 m_pCharacterBase[ii].m_xx -=(maxWidth*v0p5);
		 m_pCharacterBase[ii].m_yy -=((m_HeightMax*v0p5)- mSer_HeightSpace.Get());
		 m_pCharacterBase[ii].m_TimeDelta *= invTotalTime ;
		 if(mSer_Flags.TestFlags(eFlag_RandomOrder))
			 m_pCharacterBase[ii].m_TimeDelta = v0p5 + v0p5 *sinf(m_pCharacterBase[ii].m_TimeDelta * PackFloat::m_256p0 );

	}
	return(true);
}
#ifdef _ENGINE_EDITABLE_

void Object3DText::CloseInternal(void)
{
	if(m_pCharacterBase)
	{
		delete [] m_pCharacterBase;
		m_pCharacterBase = 0L;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

void Object3DText::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

	Object3DVirtual::ProcessPreview(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);

	const float white[4]={1.0f,1.0f,1.0f,1.0f};
	RenderObject(_frameDate,_pPreviewViewPort,white);
}
#endif

void	Object3DText::PackDynamicTextShadeIn::ComputeShadeIn(LetterState &_lstate,double _framedate)
{
	if(mSer_TypeEnum.Get()==0) return;
	if(mSer_TypeEnum.Get()==1)
	{
		float beginAt = mSer_TextShade.mSer_BeginAt.Get();
		float endAt = mSer_TextShade.mSer_EndAt.Get();
		float comerate;
		if(beginAt>endAt)
		{
			comerate = beginAt;
			beginAt = endAt;
			endAt = comerate;
		}
		if(_framedate>=endAt) return;
		if(_framedate<=beginAt)
		{
			_lstate.m_rgba[3]=PackFloat::m_0p0;
			return;
		}
		if(endAt==beginAt) return;
		float maxrate = PackFloat::m_1p0 + mSer_TextShade.mSer_OutGoingLetterRatio.Get();

		comerate=(_framedate-beginAt)*maxrate/(endAt-beginAt);
		comerate -= (  _lstate.m_TimeDelta * mSer_TextShade.mSer_OutGoingLetterRatio.Get());
		ComputeShade(_lstate,_framedate-beginAt,comerate);
		return;
	}
}
void	Object3DText::PackDynamicTextShadeOut::ComputeShadeOut(LetterState &_lstate,double _framedate)
{
	if(mSer_TypeEnum.Get()==0) return;
	float beginAt = mSer_TextShade.mSer_BeginAt.Get();
	float endAt = mSer_TextShade.mSer_EndAt.Get();
	if(mSer_TypeEnum.Get()==1)
	{
		float comerate;
		if(beginAt>endAt)
		{
			comerate = beginAt;
			beginAt = endAt;
			endAt = comerate;
		}
		if(_framedate>=endAt)
		{
			_lstate.m_rgba[3]=PackFloat::m_0p0;
			return;
		}
		if(_framedate<=beginAt) return;
		if(endAt==beginAt) return;
		float maxrate = PackFloat::m_1p0 + mSer_TextShade.mSer_OutGoingLetterRatio.Get();

		comerate=(_framedate-beginAt)/(endAt-beginAt);
		comerate=1.0f-comerate;
		comerate*=maxrate;
		comerate -= (  (1.0f-_lstate.m_TimeDelta) * mSer_TextShade.mSer_OutGoingLetterRatio.Get());
		ComputeShade(_lstate,_framedate-beginAt,comerate);
		return;
	}
	if(mSer_TypeEnum.Get()==2)
	{

		return;
	}
}
void	Object3DText::PackDynamicTextShadeIn::ComputeShade(LetterState &_lstate,double _framedate,float characterRate)
{
	if(characterRate>1.0f) characterRate=1.0f;
	else if(characterRate<0.0f) characterRate=0.0f;

	characterRate = 1.0f-characterRate;
	float normalcomerate=characterRate*characterRate;
	float ninv = 1.0f-normalcomerate;
	characterRate = characterRate*characterRate;
	characterRate = 1.0f-characterRate;

	if(mSer_TextShade.mSer_Flags.TestFlags(TextShadeComplex::bTSF_ShadeAlpha) )
	{
		_lstate.m_rgba[3] *= ninv ;
		if( _lstate.m_rgba[3]<=0.0f ) return;
	}
	const float	inv =( 1.0f-characterRate) ;
	float	fx=_lstate.m_xx,fy=_lstate.m_yy,fz=_lstate.m_zz;
	float inparam[4];
	float outparam[4];
	inparam[0]=_lstate.m_TimeDelta;
	inparam[3]=_framedate ;

	if(mSer_TextShade.mSer_Flags.TestFlags( TextShadeComplex::bTSF_CharRelative))
	{

		outparam[0] = fx;
		outparam[1] = fy;
		outparam[2] = fz;
	} else
	{
		outparam[0] = outparam[1] = outparam[2] = PackFloat::m_0p0 ;
	}
	if(mSer_TextShade.mSer_EquForPosition.GetSelectedIndex() !=0)
	{
		mSer_TextShade.mSer_EquForPosition.Compute(outparam,inparam);
	}

	_lstate.m_xx = fx*characterRate + inv*outparam[0];
	_lstate.m_yy = fy*characterRate + inv*outparam[1];
	_lstate.m_zz = fz*characterRate + inv*outparam[2];

	if(mSer_TextShade.mSer_EquForColor.GetSelectedIndex() !=0)
	{
		outparam[0] = outparam[1] = outparam[2] = PackFloat::m_0p0 ;
		mSer_TextShade.mSer_EquForColor.Compute(outparam,inparam);
		_lstate.m_rgba[0] = _lstate.m_rgba[0]*ninv+ normalcomerate*outparam[0];
		_lstate.m_rgba[1] = _lstate.m_rgba[1]*ninv+ normalcomerate*outparam[1];
		_lstate.m_rgba[2] = _lstate.m_rgba[2]*ninv+ normalcomerate*outparam[2];

	}

	if(mSer_TextShade.mSer_EquForScale.GetSelectedIndex() !=0)
	{
		outparam[0] = outparam[1] = outparam[2] = PackFloat::m_0p0 ;
		mSer_TextShade.mSer_EquForScale.Compute(outparam,inparam);
		_lstate.m_sx = _lstate.m_sx*characterRate + inv*outparam[0];
		_lstate.m_sy = _lstate.m_sy*characterRate + inv*outparam[1];
		_lstate.m_sz = _lstate.m_sz*characterRate + inv*outparam[2];
	}

	if(mSer_TextShade.mSer_EquForRotation.GetSelectedIndex() !=0)
	{
		outparam[0] =  PackFloat::m_0p0 ;
		mSer_TextShade.mSer_EquForRotation.Compute(outparam,inparam);
		_lstate.m_zrotate = _lstate.m_zrotate*characterRate + inv*outparam[0];
	}

}

void	Object3DText::RenderObject( double _frameDate,
								   VirtualMachine::InternalViewPort *_pViewPort,
									const float [4],
									unsigned int ,
									ShapeModifierList * )
{
	unsigned int ii;
	Object3DFontVirtual	*pFont = (Object3DFontVirtual *)mSer_AlphabetRef.GetObjectPointer();
	float InEqu[4];
	InEqu[3]=_frameDate;

	CharacterBase *pCharBase = m_pCharacterBase;
	for(ii=0 ; ii<m_CharacterUsed ; ii++)
	{
		LetterState	lstate;
		lstate.m_xx = pCharBase->m_xx;
		lstate.m_yy = pCharBase->m_yy;
		lstate.m_zz = pCharBase->m_zz;
		lstate.m_sx = pCharBase->m_sx;
		lstate.m_sy = pCharBase->m_sy;
		lstate.m_sz = pCharBase->m_sz;
		lstate.m_zrotate = PackFloat::m_0p0 ;
		lstate.m_rgba[0] =
		lstate.m_rgba[1] =
		lstate.m_rgba[2] =
		lstate.m_rgba[3] = PackFloat::m_1p0 ;
		lstate.m_TimeDelta = pCharBase->m_TimeDelta ;

		mSer_ShadeIn.ComputeShadeIn(lstate,_frameDate);
		mSer_ShadeOut.ComputeShadeOut(lstate,_frameDate);

		if(lstate.m_rgba[3]>0.0f)
		{
			_pViewPort->Matrix_Push();

				_pViewPort->Matrix_Translate(lstate.m_xx,lstate.m_yy,lstate.m_zz);
				_pViewPort->Matrix_Scale(lstate.m_sx,lstate.m_sy,lstate.m_sz);
				_pViewPort->Matrix_Rotate(lstate.m_zrotate,PackFloat::m_0p0,PackFloat::m_0p0,PackFloat::m_1p0);
				pFont->RenderObject( _frameDate,_pViewPort,lstate.m_rgba, pCharBase->m_ShapeIndex );
			_pViewPort->Matrix_Pop();
		}
		pCharBase++;
	}

}
