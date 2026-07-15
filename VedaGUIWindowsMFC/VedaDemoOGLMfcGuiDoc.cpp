
#include "stdafx.h"
#include "VedaDemoOGLMfcGui.h"
#include "VedaDemoOGLMfcGuiDoc.h"
#include "InterfacePrefObject.h"
#include "BaseContext.h"
#include "RegisterLibImage.h"
#include "RegisterLibDemo.h"
#include "PreviewMachine.h"
#include <time.h>
#include ".\vedademooglmfcguidoc.h"
#include "MediaAccess.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CVedaDemoOGLMfcGuiDoc, CDocument)

BEGIN_MESSAGE_MAP(CVedaDemoOGLMfcGuiDoc, CDocument)
END_MESSAGE_MAP()

CVedaDemoOGLMfcGuiDoc::CVedaDemoOGLMfcGuiDoc() :
	m_pContextManaged(NULL)
	,m_updateType(0)
	,m_pContextUpdated(NULL)
	,m_pObjectUpdated(NULL)
	,m_pMemberUpdated(NULL)
	,m_pPreviewMachine(NULL)
	,m_TotalTimeBeforeLastStop(0.0)
	,m_IsCurrentlySerializing(false)
	,m_pDocumentChildWindow(NULL)
	,m_pCurrentActionFocused(NULL)
	,m_pDefaultActionFocused(NULL)
	,m_pCurrentSoundObject(NULL)
{

	unsigned int filenewpref = ((CVedaDemoOGLMfcGuiApp *)AfxGetApp())->GetNewFilePref();

	m_pContextManaged = new BaseContext();
	switch( filenewpref )
	{
		default:
		case 0:

			RegisterLibDemo(m_pContextManaged);
		break;
		case 1:

		break;
	}
	((CVedaDemoOGLMfcGuiApp *)AfxGetApp())->SetNewFilePref(0);

	const BaseObject::ClassDescription *tt[]={&InterfacePrefObject::m_Description,NULL};
	m_pContextManaged->RegisterClassList(tt);

	const BaseObject::ClassDescription *erronedClass =
		m_pContextManaged->CheckClassListError();
	if(erronedClass!=0L)
	{
		CString errormess = "One of the class registered for the context:\n";
		errormess += erronedClass->m_CPlusPlusClassName;
		errormess +=", has a serialization identifier already used:\n\"";
		errormess += erronedClass->m_ClassName;
		errormess += "\"Thus, the developper must change this ID!\n Watch out, the aplication may serialize corrupted data !";
		AfxGetMainWnd()->MessageBox(errormess.GetString(),
			"BAD SERIALISATION ID IN CLASS CODE.", MB_OK | MB_ICONERROR );
	}

	m_pPreviewMachine = new CPreviewMachine();
	m_pContextManaged->SetMachine( m_pPreviewMachine );

	m_pContextManaged->SetMaximumNumberOfUndoableEvents( 40 );

	m_pContextManaged->AddGUIUpdateFunction( (void (*)(void *,BaseType::eVedaUpdate,BaseContext *,BaseObject *,BaseType *)) VedaStaticUpdate , this );

}

CVedaDemoOGLMfcGuiDoc::~CVedaDemoOGLMfcGuiDoc()
{

	if( m_pPreviewMachine ) m_pPreviewMachine->ShutTheFuckUp();

	if( m_pContextManaged )
	{
		delete m_pContextManaged;
		m_pContextManaged = NULL;
	}

	if( m_pPreviewMachine ) delete m_pPreviewMachine;
	m_pPreviewMachine = NULL;

}

BOOL CVedaDemoOGLMfcGuiDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_IsCurrentlySerializing = true;
	if( m_pContextManaged )
	{

		m_pContextManaged->DestroyAllObjects();

		UpdateAllViews(NULL);
	}
   POSITION pos = GetFirstViewPosition();
	if(pos != NULL)
   {
		CView* pView = GetNextView(pos);
		if( pView ) m_pDocumentChildWindow = (CMDIChildWnd *) pView->GetParentFrame();

   }
	m_IsCurrentlySerializing = false;

	return TRUE;
}

bool	CVedaDemoOGLMfcGuiDoc::IsActiveDoc()
{
	CMDIFrameWnd *pMain = (CMDIFrameWnd *) AfxGetApp()->GetMainWnd() ;
	if(!pMain) return false;
	if( !(pMain->MDIGetActive()) ) return false;

	if( pMain->MDIGetActive()->GetActiveDocument() == this ) return true;
	return false;
}

void CVedaDemoOGLMfcGuiDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{

		if( m_pContextManaged )
		{
			UINT rawByteLength = m_pContextManaged->GetSerializedDescriptionSize();
			unsigned char *rawBytes = new unsigned char[rawByteLength];
			if(rawBytes)
			{
				m_pContextManaged->Serialize_Out( rawBytes );
				ar.Write( rawBytes , rawByteLength);
				delete rawBytes;
			}
		}
	}
	else
	{

		if( m_pContextManaged )
		{
			CFile *pfile = ar.GetFile();
			if( pfile )
			{
				UINT rawByteLength = (UINT) pfile->GetLength();
				unsigned char *rawBytes = new unsigned char[rawByteLength];
				if( rawBytes )
				{
					pfile->Read( rawBytes, rawByteLength);
					m_IsCurrentlySerializing = true;
					m_pContextManaged->Serialize_In(rawBytes);
					delete rawBytes;
					m_IsCurrentlySerializing = false;

					InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
					GetPreviewSelectedBaseObject(0,&pInterfacePreviewConfiguration);
					if(pInterfacePreviewConfiguration)
					{
						m_TotalTimeBeforeLastStop = pInterfacePreviewConfiguration->GetPreviewCurrentTime();
						m_LastStartPlaySystemDate =  clock();
					}

				}
			}

		}
	}

	char    szAppPath[MAX_PATH] = "";
	CString strAppDirectory;
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));
	::SetCurrentDirectory(strAppDirectory);

}

#ifdef _DEBUG
void CVedaDemoOGLMfcGuiDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVedaDemoOGLMfcGuiDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif

void CVedaDemoOGLMfcGuiDoc::VedaStaticUpdate(void *_GUIObjectToUpdateReturned,unsigned int _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated)
{
	CVedaDemoOGLMfcGuiDoc *pthis = (CVedaDemoOGLMfcGuiDoc *)_GUIObjectToUpdateReturned;
		pthis->VedaUpdate( _updateType, _ContextUpdated, _ObjectUpdated, _MemberUpdated );
}

void	CVedaDemoOGLMfcGuiDoc::VedaUpdate( unsigned int _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated)
{

	m_updateType = _updateType ;
	m_pContextUpdated = _ContextUpdated ;
	m_pObjectUpdated = _ObjectUpdated ;
	m_pMemberUpdated = _MemberUpdated ;

	switch( _updateType )
	{
		case BaseContext::eVedaUpdate_MemberIsGoingToChange:
		{
			return;
		} break;
		case BaseContext::eVedaUpdate_ObjectNew:
		{

		} break;
		case BaseContext::eVedaUpdate_ObjectDestroyed:
		{
			if( _ObjectUpdated == m_pCurrentSoundObject ) m_pCurrentSoundObject = NULL;

			if( _ObjectUpdated == GetEditionSelectedBaseObject() )	SetEditionSelectedBaseObject(NULL);

			if( _ObjectUpdated == GetPreviewSelectedBaseObject() )	SetPreviewSelectedBaseObject(NULL);
		}
		break;
		case BaseContext::eVedaUpdate_ObjectReCreated:
		{

		} break;
		case BaseContext::eVedaUpdate_ObjectClosed:
		{

		}
		break;
		case BaseContext::eVedaUpdate_MemberChanged:
		case BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo:
		{

		}
		break;
	}

	UpdateAllViews(NULL );

	m_updateType = BaseContext::eVedaUpdate_NoUpdate;
}

void	CVedaDemoOGLMfcGuiDoc::OnMerge()
{
	if( !m_pContextManaged ) return;

	CFileDialog	cdiag(true,"avb",0,0,0,0,0);
	cdiag.DoModal();
	CString filename = cdiag.GetFileName();

	CFile	cf;
	cf.Open( filename.GetString() , CFile::modeRead );
	unsigned int ll = (unsigned int)( cf.GetLength() );
	unsigned char *pbuf = new unsigned char[ll];
	if( pbuf )
	{
		cf.Read(pbuf,ll);
		m_IsCurrentlySerializing = true;
		m_pContextManaged->MergeSerialize_In(pbuf);
		delete [] pbuf;
		m_IsCurrentlySerializing = false;
	}

	char    szAppPath[MAX_PATH] = "";
	CString strAppDirectory;
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));
	::SetCurrentDirectory(strAppDirectory);

}
void	CVedaDemoOGLMfcGuiDoc::OnFlushNames()
{
	if( !m_pContextManaged ) return;
	if (AfxGetMainWnd()->MessageBox("Watch out! This Function flushes all object names! Do we do that ?" ,"Flush names" ,MB_YESNO|MB_ICONQUESTION  )==IDNO)
     return;

	m_pContextManaged->FlushNames();
}

void	CVedaDemoOGLMfcGuiDoc::SetPreviewSelectedBaseObject(BaseObject	*pObject,unsigned int _RenderViewIndex )
{
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	BaseObject	*pPrevious;
	pPrevious = GetPreviewSelectedBaseObject();
	if( pPrevious == pObject) return ;
	if( pPrevious )
	{
		GetPreviewSelectedBaseObject(_RenderViewIndex,&pInterfacePreviewConfiguration);
		if(pInterfacePreviewConfiguration)
		{
			pInterfacePreviewConfiguration->SetPreviewCurrentTime((float)GetPreviewCurrentTime(_RenderViewIndex));
		}

	}
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(m_pContextManaged,true);
	if(!ppref) return;
	ppref->SetContextPreviewObject(  pObject, _RenderViewIndex);

	GetPreviewSelectedBaseObject(_RenderViewIndex,&pInterfacePreviewConfiguration);
	if(pInterfacePreviewConfiguration)
	{
		m_TotalTimeBeforeLastStop = pInterfacePreviewConfiguration->GetPreviewCurrentTime();
		m_LastStartPlaySystemDate =  clock();
	}
	UpdateAllViews(NULL);
}

BaseObject *CVedaDemoOGLMfcGuiDoc::GetPreviewSelectedBaseObject(unsigned int _RenderViewIndex,InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration **_ppPreviewConfiguration)

{
	if(_ppPreviewConfiguration) *_ppPreviewConfiguration = NULL;
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(m_pContextManaged);
	if(ppref) return ppref->GetContextPreviewObject( _RenderViewIndex,_ppPreviewConfiguration);
	return NULL;
}

void	CVedaDemoOGLMfcGuiDoc::SetEditionSelectedBaseObject(BaseObject	*pObject)
{
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(m_pContextManaged,true);
	if(ppref)
	{

		if( ppref->GetContextEditedObject() == pObject ) return;

		ppref->SetContextEditedObject(  pObject, 0);
	}

	UpdateAllViews(NULL);
}
BaseObject	*CVedaDemoOGLMfcGuiDoc::GetEditionSelectedBaseObject(unsigned int EditorIndex)
{
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(m_pContextManaged);
	if(ppref) return ppref->GetContextEditedObject( EditorIndex);
	return NULL;
}

bool CVedaDemoOGLMfcGuiDoc::GetPreviewPlayState(unsigned int _RenderViewIndex)
{
	InterfacePrefObject *pRef =	InterfacePrefObject::GetContextActivePref( GetVedaContext() );
	if(!pRef) return false;
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	GetPreviewSelectedBaseObject(_RenderViewIndex,&pInterfacePreviewConfiguration);
	if(!pInterfacePreviewConfiguration) return false;
	bool playState = pInterfacePreviewConfiguration->GetPlayState();

	return playState;
}
double	CVedaDemoOGLMfcGuiDoc::GetPreviewCurrentTime(unsigned int _RenderViewIndex)
{

	InterfacePrefObject *pRef =	InterfacePrefObject::GetContextActivePref( GetVedaContext() );
	if(!pRef) return 0.0;
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	GetPreviewSelectedBaseObject(_RenderViewIndex,&pInterfacePreviewConfiguration);
	if(!pInterfacePreviewConfiguration) return 0.0;

	float	playspeed = pInterfacePreviewConfiguration->GetPlaySpeed();
	float	timeend  = pInterfacePreviewConfiguration->GetPreviewEndTime();
	float	timestart= pInterfacePreviewConfiguration->GetPreviewStartTime();

	if( pInterfacePreviewConfiguration->GetPlayState()  )
	{
		clock_t now = clock() - m_LastStartPlaySystemDate;
		double dnow = ((double)now)/ CLOCKS_PER_SEC ;
		dnow *= playspeed ;
		double value = m_TotalTimeBeforeLastStop + dnow  ;
		if( value > timeend )
		{
			m_LastStartPlaySystemDate =  clock();
			m_TotalTimeBeforeLastStop = value = timestart;
		}
		if( value < timestart )
		{
			m_LastStartPlaySystemDate =  clock();
			m_TotalTimeBeforeLastStop = value = timeend;
		}
		return( value );

	} else
	{

		return m_TotalTimeBeforeLastStop;
	}
}

void	CVedaDemoOGLMfcGuiDoc::SetPreviewPlayState(bool _play,unsigned int _RenderViewIndex)
{
	InterfacePrefObject *pRef =	InterfacePrefObject::GetContextActivePref( GetVedaContext() );
	if(!pRef) return;
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	pRef->GetContextPreviewObject( _RenderViewIndex,&pInterfacePreviewConfiguration);
	if(!pInterfacePreviewConfiguration) return;

	float	playspeed = pInterfacePreviewConfiguration->GetPlaySpeed();

	if( !_play )
	{
		clock_t lastPlayDate = m_LastStartPlaySystemDate;
		m_LastStartPlaySystemDate = clock();
		clock_t now = m_LastStartPlaySystemDate - lastPlayDate;
		double dnow = ((double)now)/ CLOCKS_PER_SEC ;
		dnow *= playspeed ;
		m_TotalTimeBeforeLastStop += dnow;
		pInterfacePreviewConfiguration->SetPreviewCurrentTime((float)m_TotalTimeBeforeLastStop);
	} else
	{
		m_LastStartPlaySystemDate =  clock();
	}
	pInterfacePreviewConfiguration->SetPlayState(_play);

}
void	CVedaDemoOGLMfcGuiDoc::SetPreviewCurrentTime(double _sec,unsigned int _RenderViewIndex)
{
	InterfacePrefObject *pRef =	InterfacePrefObject::GetContextActivePref( GetVedaContext() );
	if(!pRef) return;
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	pRef->GetContextPreviewObject( _RenderViewIndex,&pInterfacePreviewConfiguration);
	if(!pInterfacePreviewConfiguration) return;

	if( _sec> pInterfacePreviewConfiguration->GetPreviewEndTime() ) _sec = pInterfacePreviewConfiguration->GetPreviewStartTime();
	m_TotalTimeBeforeLastStop = _sec ;
	m_LastStartPlaySystemDate =  clock();
	pInterfacePreviewConfiguration->SetPreviewCurrentTime( (float)_sec);

}
void	CVedaDemoOGLMfcGuiDoc::OnPreviewPrevious()
{
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(m_pContextManaged);
	if(!ppref) return;
	ppref->PreviewPrevious(0);

	UpdateAllViews(NULL);

}
void	CVedaDemoOGLMfcGuiDoc::OnEditPrevious()
{
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(m_pContextManaged);
	if(!ppref) return;
	ppref->EditPrevious();
	UpdateAllViews(NULL);
}

void	CVedaDemoOGLMfcGuiDoc::OnExportcfile()
{
	BaseContext *pveda = GetVedaContext();
	if(!pveda) return;

	CFileDialog	cdiag(FALSE,"cpp","context.cpp",OFN_OVERWRITEPROMPT,0,0,0);
	if( cdiag.DoModal ()!=IDOK ) return;

	CString filename = cdiag.GetPathName();
	pveda->ExportAsCPPTableFile( filename.GetString(),"Context");

}
void	CVedaDemoOGLMfcGuiDoc::OnExportExecutable()
{
	BaseContext *pveda = GetVedaContext();
	if(!pveda) return;

	VirtualMedia *pDemoScript,*pBootScript;
	const char *pExportName=0L;
	pDemoScript = MediaAccess::GetMainMedia( pveda, &pBootScript,&pExportName );
	if(!pDemoScript)
	{
		AfxGetMainWnd()->MessageBox("The context must have a valid *Media Access* Object To create an executable.","CAN'T CREATE EXECUTABLE", MB_OK | MB_ICONERROR );
		return;
	}
	if(!pExportName ||strlen(pExportName)<1 )
	{
		AfxGetMainWnd()->MessageBox("*Media Access* Object Got to have a valid Export Name.","CAN'T CREATE EXECUTABLE", MB_OK | MB_ICONERROR );
		return;
	}
	CString fileName = pExportName;
	CString filepath = "BinCreator\\";;

	CString contextCFilename =  ".\\";
	contextCFilename += fileName.GetString() ;
	contextCFilename += ".cpp";
	pveda->ExportAsCPPTableFile( contextCFilename.GetString(),"Context");

	CString commandline = "create ";
	commandline += pExportName;
	system(commandline);

}
void	CVedaDemoOGLMfcGuiDoc::OnUndo()
{
	BaseContext *pveda = GetVedaContext();
	if(!pveda) return;
	pveda->Undo();
}
void	CVedaDemoOGLMfcGuiDoc::OnRedo()
{
	BaseContext *pveda = GetVedaContext();
	if(!pveda) return;
	pveda->Redo();
}

void CVedaDemoOGLMfcGuiDoc::OnAddLibImage()
{
	BaseContext *pveda = GetVedaContext();
	if(!pveda) return;
	RegisterLibImage(pveda);

}
void CVedaDemoOGLMfcGuiDoc::OnAddLibBAOOM()
{
	BaseContext *pveda = GetVedaContext();
	if(!pveda) return;

}

void	CVedaDemoOGLMfcGuiDoc::SetDefaultActionFocusObject( CActionFocusable *_pObjectToFocus)
{
	m_pDefaultActionFocused = _pObjectToFocus ;
}

void	CVedaDemoOGLMfcGuiDoc::SetActionFocusObject( CActionFocusable *_pObjectToFocus )
{
	if( m_pCurrentActionFocused == _pObjectToFocus) return;
	CActionFocusable *_pPreviousFocus = m_pCurrentActionFocused;
	m_pCurrentActionFocused = _pObjectToFocus;
	if(_pPreviousFocus)
	{
		_pPreviousFocus->NotifySetActionFocusable(false);

	}
	if(_pObjectToFocus)
	{
		_pObjectToFocus->NotifySetActionFocusable(true);
	} else
	{
		m_pCurrentActionFocused = m_pDefaultActionFocused;
		if(m_pCurrentActionFocused)
			m_pCurrentActionFocused->NotifySetActionFocusable(true);
	}

}

CVedaDemoOGLMfcGuiDoc::CActionFocusable::CActionFocusable()
: m_isFocused(false)
,m_pPrevious(NULL)
,m_pNext(NULL)
{

}

void	CVedaDemoOGLMfcGuiDoc::CActionFocusable::NotifySetActionFocusable(bool _isActionFocusable)
{
	m_isFocused = _isActionFocusable;
}

void	CVedaDemoOGLMfcGuiDoc::CActionFocusable::ManageViewPortClickAction(
					VirtualMachine::InternalViewPort *,
					InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *,
					float ,float ,double ,
					unsigned int ,
					unsigned int 
						)
{

}

void CVedaDemoOGLMfcGuiDoc::OnCloseDocument()
{
	SetDefaultActionFocusObject(0L);
	 SetActionFocusObject(0L);

	CDocument::OnCloseDocument();
}

void	CVedaDemoOGLMfcGuiDoc::SetCurrentSoundObject(BaseObject *_pNewSoundObject)
{
	if(m_pCurrentSoundObject == _pNewSoundObject)
	{
		if(_pNewSoundObject != NULL)
		{
			if( GetPreviewPlayState(0) )
				_pNewSoundObject->StartPreview();
			else
				_pNewSoundObject->EndPreview();
		}
		return;
	}
	if(m_pCurrentSoundObject)
	{
		m_pCurrentSoundObject->EndPreview();
	}
	if(_pNewSoundObject)
	{
		if( GetPreviewPlayState(0) )
			_pNewSoundObject->StartPreview();
	}
	m_pCurrentSoundObject = _pNewSoundObject;

}
void CVedaDemoOGLMfcGuiDoc::OnForceLang( unsigned int _languageCode)
{
	if(!m_pContextManaged) return;
	m_pPreviewMachine->SetMachineLocalization(_languageCode);

	m_pContextManaged->CloseAllObjects();
}
