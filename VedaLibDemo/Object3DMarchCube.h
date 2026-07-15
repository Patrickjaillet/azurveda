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

/*!
	\class	Object3DMarchCube
	\ingroup	BaseObjectInherited_VedaLibDemo
	\brief	virtual Class defining a generic Object3D build with the
			technic of the <b>marching cubes</b>.
*/
class Object3DMarchCube : public Object3DVirtualDynamic
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Object3DMarchCube(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Object3DMarchCube);
	/*!
		\brief	 Nested class that describes an element of the list of terms
				used to define implicitly the object, in mSer_EquationTermList
	*/
	class EquationTerm : public PackStruct
	{
	public:
		//! \brief element constructor 
		EquationTerm(void);
		//! the reference to the equation term, the 4D equations that define the object:
		PackObjectReference	mSer_Equation;
		//! the color that identify this term:
		PackFloatByte_Color	mSer_Color;
	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new EquationTerm object.
	*/
	static BaseType *NewEquationTerm(){ return new EquationTerm(); };

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! a cube space is needed to build an object:
	PackObjectReference	mSer_MarchinCubeSpaceRef;
	//! a special lightning model can be used or not:
	PackObjectReference mSer_LightModel;
	//! list of terms to build object.
	PackList			mSer_EquationTermList;
	//! edgerate
	PackFloatByte_Limits	mSer_Thresold;
	//!	table used to convert RGBA char to float.
	float		m_ColorConversion[256];
	//! realtime tool var for marchube: number of cube in a X line.
	unsigned int	m_nxt;
	//! realtime tool var for marchube: number of cube in a X line mult NbY.
	unsigned int	m_nxtnyt;
	//! on pass2 (vertex creation), cubes that create vertexes are kept in a table if pass4 is needed.
	//! this table keeps 4 chars(X,Y,Z,edgeCreators) once for each. Maxium is like the maximum of vertex.
	unsigned char	*m_pPass4EdgeCubeIndex;
	//! RGB by vertex created
	float			*m_pVertexLightTerm; 
	//!
	unsigned int	m_NumberOfCreativeCubes;
	//!
	unsigned int	m_NbVertexCreated;
	//!
	unsigned int	m_checkdate_pass1;
	/*
			\brief	do pass 4.
			\param	_framedate date in seconds for dynamic lights.
			pass1: scalar values computation
			pass2: vertex creation
			pass3: polygon creation (done with pass2)
			pass4: light model affinage, if needed.
	*/
	void			Pass4_Diffuse(float _framedate);
	/*
			\brief	do pass 4.
			pass1: scalar values computation
			pass2: vertex creation
			pass3: polygon creation (done with pass2)
			pass4: light model affinage, if needed.
	*/
	void			Pass4_AmbientOcclusion();

#ifdef _ENGINE_EDITABLE_
	//! number of vertex created; this info is not in the mesh object m_pObject3DVirtualBuffer,and is only used in editable mode for stats.
	//unsigned int	m_CurrentNumberOfVertex;
#endif
	/*!
		\brief	just use CreateInternal() to return an error in some case...
		no use in other cases...
	*/
	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	virtual void	CloseInternal(void);
#endif
	/*!
		\brief	update the shape of the object for this date, during
				the lifetime the object, when it is created.
				by defdault, does nothing, so static object can be created
				with CreateInternal() only.
		\param	_newShapeTime the new date.
		\param	_shapeIndex the index of the shape to modify.
	*/
	virtual void CreateShape(float _newShapeTime, unsigned int _shapeIndex=0);

	/*!
		\brief	Method that creates a vertex, from an equation, by finding its normals and stuffs...
		\param	vertex object to init
		\param	_vector the x,y,z coordinate.
	*/
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
