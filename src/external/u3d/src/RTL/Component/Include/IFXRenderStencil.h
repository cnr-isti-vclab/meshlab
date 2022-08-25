//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
// IFXRenderStencil.h
// DO NOT INCLUDE THIS DIRECTLY!!!!!
// This is included with IFXRender.h
#ifndef IFX_RENDER_STENCIL_H
#define IFX_RENDER_STENCIL_H

//==============================
// IFXRenderStencil
//==============================
/**
 *	This interface encapsulates all settings relating to the stencil buffer.  It is 
 *	used to pass stencil settings into an IFXRender object using IFXRenderContext::SetStencil().
 *	Usage of the stencil buffer can be enabled with IFXRenderContext::Enable(IFX_STENCIL).  For
 *	this to work, you must have a stencil buffer allocated.
 *
 *	@see IFXRenderContext::SetStencil(), IFXRenderWindow
 */
class IFXRenderStencil
{
public:
	//=============================================
	// IFXRenderStencil Constructor/Destructor
	//=============================================
	IFXRenderStencil();
	virtual ~IFXRenderStencil();

	//================================
	// IFXRenderStencil methods
	//================================
	/**
	 *	Sets the operation to take place when the stencil test fails.
	 *	Some video cards and APIs don't support IFX_INCR or IFX_DECR.  In this case, 
	 *	they will use IFX_INCRSAT and IFX_DECRSAT.
	 *
	 *	@param eOp	Input IFXenum specifying the stencil test fail operation.  Valid
	 *				values are:
	 *				- @b IFX_KEEP: Do nothing to the stencil buffer.
	 *				- @b IFX_REPLACE: Replace the stored stencil value with the reference value.
	 *				- @b IFX_INCR: Increment the stencil buffer.  This will wrap to 0 on overflow.
	 *				- @b IFX_DECR: Decrement the stencil buffer.  This will wrap to Max (255) on overflow.
	 *				- @b IFX_INCRSAT: Increment the stencil buffer.  This will not not wrap.
	 *				- @b IFX_DECRSAT: Decrement the stencil buffer.  This will not wrap.
	 *				- @b IFX_INVERT: Do a binary inversion (1->0, 0->1) on the stencil buffer.
	 *	
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The specified operation is invalid.
	 */
	IFXRESULT IFXAPI SetFailureOp(IFXenum eOp);

	/**
	 *	Sets the operation to take place when the stencil test passes, but the depth test fails.
	 *	Some video cards and APIs don't support IFX_INCR or IFX_DECR.  In this case, 
	 *	they will use IFX_INCRSAT and IFX_DECRSAT.
	 *
	 *	@param eOp	Input IFXenum specifying the depth test fail operation.  Valid
	 *				values are:
	 *				- @b IFX_KEEP: Do nothing to the stencil buffer.
	 *				- @b IFX_REPLACE: Replace the stored stencil value with the reference value.
	 *				- @b IFX_INCR: Increment the stencil buffer.  This will wrap to 0 on overflow.
	 *				- @b IFX_DECR: Decrement the stencil buffer.  This will wrap to Max (255) on overflow.
	 *				- @b IFX_INCRSAT: Increment the stencil buffer.  This will not not wrap.
	 *				- @b IFX_DECRSAT: Decrement the stencil buffer.  This will not wrap.
	 *				- @b IFX_INVERT: Do a binary inversion (1->0, 0->1) on the stencil buffer.
	 *	
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_RANGE	The specified operation is invalid.
	 */
	IFXRESULT IFXAPI SetZFailureOp(IFXenum eOp);

	/**
	 *	Sets the operation to take place when the stencil test passes.
	 *	Some video cards and APIs don't support IFX_INCR or IFX_DECR.  In this case, 
	 *	they will use IFX_INCRSAT and IFX_DECRSAT.
	 *
	 *	@param eOp	Input IFXenum specifying the stencil test pass operation.  Valid
	 *				values are:
	 *				- @b IFX_KEEP: Do nothing to the stencil buffer.
	 *				- @b IFX_REPLACE: Replace the stored stencil value with the reference value.
	 *				- @b IFX_INCR: Increment the stencil buffer.  This will wrap to 0 on overflow.
	 *				- @b IFX_DECR: Decrement the stencil buffer.  This will wrap to Max (255) on overflow.
	 *				- @b IFX_INCRSAT: Increment the stencil buffer.  This will not not wrap.
	 *				- @b IFX_DECRSAT: Decrement the stencil buffer.  This will not wrap.
	 *				- @b IFX_INVERT: Do a binary inversion (1->0, 0->1) on the stencil buffer.
	 *	
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_RANGE	The specified operation is invalid.
	 */
	IFXRESULT IFXAPI SetPassOp(IFXenum eOp);

	/**
	 *	Sets the comparison function used for the stencil test.  The comparison takes place
	 *	between the current value of the stencil buffer and the reference value.
	 *
	 *	@param eFunc	Input IFXenum specifying the stencil test comparison function.  
	 *					Valid values are:
	 *					- @b  IFX_NEVER:  The test never passes.
	 *					- @b  IFX_LESS:  Passes when the reference value is less than the stencil value.
	 *					- @b  IFX_GREATER: Passes when the reference value is greater than the stencil value.
	 *					- @b  IFX_EQUAL:  Passes when the reference value is equal to the stencil value.
	 *					- @b  IFX_NOT_EQUAL:  Passes when the reference value is not equal to the stencil value.
	 *					- @b  IFX_LEQUAL:  Passes when the reference value is less than or equal to the stencil value.
	 *					- @b  IFX_GEQUAL:  Passes when the reference value is greater than or equal to the stencil value.
	 *					- @b  IFX_ALWAYS:  The test always passes.
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_RANGE	The specified operation is invalid.
	 */
	IFXRESULT IFXAPI SetStencilFunc(IFXenum eFunc);
	
	/**
	 *	Sets the testing mask that is used for the stencil test comparison.  This mask is applied
	 *	(binary &'ed with) both the reference value and the stencil value before the stencil
	 *	comparison function is performed.
	 *
	 *	@param uMask	Input U32 specifying the stencil test mask.  This value is clamped to
	 *					the maximum stencil value (probably 0xff, or 255).
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetTestMask(U32 uMask);
	
	/**
	 *	Sets the write mask for the stencil buffer.  This mask is a binary bit by bit write 
	 *	enable mask for the stencil buffer.  Only those stencil bits whose mask bit is 1 will
	 *	be written to.  A @a uMask of 0x0 will disable all stencil updates.
	 *
	 *	@param uMask	Input U32 specifying the stencil buffer write mask.  This value is clamped
	 *					to the maximum stencil value (probably 0xff, or 255).
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetWriteMask(U32 uMask);

	/**
	 *	Sets the stencil reference value for use in the stencil test comparison function.
	 *
	 *	@param uRef	Input U32 specifying the stencil reference value.  This value is clamped
	 *				to the maximum stencil value (probably 0xff, or 255).
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetReference(U32 uRef);
		
	/**
	 *	Gets the stencil test fail operation.
	 *
	 *	@return The stencil test fail operation.
	 *	
	 *	@retval	IFX_KEEP	Do nothing to the stencil buffer.
	 *	@retval	IFX_REPLACE	Replace the stored stencil value with the reference value.
	 *	@retval	IFX_INCR	Increment the stencil buffer.  This will wrap to 0 on overflow.
	 *	@retval	IFX_DECR	Decrement the stencil buffer.  This will wrap to Max (255) on overflow.
	 *	@retval	IFX_INCRSAT	Increment the stencil buffer.  This will not not wrap.
	 *	@retval	IFX_DECRSAT	Decrement the stencil buffer.  This will not wrap.
	 *	@retval	IFX_INVERT	Do a binary inversion (1->0, 0->1) on the stencil buffer.
	 *
	 *	@see SetFailureOp().
	 */
	IFXenum IFXAPI GetFailureOp() const;
	
	/**
	 *	Gets the depth test fail operation.
	 *
	 *	@return The depth test fail operation.
	 *	
	 *	@retval	IFX_KEEP	Do nothing to the stencil buffer.
	 *	@retval	IFX_REPLACE	Replace the stored stencil value with the reference value.
	 *	@retval	IFX_INCR	Increment the stencil buffer.  This will wrap to 0 on overflow.
	 *	@retval	IFX_DECR	Decrement the stencil buffer.  This will wrap to Max (255) on overflow.
	 *	@retval	IFX_INCRSAT	Increment the stencil buffer.  This will not not wrap.
	 *	@retval	IFX_DECRSAT	Decrement the stencil buffer.  This will not wrap.
	 *	@retval	IFX_INVERT	Do a binary inversion (1->0, 0->1) on the stencil buffer.
	 *
	 *	@see SetZFailureOp()
	 */
	IFXenum IFXAPI GetZFailureOp() const;
	
	/**
	 *	Gets the stencil test pass operation.
	 *
	 *	@return The stencil test pass operation.
	 *	
	 *	@retval	IFX_KEEP	Do nothing to the stencil buffer.
	 *	@retval	IFX_REPLACE	Replace the stored stencil value with the reference value.
	 *	@retval	IFX_INCR	Increment the stencil buffer.  This will wrap to 0 on overflow.
	 *	@retval	IFX_DECR	Decrement the stencil buffer.  This will wrap to Max (255) on overflow.
	 *	@retval	IFX_INCRSAT	Increment the stencil buffer.  This will not not wrap.
	 *	@retval	IFX_DECRSAT	Decrement the stencil buffer.  This will not wrap.
	 *	@retval	IFX_INVERT	Do a binary inversion (1->0, 0->1) on the stencil buffer.
	 *
	 *	@see SetPassOp()
	 */
	IFXenum IFXAPI GetPassOp() const;
	
	/**
	 *	Gets the stencil test comparison function.
	 *
	 *	@return The stencil test comparison function.
	 *
	 *	@retval	IFX_NEVER	The test never passes.
	 *	@retval	IFX_LESS	Passes when the reference value is less than the stencil value.
	 *	@retval	IFX_GREATER	Passes when the reference value is greater than the stencil value.
	 *	@retval	IFX_EQUAL	Passes when the reference value is equal to the stencil value.
	 *	@retval	IFX_NOT_EQUAL	Passes when the reference value is not equal to the stencil value.
	 *	@retval	IFX_LEQUAL	Passes when the reference value is less than or equal to the stencil value.
	 *	@retval	IFX_GEQUAL	Passes when the reference value is greater than or equal to the stencil value.
	 *	@retval	IFX_ALWAYS	The test always passes.
	 *
	 *	@see SetStencilFunc()
	 */
	IFXenum IFXAPI GetStencilFunc() const;
	
	/**
	 *	Gets the stencil test mask.
	 *
	 *	@return The stencil test mask.
	 *	@see SetTestMask()
	 */
	U32 IFXAPI GetTestMask() const;
	
	/**
	 *	Gets the stencil buffer write mask.
	 *
	 *	@return The stencil buffer write mask.
	 *	@see SetTestMask()
	 */
	U32 IFXAPI GetWriteMask() const;

	/**
	 *	Gets the stencil test reference value.
	 *
	 *	@return The stencil test reference value.
	 *	@see SetReference()
	 */
	U32 IFXAPI GetReference() const;

	/**
	 *	Initialize all data to default values.  Default values are:
	 *
	 *	- <b>Failure Op</b>: IFX_KEEP
	 *	- <b>ZFailure Op</b>: IFX_KEEP
	 *	- <b>Pass Op</b>: IFX_KEEP
	 *	- <b>Stencil Func</b>: IFX_ALWAYS
	 *	- <b>Stencil Reference Value</b>: 0
	 *	- <b>Stencil Test Mask</b>: 0xff
	 *	- <b>Stencil Write Mask</b>: 0xff
	 */
	void IFXAPI InitData();

protected:
	IFXenum	m_eFailOp;
	IFXenum	m_eZFailOp;
	IFXenum	m_ePassOp;
	IFXenum	m_eFunc;
	U32		m_uTestMask;
	U32		m_uWriteMask;
	U32		m_uRef;	
};

//=======================================
// IFXRenderStencil Implementation
//=======================================
IFXINLINE IFXRenderStencil::IFXRenderStencil()
{
	InitData();
}

IFXINLINE IFXRenderStencil::~IFXRenderStencil()
{
	// EMPTY
}

IFXINLINE void IFXRenderStencil::InitData()
{
	m_eFailOp = IFX_KEEP;
	m_eZFailOp = IFX_KEEP;
	m_ePassOp = IFX_KEEP;
	m_eFunc = IFX_ALWAYS;
	m_uRef = 0;
	m_uTestMask = 0xffffffff;
	m_uWriteMask = 0xffffffff;
}

IFXINLINE IFXRESULT IFXRenderStencil::SetFailureOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	if(m_eFailOp != eOp)
	{
		switch(eOp)
		{
		case IFX_KEEP:
		case IFX_ZERO:
		case IFX_REPLACE:
		case IFX_INCR:
		case IFX_DECR:
		case IFX_INVERT:
		case IFX_STENCIL:
			m_eFailOp = eOp;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderStencil::SetZFailureOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	if(m_eZFailOp != eOp)
	{
		switch(eOp)
		{
		case IFX_KEEP:
		case IFX_ZERO:
		case IFX_REPLACE:
		case IFX_INCR:
		case IFX_DECR:
		case IFX_INVERT:
		case IFX_STENCIL:
			m_eZFailOp = eOp;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderStencil::SetPassOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	if(m_ePassOp != eOp)
	{
		switch(eOp)
		{
		case IFX_KEEP:
		case IFX_ZERO:
		case IFX_REPLACE:
		case IFX_INCR:
		case IFX_DECR:
		case IFX_INVERT:
		case IFX_STENCIL:
			m_ePassOp = eOp;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderStencil::SetStencilFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	if(m_eFunc != eFunc)
	{
		switch(eFunc)
		{
		case IFX_NEVER:
		case IFX_LESS:
		case IFX_GREATER:
		case IFX_EQUAL:
		case IFX_NOT_EQUAL:
		case IFX_LEQUAL:
		case IFX_GEQUAL:
		case IFX_ALWAYS:
			m_eFunc = eFunc;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderStencil::SetTestMask(U32 uMask)
{
	IFXRESULT rc = IFX_OK;

	m_uTestMask = uMask;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderStencil::SetWriteMask(U32 uMask)
{
	IFXRESULT rc = IFX_OK;

	m_uWriteMask = uMask;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderStencil::SetReference(U32 uRef)
{
	IFXRESULT rc = IFX_OK;

	m_uRef = uRef;
	
	return rc;
}


IFXINLINE IFXenum IFXRenderStencil::GetFailureOp() const
{
	return m_eFailOp;
}

IFXINLINE IFXenum IFXRenderStencil::GetZFailureOp() const
{
	return m_eZFailOp;
}

IFXINLINE IFXenum IFXRenderStencil::GetPassOp() const
{
	return m_ePassOp;
}

IFXINLINE IFXenum IFXRenderStencil::GetStencilFunc() const
{
	return m_eFunc;
}

IFXINLINE U32 IFXRenderStencil::GetTestMask() const
{
	return m_uTestMask;
}

IFXINLINE U32 IFXRenderStencil::GetWriteMask() const
{
	return m_uWriteMask;
}

IFXINLINE U32 IFXRenderStencil::GetReference() const
{
	return m_uRef;
}



#endif // IFX_RENDER_STENCIL_H

// END OF FILE

