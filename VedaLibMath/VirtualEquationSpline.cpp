// SPDX-License-Identifier: LGPL-2.1-only

#include "VirtualEquationSpline.h"
#include "PackFloat.h"
#include <math.h>
BASEOBJECT_DECLARE_VIRTUALCLASS( "Sp", VirtualEquationSpline, VirtualEquation);

VirtualEquationSpline::VirtualEquationSpline() : VirtualEquation()
{
	REGISTER_MEMBER_FLAG( mSer_Flags ,"Flags",0,"Repeat" );
}

void VirtualEquationSpline::Compute( float _OutgoingParameterTable[4] , const float _IncomingParameterTable[4] )
{

	int flag = mSer_Flags.Get();
	float date = _IncomingParameterTable[3];
	PackList_TimeTrack *pList = GetList();

	float					d1,d2,d3,d4;
	SplineElement *pDElt1,*pDElt2,*pDElt3,*pDElt4;
	PackStruct::Cell *p1,*p2,*p3,*p4=0L;

	p1 = pList->GetFirstCell();
	if( !p1 ) return;
	p2 = pList->GetLastCell();

	d1 = ((SplineElement *)p1->GetManagedObject())->GetTimeInSecond();
	d2 = ((SplineElement *)p2->GetManagedObject())->GetTimeInSecond();

	if(date<d1 || date>d2)
	{
		if(flag & splf_Repeat)
		{
			float ll = d2-d1;
			date-=d1;

			int	nb = (int)(date/ll);
			date -= ((float)nb)*ll;
			if( date<0.0f) date+=ll;
			date+=d1;
		} else
		{
			if(date<d1 ) date =d1;
			else	date=d2;
		}
	}

	p2 = pList->GetCellByDate(date);
	if( !p2 ) return;
	pDElt2 = (SplineElement *)p2->GetManagedObject();
	d2 = pDElt2->GetTimeInSecond();

	p1 = p2->GetPreviousBrotherCell();
	if( !p1 ) p1 = p2;
	pDElt1 = (SplineElement *)p1->GetManagedObject();
	d1 = pDElt1->GetTimeInSecond();

	p3 = p2->GetNextBrotherCell();
	if( !p3 ) p3 = p2;

	pDElt3 = (SplineElement *)p3->GetManagedObject();
	d3 = pDElt3->GetTimeInSecond();
	p4 = p3->GetNextBrotherCell();
	if( !p4 ) p4 = p3;
	pDElt4 = (SplineElement *)p4->GetManagedObject();
	d4 = pDElt4->GetTimeInSecond();

	unsigned int nbDim = (unsigned int)pDElt2->mSer_Vector.GetVectorDimension() ;
	float	tt,dt,v1,v2,b,t1,t2;
	float g_2=2.0f,g_3=3.0f;
	tt =  date - d2 ;
	dt = d3-d2;
	for(unsigned int ii=0 ; ii<nbDim ; ii++)
	{
		v1 =	pDElt2->mSer_Vector.Get(ii);
		v2 =	pDElt3->mSer_Vector.Get(ii);

		if( dt != 0.0f )
		{
			t1 =  (   v2 - pDElt1->mSer_Vector.Get(ii) ) /  ( pDElt3->GetTimeInSecond() - pDElt1->GetTimeInSecond() );
			t2 =  (   pDElt4->mSer_Vector.Get(ii) - v1) /  ( pDElt4->GetTimeInSecond() - pDElt2->GetTimeInSecond() );

			b = ( -(dt*t2) - (g_2*t1*dt) + g_3*(v2-v1) ) /(dt*dt) ;
			t2 = (t2 -b*dt*g_2 -t1 ) /( g_3*(dt*dt));
			v2 = tt*tt;
			v1 += (t2*(v2*tt)  + b*v2 + (tt)*t1 );
		}
		_OutgoingParameterTable[ii] += v1;
	}

}
#ifdef _ENGINE_EDITABLE_

void VirtualEquationSpline::ProcessPreview(double _frameDate,VirtualMachine::InternalViewPort *_pPreviewViewPort,const PreviewConfiguration *_pPreviewConfiguration)
{

	ProcessPreview_DrawEquation(_frameDate,_pPreviewViewPort,_pPreviewConfiguration);

	VirtualMachine::InternalObject3DBuffer	*m_pOb;
	ProcessPreview_CreateLine(&m_pOb,2);
	if(!m_pOb) return;

	float xds1= (-_pPreviewViewPort->GetPositionX1() ) /
		(_pPreviewViewPort->GetPositionX2()-_pPreviewViewPort->GetPositionX1())
		-0.5f ;

	float xds2= (1.0f-_pPreviewViewPort->GetPositionX1()) /
		(_pPreviewViewPort->GetPositionX2()-_pPreviewViewPort->GetPositionX1())
		-0.5f ;
	float xwidth = 0.005f /
		(_pPreviewViewPort->GetPositionX2()-_pPreviewViewPort->GetPositionX1()) ;
		float yds1= (-_pPreviewViewPort->GetPositionY1() ) /
			(_pPreviewViewPort->GetPositionY2()-_pPreviewViewPort->GetPositionY1())
			-0.5f ;

		float yds2= (1.0f-_pPreviewViewPort->GetPositionY1()) /
			(_pPreviewViewPort->GetPositionY2()-_pPreviewViewPort->GetPositionY1())
			-0.5f ;
		float ywidth = 0.005f /
			(_pPreviewViewPort->GetPositionY2()-_pPreviewViewPort->GetPositionY1()) ;

	VirtualMachine::InternalVertex *pVert = m_pOb->GetFirstVertex();

	float	cellwidth=0.01f;
	pVert->m_x = -xwidth;
	pVert->m_y = cellwidth;
	pVert->m_z = 0.0f;
	pVert++;
	pVert->m_x = xwidth;
	pVert->m_y = cellwidth;
	pVert->m_z = 0.0f;
	pVert++;
	pVert->m_x = -xwidth;
	pVert->m_y = -cellwidth;
	pVert->m_z = 0.0f;
	pVert++;
	pVert->m_x = xwidth;
	pVert->m_y = -cellwidth;
	pVert->m_z = 0.0f;

	PackList_TimeTrack *pList = GetList();
	PackList::Cell *pCell = pList->GetFirstCell();
	_pPreviewViewPort->Matrix_Push();
	double lastTranslate = 0.0;
	while(pCell)
	{
		SplineElement *pSplineElt = (SplineElement *) pCell->GetManagedObject();
		float time = pSplineElt->GetTimeInSecond();
		if(time>=xds1 && time<xds2)
		{
			VirtualMachine::InternalVertex *pVert = m_pOb->GetFirstVertex();
			float val =pSplineElt->mSer_Vector.Get();
		pVert->m_x = time-xwidth;
		pVert->m_y = -val +ywidth;
		pVert->m_z = 0.0f;
		pVert++;
		pVert->m_x = time+xwidth;
		pVert->m_y = -val +ywidth;
		pVert->m_z = 0.0f;
		pVert++;
		pVert->m_x = time-xwidth;
		pVert->m_y = -val -ywidth;
		pVert->m_z = 0.0f;
		pVert++;
		pVert->m_x = time+xwidth;
		pVert->m_y = -val -ywidth;
		pVert->m_z = 0.0f;

			_pPreviewViewPort->RenderMesh(m_pOb);

		}
		pCell = pCell->GetNextBrotherCell();
	}
	_pPreviewViewPort->Matrix_Pop();
	GetMachine()->DeleteObject3DBuffer(&m_pOb);
}
#endif
