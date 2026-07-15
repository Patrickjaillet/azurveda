/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#ifndef	COM_M4NKIND_VirtualImage_H
#define COM_M4NKIND_VirtualImage_H

#include "BaseObject.h"
/*!
	\class	VirtualImage
	\ingroup BaseObjectInherited_VedaLibImage BaseObjectInheritedDocGroup
	\brief	virtual Class defining a generic Image usable by effects.
			To this level, VirtualImage manages a generic memory allocation for the bitmap.

*/
class VirtualImage : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	VirtualImage(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_VIRTUALCLASS(VirtualImage);

	/*!
		\brief	Return the pixel width of the image
	*/
	virtual unsigned int	GetPixelWidth()=0;
	/*!
		\brief	Return the pixel height of the image
	*/
	virtual unsigned int	GetPixelHeight()=0;
	/*!
		\brief	Return the number of byte component for one pixel.	
	*/
	virtual unsigned int	GetByteDepth()=0;
	/*!
		\brief	Return the bitmap buffer. never stock it elsewhere, redo a Get() and a GetBuffer().
				The format of the bitmap vary according to the class.
	*/
	inline unsigned char	*GetBuffer(){ return(m_pBuffer); };

	/*!
		\brief	If this image can change with time, it will return true,
				and UpdateToFrame() can have an effect. false by default.
		\return true if image can change with time.
	*/
	inline	bool	IsTimeDynamic() const { return m_IsTimeDynamic; };

	/*!
		\brief	If IsTimeDynamic() is true, this image can change with time.
				You have to use UpdateToFrame()to make the image change to a given date.
				by default, UpdateToFrame() does nothing.
		\param	_framedate time in seconds.
	*/
	virtual	void	UpdateToFrame(float _framedate);

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
/*==================================================================
								PROTECTED
==================================================================*/
protected:

	//! the memory chunk that is the image:
	unsigned char	*m_pBuffer;
	//! true is image can change with UpdateToFrame() , false by default.
	bool		m_IsTimeDynamic;
	//! if TimeDynamic, m_LastFrameDate should note the current date shape.
	float	m_CurrentFrameDate;
#ifdef _ENGINE_EDITABLE_
	//! for preview, we also alloc a RGBA buffer to watch the result:
	unsigned char	*m_pPeviewRGBABuffer;
#endif
	/*!
		\brief	Method that really build the object using the serializable parameters.
				For the virtual VirtualImage class, it allocs the bitmap memory.
				Close() should close everything opened by CreateInternal().
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
