// VedaDemoOGLMfcGui.h : fichier d'en-tête principal pour l'application VedaDemoOGLMfcGui
//
#pragma once

#ifndef __AFXWIN_H__
	#error inclut 'stdafx.h' avant d'inclure ce fichier pour PCH
#endif

#include "resource.h"       // symboles principaux


// CVedaDemoOGLMfcGuiApp :
// Consultez VedaDemoOGLMfcGui.cpp pour l'implémentation de cette classe
//

class CVedaDemoOGLMfcGuiApp : public CWinApp
{
public:
	CVedaDemoOGLMfcGuiApp();

protected:
	CMultiDocTemplate* m_pDocTemplate;
public:

// Substitutions
public:
	virtual BOOL InitInstance();
	inline unsigned int GetNewFilePref(){ return m_NewFilePref; };
	inline void SetNewFilePref( unsigned int _n){ m_NewFilePref=_n; };
	// Implémentation
	afx_msg void OnAppAbout();
	afx_msg void OnFileNewFrame();
	afx_msg void OnFileNew();
	afx_msg void OnEmptyContext();


	DECLARE_MESSAGE_MAP()
protected:
	//! tell which context is the doc to create.
	unsigned int	m_NewFilePref;

};

extern CVedaDemoOGLMfcGuiApp theApp;
