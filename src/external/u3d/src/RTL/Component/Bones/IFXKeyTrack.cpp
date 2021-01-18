//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

/**
	@file IFXKeyTrack.cpp
*/

#include "IFXKeyTrack.h"

#define IFXKT_REPLACE_SAMETIME  TRUE    //* replace near keyframe on insert
#define IFXKT_REPLACE_DELTA     0.01f   //* how near in time to replace

/******************************************************************************
void IFXKeyTrack::CalcInstantConst(F32 time,IFXInstant *instant,
											IFXListContext *context) const

	FUTURE perhaps something better than linear interpolation on locations

******************************************************************************/
void IFXKeyTrack::CalcInstantConst(
								   F32 time,
								   IFXInstant *instant,
								   IFXListContext *context) const
{
	if(context==NULL)
		context=(IFXListContext *)&m_current;

	Sync(time,context);

	IFXKeyFrame *after=GetCurrent(*context);
	IFXKeyFrame *before=PreDecrement(*context);

	PreIncrement(*context); // put back

	if(!before && !after)
	{
		if(GetHead())
		{
			*instant= *GetHead();
			return;
		}
		else
			instant->Reset();
	}
	else if(!before)
	{
		*instant= *after;
		return;
	}
	else if(!after)
	{
		*instant= *before;
		return;
	}
	else
	{
		F32 fraction= (time-before->Time()) /
			(after->Time()-before->Time());

		instant->Location().Interpolate(fraction,
			before->LocationConst(),after->LocationConst());
		instant->Rotation().Interpolate(fraction,
			before->RotationConst(),after->RotationConst());
		instant->Scale().Interpolate(fraction,
			before->ScaleConst(),after->ScaleConst());
	}
}

void IFXKeyTrack::InsertNewKeyFrame(F32 time,const IFXInstant &instant,
									IFXListContext *context)
{
	IFXKeyFrame *newframe=NULL;

	if(context==NULL)
		context=&m_current;

	Sync(time,context);

#if IFXKT_REPLACE_SAMETIME
	IFXListContext  *context2=context;
	IFXKeyFrame *prior=PreDecrement(*context2);
	context2=context;
	IFXKeyFrame *next=PreIncrement(*context2);

	if(prior && (time-prior->Time() < IFXKT_REPLACE_DELTA) )
	{
		IFXTRACE_GENERIC(L"InsertNewKeyFrame(time=%.6G) replacing adjacent time %.6G\n",
			time,prior->Time());
		newframe=prior;
	}
	else if(next && (next->Time()-time < IFXKT_REPLACE_DELTA) )
	{
		IFXTRACE_GENERIC(L"InsertNewKeyFrame(time=%.6G) replacing adjacent time %.6G\n",
			time,next->Time());
		newframe=next;
	}
	else
#endif
		InsertBefore(*context,newframe=new IFXKeyFrame);

	newframe->IFXInstant::operator=(instant);
	newframe->SetTime(time);
}

/**
	move context to node just after time
*/
void IFXKeyTrack::Sync(F32 time, IFXListContext* context) const
{
	IFXKeyFrame *frame=GetCurrent(*context);

	if(!frame)
	{
		if(IsAtTail(*context))
			frame=ToTail(*context);
		else
			frame=ToHead(*context);
	}

	while(frame && frame->Time()>time)
		frame=PreDecrement(*context);

	while(frame && frame->Time()<time)
		frame=PreIncrement(*context);
}

/******************************************************************************
void IFXKeyTrack::Filter(F32 deltatime)

remove track entries too close in time

******************************************************************************/
void IFXKeyTrack::Filter(F32 deltatime)
{
	//I32 original=GetNumberElements();

	IFXListContext basecontext,nextcontext;
	IFXKeyFrame *base,*next;

	ToHead(basecontext);
	while((base=GetCurrent(basecontext)) != NULL)
	{
		nextcontext=basecontext;
		PostIncrement(nextcontext);

		if(IsAtTail(nextcontext))
			break;

		next=GetCurrent(nextcontext);

		if( (next->Time()-base->Time()) < deltatime )
		{
			Delete(next);
		}
		else
		{
			PostIncrement(basecontext);
		}
	}
}

void IFXKeyTrack::Compress(F32 deltaposition,F32 deltarotation,F32 deltascale)
{
	//I32 original=GetNumberElements();

	IFXListContext basecontext,midcontext,leapcontext;
	IFXKeyFrame *base,*mid,*leap;

	ToHead(basecontext);
	while((base=GetCurrent(basecontext)) != NULL)
	{
		midcontext=basecontext;
		PostIncrement(midcontext);
		leapcontext=midcontext;
		PostIncrement(leapcontext);

		if(IsAtTail(leapcontext))
			break;

		mid=GetCurrent(midcontext);
		leap=GetCurrent(leapcontext);

		F32 fraction=(mid->Time()-base->Time())/(leap->Time()-base->Time());

		IFXVector3 intervector;
		intervector.Interpolate(fraction,base->LocationConst(),
			leap->LocationConst());

		IFXQuaternion interquat;
		interquat.Interpolate(fraction,base->RotationConst(),
			leap->RotationConst());
		IFXVector3 interscale;
		interscale.Interpolate(fraction,base->ScaleConst(),
			leap->ScaleConst());



		if(mid->LocationConst().IsApproximately(intervector,deltaposition) &&
			mid->RotationConst().IsApproximately(interquat,deltarotation) &&
			mid->ScaleConst().IsApproximately(interscale,deltascale) )
		{
			//* watch out for spans close to 180 degrees
			F32 radians;
			IFXVector3 axis;
			IFXQuaternion inverse,span;
			inverse.Invert(base->RotationConst());
			span.Multiply(inverse,leap->RotationConst());
			span.ComputeAngleAxis(radians,axis);

			if(radians<170.0*IFXTO_RAD)
				Delete(mid);
			else
				PostIncrement(basecontext);
		}
		else
		{
			PostIncrement(basecontext);
		}
	}
}
