#ifndef	COM_M4NKIND_Image2DEffectFire_H
#define COM_M4NKIND_Image2DEffectFire_H

#include "VirtualImage.h"
#include "PackObjectReference.h"
#include "PackULong_Flags.h"
#include "PackLong_WithLimits.h"
#include "PackULong_Enums.h"
#include "PackList.h"
/*!
	\class	Image2DEffectFire
	\ingroup	BaseObjectInherited_VedaLibDemo
	\brief	Image that mix images of any size or color depth into a new RGBA one. 
			it uses a layer list by source image. Each layers can affect a color channel,
			it can use a HSV->RGB mixer and manages alpha channel. 
*/

class Image2DEffectFire : public VirtualImage
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	Image2DEffectFire(void);
	
	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(Image2DEffectFire);

	/*!
		\brief	Return the number of byte component for one pixel.
			This one is no more a virtual method.
	*/
	virtual unsigned int	GetByteDepth(){ return(4); };
	/*!
		\brief	Return the pixel width of the image
	*/
	virtual unsigned int	GetPixelWidth();
	/*!
		\brief	Return the pixel height of the image
	*/
	virtual unsigned int	GetPixelHeight();
	/*
		\brief	If IsTimeDynamic() is true, this image can change with time.
				You have to use UpdateToFrame()to make the image change to a given date.
				by default, UpdateToFrame() does nothing.
		\param	_framedate time in seconds.
	*/
	virtual	void	UpdateToFrame(float _framedate);

/*==================================================================
								PROTECTED
==================================================================*/
protected:
	//! the square 2n pixel size:
	PackLong_WithLimits	mSer_2nPixelWidth;
	//! the square 2n pixel size:
	PackLong_WithLimits	mSer_2nPixelHeight;
	// ! we need a second buffer for double buffer.
	float *m_pBitmap1Alloc;
	// ! the swap buffers
	float *m_pDblBuf1;
	// ! the swap buffers
	float *m_pDblBuf2;
	//! randomval
	unsigned int m_randomval;

	/*!
		\brief	Method that really build the object using the serializable parameters.
	*/
	virtual bool CreateInternal(void);
#ifdef _ENGINE_EDITABLE_
	/*!
		\brief	that closes everything. Still, the object exist and can be rebuild the same using Create()
	*/
	virtual void	CloseInternal(void);
#endif
};

#endif
