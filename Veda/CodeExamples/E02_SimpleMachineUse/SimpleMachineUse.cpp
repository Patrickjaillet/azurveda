/*!
	\file
	\brief This example shows how to use the VirtualMachine to
		make some drawings and sounds. Note this example only use
		the VirtualMachine in a main(), without using any of the other
		AzurVeda Features like Context management.
		Example SimpleMediaClass do almost the same thing, but by using
		BaseContext and VirtualMedia support.
*/
// we use some math functions for the sound:
#include <math.h>
// we use time.h for a timer.
#include <time.h>
// we need a machine to play the media:
// note DefaultMachine is a typedef to your platform implementation.
// it is inherited from VirtualMachine.
// note we don't need any other include !
#include "DefaultMachine.h"

// we extend our sound generation object from the model in VirtualMachine.
class miniSoundGenerator : public VirtualMachine::SoundInterface
{ public:
	// implement a constructor, to init some mebers:
	miniSoundGenerator() :m_sinusPostion1(0.0f),m_sinusPostion2(0.0f) 
	{};
	// we got to fill this virtual method, on which we create our sound:
	virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )
	{
		// we can write the method here:
		// we got to add a sound signal on a float buffer.
		// Here, we use some sinus functions to make a sound:
		const unsigned int lengthToRender =_SoundBufferToAddYourSignal.m_LengthToRender ;
		// here is the buffer to add our signal:
		float			*pSoundBuffer = _SoundBufferToAddYourSignal.m_pSoundBuffer ;
		// here is the play frequency in float:
		float freq = _SoundBufferToAddYourSignal.m_PlayFrequency ;
		float speed1 =  440.0f*4.0f/freq; // this will lead to a chord :-)
		float speed2 =  293.3*4.0f/freq;
		
		unsigned int ii;
		float fp1 = m_sinusPostion1;
		float fp2 = m_sinusPostion2;
		const float leftVolume=0.6f; // of course a factor multiply the volume.
		const float rightVolume=0.7f;
		// note this nice song suffer from float format precision artifacts !
		for(ii=0 ; ii<lengthToRender ; ii++)
		{
			float vv = sin( fp1 );
			*(pSoundBuffer++) += sin( fp1 ) *leftVolume ;	// add left signal
			*(pSoundBuffer++) += sin( fp2 ) *rightVolume; // add right signal
			fp1 +=(speed1 + sin(fp2*0.0018f)*0.004f); // set a vibrato !
			fp2 += speed2+ sin(fp1*0.002f)*0.004f;
		}
		// keep the states object to ensure continuity of the signal at next call:
		m_sinusPostion1 = fp1;
		m_sinusPostion2 = fp2;
	
	}
	// we set some members here for sound generation:
	float 	m_sinusPostion1;
	float	m_sinusPostion2;
	// end of miniSoundGenerator class definition.
};

// Create3DObject init a 3D object to be drawn.
// we return false if an error occurs.
bool Create3DObject(VirtualMachine *pMachine,VirtualMachine::InternalObject3DBuffer	**pp3DObject )
{
	// ask the machine the creation of a 3D object on pointer m_p3DObject,
	// with 4 vertexes, 2 triangles.
	pMachine->NewObject3DBuffer( pp3DObject, 4,2,0 /* |VirtualMachine::bOb3D_VertexRGBA*/);
	if(*pp3DObject==0) return false; // pointer null means error !
	// give a square shape:
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
	// as the square shape will not change, 
	// this line could optimize rendering:
	(*pp3DObject)->CompileAsStatic();
	// not: we use only colors for rendering.
	// we could also build a VirtualMachine::InternalTexture
	// to render the object.
	// The object is succesfully built !
	return true;
}
// we draw a frame here:
void Draw3DObject( double _frameDate,
						VirtualMachine *pMachine,
						VirtualMachine::InternalObject3DBuffer *p3DObject,
						float sinusposition1,float sinusposition2)
{
	// get the main viewport of the machine. Usually, it will be the main screen.
	VirtualMachine::InternalViewPort *pViewPort = pMachine->GetDefaultViewPort();
	// _frameDate is given in seconds.
	// we got to draw on viewport _pViewPort, using the minimal 3D object created in the init.
	// we will rotate with the same members used for the sound generation in next method.
	// clear:
	pViewPort->Clear(0.1f,0.1f,0.1f);
	// set transformation matrix:
	pViewPort->Matrix_LoadID();
	pViewPort->Matrix_Translate(0.0f,0.0f,-1.0f);
	// make the rotation do the same thing as the sound vibrato
	pViewPort->Matrix_Rotate(sinusposition1,0.0f,0.0f,-1.0f);
	// set focale length for viszion:
	pViewPort->SetFOVLength(0.75f);
	// set the other vibrato term on the red component of the color object:
	p3DObject->SetColor(sinusposition2,0.25f,1.0f);
	// render our 3D object
	pViewPort->RenderMesh( p3DObject );

}

// and then, our main will create and play this media:
int  main(void)
{
	// this pointer will manage a 3D Object we will draw with:
	VirtualMachine::InternalObject3DBuffer	*p3DObject;
	// this is our machine:
	DefaultMachine oMachine;
	// this is our sound generator:
	miniSoundGenerator	oSoundGenerator;
	// init the machine, this should open a screen.
	oMachine.InitMachine();
	// create a 3D object using the machine, exit if failed:
	bool result = Create3DObject( &oMachine,&p3DObject);
	if(!result) return 1;

	// to start the sound generation, we just do this:
	// the sound should begin in very short lapse,
	// throwing calls to miniSoundGenerator::ProcessSoundInterupt().
	oMachine.EnableMediaSound( &oSoundGenerator, true);

	// we manage a timer and do our own loop, drawing frames.
	clock_t	startclock = clock();

	while( !oMachine.GetQuitMessage() )
	{	// get new date:
		double framedate = ((double)(clock()-startclock))/CLOCKS_PER_SEC ;
		// quit after 60 seconds:
		if(framedate>=60.0f ) break;
		// draw function:
		Draw3DObject(framedate,&oMachine,p3DObject,
						oSoundGenerator.m_sinusPostion1*0.002f, // some rotation term.
						0.5f+ ((float)sin((double)(oSoundGenerator.m_sinusPostion2*0.0018f)) *0.5f) // some color term.
						);
		// swap the main screen buffer.
		oMachine.GetDefaultViewPort()->SwapBuffer();
		oMachine.ProcessInterface();
	}
	// to stop the sound generation before quitting, we can do this:
	oMachine.EnableMediaSound( &oSoundGenerator, false);
	// delete the 3D object we have created:
	oMachine.DeleteObject3DBuffer( &p3DObject );
	// OK
	return 0;
}
