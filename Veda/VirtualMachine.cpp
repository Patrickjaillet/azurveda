// SPDX-License-Identifier: LGPL-2.1-only

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

	ShutTheFuckUp();
}
#endif

void	VirtualMachine::EnableMediaSound( SoundInterface *_pNoisyObject, bool _enable )
{

	if(_enable)
	{
		SoundUpdateListCell *pNewCell = new SoundUpdateListCell;
		if( !pNewCell ) return;
			pNewCell-> m_pMediaObject		= _pNoisyObject;
			pNewCell->m_pNext				= m_pFirstSoundObject ;
		m_pFirstSoundObject		= pNewCell;
	}else
	{
		SoundUpdateListCell **pCell = &m_pFirstSoundObject ;
		while( *(pCell) != 0L )
		{
			if( _pNoisyObject == (*pCell)->m_pMediaObject )
			{
				SoundUpdateListCell *pCellToDestroy = (*pCell);

				*pCell = pCellToDestroy->m_pNext ;

				delete pCellToDestroy;

				return;
			}
			pCell = &((*pCell)->m_pNext) ;
		}

	}
}

#ifdef _ENGINE_EDITABLE_

void	VirtualMachine::ShutTheFuckUp()
{
	while( m_pFirstSoundObject )
		EnableMediaSound( m_pFirstSoundObject->m_pMediaObject,false);

}
#endif

void	VirtualMachine::SetMachineTitle(const char *_pMachineTitle)
{

}
