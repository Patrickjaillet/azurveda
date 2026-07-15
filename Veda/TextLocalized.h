#ifndef	COM_M4NKIND_TextLocalized_H
#define COM_M4NKIND_TextLocalized_H
#include "BaseObject.h"
#include "PackString_MultipleLines.h"
#include "PackULong_Enums.h"
#include "PackList.h"
/*!
	\class	TextLocalized
	\ingroup BaseObjectInheritedDocGroup BaseObjectInherited_Veda
	\brief	Define a text and possibly other localized versions.
*/

class TextLocalized : public BaseObject
{
/*==================================================================
								PUBLIC
==================================================================*/
public:
	/*!
		\brief	Constructor. There should only be members initialisation there.
	*/
	TextLocalized(void);

	/*!
		\brief	Class Inheritance Descriptors. Also forces a destructor. See BASEOBJECT_DECLARE_... in .cpp
	*/
	BASEOBJECT_DEFINE_CLASS(TextLocalized);
	/*!
		\brief	return number of characters, including space, tabs and returns.
	*/
	unsigned int	GetLength();

	/*!
		\brief	return a caracter of the text, as a string.
	*/
	const char	*GetChar(unsigned int _index);


/*==================================================================
								PROTECTED
==================================================================*/
protected:
	/*!
		\brief	Method that really build the object using the serializable parameters.
				Close() should close everything opened by CreateInternal().
	*/
	virtual bool CreateInternal(void);
	/*
		\class	LocalText
		\brief nested class.
	*/
	class LocalText : public PackStruct
	{ public:
		//! constructor
		LocalText();
		//! target language. Use VirtualMachine::eMachineLocalization
		PackULong_Enums	mSer_LocalizationEnum;
		//! Base default text.
		PackString_MultipleLines		mSer_BaseText;

	};
	/*!
		\brief	static method passed to the list to create new elements.
		\return a new  object.
	*/
	static BaseType *NewLocalText();

	//! list of all localized version.
	PackList	mSer_TextList;
	//! buffer...
	char	m_charbuffer[8];

	/*!
		\brief	Synchronize Text To Machine Language
	*/
	void	SynchronizeTextToMachineLanguage();
	//! the text corresponding to the last langage checked:
	PackString_MultipleLines	*m_pCurrentText;
	//! ...for this langage enum:
	int		m_TextLanguageEnum;

};

#endif
