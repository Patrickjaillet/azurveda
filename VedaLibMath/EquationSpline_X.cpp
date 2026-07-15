// SPDX-License-Identifier: LGPL-2.1-only

#include "EquationSpline_X.h"
#include <math.h>
#ifdef _ENGINE_EDITABLE_
#include "PackResource.h"
#endif

BASEOBJECT_DECLARE_CLASS( "X", EquationSpline_X, VirtualEquationSpline,"Equation Spline x=f(t)","SplineX.","1 Dimension Spline curve defined by a list of keys. X=spline(t)" );

EquationSpline_X::EquationSpline_X() : VirtualEquationSpline()
	,mSer_List(NewSplineElementX)
{
	REGISTER_MEMBER( mSer_List,"Key List");

	BASEOBJECT_REGISTER_TOOLMETHOD( tm_VEquationSpline_Shift,0,"Shift","Shift all values.");
	BASEOBJECT_REGISTER_TOOLMETHOD( tm_VEquationSpline_Scale,0,"Scale","Scale all values.");
	BASEOBJECT_REGISTER_TOOLMETHOD( tm_VEquationSpline_Optimize,0,"Optimize","cull useless values.");
	BASEOBJECT_REGISTER_TOOLMETHOD( tm_VEquationSpline_ImportWaveVolume,0,"Imp.WaveVol","wave volume aproximation.");
	BASEOBJECT_REGISTER_TOOLMETHOD( tm_VEquationSpline_ImportWaveFreq,0,"Imp.WaveFreq","wave freq. aprox.");

}
#ifdef _ENGINE_EDITABLE_

void EquationSpline_X::ExecuteToolMethod( unsigned int _MethodIDToExecute )
{

	VirtualEquation::ExecuteToolMethod(_MethodIDToExecute);

	switch(_MethodIDToExecute)
	{
		default:break;
		case tm_VEquationSpline_Shift:
			ToolMethod_Shift();
			break;
		case tm_VEquationSpline_Scale:
			ToolMethod_Scale();
			break;
		case tm_VEquationSpline_Optimize:
			ToolMethod_Optimize();
			break;
		case tm_VEquationSpline_ImportWaveVolume:
			ToolMethod_ImportWaveVolume();
			break;
		case tm_VEquationSpline_ImportWaveFreq:
			ToolMethod_ImportWaveFreq();
			break;
	}
}
#endif
#ifdef _ENGINE_EDITABLE_

void EquationSpline_X::ToolMethod_Shift()
{

}
#endif
#ifdef _ENGINE_EDITABLE_

void EquationSpline_X::ToolMethod_Scale()
{

}
#endif
#ifdef _ENGINE_EDITABLE_

void EquationSpline_X::ToolMethod_Optimize()
{

}
#endif
#ifdef _ENGINE_EDITABLE_

void EquationSpline_X::ToolMethod_ImportWaveVolume()
{
	char FilePath[256];
	bool result = GetMachine()->FileRequester("Load .Wave for Volume approximation",
									FilePath,255);
	if(!result) return;
	PackResource	resource;

	resource.SetFile(FilePath);
	unsigned int	byteLength = resource.GetByteLength() ;
	if(byteLength==0) return;

	struct	waveInfo	swaveInfo;
	result = ToolMethod_Import_StartLoadWave(resource,swaveInfo);
	if(!result) return;

	const unsigned int	 UnitperSec=3;

	unsigned int	nbSample	= (swaveInfo.dataEndOffset-swaveInfo.dataStartOffset)>>1;
	if(swaveInfo.nChannels ==2) nbSample>>=1;
	unsigned int	nbUnitPass1 = ((nbSample<<UnitperSec)/swaveInfo.nSamplesPerSec)+1;
	if(nbUnitPass1==0) return;
	unsigned char	*pVolumeTable = new unsigned char[nbUnitPass1];
	unsigned int ii=0,jj=0;
	const unsigned char *pBinary=0L;
	unsigned int	binaryLengthReturned;
	unsigned char	smoothmax = 0;
	short allTimeMax =0;
	short allTimeMin =0;
	for(ii=0 ; ii<nbUnitPass1 && jj<nbSample ; ii++ )
	{
		unsigned int NextPassFirst=(swaveInfo.nSamplesPerSec*(ii+1))>>UnitperSec;
		if(NextPassFirst>nbSample) NextPassFirst=nbSample;

		unsigned int	binaryLengthToGet=(NextPassFirst-jj)<<1;
		if(swaveInfo.nChannels ==2) binaryLengthToGet<<=1;
		resource.GetBinaryAtOffset(pBinary,binaryLengthReturned,swaveInfo.dataStartOffset,
									binaryLengthToGet);
		if(binaryLengthReturned != binaryLengthToGet) break;
		swaveInfo.dataStartOffset += binaryLengthReturned;
		unsigned char	maxvolume = 0;

		for( ;jj<NextPassFirst;jj++)
		{
			signed short left = (signed short)(pBinary[0]|(pBinary[1]<<8));
			pBinary+=2;
			if(left>allTimeMax)allTimeMax=left;
			if(left<allTimeMin)allTimeMin=left;

			signed short right=0;
			if(swaveInfo.nChannels ==2)
			{
				right = (signed short)(pBinary[0]|(pBinary[1]<<8));
				pBinary+=2;
			}
			if(left==-32768)left=32767;
			if(left<0)left=-left;
			unsigned char volumeleft = left>>11;

			if(volumeleft>smoothmax) smoothmax = volumeleft;
			if(smoothmax>maxvolume) maxvolume = smoothmax;
		}
		if(smoothmax<=8) smoothmax=0;
		else  smoothmax-=8;
		pVolumeTable[ii] = maxvolume;
	}

	while(ii<nbUnitPass1)
	{
		pVolumeTable[ii] = 0;
		ii++;
	}

	unsigned int	nbKept=1;
	unsigned int lastKeptIndex = 0;
	unsigned char lastKeptValue = pVolumeTable[0];
	for(ii=1 ; ii<(nbUnitPass1-1) ; )
	{

		float lastSlope = (((float)pVolumeTable[ii])-((float)lastKeptValue))
							/( (float)(ii-lastKeptIndex) );

		jj = ii+1;
		while( jj<(nbUnitPass1-1) )
		{
			float newSlope = (((float)pVolumeTable[jj])-((float)lastKeptValue))
					/( (float)(jj-lastKeptIndex) );
			if(newSlope != lastSlope ) break;
			jj++;
		}
		ii = jj;

		nbKept++;
		lastKeptIndex = ii;
		lastKeptValue = pVolumeTable[ii];
		ii++;
	}

	if(nbKept>6000)
	{
		delete [] pVolumeTable ;
		return;
	}

	PackList_TimeTrack	BuildList(NewSplineElementX);

	PackList::Cell *pCell=BuildList.AddElement();
	SplineElement *pSplineElt = (SplineElement *)pCell->GetManagedObject();
	pSplineElt->SetTimeInSecond(0.0);
	pSplineElt->mSer_Vector.Set(0.0f);
	nbKept = 0;
	lastKeptIndex = 0;
	lastKeptValue = pVolumeTable[0];
	for(ii=1 ; ii<(nbUnitPass1-1) ; )
	{

		float lastSlope = (((float)pVolumeTable[ii])-((float)lastKeptValue))
							/( (float)(ii-lastKeptIndex) );

		jj = ii+1;
		while( jj<(nbUnitPass1-1) )
		{
			float newSlope = (((float)pVolumeTable[jj])-((float)lastKeptValue))
					/( (float)(jj-lastKeptIndex) );
			if(newSlope != lastSlope ) break;
			jj++;
		}
		ii = jj;

		nbKept++;
		lastKeptIndex = ii;
		lastKeptValue = pVolumeTable[ii];
		ii++;

		PackList::Cell *pCell=BuildList.AddElement();
		SplineElement *pSplineElt = (SplineElement *)pCell->GetManagedObject();
		double timeSec = ((float)lastKeptIndex)/((float)(1<<UnitperSec));
		pSplineElt->SetTimeInSecond(timeSec);
		pSplineElt->mSer_Vector.Set(((float)lastKeptValue)*(1.0f/15.0f));

	}
	PackList_TimeTrack *pTimeList = GetList();

	pTimeList->MakeCloneOf(BuildList);

	delete [] pVolumeTable ;
}
#endif
#ifdef _ENGINE_EDITABLE_

void EquationSpline_X::ToolMethod_ImportWaveFreq()
{

}
#endif

#ifdef _ENGINE_EDITABLE_

bool EquationSpline_X::ToolMethod_Import_StartLoadWave(PackResource &_resource,struct	waveInfo &_WaveInfo)
{
#define TOTAG(a,b,c,d) ((a<<24)|(b<<16)|(c<<8)|d)

	unsigned int rifftag;
	unsigned int filesize;
	unsigned int fileOffset=0;
	if(!_resource.GetLE_UnsignedInt(rifftag,fileOffset))return false;

	if(rifftag != TOTAG('F','F','I','R')) return false;
	if(!_resource.GetLE_UnsignedInt(filesize,fileOffset))return false;
	if(!_resource.GetLE_UnsignedInt(rifftag,fileOffset))return false;
	if(rifftag != TOTAG('E','V','A','W')) return false;
	if(!_resource.GetLE_UnsignedInt(rifftag,fileOffset))return false;
	if(rifftag != TOTAG(' ','t','m','f')) return false;
	unsigned int jumpToData;
	if(!_resource.GetLE_UnsignedInt(jumpToData,fileOffset))return false;
	jumpToData +=fileOffset;

	short    wFormatTag;
    short    nChannels;
    int   nSamplesPerSec;

    short    nBlockAlign;
    short    wBitsPerSample;
    short    cbSize;

	if(!_resource.GetLE_SignedShort(wFormatTag,fileOffset))return false;
	if(!_resource.GetLE_SignedShort(nChannels,fileOffset))return false;
	if(!_resource.GetLE_SignedInt(nSamplesPerSec,fileOffset))return false;
	fileOffset+=4;

	if(!_resource.GetLE_SignedShort(nBlockAlign,fileOffset))return false;
	if(!_resource.GetLE_SignedShort(wBitsPerSample,fileOffset))return false;
	if(!_resource.GetLE_SignedShort(cbSize,fileOffset))return false;

	fileOffset = jumpToData;
	if(!_resource.GetLE_UnsignedInt(rifftag,fileOffset))return false;
	if(rifftag != TOTAG('a','t','a','d')) return false;
	if(!_resource.GetLE_UnsignedInt(_WaveInfo.dataEndOffset,fileOffset))return false;
	_WaveInfo.dataStartOffset = fileOffset;
	_WaveInfo.dataEndOffset +=_WaveInfo.dataStartOffset;
	_WaveInfo.nChannels = nChannels;
	_WaveInfo.nSamplesPerSec = nSamplesPerSec;
	return true;
}
#endif
