#ifndef	COM_M4NKIND_Object3DMarchCube_H
#define COM_M4NKIND_Object3DMarchCube_H

#include "Object3DVirtualDynamic.h"

#include "PackULong_Flags.h"
#include "PackObjectReference.h"
#include "PackFloatByte_Limits.h"
#include "PackFloatByte_Color.h"
#include "PackList.h"
#include "MarchinCubeSpace.h"
class Equ_XYZD_Virtual;

class Object3DMarchCube : public Object3DVirtualDynamic
{

public:

	Object3DMarchCube(void);

	BASEOBJECT_DEFINE_CLASS(Object3DMarchCube);

	class EquationTerm : public PackStruct
	{
	public:

		EquationTerm(void);

		PackObjectReference	mSer_Equation;

		PackFloatByte_Color	mSer_Color;
	};

	static BaseType *NewEquationTerm(){ return new EquationTerm(); };

protected:

	PackObjectReference	mSer_MarchinCubeSpaceRef;

	PackObjectReference mSer_LightModel;

	PackList			mSer_EquationTermList;

	PackFloatByte_Limits	mSer_Thresold;

	float		m_ColorConversion[256];

	unsigned int	m_nxt;

	unsigned int	m_nxtnyt;

	unsigned char	*m_pPass4EdgeCubeIndex;

	float			*m_pVertexLightTerm;

	unsigned int	m_NumberOfCreativeCubes;

	unsigned int	m_NbVertexCreated;

	unsigned int	m_checkdate_pass1;

	void			Pass4_Diffuse(float _framedate);

	void			Pass4_AmbientOcclusion();

#ifdef _ENGINE_EDITABLE_

#endif

	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif

	virtual void CreateShape(float _newShapeTime, unsigned int _shapeIndex=0);

	inline void    CreateOneVertex_X_RGBA( const MarchinCubeSpace::MarchCubeExtraValue *_pCubeExtraValue,
							VirtualMachine::InternalVertex *_pvtx,
							float _rate );
	inline void    CreateOneVertex_Y_RGBA( const MarchinCubeSpace::MarchCubeExtraValue *_pCubeExtraValue,
							VirtualMachine::InternalVertex *_pvtx,
							float _rate );
	inline void    CreateOneVertex_Z_RGBA( const MarchinCubeSpace::MarchCubeExtraValue *_pCubeExtraValue,
							VirtualMachine::InternalVertex *_pvtx,
							float _rate );

	inline void    CreateOneVertex_X_Normal( const float *_pCubeValue,
								VirtualMachine::InternalVertex *_pvtx,
								float _rate );
	inline void    CreateOneVertex_Y_Normal( const float *_pCubeValue,
								VirtualMachine::InternalVertex *_pvtx,
								float _rate );
	inline void    CreateOneVertex_Z_Normal( const float *_pCubeValue,
								VirtualMachine::InternalVertex *_pvtx,
								float _rate );

};

#endif
