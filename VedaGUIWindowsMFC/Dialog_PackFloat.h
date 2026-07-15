#pragma once
#include "Dialog_AbstractPackSerializable.h"
#include "afxwin.h"
#include "afxcmn.h"

class CDialog_PackFloat : public CDialog_AbstractPackSerializable
{
	DECLARE_DYNAMIC(CDialog_PackFloat)

public:
	CDialog_PackFloat(CWnd* pParent = NULL);
	virtual ~CDialog_PackFloat();

	enum { IDD = IDD_PACKFLOAT };

	static const char *GetManagedClassName(){ return("PackFloat");   };
	static CDialog_AbstractPackSerializable *NewInstance(CWnd* pParent);

	virtual void Create(CWnd* pParent = NULL);
	virtual	void SetPackSerializable(	BaseType *_objToManage,
										CVedaDemoOGLMfcGuiDoc *_pDoc,
										CView				  *_pView
										);
	virtual	void	Update(void);

	virtual	void ChangeValueOfManagedObjectWithCursor(
								VirtualMachine::InternalViewPort *_pClickedViewPort,
						unsigned int _validityBits,
						float _x,float _y,float _z,double _time );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

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

	CStatic m_StaticD;
	CEdit m_editCtrlD;
};
