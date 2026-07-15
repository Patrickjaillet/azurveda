/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "VirtualMachine.h"

VirtualMachine::VirtualMachine() :
	m_pDefaultViewPort(0L)
	,m_QuitMessage(false)
	,m_pFirstSoundObject(0L)
{
}
#ifdef _ENGINE_EDITABLE_
VirtualMachine::~VirtualMachine(void)
{
	// bass !
	ShutTheFuckUp();
}
#endif
/*!
	\brief	Register a media object that will then generate a mixed sound through
			VirtualMedia::ProcessSoundInterupt() The sound will start instantly.
			if false, Unregister a media object previously registered
			with EnableMediaSound(). Then sound stops instantly.
	\param	_pNoisyObject 
*/
void	VirtualMachine::EnableMediaSound( SoundInterface *_pNoisyObject, bool _enable )
{
	// IMPORTANT NOTE: a sound thread can be thrown within this function, and use this list:
	// it implies the list must be safe between any instruction !
	if(_enable)
	{	//TODO : don't link twice the same object.
		SoundUpdateListCell *pNewCell = new SoundUpdateListCell;
		if( !pNewCell ) return;
			pNewCell-> m_pMediaObject		= _pNoisyObject;
			pNewCell->m_pNext				= m_pFirstSoundObject ;
		m_pFirstSoundObject		= pNewCell; // this instruction officially start the sound.
	}else
	{	// unlink: 
		SoundUpdateListCell **pCell = &m_pFirstSoundObject ;
		while( *(pCell) != 0L )
		{
			if( _pNoisyObject == (*pCell)->m_pMediaObject )
			{
				SoundUpdateListCell *pCellToDestroy = (*pCell);
				//unlink: and stop the sound:
				*pCell = pCellToDestroy->m_pNext ;
				//destroy:
				delete pCellToDestroy;
				// ok
				return;
			}
			pCell = &((*pCell)->m_pNext) ;
		}
	
	}
}

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Now, azurveda virtual machine implements a very efficient 
			BASS technology known as "ShutTheFuckUp" !
	\return	VolumeValue [0,1]
*/
void	VirtualMachine::ShutTheFuckUp()
{
	while( m_pFirstSoundObject )
		EnableMediaSound( m_pFirstSoundObject->m_pMediaObject,false);

}
#endif
/*
	\brief	After init,Set a general machine title, for example, to be displayed
		as a windows title. You can change the machine title at any time.
		Default behaviour: does nothing.
	\param	_pMachineTitle	const string to be displayed.
*/
void	VirtualMachine::SetMachineTitle(const char *_pMachineTitle)
{
	//... :-)
}