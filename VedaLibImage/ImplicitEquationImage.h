// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_ImplicitEquationImage_H
#define COM_M4NKIND_ImplicitEquationImage_H

#include "VirtualImage.h"
#include "PackLong_WithLimits.h"
#include "PackObjectReference.h"
#include "PackULong_Flags.h"
#include "PackULong_Enums.h"

class ImplicitEquationImage : public VirtualImage
{

public:

	ImplicitEquationImage(void);

	BASEOBJECT_DEFINE_CLASS(ImplicitEquationImage);

	virtual unsigned int	GetPixelWidth();

	virtual unsigned int	GetPixelHeight();

	virtual unsigned int	GetByteDepth();

	virtual	void	UpdateToFrame(float _framedate);

protected:
	enum {
		eflag_TimeDynamic=1
	};

	PackULong_Flags			mSer_EquaFlags;
	enum {
		etype_XToGrey=0,
		etype_XYZToRGBA,
	};

	PackULong_Enums			mSer_ResultTypeEnum;

	PackLong_WithLimits		mSer_PixelWidth;

	PackLong_WithLimits		mSer_PixelHeight;

	PackObjectReference		mSerRef_Equation;

	virtual bool CreateInternal(void);

	void	ProtectedDraw(float _framedate);

};

#endif
