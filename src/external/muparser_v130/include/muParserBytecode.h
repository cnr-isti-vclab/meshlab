/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2004-2008 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#ifndef MU_PARSER_BYTECODE_H
#define MU_PARSER_BYTECODE_H

#include <cassert>
#include <string>
#include <stack>
#include <vector>

#include "muParserDef.h"
#include "muParserError.h"
#include "muParserToken.h"

/** \file
    \brief Definition of the parser bytecode class.
*/


namespace mu
{


/** \brief Bytecode implementation of the Math Parser.

  The bytecode contains the formula converted to revers polish notation stored in a continious
  memory area. Associated with this data are operator codes, variable pointers, constant 
  values and function pointers. Those are necessary in order to calculate the result.
  All those data items will be casted to the underlying datatype of the bytecode.

  \author (C) 2004, 2005 Ingo Berg 
*/
class ParserByteCode
{
public:
    /** \brief Underlying type of the container.

       The bytecode is a vector of this type containing control codes,
       values and pointers. Values and pointer will be casted to this 
       type before their storage.
    */
    typedef bytecode_type map_type;

private:

    /** \brief Token type for internal use only. */
    typedef ParserToken<value_type, string_type> token_type;

    /** \brief Core type of the bytecode. */
    typedef std::vector<map_type> storage_type;

    /** \brief Position in the Calculation array. */
    unsigned m_iStackPos;

    /** \brief Core type of the bytecode. */
    storage_type m_vBase;

    /** \brief Size of a value entry in the bytecode, relative to TMapType size. */
    const int mc_iSizeVal;

    /** \brief Size of a pointer, relative to size of underlying TMapType.
       
        \attention The size is related to the size of TMapType not bytes!
    */
    const int mc_iSizePtr;

    /** \brief A value entry requires that much entires in the bytecode. 
        
        Value entry consists of:
        <ul>
          <li>One entry for Stack index</li>
          <li>One entry for Token identifier</li>
          <li>mc_iSizeVal entries for the value</li>
        <ul>

        \sa AddVal(TBaseData a_fVal)
    */
    const int mc_iSizeValEntry;

    void StorePtr(void *a_pAddr);

public:
    ParserByteCode();
   ~ParserByteCode();
    ParserByteCode(const ParserByteCode &a_ByteCode);
    ParserByteCode& operator=(const ParserByteCode &a_ByteCode);
    void Assign(const ParserByteCode &a_ByteCode);

    void AddVar(value_type *a_pVar);
    void AddVal(value_type a_fVal);
    void AddOp(ECmdCode a_Oprt);
    void AddAssignOp(value_type *a_pVar);
    void AddFun(void *a_pFun, int a_iArgc);
    void AddStrFun(void *a_pFun, int a_iArgc, int a_iIdx);

    void Finalize();
    void clear();

    std::size_t GetBufSize() const;

    const map_type* GetRawData() const;

    /** \brief Return size of a value entry. 
    
      That many bytecode entries are necessary to store a value.

      \sa mc_iSizeVal
    */
    unsigned GetValSize() const 
    {
      return mc_iSizeVal;
    }

    /** \brief Return size of a pointer entry. 
    
      That many bytecode entries are necessary to store a pointer.

      \sa mc_iSizePtr
    */
    unsigned GetPtrSize() const 
    {
      return mc_iSizePtr;
    }

    void RemoveValEntries(unsigned a_iNumber);
    void AsciiDump();
};

} // namespace mu

#endif


