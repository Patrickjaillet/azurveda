#include "TextLocalized.h"
#include "PackFloat.h"
BASEOBJECT_DECLARE_CLASS( "text", TextLocalized, BaseObject,"Text","text","A Localizable Text");

TextLocalized::TextLocalized() : BaseObject()
	,mSer_TextList( NewLocalText )
{
	REGISTER_MEMBER( mSer_TextList,"TranslationList" );

}

TextLocalized::LocalText::LocalText() : PackStruct()
{
	REGISTER_MEMBER_ENUMS(mSer_LocalizationEnum,"LocalVersion",0,"Default.English.French.Spanish.German.Italian.Polish.Portuguese");
	REGISTER_MEMBER(mSer_BaseText,"Text");
}

BaseType *TextLocalized::NewLocalText(){ return new LocalText(); };

bool TextLocalized::CreateInternal(void)
{
	m_TextLanguageEnum = -1 ;
	m_pCurrentText = 0L;
	return true;
}

unsigned int TextLocalized::GetLength()
{
	SynchronizeTextToMachineLanguage();
	if(!m_pCurrentText) return 0;
	return m_pCurrentText->length();
}

const char	*TextLocalized::GetChar(unsigned int _index)
{
	SynchronizeTextToMachineLanguage();
	if(!m_pCurrentText) return 0;
	m_charbuffer[0]=m_pCurrentText->Get()[_index] ;
	m_charbuffer[1]=0;
	return &(m_charbuffer[0]);
}

void	TextLocalized::SynchronizeTextToMachineLanguage()
{
	int currentLanguage = (int) GetMachine()->GetCurrentWorldLocalizationEnum();
	if(m_TextLanguageEnum == currentLanguage) return;
	m_TextLanguageEnum = currentLanguage;
	PackList::Cell *pCell = mSer_TextList.GetFirstCell();
	LocalText *pDefault=0L;
	while(pCell)
	{
		LocalText *pLocalText = (LocalText *) pCell->GetManagedObject();
		if( (int)pLocalText->mSer_LocalizationEnum.Get() == currentLanguage )
		{
			m_pCurrentText = &(pLocalText->mSer_BaseText);
			return;
		} else if(pLocalText->mSer_LocalizationEnum.Get() ==VirtualMachine::eMachineLocalization_LocalisationNotImplemented)
		{
			pDefault = pLocalText;
		}
		pCell = pCell->GetNextBrotherCell();
	}

	if(pDefault)
	{
		m_pCurrentText = &(pDefault->mSer_BaseText);
		return;
	}
	m_pCurrentText = 0L;

}
