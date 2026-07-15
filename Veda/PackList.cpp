/*! \file 
	\author victorien ferry & www.m4nkind.com
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "PackList.h"
#include "BaseObject.h"
#include "BaseContext.h"
PackList::PackList( BaseTypeCreatorCallBackFunction _func) : PackStruct() 
	,m_CreatorFunc(_func)
{

}

#ifdef _ENGINE_EDITABLE_
PackList::~PackList(void)
{ 
	DeleteAllElements();

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Add a new element, at the given index.
	\param	_indexWhereToInsert the index where to insert. if -1, at the end.
	\param _pObjectToAdd object to manage, or NULL to set it after on the cell.
	\return  the element created.
*/
PackStruct::Cell *PackList::AddElement(  int _indexWhereToInsert,BaseType *_uselessAlwaysNullSetToOverideMethod )
{
	// try to add new cell:
	BaseType *_pObjectToAdd = m_CreatorFunc();
	if( !_pObjectToAdd ) return(0L);  

	BeginChangeNotification();

		Cell *pCell = PackStruct::AddElement(_indexWhereToInsert,_pObjectToAdd);
		if( !pCell ){ 
			delete _pObjectToAdd; 
			EndChangeNotification();
			return(0L);
		}

	EndChangeNotification();

	return( pCell );
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief delete an element at a given index. It close and destroy the object.
	\param	_index index of the object to destroy.
*/
void PackList::DeleteElement( unsigned int _index)
{
	// note: we notify a change for the whole list object
	// and then, the deletion of one member:
	BeginChangeNotification();
		BaseType *pOb = PackStruct::Get(_index);
		if(pOb  ) delete pOb;
		PackStruct::DeleteElement(_index);
	EndChangeNotification();
	// send special eVedaUpdate_MemberDestroyed message:
	if( GetObjectThatManagesThis() && GetObjectThatManagesThis()->GetContext() )
		GetObjectThatManagesThis()->GetContext()->
			NotifyObjectChange( BaseContext::eVedaUpdate_MemberDestroyed,GetObjectThatManagesThis() , this );

}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief delete an element. It closes and destroys the object.
	\param _pObWhichCellIsToDestroy ob WhichCellIsToDestroy.
*/
void PackList::DeleteElement( BaseType *_pObWhichCellIsToDestroy)
{
	register Cell *pPrevCell,*pAfterCell,*pCell = m_pFirstCell ;

	while( pCell ) 
	{
		if( pCell->GetManagedObject() == _pObWhichCellIsToDestroy )
		{	
			BeginChangeNotification();
					// pCell to destroy
					// unlink:
					pPrevCell = pCell->GetPreviousBrotherCell();
					pAfterCell = pCell->GetNextBrotherCell();

					if( m_pLastCell == pCell) m_pLastCell = pPrevCell;
					if( m_pFirstCell == pCell) m_pFirstCell = pAfterCell;

					if( pPrevCell ) pPrevCell->SetNextBrotherCell(pAfterCell);
					if( pAfterCell ) pAfterCell->SetPreviousBrotherCell(pPrevCell);
					delete pCell;

					// destroy:
					delete _pObWhichCellIsToDestroy;
					m_NumberOfCell--;
			EndChangeNotification();
			// send special eVedaUpdate_MemberDestroyed message:
			if( GetObjectThatManagesThis() && GetObjectThatManagesThis()->GetContext() )
				GetObjectThatManagesThis()->GetContext()->
					NotifyObjectChange( BaseContext::eVedaUpdate_MemberDestroyed,GetObjectThatManagesThis() , this );

			// cell deleted:
			return;
		}
		//old pPrevCell = pCell;
		pCell = pCell->GetNextBrotherCell();
	};
	// if here, failed.	

}
#endif

/*!
	\brief	Read the object description from a byte chunk. Could crash if chunk not valid.
	\param	_pDescriptionChunk the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
const unsigned char * PackList::Serialize_In( const unsigned char * _pDescriptionChunk)
{
#ifdef _ENGINE_EDITABLE_
	// delete all serializable members if there were any:
	// note, in non editable mode, PackStruct method is used without list deletion:	
		DeleteAllElements();
#endif
	// find the end of the list chunk:
	register const unsigned char	*pEndOfChunk = _pDescriptionChunk ;
	register Cell	*pCell;
	//  serialize the chunk length attribute:
	_pDescriptionChunk = mSerUlong_ChunkLength.Serialize_In(_pDescriptionChunk);
	// the end of the chunk is:
	pEndOfChunk += mSerUlong_ChunkLength.Get() ;

	// serialize all:
	while( (_pDescriptionChunk < pEndOfChunk) )
	{
		// EQUIVALENT TO ADDELEMENT() without notify events:
		// try to add new cell:
		pCell = 0L;
		BaseType *_pObjectToAdd = m_CreatorFunc();
		if( _pObjectToAdd ) 
		{
			pCell = PackStruct::AddElement(-1,_pObjectToAdd);
			if( !pCell ){ delete _pObjectToAdd; }
		}
		if( pCell )
			_pDescriptionChunk =_pObjectToAdd->Serialize_In( _pDescriptionChunk );
		else
			_pDescriptionChunk = pEndOfChunk; // no memory case.
	}	

	return( pEndOfChunk );

}
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Acts like Serialize_In() but keep all previous objects.
			So you can load and mix 2 or more context this way.
	\param	_pDescriptionChunk the binary image of a context.
	\param _pendName specify a name to add at the end of each new object.
*/
const unsigned char * PackList::MergeSerialize_In( const unsigned char * _pDescriptionChunk,const char *_pendName)
{
	// the difference is that we don't destroy previous elements before adding.
	// find the end of the list chunk:
	register const unsigned char	*pEndOfChunk = _pDescriptionChunk ;
	register Cell	*pCell;
	//  serialize the chunk length attribute:
	_pDescriptionChunk = mSerUlong_ChunkLength.Serialize_In(_pDescriptionChunk);
	// the end of the chunk is:
	pEndOfChunk += mSerUlong_ChunkLength.Get() ;

	// serialize all:
	while( (_pDescriptionChunk < pEndOfChunk) )
	{
		pCell =  AddElement(); // occur a NEW object.
		if( pCell )
			_pDescriptionChunk = pCell->GetManagedObject()->Serialize_In( _pDescriptionChunk );

		pCell = pCell->GetNextBrotherCell();
	}	

	return( _pDescriptionChunk );
}
#endif

#ifdef _ENGINE_EDITABLE_
/*!
	\brief	get the size of the whole byte chunk that will be written by Serialize_Out().
	\return	byte size of the serialisation to do.
*/
unsigned int PackList::GetSerializedDescriptionSize(void)
{
	// size of mSerUlong_JumpToNextChunk if it were set to zero:
	register unsigned int deltaJumpLength; //mSerUlong_ChunkLength.GetSerializedDescriptionSize();
	register unsigned int length=1; //mSerUlong_ChunkLength.GetSerializedDescriptionSize();
	register Cell	*pCell = m_pFirstCell ;
	// add size of the objects in the list:
	while( pCell )
	{
		length += pCell->GetManagedObject()->GetSerializedDescriptionSize();
		pCell = pCell->GetNextBrotherCell();
	}
	// got to reset the chunk length member:
	//---------- we got to check that mSerUlong_JumpToNextChunk don't grow with the right size !
	mSerUlong_ChunkLength.Set( length );
	deltaJumpLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() - 1 ; //1= previous size that might changed.
	if( deltaJumpLength > 0 )
	{
		length += deltaJumpLength;
		//---------- we got to check that twice:
		deltaJumpLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() ;
		mSerUlong_ChunkLength.Set( length );
		deltaJumpLength = mSerUlong_ChunkLength.GetSerializedDescriptionSize() - deltaJumpLength ;
		if( deltaJumpLength > 0 )
		{
			length += deltaJumpLength;
			mSerUlong_ChunkLength.Set( length );
		}
	}
	return( length );
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	write the Current object definition to a Chunk using private packed types, recursively.
	\param	_pDescriptionChunkToFill the chunk where to write the objet description chunk. 
	\return the end of the chunk written, possibly unlocated. Don't use this if you don't need it.
*/
unsigned char * PackList::Serialize_Out(unsigned char * _pDescriptionChunkToFill)
{
	register Cell	*pCell = m_pFirstCell ;
	_pDescriptionChunkToFill = mSerUlong_ChunkLength.Serialize_Out( _pDescriptionChunkToFill );
	while( pCell )
	{
		_pDescriptionChunkToFill = pCell->GetManagedObject()->Serialize_Out( _pDescriptionChunkToFill );
		pCell = pCell->GetNextBrotherCell();
	}
	return(_pDescriptionChunkToFill);
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Edition tool: Exchange 2 neighbour elements in the list, if there is a next element.
			if index is 0, 0 will be 1 and 1 will be 0.
	\param	_indexOfElementToSwapWithNext any row, from zero to GetNumberOfCell()-2
*/
void	PackList::SwapElements( unsigned int _indexOfElementToSwapWithNext)
{
	if(GetNumberOfCell()<2) return;
	if(_indexOfElementToSwapWithNext>(GetNumberOfCell()-2)) return;
	Cell *pCell1 = m_pFirstCell;
	while(_indexOfElementToSwapWithNext)
	{
		pCell1=pCell1->GetNextBrotherCell();
		_indexOfElementToSwapWithNext--;
	}
 BeginChangeNotification();

	// this is now, very touchy,look !
	Cell *pCell2 =pCell1->GetNextBrotherCell();
	Cell *pCell2Next =pCell2->GetNextBrotherCell();
	Cell *pCell1Prev =pCell1->GetPreviousBrotherCell();

	pCell1->SetPreviousBrotherCell(pCell2); 
	pCell1->SetNextBrotherCell(pCell2Next);
	pCell2->SetNextBrotherCell(pCell1); 
	pCell2->SetPreviousBrotherCell(pCell1Prev);
	if(pCell2Next) pCell2Next->SetPreviousBrotherCell(pCell1);
	else m_pLastCell = pCell1;
	if(pCell1Prev) pCell1Prev->SetNextBrotherCell(pCell2);
	else m_pFirstCell =  pCell2;
 EndChangeNotification();
	// send special eVedaUpdate_MemberDestroyed message:
	if( GetObjectThatManagesThis() && GetObjectThatManagesThis()->GetContext() )
		GetObjectThatManagesThis()->GetContext()->
			NotifyObjectChange( BaseContext::eVedaUpdate_MemberDestroyed,GetObjectThatManagesThis() , this );
	
}
#endif
#ifdef _ENGINE_EDITABLE_
/*!
	\brief	Insert a new element after _indexOfElementToClone,
		and clone it with the previous element shape. Note this
		can exclude some data members, like Object reference.
	\param	_indexOfElementToClone any row, from zero to GetNumberOfCell()-1
*/
void	PackList::CloneElement( unsigned int _indexOfElementToClone)
{
	// almost same as add:
	// try to add new cell:
	BaseType *_pObjectToAdd = m_CreatorFunc();
	if( !_pObjectToAdd ) return;  

	BeginChangeNotification();

		Cell *pCell = PackStruct::AddElement(_indexOfElementToClone,_pObjectToAdd);
		if( !pCell ){ 
			delete _pObjectToAdd; 
			EndChangeNotification();
			return;
		}
		Cell *pSourceCell =pCell->GetNextBrotherCell();
		if(pSourceCell) // should be always true.
		{
			BaseType *pSourceObj = (BaseType *)(pSourceCell->GetManagedObject());
			unsigned int serlength = pSourceObj->GetSerializedDescriptionSize();
			unsigned char *pShape = new unsigned char[serlength];
			if(pShape)
			{
				BaseType *pNewObj = (BaseType *)(pCell->GetManagedObject());

				pSourceObj->Serialize_Out(pShape);
				pNewObj->Serialize_In(pShape);
				delete [] pShape;
			}

		}
	EndChangeNotification();
	

}
#endif