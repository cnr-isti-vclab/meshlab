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
	@file IFXTransformStack.h 
*/

#ifndef IFXTRANSFORMSTACK_H
#define IFXTRANSFORMSTACK_H

#include "IFXTransform.h"
#include "IFXMemory.h"

/**
	\brief Stack of IFXTransforms.

	This stack supports the basic stack operations like push and pop.

	Implemented as never shrinking array, only expanding.
	Each expansion doubles the amount of available space.

	This class is not implemented with IFXArray<IFXTransformStack>,
	but probably should be.

	All transforms are owned by the stack and will be deleted with it.

	It is not safe to retain pointers into the stack as the buffer
	may move when expanded.

	@todo: Inherit from existing IFXArray to take care of resizing.
*/

/**This class supports the basic stack operations of IFXTransform.*/
class IFXTransformStack
{
public:
	/** Initializes the stack using the optional argument
	to specify the initial size of the allocated
	space made available.  Additional space is
	automatically allocated when needed. */
	IFXTransformStack(int initial_size=16)
	{
		m_current=0;
		m_filled=0;
		m_allocated=0;
		m_stack=NULL;
		Resize(initial_size);
	};

	~IFXTransformStack(void)
	{
		if(m_stack)
			IFXDeallocate(m_stack);
	};

	/// Returns the transform on the top of the stack.
	IFXTransform  &GetCurrent(void)   { return m_stack[m_current]; };

	/** Returns the transform on the top of the stack
	(const version). */
	IFXTransform  &GetCurrentConst(void) const
	{ return m_stack[m_current]; };

	/** Gets the stack level of the top transform.  This is
	the current number of transforms on the stack. */
	int       GetLevel(void) const  { return m_current; };

	/** Adds another transform to the top of the stack and
	use it as the current transform. */
	void      Push(void);
	/** Removes the top tranform from the stack and use
	the one below it as the current transform. */
	bool      Pop(void);

private:
	IFXTransform  &GetTransform(int level);
	void      Resize(long needed);

	IFXTransform  *m_stack;
	int       m_current;
	int       m_filled;
	int       m_allocated;
};

IFXINLINE void IFXTransformStack::Push(void)
{
	// NOTE: new transform's constructor not run

	m_current++;
	Resize(m_current+1);

	// NOTE: this copies everything that is valid
	// watch out for efficiency here
	m_stack[m_current]=m_stack[m_current-1];
}

IFXINLINE bool IFXTransformStack::Pop(void)
{
	if(m_current<1)
		return false;

	m_current--;
	return true;
}

IFXINLINE IFXTransform &IFXTransformStack::GetTransform(int level)
{
	// if nothing there, return bottom-most
	if(level<0 || level>m_filled-1)
		level=0;

	return m_stack[level];
}

IFXINLINE void IFXTransformStack::Resize(long needed)
{
	if(m_allocated<needed)
	{
		m_allocated*=2;

		if(m_allocated<needed)
			m_allocated=needed;

		m_stack=(IFXTransform *)IFXReallocate(m_stack,
			m_allocated*sizeof(IFXTransform));
	}

	if(m_filled<needed)
		m_filled=needed;
}



#endif /* __IFXTransformStack_h__ */
