// SPDX-License-Identifier: LGPL-2.1-only

#include <Time.h>
#include "BaseContext.h"
#include "OGLMachineWinDxSound.h"
#include "VirtualMedia.h"
#include "MediaAccess.h"

extern  unsigned char Context[];
extern const BaseObject::ClassDescription *ContextClassTable[];

int WINAPI WinMain( HINSTANCE   hInstance,
                    HINSTANCE   hPrevInstance,
                    LPSTR       lpCmdLine,
                    int         nCmdShow)
{

		OGLMachineWinDxSound	oMachine;
		unsigned int result = oMachine.InitMachine();

		if(result == VirtualMachine::vmr_FAILED) return 0;

		BaseContext	oContext;
		oContext.RegisterClassList( ContextClassTable );

		oContext.SetMachine( &oMachine );

		oContext.Serialize_In( Context );

		VirtualMedia *pDemoScript,*pBootScript;
		const char *pDemoName=0L;

		pDemoScript = MediaAccess::GetMainMedia( &oContext, &pBootScript,&pDemoName );
		if(!pDemoScript) return 0;
		if(pDemoName) oMachine.SetMachineTitle(pDemoName);

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

		pDemoScript->Create();
	}
	clock_t	startclock = clock();
	oMachine.SetSoundCurrentTime(0,0);
	pDemoScript->SetSound(true);
	while( !oMachine.GetQuitMessage()

			)
	{
		double framedate = ((double)(clock()-startclock))/CLOCKS_PER_SEC ;

		if(framedate>=pDemoScript->GetTimeLength() ) break;
		pDemoScript->ProcessMediaOnDefaultViewport( framedate );
		oMachine.GetDefaultViewPort()->SwapBuffer();
		oMachine.ProcessInterface();
	}
	pDemoScript->SetSound(false);
	return 0;
}
