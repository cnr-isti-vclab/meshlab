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
	@file IFXKeyTrack.h
*/

#ifndef IFXKEYTRACK_H
#define IFXKEYTRACK_H

#include "IFXList.h"
#include "IFXKeyFrame.h"
#include "IFXVoidStar.h"

/**
	A list of keyframes that represent a single track of 3D motion.
*/
class IFXKeyTrack : public IFXList<IFXKeyFrame>
	{
	public:
						IFXKeyTrack(void)
						{
							SetName("Track");
							ToHead(m_current);
							SetAutoDestruct(true);
						}

						/// Standard deep copy.
		IFXKeyTrack		&operator=(const IFXKeyTrack &other)
						{
							IFXList<IFXKeyFrame>::operator=(other);
							return *this;
						}

						/** Creates and appends a new keyframe.  The given
							variant is set to point to the new object. */
virtual	void		 IFXAPI 	AllocElement(IFXVariant *get)
						{
							IFXKeyFrame *entry=new IFXKeyFrame();
							Append(entry);
							*get=entry;
						}

						/** The given instant is set to an interpolated
							value along the track of data elements at the
							time specified.  An optional reusable
							context can be provided to retain state that
							should speed up searching the list for multiple
							requests with small variations in time. */
		void			CalcInstantConst(float time,IFXInstant *instant,
									IFXListContext *context=NULL) const;

						/** Creates and inserts a new keyframe at the given
							time and value.  If the optional context is
							provided, multiple nearby insertions should
							be faster. */
		void			InsertNewKeyFrame(float time,const IFXInstant &instant,
											IFXListContext *context=NULL);

						/// Returns the name of overall track.
		IFXString		GetName(void)				{ return m_name; };
						/// Returns the name of overall track (const).
const	IFXString		&GetNameConst(void) const	{ return m_name; };
						/// Sets the name of overall track.
		void			SetName(IFXString set)		{ m_name=set; };

						/** Removes all keyframes that differ in time by less
							than the given value.  If a long stream of
							keyframes contains very small differences in time,
							enough keyframes are retained to maintain
							differences just over the given delta time. */
		void			Filter(F32 deltatime);
						/** Removes all keyframes that can be accurately
							interpolated by using the adjacent keyframes
							in the list.  The arguments specify the allowable
							error. */
		void			Compress(F32 deltaposition,F32 deltarotation,
														F32	deltascale=0.01f);

	private:

						/** (internal) Aligns the context between the nodes
							just above and below the given time. */
		void			Sync(float time,IFXListContext *context) const;

		IFXString		m_name;
		IFXListContext	m_current;
	};

#endif
