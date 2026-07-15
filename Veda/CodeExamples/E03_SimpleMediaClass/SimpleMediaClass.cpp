
#include "BaseContext.h"
#include "VirtualMedia.h"

#include <math.h>

#include <time.h>

#include "DefaultMachine.h"

class miniMedia : public VirtualMedia
{ public:

    miniMedia() : VirtualMedia() {};

	BASEOBJECT_DEFINE_CLASS(miniMedia);

    virtual void ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort );
    virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal );

	virtual bool CreateInternal(void);
	virtual void CloseInternal(void);

	float 	m_sinusPostion1;
	float	m_sinusPostion2;

	VirtualMachine::InternalObject3DBuffer	*m_p3DObject;
};

BASEOBJECT_DECLARE_CLASS( "MINI", miniMedia, VirtualMedia,"Media Example Class","DefaultName","This is a class done to show how easy it is to make your own media." );

bool miniMedia::CreateInternal(void)
{

	m_MediaTimeLength = 60.0f;

	m_sinusPostion1 = m_sinusPostion2 = 0.0f;

	GetMachine()->NewObject3DBuffer( &m_p3DObject, 4,2,0 );
	if(m_p3DObject==0) return false;

	VirtualMachine::InternalVertex *pVert = m_p3DObject->GetFirstVertex();
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

	VirtualMachine::InternalTriangle *pTriangle = m_p3DObject->GetFirstTriangle();
	pTriangle->m_p0 = 1;
	pTriangle->m_p1 = 2;
	pTriangle->m_p2 = 0;
	pTriangle++;
	pTriangle->m_p0 = 0;
	pTriangle->m_p1 = 2;
	pTriangle->m_p2 = 3;
	m_p3DObject->SetNumberOfActiveTriangle(2);

	m_p3DObject->CompileAsStatic();

	return true;
}
void miniMedia::CloseInternal(void)
{

	GetMachine()->DeleteObject3DBuffer( &m_p3DObject );

}

void miniMedia::ProcessMedia( double ,VirtualMachine::InternalViewPort *_pViewPort )
{

	_pViewPort->Clear(0.1f,0.1f,0.1f);

	_pViewPort->Matrix_LoadID();
	_pViewPort->Matrix_Translate(0.0f,0.0f,-1.0f);

	_pViewPort->Matrix_Rotate(sin(m_sinusPostion1*0.002f),0.0f,0.0f,-1.0f);

	_pViewPort->SetFOVLength(0.75f);

	float vibratoterm2 =0.5f+ sin(m_sinusPostion2*0.0018f) *0.5f;
	m_p3DObject->SetColor(vibratoterm2,0.25f,1.0f);

	_pViewPort->RenderMesh( m_p3DObject );

}
void miniMedia::ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )
{

	const unsigned int	lengthToRender =_SoundBufferToAddYourSignal.m_LengthToRender ;

	float			*pSoundBuffer = _SoundBufferToAddYourSignal.m_pSoundBuffer ;

	const float freq = _SoundBufferToAddYourSignal.m_PlayFrequency ;
	float speed1 =  440.0f*4.0f/freq;
	float speed2 =  293.3*4.0f/freq;

	unsigned int ii;
	float fp1 = m_sinusPostion1;
	float fp2 = m_sinusPostion2;
	const float leftVolume=0.6f;
	const float rightVolume=0.7f;

	for(ii=0 ; ii<lengthToRender ; ii++)
	{
		*(pSoundBuffer++) += sin( fp1 ) *leftVolume ;
		*(pSoundBuffer++) += sin( fp2 ) *rightVolume;
		fp1 +=(speed1 + sin(fp2*0.0018f)*0.004f);
		fp2 += speed2+ sin(fp1*0.002f)*0.004f;
	}

	m_sinusPostion1 = fp1;
	m_sinusPostion2 = fp2;
}

int main(void)
{

	{
    DefaultMachine oMachine;
	{
	BaseContext     oContext;

    oMachine.InitMachine();

	const BaseObject::ClassDescription *classList[]=
	{
		& VirtualMedia::m_Description,
			& miniMedia::m_Description,
		0L
	};
    oContext.RegisterClassList(classList);

    oContext.SetMachine(&oMachine);

	miniMedia *pMiniMedia = (miniMedia *)oContext.NewObject( miniMedia::m_Description );
	if( pMiniMedia==0L ) return 1;

	int result = pMiniMedia->Create();

	if(result<0) return 1;

	clock_t	startclock = clock();

	pMiniMedia->SetSound(true);
	while( !oMachine.GetQuitMessage() )
	{
		double framedate = ((double)(clock()-startclock))/CLOCKS_PER_SEC ;

		if(framedate>=pMiniMedia->GetTimeLength() ) break;
		pMiniMedia->ProcessMediaOnDefaultViewport( framedate );

		oMachine.GetDefaultViewPort()->SwapBuffer();
		oMachine.ProcessInterface();
	}

	pMiniMedia->SetSound(false);

		}
	}

	return 0;
}
