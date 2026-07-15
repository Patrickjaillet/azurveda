// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_LightModel_H
#define COM_M4NKIND_LightModel_H
#include "BaseObject.h"
#include "PackULong_Flags.h"
#include "PackDynamicFloat.h"
#include "PackList.h"
#include "PackObjectReference.h"
#include "PackFloatByte_Limits.h"

class LightModel : public BaseObject
{

public:

	LightModel( );

	BASEOBJECT_DEFINE_CLASS(LightModel);

	inline bool IsAmbientOcclusionActive() { return mSer_Flags.TestFlags(lmf_AmbientOcclusion);  };

	inline PackList::Cell *GetFirstLinearLight() { return mSer_LinearLightList.GetFirstCell(); };

	inline PackList::Cell *GetFirstDifuseLight() { return mSer_DifuseLightList.GetFirstCell(); };

	class LinearLight : public PackStruct
	{
	public:

		LinearLight(void);

		PackDynamicFloat	mSer_Color;

		PackDynamicFloat	mSer_Vector;
	};

	static BaseType *NewLinearLight();

	class DifuseLight : public PackStruct
	{
	public:

		DifuseLight(void);

		PackDynamicFloat	mSer_Color;

		PackDynamicFloat	mSer_Position;
	};

	static BaseType *NewDifuseLight();
		typedef enum
	{
		lmf_AmbientOcclusion=1
	} lightmodelflag ;

	PackULong_Flags		mSer_Flags;

	PackFloatByte_Limits	mSer_OcclusionStrength;

	PackDynamicFloat	mSer_AmbientColor;

	PackList			mSer_LinearLightList;

	PackList			mSer_DifuseLightList;

protected:

};
#endif
