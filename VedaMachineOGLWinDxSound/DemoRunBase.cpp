/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
			It is the entry point to executable directly linked with a context chunk 
			for executable generation.
*/
 
//#include <stdio.h>  
#include <Time.h>
#include "BaseContext.h"
#include "OGLMachineWinDxSound.h"
#include "VirtualMedia.h"
#include "MediaAccess.h"
 
extern  unsigned char Context[];
extern const BaseObject::ClassDescription *ContextClassTable[];

//#include "RegisterLibDemo.h"

//int main(int argc, char* argv[])
//int main(int argc, _TCHAR* argv[])
int WINAPI WinMain( HINSTANCE   hInstance,          // Instance
                    HINSTANCE   hPrevInstance,      // Previous Instance
                    LPSTR       lpCmdLine,          // Command Line Parameters
                    int         nCmdShow)           // Window Show State
{
	// start the machine:
		OGLMachineWinDxSound	oMachine;
		unsigned int result = oMachine.InitMachine();

		if(result == VirtualMachine::vmr_FAILED) return 0;
	// start a new context:
		BaseContext	oContext;
		oContext.RegisterClassList( ContextClassTable );
		//RegisterLibDemo( &oContext );
		oContext.SetMachine( &oMachine );

	// read demo context
		oContext.Serialize_In( Context );

	// get the main script and boot scripts:
		VirtualMedia *pDemoScript,*pBootScript;
		const char *pDemoName=0L;

		pDemoScript = MediaAccess::GetMainMedia( &oContext, &pBootScript,&pDemoName );
		if(!pDemoScript) return 0;
		if(pDemoName) oMachine.SetMachineTitle(pDemoName);

	// if boot script, draw it while creating main script
	if(pBootScript && (pBootScript->Create()==0))
	{
		pBootScript->SetSound(true);
		float totalToBuild=0.0f;
		float BuiltValue;
		while( 	(BuiltValue = (float)pDemoScript->Create(false)) > 0.0f )
		{
			if(totalToBuild==0.0f) totalToBuild=BuiltValue;
			float constructionRatio = (1.0f-(BuiltValue/totalToBuild))*10.0f;
			pBootScript->ProcessMediaOnDefaultViewport( constructionRatio );
			oMachine.GetDefaultViewPort()->SwapBuffer();
			oMachine.ProcessInterface();
			if(oMachine.GetQuitMessage()) return 0;
		}
	}else
	{
		// Create the script without bootscript
		pDemoScript->Create();
	}
	clock_t	startclock = clock();
	oMachine.SetSoundCurrentTime(0,0);
	pDemoScript->SetSound(true);
	while( !oMachine.GetQuitMessage() 
			// &&  == 0
			)
	{
		double framedate = ((double)(clock()-startclock))/CLOCKS_PER_SEC ;
		// quit if end.
		if(framedate>=pDemoScript->GetTimeLength() ) break; 
		pDemoScript->ProcessMediaOnDefaultViewport( framedate );
		oMachine.GetDefaultViewPort()->SwapBuffer();
		oMachine.ProcessInterface();
	}
	pDemoScript->SetSound(false);
	return 0;
}

