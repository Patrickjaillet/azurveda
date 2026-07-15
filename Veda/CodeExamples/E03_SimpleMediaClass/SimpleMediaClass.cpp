/*!
	\file
	\brief This example shows how to extend the context class VirtualMedia
			To create a sound and 3D demo.
			It almost does the same as example SimpleMachineUSe, except
			that we now do it Through a standard Media object. Continuing this example,
			we could imagine to integrate this media class into any other 
			Program in a standard manner, mixing it with MediaTimePattern for example. 
*/
#include "BaseContext.h"
#include "VirtualMedia.h"
// we use some math functions for the sound:
#include <math.h>
// we use time.h for a timer.
#include <time.h>
// we need a machine to play the media:
// note DefaultMachine is a typedef to your platform implementation:
#include "DefaultMachine.h"
// first , we define our media class like in a header file.
// We will not register serializable members,
// but we will extend media methods:
class miniMedia : public VirtualMedia
{ public: 
    // constructor:
    miniMedia() : VirtualMedia() {};
	// define context class descriptor and destuctor: 
	BASEOBJECT_DEFINE_CLASS(miniMedia);
    // extend methods from VirtualMedia to make our media to draw and play things:
    virtual void ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort );
    virtual void ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal );
	// extend creation and close methods from BaseObject, to make our own inits:
	virtual bool CreateInternal(void);
	virtual void CloseInternal(void);
	// now declare some specific members for our routines.
	// we could declare serializable members here, na deven dynamic pointers
	// to other context objects, but it is  not the point of the example.
	// (see examples 01 with PackStruct or any class code).
	// state for sound generation:
	float 	m_sinusPostion1;
	float	m_sinusPostion2;
	// simple 3D object to be built at init, and then drawn:
	//! object created for preview.
	VirtualMachine::InternalObject3DBuffer	*m_p3DObject;
};
// ----- Here we have .cpp-like methods for miniMedia -----
// declare class descriptor, its makes possible self-build GUI and context serialization (we will not use in this example):
BASEOBJECT_DECLARE_CLASS( "MINI", miniMedia, VirtualMedia,"Media Example Class","DefaultName","This is a class done to show how easy it is to make your own media." );
// CreateInternal must do all inits for a context object, and everything opened
// should be close in CloseInternal().
bool miniMedia::CreateInternal(void)
{
	// use superclass member to tell the media time length:
	m_MediaTimeLength = 60.0f; // 60 seconds.
	// reset state members
	m_sinusPostion1 = m_sinusPostion2 = 0.0f;
	// ask the machine the creation of a 3D object on pointer m_p3DObject,
	// with 4 vertexes, 2 triangles.
	GetMachine()->NewObject3DBuffer( &m_p3DObject, 4,2,0 /* |VirtualMachine::bOb3D_VertexRGBA*/);
	if(m_p3DObject==0) return false; // pointer null means error !
	// give a square shape:
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
	// as the square shape will not change, 
	// this line could optimize rendering:
	m_p3DObject->CompileAsStatic();
	// not: we use only colors for rendering.
	// we could also build a VirtualMachine::InternalTexture
	// to render the object.
	// The object is succesfully built !
	return true;
}
void miniMedia::CloseInternal(void)
{
	// delete the object in case it has been created.
	//note all machine delete calls do nothing if pointer is null.
	// so no need for a test:
	GetMachine()->DeleteObject3DBuffer( &m_p3DObject );

}
// we need our class descriptors, ussually with the methods in a .cpp file:
void miniMedia::ProcessMedia( double _frameDate,VirtualMachine::InternalViewPort *_pViewPort )
{
	// ProcessMedia() is useed to manage drawing, frame by frame.
	// _frameDate is given in seconds.
	// we got to draw on viewport _pViewPort, using the minimal 3D object created in the init.
	// we will rotate with the same members used for the sound generation in next method.
	// clear:
	_pViewPort->Clear(0.1f,0.1f,0.1f);
	// set transformation matrix:
	_pViewPort->Matrix_LoadID();
	_pViewPort->Matrix_Translate(0.0f,0.0f,-1.0f);
	// make the rotation do the same thing as the sound vibrato
	_pViewPort->Matrix_Rotate(sin(m_sinusPostion1*0.002f),0.0f,0.0f,-1.0f);
	// set focale length for viszion:
	_pViewPort->SetFOVLength(0.75f);
	// set the other vibrato term on the red component of the color object:
	float vibratoterm2 =0.5f+ sin(m_sinusPostion2*0.0018f) *0.5f;
	m_p3DObject->SetColor(vibratoterm2,0.25f,1.0f);
	// render our 3D object
	_pViewPort->RenderMesh( m_p3DObject );

}
void miniMedia::ProcessSoundInterupt( VirtualMachine::SoundBufferToAddYourSignal &_SoundBufferToAddYourSignal )
{
	// ProcessSoundInterupt() ask us to add a sound signal on a float buffer.
	// Here, we use some sinus functions to make a sound:
	const unsigned int	lengthToRender =_SoundBufferToAddYourSignal.m_LengthToRender ;
	// here is the buffer to add our signal:
	float			*pSoundBuffer = _SoundBufferToAddYourSignal.m_pSoundBuffer ;
	// here is the play frequency in float:
	const float freq = _SoundBufferToAddYourSignal.m_PlayFrequency ;
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
// and then, our main will create and play this media:
int main(void)
{
    // we need a context and a machine: note: as we will linik the machine
	// to the context, we must assure the context is deleted first at the end,
	// and then the machine is deleted. these 2 brackets assures this:
	{
    DefaultMachine oMachine;
	{
	BaseContext     oContext;
    // init the machine, this should open a screen.
    oMachine.InitMachine();
    //register our class to the context:
	const BaseObject::ClassDescription *classList[]=
	{    
		& VirtualMedia::m_Description,  // we need to register all parent class after BaseObject.
			& miniMedia::m_Description, // class built over VirtualMedia. 
		0L // list end.
	};
    oContext.RegisterClassList(classList);
    // tell the context to work with the machine:
    oContext.SetMachine(&oMachine);
	
	// now do a simple edition on the context, just to create our object:
	miniMedia *pMiniMedia = (miniMedia *)oContext.NewObject( miniMedia::m_Description );
	if( pMiniMedia==0L ) return 1; // error ?

	// any context object should be inited with Create(),
	// and its method used if the creation succed.
	// note the object init is overriden with BaseObject::CreateInternal()
	// and BaseObject::CloseInternal().
	int result = pMiniMedia->Create();
	// a negative value means a construction error.
	// a 0 value mean construction is done, and positive values means
	// the object need more Create() calls, which can only happen with Create(false).
	// Create(false) can be used for delayed creation.
	// true means immediate creation and is a default parameter.
	if(result<0) return 1;
	// when using context object, you should first ensure that the object
	// is well created, before using its methods. Here, a miniMedia could
	// fail because the 3D object creation in CreateInternal() could fail,
	// because of short memory for example. So now we can use miniMedia.
						
	// we do our own loop in the main, manage a timer,
	// and ask the media to draw frames whenever we can:
	// You could ask yourself why this is not automatized by the Media mechanism.
	// The answer is AzurVeda's Context framework must keep independant
	// from any shape of application. 
	clock_t	startclock = clock();
	// we use the sound aspect of the media:
	pMiniMedia->SetSound(true);
	while( !oMachine.GetQuitMessage() )
	{	// get new date:
		double framedate = ((double)(clock()-startclock))/CLOCKS_PER_SEC ;
		// quit if media length ended.
		if(framedate>=pMiniMedia->GetTimeLength() ) break; 
		pMiniMedia->ProcessMediaOnDefaultViewport( framedate );
		// swap the main screen buffer.
		oMachine.GetDefaultViewPort()->SwapBuffer();
		oMachine.ProcessInterface();
	}
	// if we continue something after the loop, this should be done:
	pMiniMedia->SetSound(false);
	// close the 2 brackets, it delete the context, and then the machine, in this order:
		}
	}
	// OK
	return 0;
}
