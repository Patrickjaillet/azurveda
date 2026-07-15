// VedaDemoOGLMfcGuiDoc.cpp : implémentation de la classe CVedaDemoOGLMfcGuiDoc
//

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

// CVedaDemoOGLMfcGuiDoc

IMPLEMENT_DYNCREATE(CVedaDemoOGLMfcGuiDoc, CDocument)

BEGIN_MESSAGE_MAP(CVedaDemoOGLMfcGuiDoc, CDocument)
END_MESSAGE_MAP()

// construction ou destruction de CVedaDemoOGLMfcGuiDoc

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
	// create a MFC document means create a veda context:
	unsigned int filenewpref = ((CVedaDemoOGLMfcGuiApp *)AfxGetApp())->GetNewFilePref(); 
	
	m_pContextManaged = new BaseContext();
	switch( filenewpref )
	{
		default:
		case 0:
			//m_pContextManaged = new VedaDemoContext();
			RegisterLibDemo(m_pContextManaged);
		break;
		case 1:
			
		break;
	}
	((CVedaDemoOGLMfcGuiApp *)AfxGetApp())->SetNewFilePref(0);
	// register known class !
	const BaseObject::ClassDescription *tt[]={&InterfacePrefObject::m_Description,NULL};
	m_pContextManaged->RegisterClassList(tt);
	// check is classes was OK:
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
	// link a GL Preview machine to the context before any serialization:
	m_pPreviewMachine = new CPreviewMachine();
	m_pContextManaged->SetMachine( m_pPreviewMachine );
	//  we decide of a undo maximum number, it enables undo management:
	m_pContextManaged->SetMaximumNumberOfUndoableEvents( 40 );
	// link an update function so that the veda context notify its changes:
	m_pContextManaged->AddGUIUpdateFunction( (void (*)(void *,BaseType::eVedaUpdate,BaseContext *,BaseObject *,BaseType *)) VedaStaticUpdate , this );

}

CVedaDemoOGLMfcGuiDoc::~CVedaDemoOGLMfcGuiDoc()
{
	// we need to unlink all noisy objects  so that the sound thread don't use anything.
	if( m_pPreviewMachine ) m_pPreviewMachine->ShutTheFuckUp();
	//destroy MFC doc means destroy veda context:
	if( m_pContextManaged )
	{	
		delete m_pContextManaged;
		m_pContextManaged = NULL;
	}
	// ...then destroy preview machine:
	if( m_pPreviewMachine ) delete m_pPreviewMachine;
	m_pPreviewMachine = NULL;

}

BOOL CVedaDemoOGLMfcGuiDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	//  code de réinitialisation
	// (les documents SDI réutiliseront ce document)
	m_IsCurrentlySerializing = true;
	if( m_pContextManaged )
	{	
		// vers veda:
		m_pContextManaged->DestroyAllObjects();
		//redessine tout:
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
	if( !(pMain->MDIGetActive()) ) return false; // can happen if all doc destroyed,
												// and a call through a destruction.
	if( pMain->MDIGetActive()->GetActiveDocument() == this ) return true;
	return false;
}

// sérialisation de CVedaDemoOGLMfcGuiDoc

void CVedaDemoOGLMfcGuiDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		//code de stockage
		// redirection vers serialization veda:
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
		//  code de chargement
		// redirection vers serialization veda:
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
					// if there is a preview object, init its preview time:
					InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
					GetPreviewSelectedBaseObject(0,&pInterfacePreviewConfiguration);
					if(pInterfacePreviewConfiguration)
					{
						m_TotalTimeBeforeLastStop = pInterfacePreviewConfiguration->GetPreviewCurrentTime();
						m_LastStartPlaySystemDate =  clock();
					}

				}
			}
	
		} // end if m_pContextManaged 
	}
	// reset dir because of dialog
	char    szAppPath[MAX_PATH] = "";
	CString strAppDirectory;
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
	// Extract directory
	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));
	::SetCurrentDirectory(strAppDirectory);

}
// diagnostics pour CVedaDemoOGLMfcGuiDoc

#ifdef _DEBUG
void CVedaDemoOGLMfcGuiDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVedaDemoOGLMfcGuiDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
// commandes pour CVedaDemoOGLMfcGuiDoc
/*old void	CVedaDemoOGLMfcGuiDoc::SetEditionSelectedBaseObject(BaseObject	*pObject)
{
	if( pObject == m_pSelectedBaseObject ) return; // already OK.
	m_pSelectedBaseObject = pObject ;
	UpdateAllViews(NULL);
}*/
/*old
BaseObject	*CVedaDemoOGLMfcGuiDoc::GetEditionSelectedBaseObject()
{
// SetEditionSelectedBaseObject
	return(m_pSelectedBaseObject);
}*/
// the static one:
void CVedaDemoOGLMfcGuiDoc::VedaStaticUpdate(void *_GUIObjectToUpdateReturned,unsigned int _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated)
{	// redirect this static callback to a virtual object method:
	CVedaDemoOGLMfcGuiDoc *pthis = (CVedaDemoOGLMfcGuiDoc *)_GUIObjectToUpdateReturned;
		pthis->VedaUpdate( _updateType, _ContextUpdated, _ObjectUpdated, _MemberUpdated );
}
// the virtual one:
void	CVedaDemoOGLMfcGuiDoc::VedaUpdate( unsigned int _updateType,BaseContext *_ContextUpdated,BaseObject *_ObjectUpdated,BaseType *_MemberUpdated)
{

	m_updateType = _updateType ;
	m_pContextUpdated = _ContextUpdated ;
	m_pObjectUpdated = _ObjectUpdated ;
	m_pMemberUpdated = _MemberUpdated ;

//  ------ update not linked to view behaviour:
	// manage veda updates:
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
			// if the current preference object is the object destroyed, update:
			// if the "selected" object is destroyed, unselect:
			if( _ObjectUpdated == GetEditionSelectedBaseObject() )	SetEditionSelectedBaseObject(NULL);
			// if the "Preview" object is destroyed, unselect:
			if( _ObjectUpdated == GetPreviewSelectedBaseObject() )	SetPreviewSelectedBaseObject(NULL);
		}
		break;
		case BaseContext::eVedaUpdate_ObjectReCreated:
		{/*
			if( GetPreviewSelectedBaseObject() )
				GetPreviewSelectedBaseObject()->AssureObjectIsCreatedAccordingToAllUsedObjectMembers();*/

		} break;
		case BaseContext::eVedaUpdate_ObjectClosed:
		{	

		}
		break;
		case BaseContext::eVedaUpdate_MemberChanged:
		case BaseContext::eVedaUpdate_MemberChangedBecauseOfUndo:
		{
			// notify the update informations to all views:
			/*re
			UpdateAllViews(NULL );
			// once:
			m_updateType = BaseContext::eVedaUpdate_NoUpdate;
			// update Preview object:
			if( GetPreviewSelectedBaseObject() )
				GetPreviewSelectedBaseObject()->Create();
			return;
			*/


		}
		break;
	}


	//----- 
	// notify the update informations to all views:
	UpdateAllViews(NULL );
	// once:
	m_updateType = BaseContext::eVedaUpdate_NoUpdate;
}
// ---------------------------------------
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
	// reset dir because of dialog
	char    szAppPath[MAX_PATH] = "";
	CString strAppDirectory;
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
	// Extract directory
	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));
	::SetCurrentDirectory(strAppDirectory);

}
void	CVedaDemoOGLMfcGuiDoc::OnFlushNames()
{
	if( !m_pContextManaged ) return;
	if (AfxGetMainWnd()->MessageBox("Watch out! This Function flushes all object names! Do we do that ?" ,"Flush names" ,MB_YESNO|MB_ICONQUESTION  /*QUESTION*/)==IDNO)
     return;

	m_pContextManaged->FlushNames();
}


void	CVedaDemoOGLMfcGuiDoc::SetPreviewSelectedBaseObject(BaseObject	*pObject,unsigned int _RenderViewIndex )
{
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	BaseObject	*pPrevious;
	pPrevious = GetPreviewSelectedBaseObject();
	if( pPrevious == pObject) return ;// do nothing if same.
	if( pPrevious )
	{	
		GetPreviewSelectedBaseObject(_RenderViewIndex,&pInterfacePreviewConfiguration);
		if(pInterfacePreviewConfiguration)
		{
			pInterfacePreviewConfiguration->SetPreviewCurrentTime(GetPreviewCurrentTime(_RenderViewIndex));
		}
	
	}
	InterfacePrefObject *ppref = InterfacePrefObject::GetContextActivePref(m_pContextManaged,true);
	if(!ppref) return;
	ppref->SetContextPreviewObject(  pObject, _RenderViewIndex);
	// reset time to the preview configuration of this object:
	//InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	GetPreviewSelectedBaseObject(_RenderViewIndex,&pInterfacePreviewConfiguration);
	if(pInterfacePreviewConfiguration)
	{
		m_TotalTimeBeforeLastStop = pInterfacePreviewConfiguration->GetPreviewCurrentTime();
		m_LastStartPlaySystemDate =  clock();
	}
	UpdateAllViews(NULL);
}
/*!
	\brief	Get a current Preview object for a context.
				If there are none, return 0L.
	\param _RenderViewIndex if the viewer support multiple previews, the index of the preview affected.
	\param _pPreviewConfiguration if not nul, return a pointer to the configuration preview to it.
	\return the current Preview object or 0L.
*/
BaseObject *CVedaDemoOGLMfcGuiDoc::GetPreviewSelectedBaseObject(unsigned int _RenderViewIndex,InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration **_ppPreviewConfiguration)
//BaseObject	*CVedaDemoOGLMfcGuiDoc::GetPreviewSelectedBaseObject(unsigned int previewIndex)
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
		// do nothing if effect nul:
		if( ppref->GetContextEditedObject() == pObject ) return;
		//select new, or object, as selected object number 0:
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

/*
	\brief	Get play/pause state for a Preview.
	\return	 if true, play, if false, pause.
*/
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
double	CVedaDemoOGLMfcGuiDoc::GetPreviewCurrentTime(unsigned int _RenderViewIndex) // in seconds.
{
	// get PreviewConfiguration object for the current preview object/conf.
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
	{ // stop		
	
		return m_TotalTimeBeforeLastStop;
	}
}
/*
	\brief	set play/pause state for a Preview.
	\param	_play if true, play, if false, pause.
*/
void	CVedaDemoOGLMfcGuiDoc::SetPreviewPlayState(bool _play,unsigned int _RenderViewIndex)
{
	InterfacePrefObject *pRef =	InterfacePrefObject::GetContextActivePref( GetVedaContext() );
	if(!pRef) return;
	InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *pInterfacePreviewConfiguration=NULL;
	BaseObject *pPreviewObject = pRef->GetContextPreviewObject( _RenderViewIndex,&pInterfacePreviewConfiguration);
	if(!pInterfacePreviewConfiguration) return;
	//if(pInterfacePreviewConfiguration->get play)

	float	playspeed = pInterfacePreviewConfiguration->GetPlaySpeed();
	float	timeend  = pInterfacePreviewConfiguration->GetPreviewEndTime();
	float	timestart= pInterfacePreviewConfiguration->GetPreviewStartTime();

	if( !_play ) // stop
	{
		clock_t lastPlayDate = m_LastStartPlaySystemDate;
		m_LastStartPlaySystemDate = clock();
		clock_t now = m_LastStartPlaySystemDate - lastPlayDate;
		double dnow = ((double)now)/ CLOCKS_PER_SEC ;
		dnow *= playspeed ;
		m_TotalTimeBeforeLastStop += dnow;
		pInterfacePreviewConfiguration->SetPreviewCurrentTime(m_TotalTimeBeforeLastStop);
	} else // play
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
	BaseObject *pPreviewObject = pRef->GetContextPreviewObject( _RenderViewIndex,&pInterfacePreviewConfiguration);
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

	// CWinApp
	//old CFileDialog	cdiag(FALSE,"","demo",OFN_OVERWRITEPROMPT,0,0,0);
	//old if( cdiag.DoModal ()!=IDOK ) return;
	// write cpp file:
	//CString fileName = cdiag.GetFileName();
	//CString filepath = cdiag.GetPathName();

	//CString dotbat;

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

	CString contextCFilename =  ".\\"; // ".\\"
	contextCFilename += fileName.GetString() ;
	contextCFilename += ".cpp";
	pveda->ExportAsCPPTableFile( contextCFilename.GetString(),"Context");

	// compile cpp file
/*	CString compileCommand = ".\\cl.exe -c "; // BinCreator
	compileCommand += fileName.GetString();
	compileCommand += ".cpp";
	//system( compileCommand.GetString() );
	dotbat = "cd BinCreator\r\n";
	dotbat += compileCommand.GetString();
	dotbat +="\r\n";
*/
	//system("cd BinCreator");
	CString commandline = "create "; // name of create.bat
	commandline += pExportName;
	system(commandline);
//system( compileCommand.GetString() );
	// link data .obj to exe:
/*	CString linkCommand = ".\\link.exe ";
	linkCommand += fileName.GetString();
	linkCommand += ".obj .\\DemoRunBase.obj "; // .\\Icon.res
	linkCommand += "/OUT:\"./"; // add exe output name.
	linkCommand += fileName.GetString();
	linkCommand += ".exe\" ";*/
// NOTOK: oleaut32.lib ole32.lib advapi32.lib comdlg32.lib shell32.lib winspool.lib
// OK: uuid.lib gdi32.lib kernel32.lib user32.lib
/*	CString optionSet="\
/INCREMENTAL:NO /NOLOGO /NODEFAULTLIB /NODEFAULTLIB:\"LIBC\" \
/OPT:REF /OPT:ICF /LTCG opengl32.lib dsound.lib dxguid.lib kernel32.lib user32.lib MSVCRT98.LIB MSVCRT.LIB  \
 gdi32.lib \
uuid.lib odbc32.lib odbccp32.lib \
.\Veda.lib \
.\VedaBAOOM.lib \
.\VedaDemo.lib \
.\VedaDemoOGL.lib \
.\VedaImage.lib \
.\VedaMath.lib \
.\VedaLibImageJPEG.lib \
.\VedaLIBSoundMP3.lib \
.\VedaLIBSoundXM.lib \
.\VedaMachineOGLWinDxSound.lib \
";*/
/* old
..\\..\\VedaMachineOGLWinDxSound\\VedaRelease\\Veda.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaLibBAOOMRelease\\VedaBAOOM.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaLibDemoRelease\\VedaDemo.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaMachineOGLRelease\\VedaDemoOGL.lib \
..\\..\\VedaMachineOGLWinDxSound\\vedalibimagerelease\\VedaImage.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaLibMathRelease\\VedaMath.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaLibImageJPEGRelease\\VedaLibImageJPEG.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaLibSoundMP3Release\\VedaLIBSoundMP3.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaLibSoundXMRelease\\VedaLIBSoundXM.lib \
..\\..\\VedaMachineOGLWinDxSound\\VedaMachineOGLWinDxSoundRelease\\VedaMachineOGLWinDxSound.lib \
*/
	/*
	linkCommand+= optionSet.GetString();
	system( linkCommand.GetString() );
	
	// use upx to shorten executable size:
	CString CommandPack = "upx -9 -f ";
	CommandPack += fileName.GetString();
	CommandPack += ".exe";
	system( CommandPack.GetString() );

	// execute...
	CString CommandExecute = fileName.GetString();
	CommandExecute +=".exe";
	system( CommandExecute.GetString() );*/
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
/*
	RegisterLibImage(pveda );
	CString	str =  pdoc->GetVedaContext()->GetName() ;
	InsertClassNode( pdoc->GetVedaContext(), TVI_ROOT,str );
	*/
}
void CVedaDemoOGLMfcGuiDoc::OnAddLibBAOOM()
{
	BaseContext *pveda = GetVedaContext();
	if(!pveda) return;
	/*
	RegisterLibBAOOM( pdoc->GetVedaContext() );
	CString	str =  pdoc->GetVedaContext()->GetName() ;
	InsertClassNode( pdoc->GetVedaContext(), TVI_ROOT,str );
	*/
}
/*!
	\brief	set the default focus. when focus is killed, it become
		automatically the default.
	\param _pObjectToFocus the object, or 0L or nothing, when there is no more object to edit.
*/
void	CVedaDemoOGLMfcGuiDoc::SetDefaultActionFocusObject( CActionFocusable *_pObjectToFocus)
{
	m_pDefaultActionFocused = _pObjectToFocus ;
}

/*!
	\brief	set an object from the context (object or object members) that is to be edited
			by actions on the preview.
	\param _pObjectToFocus the object, or 0L or nothing, when there is no more object to edit.
*/
void	CVedaDemoOGLMfcGuiDoc::SetActionFocusObject( CActionFocusable *_pObjectToFocus )
{
	if( m_pCurrentActionFocused == _pObjectToFocus) return;
	CActionFocusable *_pPreviousFocus = m_pCurrentActionFocused;
	m_pCurrentActionFocused = _pObjectToFocus; // can be 0L
	if(_pPreviousFocus)
	{
		_pPreviousFocus->NotifySetActionFocusable(false);
		/*
		BaseType *pMember = _pPreviousFocus->GetManagedObject();
		BaseObject *pOb = pMember->GetObjectThatManagesThis();
		BaseContext *pContext = NULL;
		if(pOb) pContext = pOb->GetContext();
		VedaUpdate( BaseContext::eVedaUpdate_MemberChanged,pContext,pOb,pMember);
		*/
	}
	if(_pObjectToFocus)
	{/*
		BaseType *pMember = _pObjectToFocus->GetManagedObject();
		BaseObject *pOb = pMember->GetObjectThatManagesThis();
		BaseContext *pContext = NULL;
		if(pOb) pContext = pOb->GetContext();
		VedaUpdate( BaseContext::eVedaUpdate_MemberChanged,pContext,pOb,pMember);
	*/
		_pObjectToFocus->NotifySetActionFocusable(true);
	} else
	{
		m_pCurrentActionFocused = m_pDefaultActionFocused;
		if(m_pCurrentActionFocused)
			m_pCurrentActionFocused->NotifySetActionFocusable(true);
	}

}

/*
	\brief constr.
*/
CVedaDemoOGLMfcGuiDoc::CActionFocusable::CActionFocusable() 
: m_isFocused(false)
,m_pPrevious(NULL)
,m_pNext(NULL)
{

}
/*
	\brief	send when the object become the actionfocused one.	
*/
void	CVedaDemoOGLMfcGuiDoc::CActionFocusable::NotifySetActionFocusable(bool _isActionFocusable)
{
	m_isFocused = _isActionFocusable;
}
/*
	\brief	a viewport click will affect something in a way or another
				according to the active CActionFocusable. It is a 
				virtual method to be overriden.
	\param	_pClickedViewPort the viewport that has been clicked.
	\param	_xClic the position clicked in _pClickedViewPort's ratio
	\param	_yClic the position clicked in _pClickedViewPort's ratio
	\param	_previewtimesec preview time position.
	\param  _mouseButton index of the mouse button. 0=left,1=center,...
	\param	_mouseButtonEvent 0=clickdown, 1=move when down,2=up.
*/
void	CVedaDemoOGLMfcGuiDoc::CActionFocusable::ManageViewPortClickAction(  
					VirtualMachine::InternalViewPort *_pClickedViewPort,
					InterfacePrefObject::PreviewEdited::InterfacePreviewConfiguration *_pPreviewConfToModify,
					float _xClic,float _yClic,double _previewtimesec,
					unsigned int _mouseButton,
					unsigned int _mouseButtonEvent
						)
{
	// by default, do nothing.

}

void CVedaDemoOGLMfcGuiDoc::OnCloseDocument()
{
	SetDefaultActionFocusObject(0L);
	 SetActionFocusObject(0L);

	CDocument::OnCloseDocument();
}
/*
	\brief	Set the object actually previewed, to enable/disable sound 
	\param	_pNewSoundObject
*/
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
	// force recreate:
	m_pContextManaged->CloseAllObjects();
}
