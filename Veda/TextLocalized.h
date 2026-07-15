#ifndef	COM_M4NKIND_TextLocalized_H
#define COM_M4NKIND_TextLocalized_H
#include "BaseObject.h"
#include "PackString_MultipleLines.h"
#include "PackULong_Enums.h"
#include "PackList.h"

class TextLocalized : public BaseObject
{

public:

	TextLocalized(void);

	BASEOBJECT_DEFINE_CLASS(TextLocalized);

	unsigned int	GetLength();

	const char	*GetChar(unsigned int _index);

protected:

	virtual bool CreateInternal(void);

	class LocalText : public PackStruct
	{ public:

		LocalText();

		PackULong_Enums	mSer_LocalizationEnum;

		PackString_MultipleLines		mSer_BaseText;

	};

	static BaseType *NewLocalText();

	PackList	mSer_TextList;

	char	m_charbuffer[8];

	void	SynchronizeTextToMachineLanguage();

	PackString_MultipleLines	*m_pCurrentText;

	int		m_TextLanguageEnum;

};

#endif
