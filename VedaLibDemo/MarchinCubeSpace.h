#ifndef	COM_M4NKIND_MarchinCubeSpace_H
#define COM_M4NKIND_MarchinCubeSpace_H

#include "BaseObject.h"
#include "PackLong_WithLimits.h"
/*!
	\class	MarchinCubeSpace
	\ingroup	BaseObjectInherited_VedaLibDemo
	\brief	Define a Space Matrix Grid used by Object3DMarchCube to generate objects
			using a marching cube algorythm.

*/

class MarchinCubeSpace : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	MarchinCubeSpace(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(MarchinCubeSpace);

	// note for PPC implementation floats must be 4-aligned.
	//! \struct MarchCubeIndex
	//! \brief hold the creation of vertex on edges in pass2, and is read in pass3 at polygon creation.
	typedef struct { // DONT MODIFY MEMBER ORDER ! DONT ADD VIRTUAL METHODS !!
		unsigned short  m_vx;    // vertex index of the one created on branch going X,
		unsigned short  m_vy;    // vertex index of the one created on branch Y
		unsigned short  m_vz;    // vertex index of the one created on branch Z.
	} MarchCubeIndex;
	// this must correspond to marchcube struct members offsets:
	static const unsigned int ofs_x=0;
	static const unsigned int ofs_y=sizeof(unsigned short);
	static const unsigned int ofs_z=sizeof(unsigned short)*2;

	// this is an extra value for space vertex set in pass1. can be used a way or another.
	// by using a 4 byte union, one alloc can be used a way or another.
	typedef unsigned char  tchar4[4];

		//
	typedef union {
		unsigned int	u_WholeBitField;	// can be used to be set to 0 or 0xffffffff.
		tchar4			u_RGBA;		// Color
	} MarchCubeExtraValue;


	/*!
		\brief	Access to the cube table:
		\return the cube table
	*/
	inline MarchCubeIndex	*GetCubeTable_Index(){ return m_pCubeTable_Index ; };
	/*!
		\brief	Access to the cube table: inside/outside flag bits.
		\return the cube table
	*/
	inline unsigned char	*GetCubeTable_Flags(){ return m_pCubeTable_Flags ; };
	/*!
		\brief	Access to the cube table: scalar values at corners.
		\return the cube table
	*/
	inline float		*GetCubeTable_Values(){ return m_pCubeTable_Values ; };
	/*!
		\brief	Access to the cube table: Extra values at corners. cf MarchCubeExtraValue.
		\return the cube table
	*/
	inline MarchCubeExtraValue *GetCubeTable_ExtraValues(){ return m_pCubeTable_ExtraValue ; };
	/*!
		\brief	Access to the cube table:  date when the cube was last processed.
				It prevent a "clean pass" on the whole cubespace at the beginning.
				note: each date is in special "pass" values. one pass date in unique to 
				a whole space calculation AND pass.
		\return the cube table
	*/
	inline unsigned int		*GetCubeTable_CheckDate(){ return m_pCubeTable_CheckDate ; };

	/*!
		\brief	Access to the poly Index Table for this cube dimensions:
		\return the cube poly Index Table.
	*/
	inline const unsigned int	*GetPolyIndexTable(){ return m_polyIndexTable ; };
	/*!
		\brief	Access to the number of SUBDIVISIONS in the X axis. (number of cell is +1)
		\return  the total number of SUBDIVISIONS in the X axis
	*/
	inline unsigned int	GetLengthX(){ return mSer_LengthX.Get() ; };
	/*!
		\brief	Access to the number of SUBDIVISIONS in the X axis. (number of cell is +1)
		\return  the total number of SUBDIVISIONS in the X axis
	*/
	inline unsigned int	GetLengthY(){ return mSer_LengthY.Get() ; };
	/*!
		\brief	Access to the number of SUBDIVISIONS in the X axis. (number of cell is +1)
		\return  the total number of SUBDIVISIONS in the X axis
	*/
	inline unsigned int	GetLengthZ(){ return mSer_LengthZ.Get() ; };
	/*!
		\brief	Get 3 new checkdate number for the 3 passes. Statement: no checkdate on the cube got
				one of these 3 new values.
			0->pass1
			1->8 pass2, with edgecheck on low bytes.
			9->16 pass3,...
		\return a new checkdate.
	*/
	inline unsigned int	GetNextCheckDate(){ m_CheckDate+=17; return m_CheckDate;  };
	
/*==================================================================
								PROTECTED
==================================================================*/
protected:

	//! number of cube in the X axis
	PackLong_WithLimits		mSer_LengthX;

	//! number of cube in the Y axis
	PackLong_WithLimits		mSer_LengthY;

	//! number of cube in the Z axis
	PackLong_WithLimits		mSer_LengthZ;

	//!we do one alloc and we align some of the table inside:
    unsigned char			*m_pMainTableAlloc;
	//! the current checkdate. 0 when object is new.
	unsigned int			m_CheckDate;

	//! the vertex created index of the 3 edges:
    MarchCubeIndex			*m_pCubeTable_Index;
	//! the byte per cube flags:  0 means out, 1 means in.
	unsigned char			*m_pCubeTable_Flags;
	//! the space scalar values:
	float					*m_pCubeTable_Values;
	//! the date of the last frame this cube was processed, for windowing.
	unsigned int			*m_pCubeTable_CheckDate;
	//! an extra value by space vertex. see union MarchCubeExtraValue.
	MarchCubeExtraValue		*m_pCubeTable_ExtraValue;
	//! the polygon table for each 256 cube configurations, with good edges offset for this CubeSpace.
	unsigned int			*m_polyIndexTable;

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

	bool    PreGenerateTriangleTables( void );

};

#endif
