
#include <math.h>

#include <time.h>

#include "DefaultMachine.h"

class miniSoundGenerator : public VirtualMachine::SoundInterface
{ public:

	miniSoundGenerator() :m_sinusPostion1(0.0f),m_sinusPostion2(0.0f)
	{};

	virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )
	{

		const unsigned int lengthToRender =_SoundBufferToAddYourSignal.m_LengthToRender ;

		float			*pSoundBuffer = _SoundBufferToAddYourSignal.m_pSoundBuffer ;

		float freq = _SoundBufferToAddYourSignal.m_PlayFrequency ;
		float speed1 =  440.0f*4.0f/freq;
		float speed2 =  293.3*4.0f/freq;

		unsigned int ii;
		float fp1 = m_sinusPostion1;
		float fp2 = m_sinusPostion2;
		const float leftVolume=0.6f;
		const float rightVolume=0.7f;

		for(ii=0 ; ii<lengthToRender ; ii++)
		{
			float vv = sin( fp1 );
			*(pSoundBuffer++) += sin( fp1 ) *leftVolume ;
			*(pSoundBuffer++) += sin( fp2 ) *rightVolume;
			fp1 +=(speed1 + sin(fp2*0.0018f)*0.004f);
			fp2 += speed2+ sin(fp1*0.002f)*0.004f;
		}

		m_sinusPostion1 = fp1;
		m_sinusPostion2 = fp2;

	}

	float 	m_sinusPostion1;
	float	m_sinusPostion2;

};

bool Create3DObject(VirtualMachine *pMachine,VirtualMachine::InternalObject3DBuffer	**pp3DObject )
{

	pMachine->NewObject3DBuffer( pp3DObject, 4,2,0 );
	if(*pp3DObject==0) return false;

	VirtualMachine::InternalVertex *pVert = (*pp3DObject)->GetFirstVertex();
	pVert->m_x = -0.5f;
	pVert->m_y = -0.5f;
	pVert->m_z = 0.0f;
	pVert++;
	pVert->m_x = 0.5f;
	pVert->m_y = -0.5f;
	pVert->m_z = 0.0f;
	pVert++;
	pVert->m_x = 0.5f;
	pVert->m_y = 0.5f;
	pVert->m_z = 0.0f;
	pVert++;
	pVert->m_x = -0.5f;
	pVert->m_y = 0.5f;
	pVert->m_z = 0.0f;

	VirtualMachine::InternalTriangle *pTriangle = (*pp3DObject)->GetFirstTriangle();
	pTriangle->m_p0 = 1;
	pTriangle->m_p1 = 2;
	pTriangle->m_p2 = 0;
	pTriangle++;
	pTriangle->m_p0 = 0;
	pTriangle->m_p1 = 2;
	pTriangle->m_p2 = 3;
	(*pp3DObject)->SetNumberOfActiveTriangle(2);

	(*pp3DObject)->CompileAsStatic();

	return true;
}

void Draw3DObject( double _frameDate,
						VirtualMachine *pMachine,
						VirtualMachine::InternalObject3DBuffer *p3DObject,
						float sinusposition1,float sinusposition2)
{

	VirtualMachine::InternalViewPort *pViewPort = pMachine->GetDefaultViewPort();

	pViewPort->Clear(0.1f,0.1f,0.1f);

	pViewPort->Matrix_LoadID();
	pViewPort->Matrix_Translate(0.0f,0.0f,-1.0f);

	pViewPort->Matrix_Rotate(sinusposition1,0.0f,0.0f,-1.0f);

	pViewPort->SetFOVLength(0.75f);

	p3DObject->SetColor(sinusposition2,0.25f,1.0f);

	pViewPort->RenderMesh( p3DObject );

}

int  main(void)
{

	VirtualMachine::InternalObject3DBuffer	*p3DObject;

	DefaultMachine oMachine;

	miniSoundGenerator	oSoundGenerator;

	oMachine.InitMachine();

	bool result = Create3DObject( &oMachine,&p3DObject);
	if(!result) return 1;

	oMachine.EnableMediaSound( &oSoundGenerator, true);

	clock_t	startclock = clock();

	while( !oMachine.GetQuitMessage() )
	{
		double framedate = ((double)(clock()-startclock))/CLOCKS_PER_SEC ;

		if(framedate>=60.0f ) break;

		Draw3DObject(framedate,&oMachine,p3DObject,
						oSoundGenerator.m_sinusPostion1*0.002f,
						0.5f+ ((float)sin((double)(oSoundGenerator.m_sinusPostion2*0.0018f)) *0.5f)
						);

		oMachine.GetDefaultViewPort()->SwapBuffer();
		oMachine.ProcessInterface();
	}

	oMachine.EnableMediaSound( &oSoundGenerator, false);

	oMachine.DeleteObject3DBuffer( &p3DObject );

	return 0;
}
