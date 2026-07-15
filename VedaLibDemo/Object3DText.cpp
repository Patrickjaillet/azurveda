#include "Object3DText.h"
#include "Object3DFontVirtual.h"
#include "Object3DText.h"
#include "VirtualEquation.h"
#include "TextLocalized.h"
#include <math.h>
#ifndef sqrtf
#define sqrtf sqrt
#endif
#ifndef sinf
#define sinf sin
#endif
BASEOBJECT_DECLARE_CLASS( "Text", Object3DText, Object3DVirtual,"3D Object Text","Text3D"," 3D Scene where Objects are defined by a text and a 3D Object Font." );

Object3DText::Object3DText() : Object3DVirtual()
	,m_pCharacterBase(0L)
//	,mSer_LetterShadeList(NewTextShade)
{
	REGISTER_MEMBER_FLAG(mSer_Flags,"Flags",0,"OneLine.MoveWords.RandomOrder");
	REGISTER_MEMBER_REFERENCE( mSer_TextRef,"Text",TextLocalized::m_Description );	
	REGISTER_MEMBER_REFERENCE(	mSer_AlphabetRef,"3D Font",Object3DFontVirtual::m_Description );

	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(mSer_InterSpace,"InterSpace",0.0f,0.0f,2.0f);
	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(mSer_SpaceWidth,"SpaceWidth",0.203125f,0.0f,2.0f);

	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(mSer_HeightSpace,"HeightSpace",0.5f,0.0f,2.0f);
	REGISTER_MEMBER_PACKFLOATBYTE_LIMITS(mSer_HeightScale,"HeightScale",1.0f,0.0f,8.0f);

	//REGISTER_MEMBER(mSer_LetterShadeList,"Letter Shades");
	REGISTER_MEMBER(mSer_ShadeIn,"ShadeIn");
	REGISTER_MEMBER(mSer_ShadeOut,"ShadeOut");
	REGISTER_MEMBER(mSer_Shade,"Shade");
}
// constructor:
Object3DText::TextShadeComplex::TextShadeComplex() : PackStruct()
//	,mSer_TimeToApplyEquation(PackFloat::vd_X)
	,mSer_EquForPosition(PackFloat::vd_XYZ)
	,mSer_EquForRotation(PackFloat::vd_XYZ)
	,mSer_EquForScale(PackFloat::vd_XYZ)
	,mSer_EquForColor(PackFloat::vd_XYZ)
{
	REGISTER_MEMBER_FLAG( mSer_Flags,"Flags",bTSF_ShadeAlpha,"ShadeAlpha.Relative");

	REGISTER_MEMBER_PACKFLOAT(mSer_BeginAt,"BeginAt",0.0f);
	REGISTER_MEMBER_PACKFLOAT(mSer_EndAt,"EndAt",2.0f);

	//REGISTER_MEMBER(mSer_TimeToApplyEquation,"TimeToApply");
	REGISTER_MEMBER_PACKFLOATBYTE_0_1(mSer_OutGoingLetterRatio,"LetterRatio",0.75f);
	REGISTER_MEMBER(mSer_EquForPosition,"Position");
	REGISTER_MEMBER(mSer_EquForRotation,"Rotation");
	REGISTER_MEMBER(mSer_EquForScale,"Scale");
	REGISTER_MEMBER(mSer_EquForColor,"Color");
}
//! constructor:
Object3DText::PackDynamicTextShadeIn::PackDynamicTextShadeIn()
{
	// possible shapes:
	// 1:
	REGISTER_MEMBER(mSer_TextShade,"Complex");
}
//! constructor:
Object3DText::PackDynamicTextShadeOut::PackDynamicTextShadeOut() : PackDynamicTextShadeIn()
{
#ifdef _ENGINE_EDITABLE_
	// set other default values for end:
	mSer_TextShade.mSer_BeginAt.Set(6.0f);
	mSer_TextShade.mSer_EndAt.Set(8.0f);
#endif
}
/*!
	\brief	static method passed to the list to create new elements.
	\return a new  object.
*/
//BaseType *Object3DText::NewTextShade(){ return new TextShade; }
/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by CreateInternal().
*/
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
		return false; // possible nonsense.
	}
	m_pCharacterBase = new CharacterBase[llength];
	if(!m_pCharacterBase)
	{	BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_MEMORY();
		return false;
	}
	// precalculate final position of all characters:
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
		float	scaley=1.0f;
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
					yy += mSer_HeightSpace.Get(); //0.5f;
					// time delta the same for the whole word:
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
			m_pCharacterBase[nbUsed].m_TimeDelta = timeDelta; // it will be rescaled !
			// time delta different for each letter:
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
	// rescale timedelta to 0.0->1.0f
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
/*!
	\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
*/
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
/*!
	\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
			created object. This is done with this method. sub classes can implement it (or not) in
			any way, to explicit current shape of an object.<br>

	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pPreviewViewPort the viewport to render. Can't be 0L.
	\param	_pPreviewConfiguration
*/
void Object3DText::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{
	// set matrix with supercall:
	Object3DVirtual::ProcessPreview(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);
	// draw whole text:
	const float white[4]={1.0f,1.0f,1.0f,1.0f};
	RenderObject(_frameDate,_pPreviewViewPort,white);
}
#endif
//!
void	Object3DText::PackDynamicTextShadeIn::ComputeShadeIn(LetterState &_lstate,double _framedate)
{
	if(mSer_TypeEnum.Get()==0) return;
	if(mSer_TypeEnum.Get()==1) // complex shade
	{
		float beginAt = mSer_TextShade.mSer_BeginAt.Get();
		float endAt = mSer_TextShade.mSer_EndAt.Get();
		float comerate;
		if(beginAt>endAt)
		{	// inversion:
			comerate = beginAt;
			beginAt = endAt;
			endAt = comerate;
		}
		if(_framedate>=endAt) return; // no shade.
		if(_framedate<=beginAt)
		{	
			_lstate.m_rgba[3]=PackFloat::m_0p0; // mean exit letter.
			return; // letter transparent.
		}
		if(endAt==beginAt) return;
		float maxrate = PackFloat::m_1p0 + mSer_TextShade.mSer_OutGoingLetterRatio.Get();
		//if(_framedate<=beginAt) comerate=0.0f; // exit done.
		//else if(_framedate>=endAt)  comerate=maxrate;// exit done.
		comerate=(_framedate-beginAt)*maxrate/(endAt-beginAt); //[0,1]*maxrate
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
	if(mSer_TypeEnum.Get()==1) // complex shade
	{
		float comerate;
		if(beginAt>endAt)
		{	// inversion:
			comerate = beginAt;
			beginAt = endAt;
			endAt = comerate;
		}
		if(_framedate>=endAt)
		{	
			_lstate.m_rgba[3]=PackFloat::m_0p0; // mean exit letter.
			return; // letter transparent.
		}
		if(_framedate<=beginAt) return; // no shade.
		if(endAt==beginAt) return;
		float maxrate = PackFloat::m_1p0 + mSer_TextShade.mSer_OutGoingLetterRatio.Get();
		//if(_framedate<=beginAt) comerate=0.0f; // exit done.
		//else if(_framedate>=endAt)  comerate=maxrate;// exit done.
		comerate=(_framedate-beginAt)/(endAt-beginAt); //[0,1]*maxrate
		comerate=1.0f-comerate;
		comerate*=maxrate;
		comerate -= (  (1.0f-_lstate.m_TimeDelta) * mSer_TextShade.mSer_OutGoingLetterRatio.Get());
		ComputeShade(_lstate,_framedate-beginAt,comerate);
		return;
	}
	if(mSer_TypeEnum.Get()==2) //reverse In at...
	{


		return;
	}
}
void	Object3DText::PackDynamicTextShadeIn::ComputeShade(LetterState &_lstate,double _framedate,float characterRate)
{
	if(characterRate>1.0f) characterRate=1.0f;
	else if(characterRate<0.0f) characterRate=0.0f;
	// inverse and square for nice curve
	characterRate = 1.0f-characterRate;
	float normalcomerate=characterRate*characterRate;
	float ninv = 1.0f-normalcomerate;
	characterRate = characterRate*characterRate;
	characterRate = 1.0f-characterRate;
	// end.
	// [0,2] -= [0,1]
	if(mSer_TextShade.mSer_Flags.TestFlags(TextShadeComplex::bTSF_ShadeAlpha) )
	{	
		_lstate.m_rgba[3] *= ninv ;
		if( _lstate.m_rgba[3]<=0.0f ) return; // the letter completely disappeared.
	}
	const float	inv =( 1.0f-characterRate) ;
	float	fx=_lstate.m_xx,fy=_lstate.m_yy,fz=_lstate.m_zz;
	float inparam[4];
	float outparam[4];
	inparam[0]=_lstate.m_TimeDelta; // parameter for all
	inparam[3]=_framedate ;
	// position stuff:
	if(mSer_TextShade.mSer_Flags.TestFlags( TextShadeComplex::bTSF_CharRelative))
	{
		// if  relative, origin is default position:
		outparam[0] = fx;
		outparam[1] = fy;
		outparam[2] = fz;
	} else
	{	// else orgin is the text center:
		outparam[0] = outparam[1] = outparam[2] = PackFloat::m_0p0 ;
	}
	if(mSer_TextShade.mSer_EquForPosition.GetSelectedIndex() !=0)
	{
		mSer_TextShade.mSer_EquForPosition.Compute(outparam,inparam);
	}
	// we shade the equation result to the final position:
	_lstate.m_xx = fx*characterRate + inv*outparam[0]; 
	_lstate.m_yy = fy*characterRate + inv*outparam[1]; 
	_lstate.m_zz = fz*characterRate + inv*outparam[2]; 
	// color stuff:
	if(mSer_TextShade.mSer_EquForColor.GetSelectedIndex() !=0)
	{
		outparam[0] = outparam[1] = outparam[2] =/* outparam[3]=*/ PackFloat::m_0p0 ;
		mSer_TextShade.mSer_EquForColor.Compute(outparam,inparam);
		_lstate.m_rgba[0] = _lstate.m_rgba[0]*ninv+ normalcomerate*outparam[0];
		_lstate.m_rgba[1] = _lstate.m_rgba[1]*ninv+ normalcomerate*outparam[1];
		_lstate.m_rgba[2] = _lstate.m_rgba[2]*ninv+ normalcomerate*outparam[2];
		//_lstate.m_rgba[3] = _lstate.m_rgba[3]*characterRate/*+ inv*outparam[3]*/;
	}
	// zoom stuff:
	if(mSer_TextShade.mSer_EquForScale.GetSelectedIndex() !=0)
	{
		outparam[0] = outparam[1] = outparam[2] = PackFloat::m_0p0 ;
		mSer_TextShade.mSer_EquForScale.Compute(outparam,inparam);
		_lstate.m_sx = _lstate.m_sx*characterRate + inv*outparam[0]; 
		_lstate.m_sy = _lstate.m_sy*characterRate + inv*outparam[1]; 
		_lstate.m_sz = _lstate.m_sz*characterRate + inv*outparam[2]; 
	}
	// rotate stuff:
	if(mSer_TextShade.mSer_EquForRotation.GetSelectedIndex() !=0)
	{
		outparam[0] = /*outparam[1] = outparam[2] =*/ PackFloat::m_0p0 ;
		mSer_TextShade.mSer_EquForRotation.Compute(outparam,inparam);
		_lstate.m_zrotate = _lstate.m_zrotate*characterRate + inv*outparam[0]; 
	}
	//_lstate.m_zrotate -= inv ;
}
/*!
	\brief	Process a real time computing and drawing. Can be extended.
	\param	_frameDate a date, in second, which defines the effect cinematic.
	\param	_pViewPort	where to render.
	\param	_shapeIndex	optional index for multiple shape objects. def to zero.
	\param _pModifierList optional ShapeModifierList pointer, to modify objects.
*/
void	Object3DText::RenderObject( double _frameDate, 
								   VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex,
									ShapeModifierList *_pModifierList )
{
	unsigned int ii;
	Object3DFontVirtual	*pFont = (Object3DFontVirtual *)mSer_AlphabetRef.GetObjectPointer();
	float outEqu[4],InEqu[4];
	InEqu[3]=_frameDate;	// the incoming parameters only use time.

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

		if(lstate.m_rgba[3]>0.0f) // alpha to zero or less means no character drawn.
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
