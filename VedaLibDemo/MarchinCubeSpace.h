#ifndef	COM_M4NKIND_MarchinCubeSpace_H
#define COM_M4NKIND_MarchinCubeSpace_H

#include "BaseObject.h"
#include "PackLong_WithLimits.h"

class MarchinCubeSpace : public BaseObject
{

public:

	MarchinCubeSpace(void);

	BASEOBJECT_DEFINE_CLASS(MarchinCubeSpace);

	typedef struct {
		unsigned short  m_vx;
		unsigned short  m_vy;
		unsigned short  m_vz;
	} MarchCubeIndex;

	static const unsigned int ofs_x=0;
	static const unsigned int ofs_y=sizeof(unsigned short);
	static const unsigned int ofs_z=sizeof(unsigned short)*2;

	typedef unsigned char  tchar4[4];

	typedef union {
		unsigned int	u_WholeBitField;
		tchar4			u_RGBA;
	} MarchCubeExtraValue;

	inline MarchCubeIndex	*GetCubeTable_Index(){ return m_pCubeTable_Index ; };

	inline unsigned char	*GetCubeTable_Flags(){ return m_pCubeTable_Flags ; };

	inline float		*GetCubeTable_Values(){ return m_pCubeTable_Values ; };

	inline MarchCubeExtraValue *GetCubeTable_ExtraValues(){ return m_pCubeTable_ExtraValue ; };

	inline unsigned int		*GetCubeTable_CheckDate(){ return m_pCubeTable_CheckDate ; };

	inline const unsigned int	*GetPolyIndexTable(){ return m_polyIndexTable ; };

	inline unsigned int	GetLengthX(){ return mSer_LengthX.Get() ; };

	inline unsigned int	GetLengthY(){ return mSer_LengthY.Get() ; };

	inline unsigned int	GetLengthZ(){ return mSer_LengthZ.Get() ; };

	inline unsigned int	GetNextCheckDate(){ m_CheckDate+=17; return m_CheckDate;  };

protected:

	PackLong_WithLimits		mSer_LengthX;

	PackLong_WithLimits		mSer_LengthY;

	PackLong_WithLimits		mSer_LengthZ;

    unsigned char			*m_pMainTableAlloc;

	unsigned int			m_CheckDate;

    MarchCubeIndex			*m_pCubeTable_Index;

	unsigned char			*m_pCubeTable_Flags;

	float					*m_pCubeTable_Values;

	unsigned int			*m_pCubeTable_CheckDate;

	MarchCubeExtraValue		*m_pCubeTable_ExtraValue;

	unsigned int			*m_polyIndexTable;

	virtual bool CreateInternal(void);

#ifdef _ENGINE_EDITABLE_

	virtual void	CloseInternal(void);
#endif

	bool    PreGenerateTriangleTables( void );

};

#endif
