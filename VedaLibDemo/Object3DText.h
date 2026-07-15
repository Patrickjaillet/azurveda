#ifndef	COM_M4NKIND_Object3DText_H
#define COM_M4NKIND_Object3DText_H

#include "Object3DVirtual.h"
#include "PackObjectReference.h"
#include "PackULong_Flags.h"
#include "PackFloatByte_Limits.h"
#include "PackList.h"
#include "PackDynamicFloat.h"

class Object3DText : public Object3DVirtual
{

public:

	Object3DText(void);

	BASEOBJECT_DEFINE_CLASS(Object3DText);
#ifdef _ENGINE_EDITABLE_

	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif

	virtual void	RenderObject( double _frameDate,
									VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex=0,
									ShapeModifierList *_pModifierList=0L );

protected:
	enum {

		eFlag_OneLine=1,

		eFlag_MoveWords=2,

		eFlag_RandomOrder=4,

	};

	PackULong_Flags		mSer_Flags;

	PackObjectReference	mSer_AlphabetRef;

	PackObjectReference	mSer_TextRef;

	PackFloatByte_Limits	mSer_InterSpace;

	PackFloatByte_Limits	mSer_SpaceWidth;

	PackFloatByte_Limits	mSer_HeightSpace;

	PackFloatByte_Limits	mSer_HeightScale;

	float m_HeightMax;

	float m_WidthMax;
	typedef struct
	{
		float	m_xx,m_yy,m_zz;
		float	m_sx,m_sy,m_sz;
		float	m_zrotate;
		float	m_rgba[4];
		float	m_TimeDelta;
	} LetterState ;

	class TextShadeComplex : public PackStruct
	{ public:

		TextShadeComplex();
		enum {
			bTSF_ShadeAlpha=1,
			bTSF_CharRelative=2,
		};

		PackULong_Flags			mSer_Flags;
		PackFloat				mSer_BeginAt;
		PackFloat				mSer_EndAt;

		PackFloatByte_Limits	mSer_OutGoingLetterRatio;
		PackDynamicFloat		mSer_EquForPosition;
		PackDynamicFloat		mSer_EquForRotation;
		PackDynamicFloat		mSer_EquForScale;
		PackDynamicFloat		mSer_EquForColor;
	};

	class PackDynamicTextShadeIn : public PackDynamicType
	{ public:

		PackDynamicTextShadeIn();

		TextShadeComplex	mSer_TextShade;
		void	ComputeShadeIn(LetterState &_lstate,double _framedate);

	protected:
		void	ComputeShade(LetterState &_lstate,double _framedate,float characterRate);
	};

	class PackDynamicTextShadeOut : public PackDynamicTextShadeIn
	{ public:

		PackDynamicTextShadeOut();

		void	ComputeShadeOut(LetterState &_lstate,double _framedate);
	};

	class PackDynamicTextShade : public PackDynamicType
	{ public:

	};

	PackDynamicTextShadeIn	mSer_ShadeIn;

	PackDynamicTextShadeOut	mSer_ShadeOut;

	PackDynamicTextShade	mSer_Shade;

	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif

	class CharacterBase {
	public:

		unsigned int m_ShapeIndex;

		float	m_xx;
		float	m_yy;
		float	m_zz;

		float	m_sx;
		float	m_sy;
		float	m_sz;

		float	m_TimeDelta;
	};

	CharacterBase	*m_pCharacterBase;

	unsigned int	m_CharacterUsed;

};

#endif
