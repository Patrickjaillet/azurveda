#ifndef	COM_M4NKIND_Object3DText_H
#define COM_M4NKIND_Object3DText_H

#include "Object3DVirtual.h"
#include "PackObjectReference.h"
#include "PackULong_Flags.h"
#include "PackFloatByte_Limits.h"
#include "PackList.h"
#include "PackDynamicFloat.h"
/*!
	\class	Scene3D
	\ingroup	BaseObjectInherited_VedaLibDemo
	\brief Class that stands for a playable 3D Scene defined by a list of objects and their cinematics.
	
*/

class Object3DText : public Object3DVirtual
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Object3DText(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Object3DText);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	a GUI could need to play, draw, print, or output from any way, a preview of a 
				created object. This is done with this method. sub classes can implement it (or not) in
				any way, to explicit current shape of an object.<br>

		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pPreviewViewPort the viewport to render. Can't be 0L.
		\param	_pPreviewConfiguration
	*/
	virtual	void ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration);
#endif
	/*!
		\brief	Process a real time computing and drawing. Can be extended.
		\param	_frameDate a date, in second, which defines the effect cinematic.
		\param	_pViewPort description of the viewport where to render.
		\param	_rgba red green blue alpha component.
		\param	_shapeIndex	optional index for multiple shape objects. def to zero.
		\param _pModifierList optional ShapeModifierList pointer, to modify objects.
	*/
	virtual void	RenderObject( double _frameDate, 
									VirtualMachine::InternalViewPort *_pViewPort,
									const float _rgba[4],
									unsigned int _shapeIndex=0,
									ShapeModifierList *_pModifierList=0L );
/*==================================================================
								PROTECTED
==================================================================*/
protected:
	enum {
		//! all lines return are ignored if true:
		eFlag_OneLine=1,
		//! all words have characters with same time delta.
		eFlag_MoveWords=2,
		//! letters get a random order
		eFlag_RandomOrder=4,
		//! all lines are shifted to the center:
		//eFlag_Center=8,
		//! all lines are shifted to the right:
		//eFlag_Right=16,
	};
	//! draw flags 
	PackULong_Flags		mSer_Flags;
	//! the alphabet used to draw the text
	PackObjectReference	mSer_AlphabetRef;
	//! the text that define the scene.
	PackObjectReference	mSer_TextRef;
	//! how sould the space width scale.
	PackFloatByte_Limits	mSer_InterSpace;
	//! how sould the space width scale.
	PackFloatByte_Limits	mSer_SpaceWidth;
	//! space between lines
	PackFloatByte_Limits	mSer_HeightSpace;
	//! how sould the height scale.
	PackFloatByte_Limits	mSer_HeightScale;

	//! maximum width in which the text is took.
	float m_HeightMax;
	//! maximum height in which the text is took.
	float m_WidthMax;
	typedef struct
	{
		float	m_xx,m_yy,m_zz;
		float	m_sx,m_sy,m_sz;
		float	m_zrotate;
		float	m_rgba[4];
		float	m_TimeDelta;
	} LetterState ;
	/*
		\class TextShadeComplex
		\brief	nested class. define a text shade.
	*/
	class TextShadeComplex : public PackStruct
	{ public:
		// constructor:
		TextShadeComplex();
		enum {
			bTSF_ShadeAlpha=1,
			bTSF_CharRelative=2,
		};
		//! Flags
		PackULong_Flags			mSer_Flags;
		PackFloat				mSer_BeginAt;
		PackFloat				mSer_EndAt;
		//! comin ratio.0= letter by letter, 1= all text go in the same time.
		PackFloatByte_Limits	mSer_OutGoingLetterRatio;
		PackDynamicFloat		mSer_EquForPosition;
		PackDynamicFloat		mSer_EquForRotation;
		PackDynamicFloat		mSer_EquForScale;
		PackDynamicFloat		mSer_EquForColor;
	};
	/*
		\class PackDynamicTextShadeIn
		\brief	the multiple shape a shade can take.
	*/
	class PackDynamicTextShadeIn : public PackDynamicType
	{ public:
		//! constructor
		PackDynamicTextShadeIn();
		//! first shape
		TextShadeComplex	mSer_TextShade;
		void	ComputeShadeIn(LetterState &_lstate,double _framedate);
		//!
	protected:
		void	ComputeShade(LetterState &_lstate,double _framedate,float characterRate);
	};
	/*
		\class PackDynamicTextShadeOut
		\brief	the multiple shape a shade can take.
	*/
	class PackDynamicTextShadeOut : public PackDynamicTextShadeIn
	{ public:
		//! constructor
		PackDynamicTextShadeOut();
		//!
		void	ComputeShadeOut(LetterState &_lstate,double _framedate);
	};
	/*
		\class PackDynamicTextShadeOut
		\brief	the multiple shape a shade can take.
	*/
	class PackDynamicTextShade : public PackDynamicType
	{ public:

	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new  object.
	*/
	//static BaseType *NewTextShade();

	//! shade to apply to make letter come in:
	PackDynamicTextShadeIn	mSer_ShadeIn;
	//! shade to apply to make letter go out:
	PackDynamicTextShadeOut	mSer_ShadeOut;
	//!  shade to apply all the time:
	PackDynamicTextShade	mSer_Shade;

	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	virtual void	CloseInternal(void);
#endif
	/*!
		\brief	private nested class: precalculation.
	*/
	class CharacterBase {
	public:
		//! shape index in the font.
		unsigned int m_ShapeIndex; 
		//! final position in text's space:
		float	m_xx;
		float	m_yy;
		float	m_zz;
		//! final scale in text's space:
		float	m_sx;
		float	m_sy;
		float	m_sz;
		//! time delta ratio in the text:
		float	m_TimeDelta;
	};
	//! precalculation of final position for all characters:
	CharacterBase	*m_pCharacterBase;
	//!
	unsigned int	m_CharacterUsed;

};

#endif
