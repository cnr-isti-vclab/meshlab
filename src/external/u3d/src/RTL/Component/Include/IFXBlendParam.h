
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

#ifndef __IFXBlendParam_h__
#define __IFXBlendParam_h__



/**************************************************************************//**
	\brief Configuration for a motion blend

	StopLast:		true  = last motion stops its current time at start of blend
					false = last motion continues during blend

	Incremental:	true  = continuously uses last blend in place of
							computed last motion
							(alters linearity and makes StopLast irrelevant)
					false = actual last motion is used directly

	StartTime:		where in next motion the current time of last motion
					is started, basically where you plan to set the time
					for the first new frame

	Realignment:	an offset into the last motion where the next motion's
					StartTime aligns better.
					(time scaling and/or delay used to compensate)

	BlendTime:		how long from StartTime the blend transitions
					from last motion to next motion

	Exponent:		adjusts the linearity of the weighting over time
					weight_effective = weight ^ Exponent
*//***************************************************************************/
/**This class configures a motion blend.*/
class IFXBlendParam
	{
	public:
				IFXBlendParam(void)
					{
					m_stoplast=true;
					m_incremental=true;
					m_starttime=0.0f;
					m_realignment=0.0f;
					m_blendtime=0.5f;
					m_exponent=1.0f;
					};

		void	operator=(const IFXBlendParam &other)
					{
					m_stoplast=other.m_stoplast;
					m_incremental=other.m_incremental;
					m_starttime=other.m_starttime;
					m_realignment=other.m_realignment;
					m_blendtime=other.m_blendtime;
					m_exponent=other.m_exponent;
					};
		/**Determines whether last motion stops or continues. If true, last motion stops 
		its current time at start of blend;	if false, last motion continues during blend.*/
		bool	GetStopLastConst(void) const	{ return m_stoplast; };
		/**Sets whether last motions stops or continues. If true, last motion stops its 
		current time at start of blend;if false, last motion continues during blend.*/
		void	SetStopLast(bool set)			{ m_stoplast=set; };
		/**Determines whether to use last blend or last motion. If true, continuously uses 
		last blend in place of computed last motion(alters linearity and makes StopLast 
		irrelevant); if false, actual last motion is used directly.*/
		bool	GetIncrementalConst(void) const	{ return m_incremental; };
		/**Sets whether to use last blend or last motion. If true, continuously uses last blend in place of computed last motion
		(alters linearity and makes StopLast irrelevant); if false, actual last 
		motion is used directly.*/
		void	SetIncremental(bool set)		{ m_incremental=set; };
		/**Gets start time where you plan to set the time for the first new frame.*/
		F32		GetStartTimeConst(void) const	{ return m_starttime; };
		/**Sets start time where you plan to set the time for the first new frame.*/
		void	SetStartTime(F32 set)			{ m_starttime=set; };
		/**Gets an offset into the last motion where the next motion's StartTime 
		aligns better.*/
		F32		GetRealignmentConst(void) const	{ return m_realignment; };
		/**Sets an offset into the last motion where the next motion's StartTime 
		aligns better.*/
		void	SetRealignment(F32 set)			{ m_realignment=set; };
		/**Gets how long from StartTime the blend transitions from last motion to 
		next motion.*/
		F32		GetBlendTimeConst(void) const	{ return m_blendtime; };
		/**Sets how long from StartTime the blend transitions from last motion to 
		next motion.*/
		void	SetBlendTime(F32 set)			{ m_blendtime=set; };
		/**Gets the adjustment amount of the linearity ofthe weighting over time.*/
		F32		GetExponentConst(void) const	{ return m_exponent; };
		/**Sets the adjustment amount of the linearity ofthe weighting over time.*/
		void	SetExponent(F32 set)			{ m_exponent=set; };

	private:
		bool	m_stoplast;
		bool	m_incremental;
		F32		m_starttime;
		F32		m_realignment;
		F32		m_blendtime;
		F32		m_exponent;
	};

#endif /* __IFXBlendParam_h__ */

