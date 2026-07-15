
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CVedaDemoOGLMfcGuiApp : public CWinApp
{
public:
	CVedaDemoOGLMfcGuiApp();

protected:
	CMultiDocTemplate* m_pDocTemplate;
public:

public:
	virtual BOOL InitInstance();
	inline unsigned int GetNewFilePref(){ return m_NewFilePref; };
	inline void SetNewFilePref( unsigned int _n){ m_NewFilePref=_n; };

	afx_msg void OnAppAbout();
	afx_msg void OnFileNewFrame();
	afx_msg void OnFileNew();
	afx_msg void OnEmptyContext();

	DECLARE_MESSAGE_MAP()
protected:

	unsigned int	m_NewFilePref;

};

extern CVedaDemoOGLMfcGuiApp theApp;
