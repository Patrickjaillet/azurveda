// SPDX-License-Identifier: LGPL-2.1-only

#ifndef	COM_M4NKIND_PerlinNoiseImage_H
#define COM_M4NKIND_PerlinNoiseImage_H

#include "ProceduralImage.h"

class PerlinNoiseImage : public ProceduralImage
{

public:

	PerlinNoiseImage(void);

	BASEOBJECT_DEFINE_CLASS(PerlinNoiseImage);

protected:

	virtual bool CreateInternal(void);
private:
	unsigned char GetPerlinMergeRandom( int vv, unsigned char uforce  );
};

#endif
