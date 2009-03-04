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

#include "muParserBytecode.h"

#include <cassert>
#include <string>
#include <stack>
#include <vector>
#include <iostream>

#include "muParserDef.h"
#include "muParserError.h"
#include "muParserToken.h"

/** \file
    \brief Implementation of the parser bytecode class.
*/


namespace mu
{
  //---------------------------------------------------------------------------
  /** Bytecode default constructor. */
  ParserByteCode::ParserByteCode()
    :m_iStackPos(0)
    ,m_vBase()
    ,mc_iSizeVal( std::max( (int)sizeof(value_type)  / (int)sizeof(map_type), 1 ) )
    ,mc_iSizePtr( std::max( (int)sizeof(value_type*) / (int)sizeof(map_type), 1 ) )
    ,mc_iSizeValEntry( 2 + mc_iSizeVal)
  {
    m_vBase.reserve(1000);
  }

  //---------------------------------------------------------------------------
  /** \brief Destructor (trivial).*/
  ParserByteCode::~ParserByteCode()
  {}

  //---------------------------------------------------------------------------
  /** \brief Copy constructor. 
    
      Implemented in Terms of Assign(const ParserByteCode &a_ByteCode)
  */
  ParserByteCode::ParserByteCode(const ParserByteCode &a_ByteCode)
    :mc_iSizeVal( sizeof(value_type)/sizeof(map_type) )
    ,mc_iSizePtr( sizeof(value_type*) / sizeof(map_type) )
    ,mc_iSizeValEntry( 2 + mc_iSizeVal)
  {
    Assign(a_ByteCode);
  }

  //---------------------------------------------------------------------------
  /** \brief Assignment operator.
    
      Implemented in Terms of Assign(const ParserByteCode &a_ByteCode)
  */
  ParserByteCode& ParserByteCode::operator=(const ParserByteCode &a_ByteCode)
  {
    Assign(a_ByteCode);
    return *this;
  }

  //---------------------------------------------------------------------------
  /** \brief Store an address in bytecode.
  
      \param a_pAddr Address to be stored.
      \throw nothrow
  */
  void ParserByteCode::StorePtr(void *a_pAddr)
  {
    #if defined(_MSC_VER)
      #pragma warning( disable : 4311 )
    #endif

    // demo code for packing / unpacking pointers into bytecode
//    void *ptr(NULL);
//    double **pVal;
//    double fVal;
//    map_type dbg[2]; 
//    dbg[0] = *( reinterpret_cast<map_type*>(&a_pAddr) ), 
//    dbg[1] = *( reinterpret_cast<map_type*>(&a_pAddr) + 1 );     
// Version 1:
//    *( (map_type*)&ptr+0) = dbg[0];
//    *( (map_type*)&ptr+1) = dbg[1];
// Version 2:
//    memcpy(&ptr, dbg, sizeof(dbg));
// Version 3:    
//    pVal = (double**)dbg;
//    fVal = **(double**)dbg;

    for (int i=0; i<mc_iSizePtr; ++i)
      m_vBase.push_back( *( reinterpret_cast<map_type*>(&a_pAddr) + i ) );

    #if defined(_MSC_VER)
      #pragma warning( default : 4311 )
    #endif
  }

  //---------------------------------------------------------------------------
  /** \brief Copy state of another object to this. 
    
      \throw nowthrow
  */
  void ParserByteCode::Assign(const ParserByteCode &a_ByteCode)
  {
    if (this==&a_ByteCode)    
      return;

    m_iStackPos = a_ByteCode.m_iStackPos;
    m_vBase = a_ByteCode.m_vBase;
  }

  //---------------------------------------------------------------------------
  /** \brief Add a Variable pointer to bytecode. 
      \param a_pVar Pointer to be added.
      \throw nothrow
  */
  void ParserByteCode::AddVar(value_type *a_pVar)
  {
    m_vBase.push_back( ++m_iStackPos );
    m_vBase.push_back( cmVAR );

    StorePtr(a_pVar);

    const int iSize = mc_iSizeVal - mc_iSizePtr;
    assert(iSize>=0);

    // Make sure variable entries have the same size as value entries.
    // (necessary for optimization; fill with zeros)
    for (int i=0; i<iSize; ++i)
      m_vBase.push_back(0);
  }

  //---------------------------------------------------------------------------
  /** \brief Add a Variable pointer to bytecode. 

      Value entries in byte code consist of:
      <ul>
        <li>value array position of the value</li>
        <li>the operator code according to ParserToken::cmVAL</li>
        <li>the value stored in #mc_iSizeVal number of bytecode entries.</li>
      </ul>

      \param a_pVal Value to be added.
      \throw nothrow
  */
  void ParserByteCode::AddVal(value_type a_fVal)
  {
    m_vBase.push_back( ++m_iStackPos );
    m_vBase.push_back( cmVAL );

    for (int i=0; i<mc_iSizeVal; ++i)
      m_vBase.push_back( *(reinterpret_cast<map_type*>(&a_fVal) + i) );
  }

  //---------------------------------------------------------------------------
  /** \brief Add an operator identifier to bytecode. 
    
      Operator entries in byte code consist of:
      <ul>
        <li>value array position of the result</li>
        <li>the operator code according to ParserToken::ECmdCode</li>
      </ul>

      \sa  ParserToken::ECmdCode
  */
  void ParserByteCode::AddOp(ECmdCode a_Oprt)
  {
    m_vBase.push_back(--m_iStackPos);
    m_vBase.push_back(a_Oprt);
  }

  //---------------------------------------------------------------------------
  /** \brief Add an assignement operator
    
      Operator entries in byte code consist of:
      <ul>
        <li>cmASSIGN code</li>
        <li>the pointer of the destination variable</li>
      </ul>

      \sa  ParserToken::ECmdCode
  */
  void ParserByteCode::AddAssignOp(value_type *a_pVar)
  {
    m_vBase.push_back(--m_iStackPos);
    m_vBase.push_back(cmASSIGN);
    StorePtr(a_pVar);
  }

  //---------------------------------------------------------------------------
  /** \brief Add function to bytecode. 

      \param a_iArgc Number of arguments, negative numbers indicate multiarg functions.
      \param a_pFun Pointer to function callback.
  */
  void ParserByteCode::AddFun(void *a_pFun, int a_iArgc)
  {
    if (a_iArgc>=0)
    {
      m_iStackPos = m_iStackPos - a_iArgc + 1; 
    }
    else
    {
      m_iStackPos = m_iStackPos + a_iArgc + 1; 
    }

    m_vBase.push_back(m_iStackPos);
    m_vBase.push_back(cmFUNC);
	  m_vBase.push_back(a_iArgc);

    StorePtr(a_pFun);
  }

  //---------------------------------------------------------------------------
  /** \brief Add Strung function entry to the parser bytecode. 
      \throw nothrow

      A string function entry consists of the stack position of the return value,
      followed by a cmSTRFUNC code, the function pointer and an index into the 
      string buffer maintained by the parser.
  */
  void ParserByteCode::AddStrFun(void *a_pFun, int a_iArgc, int a_iIdx)
  {
    m_iStackPos = m_iStackPos - a_iArgc + 1;
    m_vBase.push_back(m_iStackPos);
    m_vBase.push_back(cmFUNC_STR);
	  m_vBase.push_back(a_iArgc);
    m_vBase.push_back(a_iIdx);

    StorePtr(a_pFun);
  }

  //---------------------------------------------------------------------------
  /** \brief Add end marker to bytecode.
      
      \throw nothrow 
  */
  void ParserByteCode::Finalize()
  {
    m_vBase.push_back(cmEND);	
    m_vBase.push_back(cmEND);	
//    m_vBase.push_back(cmEND);	

    // shrink bytecode vector to fit
    storage_type(m_vBase).swap(m_vBase);
  }

  //---------------------------------------------------------------------------
  /** \brief Get Pointer to bytecode data storage. */
  const ParserByteCode::map_type* ParserByteCode::GetRawData() const
  {
    assert(m_vBase.size());
    return &m_vBase[0];
  }

  //---------------------------------------------------------------------------
  std::size_t ParserByteCode::GetBufSize() const
  {
    return m_vBase.size();
  }

  //---------------------------------------------------------------------------
  /** \brief Delete the bytecode. 
  
      \throw nothrow

      The name of this function is a violation of my own coding guidelines
      but this way it's more in line with the STL functions thus more 
      intuitive.
  */
  void ParserByteCode::clear()
  {
    m_vBase.clear();
    m_iStackPos = 0;
  }

  //---------------------------------------------------------------------------
  /** \brief Remove a value number of entries from the bytecode. 
    
      \attention Currently I don't test if the entries are really value entries.
  */
  void ParserByteCode::RemoveValEntries(unsigned a_iNumber)
  {
    unsigned iSize = a_iNumber * mc_iSizeValEntry;   
    assert( m_vBase.size() >= iSize );
    m_vBase.resize(m_vBase.size()-iSize);

    assert(m_iStackPos >= a_iNumber);
    m_iStackPos -= (a_iNumber);
  }

  //---------------------------------------------------------------------------
  /** \brief Dump bytecode (for debugging only!). */
  void ParserByteCode::AsciiDump()
  {
    if (!m_vBase.size()) 
    {
      std::cout << "No bytecode available\n";
      return;
    }

    std::cout << "Entries:" << (int)m_vBase.size() 
              << " (ValSize:" << mc_iSizeVal 
              << " entries, PtrSize:" << mc_iSizePtr 
              << " entries, MapSize:" << sizeof(map_type) 
              << " byte)\n";
    int i = 0;

    while ( i<(int)m_vBase.size() && m_vBase[i] != cmEND)
    {
      std::cout << "IDX[" << (int)m_vBase[i++] << "]\t";
      switch (m_vBase[i])
      {
        case cmVAL: std::cout << "VAL "; ++i;
                    std::cout << "[" << *( reinterpret_cast<double*>(&m_vBase[i]) ) << "]\n";
                    i += mc_iSizeVal;
                    break;

        case cmVAR: std::cout << "VAR "; ++i;
  	                std::cout << "[ADDR: 0x" << std::hex << *(value_type**)&m_vBase[i] << "]\n"; 
                    i += mc_iSizePtr;

                    // Variable entries have the same size like value entries
                    // the remaining spave must be skipped
                    i+= std::max(mc_iSizeVal - mc_iSizePtr, 0);
                    break;
      			
        case cmFUNC:
                    std::cout << "CALL\t"; ++i;
                    std::cout << "[ARG:" << std::dec << (int)m_vBase[i] << "]"; ++i;
	                  std::cout << "[ADDR: 0x" << std::hex << *(value_type**)&m_vBase[i] << "]\n"; 
                    i += mc_iSizePtr;
                    break;

        case cmFUNC_STR:
                    std::cout << "CALL STRFUNC\t"; ++i;
                    std::cout << "[ARG:" << std::dec << (int)m_vBase[i] << "]"; ++i;
                    std::cout << "[IDX:" << std::dec << (int)m_vBase[i] << "]"; ++i;
                    std::cout << "[ADDR: 0x" << *(value_type**)&m_vBase[i] << "]\n"; 
                    i += mc_iSizePtr;
                    break;

        case cmLT:  std::cout << "LT\n"; ++i; break;
        case cmGT:  std::cout << "GT\n"; ++i; break;
        case cmLE:  std::cout << "LE\n"; ++i; break;
        case cmGE:  std::cout << "GE\n"; ++i; break;
        case cmEQ:  std::cout << "EQ\n"; ++i; break;
        case cmNEQ: std::cout << "NEQ\n"; ++i; break;
        case cmADD: std::cout << "ADD\n"; ++i; break;
        case cmAND: std::cout << "AND\n"; ++i; break;
        case cmOR:  std::cout << "OR\n";  ++i; break;
        case cmXOR: std::cout << "XOR\n"; ++i; break;
        case cmSUB: std::cout << "SUB\n"; ++i; break;
        case cmMUL: std::cout << "MUL\n"; ++i; break;
        case cmDIV: std::cout << "DIV\n"; ++i; break;
        case cmPOW: std::cout << "POW\n"; ++i; break;

        case cmASSIGN: 
                    std::cout << "ASSIGN\t"; ++i; 
                    std::cout << "[ADDR: 0x" << *(value_type**)&m_vBase[i] << "]\n"; 
                    i += mc_iSizePtr;
                    break; 

        default:    std::cout << "(unknown code: " << (int)m_vBase[i] << ")\n"; 
                    ++i;	
                    break;
      } // switch cmdCode
    } // while bytecode

    std::cout << "END" << std::endl;
  }
} // namespace mu
