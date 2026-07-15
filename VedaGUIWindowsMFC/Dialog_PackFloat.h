#pragma once
#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"
#include "afxcmn.h"

// Boîte de dialogue CDialog_PackFloat

class CDialog_PackFloat : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackFloat)

public:
	CDialog_PackFloat(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialog_PackFloat();

// Données de boîte de dialogue
	enum { IDD = IDD_PACKFLOAT };

	static const char *GetManagedClassName(){ return("PackFloat");   };
	static CDialog_AbstractPackSerializable *NewInstance(CWnd* pParent);

	virtual void Create(CWnd* pParent = NULL);
	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);
	virtual	void	Update(void);
	/*
		\brief	if an object of any type is managed, it could be changed by a cursor state.
				by default, it does nothing. It has to be extended.
		\param	_validityBits tels if _x,_y,_z and _time are  valid
		\param	_x x position.
		\param	_y y position.
		\param	_z z position.
		\param	_time the time of the cursor in seconds.
	*/
	virtual	void ChangeValueOfManagedObjectWithCursor(
								VirtualMachine::InternalViewPort *_pClickedViewPort,
						unsigned int _validityBits,
						float _x,float _y,float _z,double _time );
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	CEdit m_editCtrl;
	virtual BOOL OnInitDialog();
	CEdit m_editCtrlY;
	CEdit m_editCtrlZ;
	CStatic m_StaticX;
	CStatic m_StaticY;
	CStatic m_StaticZ;
	CStatic m_EditBT_X;
	//CStatic m_EditBt_XY;
	//CStatic m_EditBt_XYZ;
	CStatic m_StaticD;
	CEdit m_editCtrlD;
};
