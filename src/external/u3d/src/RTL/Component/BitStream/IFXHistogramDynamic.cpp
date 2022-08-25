//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
//
//  DESCRIPTION
//    This file contains the implementation of the class for a dynamic sparse
//      Histogram object.
//*****************************************************************************

#include <memory.h>
#include "IFXException.h"
#include "IFXHistogramDynamic.h"
#include "IFXOSUtilities.h"

const U32 IFXHistogramDynamic::m_uMaximumSymbolInHistogram = 0x0000FFFF;

#ifdef __GNUC__
void UpdateCumulativeCountSSE2(U32 uLoopCount, U16 *pu16Start) __attribute__ ((noinline));
void UpdateCumulativeCountMMX(U32 uLoopCount, U16 *pu16Start) __attribute__ ((noinline));
#else
void UpdateCumulativeCountSSE2(U32 uLoopCount, U16 *pu16Start);
void UpdateCumulativeCountMMX(U32 uLoopCount, U16 *pu16Start);
#endif

// Constructor
IFXHistogramDynamic::IFXHistogramDynamic(U32 uElephant)
{
  m_uOptimizationMode = IFXOSCheckCPUFeature(IFXCPUFeature_SSE2) ? IFXOptimization_SSE2 : IFXOSCheckCPUFeature(IFXCPUFeature_MMX) ? IFXOptimization_MMX : 0;
  m_uNumSymbols = 100;
  m_pu16RawSymbolCount = new U16[m_uNumSymbols + 8];
  m_pu16RawCumulativeCount4 = new U16[(m_uNumSymbols>>2)+9];
  m_pu16SymbolCount = m_pu16RawSymbolCount ;
  // Align the pointers on 16 byte boundary
  while((UPTR)m_pu16SymbolCount & (UPTR)0x0000000Fu) {
    m_pu16SymbolCount+=2;
  }
  m_pu16CumulativeCount4 = m_pu16RawCumulativeCount4;
  while((UPTR)m_pu16CumulativeCount4 & (UPTR)0x0000000Fu) {
    m_pu16CumulativeCount4+=2;
  }

  IFXASSERT(m_pu16SymbolCount);
  IFXASSERT(m_pu16CumulativeCount4);
  // If the new fails, the pointer will be NULL
  // The pointers should be checked for NULL before use
  if(m_pu16SymbolCount && m_pu16CumulativeCount4) {
    memset(m_pu16SymbolCount,0,sizeof(U16)*m_uNumSymbols);
    memset(m_pu16CumulativeCount4,0,sizeof(U16)*((m_uNumSymbols>>2)+1));
    m_pu16SymbolCount[0] = 1;
    m_pu16CumulativeCount4[0] = 1;
  } else {
    m_uNumSymbols = 0;
  }
  m_uElephant = uElephant;
}

// Destructor
IFXHistogramDynamic::~IFXHistogramDynamic()
{
  IFXDELETE_ARRAY(m_pu16RawSymbolCount);
  IFXDELETE_ARRAY(m_pu16RawCumulativeCount4);
}


// Symbol / Frequency conversion methods
// GetSymbolFreq - Returns the approximate occurence count of this symbol
U32 IFXHistogramDynamic::GetSymbolFreq(U32 symbol)
{
  U32 uCount = 0;

  if(m_pu16SymbolCount) {
    if(symbol < m_uNumSymbols) {
      uCount = (U32) m_pu16SymbolCount[symbol];
    } // else { uCount = 0; }
  }
  return uCount;
}

// GetCumSymbolFreq - Returns the sum of all symbol freqs for symbols less than the given symbol
U32 IFXHistogramDynamic::GetCumSymbolFreq(U32 symbol)
{
  U32 uCumSymbolFreq4 = 1;

  if(m_pu16CumulativeCount4 && m_pu16SymbolCount) {
    if(symbol <= m_uNumSymbols) {
      U32 uCount4 = (U32) m_pu16CumulativeCount4[symbol>>2];
      uCumSymbolFreq4 = ((U32)m_pu16CumulativeCount4[0]) - uCount4;
      U32 i;
      for(i= (symbol & -4 ); (i < symbol); i++) {
        uCumSymbolFreq4 += (U32) m_pu16SymbolCount[i];
      }
    } else {
      uCumSymbolFreq4 = ((U32)m_pu16CumulativeCount4[0]);
    }

  }
  return uCumSymbolFreq4;
}

// GetTotalSymbolFreq - Returns the approximate occurence count of all symbols
U32 IFXHistogramDynamic::GetTotalSymbolFreq()
{
  U32 uTotalSymbolFreq4 = 1;

  if(m_pu16CumulativeCount4) {
    uTotalSymbolFreq4 = (U32) m_pu16CumulativeCount4[0];
  }
  return uTotalSymbolFreq4;
}

// GetSymbolFromFreq - Return the symbol that corresponds to the given cumulative frequency
U32 IFXHistogramDynamic::GetSymbolFromFreq(U32 uCumulativeFrequency)
{
  U32 uSymbol4 = 0;

  if(m_pu16CumulativeCount4) {
    U32 uTotalCumulativeCount4 = (U32) m_pu16CumulativeCount4[0];
    if(uCumulativeFrequency < (U32) uTotalCumulativeCount4) {
      U32 uLow = 0;
      U32 uHigh = (m_uNumSymbols>>2);
      U16 *pu16Temp = m_pu16CumulativeCount4;
      while ((uHigh - uLow) > 4) {
        //U32 mid = (low + high) / 2;
        U32 mid = (uLow + uLow + uLow + uHigh) >> 2;
        //U32 mid = (((low + high)<< 2) + low - high) >> 3;
        if(uCumulativeFrequency >= (uTotalCumulativeCount4 - (U32) pu16Temp[mid])) {
          uLow = mid;
        } else {
          uHigh = mid;
        }
      }

      uLow <<=2;
      uHigh <<=2;
      uHigh += 3;

      while ((uHigh - uLow) > 4) {
        U32 mid = (uLow + uHigh) / 2;
        //U32 mid = (low + low + low + high) >> 2;
        //U32 mid = (((low + high)<< 2) + low - high) >> 3;
        if(uCumulativeFrequency >= IFXHistogramDynamic::GetCumSymbolFreq(mid) ) {
          uLow = mid;
        } else {
          uHigh = mid;
        }
      }

      U32 i;
      for(i=uLow;i<=uHigh;i++) {
        if(uCumulativeFrequency >= IFXHistogramDynamic::GetCumSymbolFreq(i) ) {
          uSymbol4 = i;
        } else {
          break;
        }
      }
    }
  }

  return uSymbol4;
}

void IFXHistogramDynamic::AddSymbolSSE2(U32 symbol)
{
  U32 i=0;
  try
  {
    if(symbol <= m_uMaximumSymbolInHistogram) {

      // if necessary, scale down the counts
      if(m_pu16SymbolCount && m_pu16CumulativeCount4) {
        if(m_pu16CumulativeCount4[0] >= m_uElephant) {
          memset(m_pu16CumulativeCount4,0,sizeof(U16)*((m_uNumSymbols>>2)+1));
          for(i=m_uNumSymbols;i>0;i--) {
            m_pu16SymbolCount[i-1] >>= 1;
            m_pu16CumulativeCount4[(i-1)/4] += m_pu16SymbolCount[i-1];
          }
          for(i=(m_uNumSymbols>>2);i>0;i--) {
            m_pu16CumulativeCount4[i-1] += m_pu16CumulativeCount4[i];
          }

          // but don't lose the escape
          m_pu16SymbolCount[0] += 1;
          m_pu16CumulativeCount4[0] += 1;
        }
      }

      // if necessary, reallocate the arrays
      if(symbol >= m_uNumSymbols) {
        U16 *pu16OldSymbolCount = m_pu16SymbolCount;
        U16 *pu16OldCumulativeCount4 = m_pu16CumulativeCount4;
        U16 *pu16OldRawSymbolCount = m_pu16RawSymbolCount;
        U16 *pu16OldRawCumulativeCount4 = m_pu16RawCumulativeCount4;
        U32 uOldNumSymbols = m_uNumSymbols;

        m_uNumSymbols = symbol + 100;
        m_pu16RawSymbolCount = new U16[m_uNumSymbols+8];
        m_pu16RawCumulativeCount4 = new U16[(m_uNumSymbols>>2)+9];
        m_pu16SymbolCount = m_pu16RawSymbolCount;
        while((UPTR)m_pu16SymbolCount & (UPTR)0x0000000Fu) {
          m_pu16SymbolCount+=2;
        }
        m_pu16CumulativeCount4 = m_pu16RawCumulativeCount4;
        while((UPTR)m_pu16CumulativeCount4 & (UPTR)0x0000000Fu) {
          m_pu16CumulativeCount4+=2;
        }

        IFXASSERT(m_pu16SymbolCount);
        IFXASSERT(m_pu16CumulativeCount4);
        if(m_pu16SymbolCount && m_pu16CumulativeCount4) {
          memset(m_pu16SymbolCount,0,sizeof(U16)*m_uNumSymbols);
          memset(m_pu16CumulativeCount4,0,sizeof(U16)*((m_uNumSymbols>>2)+1));
          if(pu16OldSymbolCount && pu16OldCumulativeCount4) {
            memcpy(m_pu16SymbolCount,pu16OldSymbolCount,sizeof(U16)*uOldNumSymbols);
            memcpy(m_pu16CumulativeCount4,pu16OldCumulativeCount4,sizeof(U16)*((uOldNumSymbols>>2)+1));
          }
        }
        IFXDELETE_ARRAY(pu16OldRawSymbolCount );
        IFXDELETE_ARRAY(pu16OldRawCumulativeCount4);
      }

      if(m_pu16SymbolCount && m_pu16CumulativeCount4) {
        m_pu16SymbolCount[symbol] += 1;

        // This is the SSE2 version
        U16 *pu16Start = m_pu16CumulativeCount4;

        U32 uLoopCount = ((symbol>>2)+1) >> 6;
        if(uLoopCount) {
          UpdateCumulativeCountSSE2(uLoopCount, pu16Start);
          uLoopCount <<= 6;
        }
        for(i = uLoopCount; i < (symbol>>2)+1 ; i++) {
          pu16Start[i]++;
        }

      } else {
        throw IFXException(IFX_E_OUT_OF_MEMORY);
      }
    }
  }

  catch(...)
  {
    throw;
  }
}

void IFXHistogramDynamic::AddSymbolMMX(U32 symbol)
{
  U32 i=0;
  try
  {
    if(symbol <= m_uMaximumSymbolInHistogram) {

      // if necessary, scale down the counts
      if(m_pu16SymbolCount && m_pu16CumulativeCount4) {
        if(m_pu16CumulativeCount4[0] >= m_uElephant) {
          memset(m_pu16CumulativeCount4,0,sizeof(U16)*((m_uNumSymbols>>2)+1));
          for(i=m_uNumSymbols;i>0;i--) {
            m_pu16SymbolCount[i-1] >>= 1;
            m_pu16CumulativeCount4[(i-1)/4] += m_pu16SymbolCount[i-1];
          }
          for(i=(m_uNumSymbols>>2);i>0;i--) {
            m_pu16CumulativeCount4[i-1] += m_pu16CumulativeCount4[i];
          }

          // but don't lose the escape
          m_pu16SymbolCount[0] += 1;
          m_pu16CumulativeCount4[0] += 1;
        }
      }

      // if necessary, reallocate the arrays
      if(symbol >= m_uNumSymbols) {
        U16 *pu16OldSymbolCount = m_pu16SymbolCount;
        U16 *pu16OldCumulativeCount4 = m_pu16CumulativeCount4;
        U16 *pu16OldRawSymbolCount = m_pu16RawSymbolCount;
        U16 *pu16OldRawCumulativeCount4 = m_pu16RawCumulativeCount4;
        U32 uOldNumSymbols = m_uNumSymbols;

        m_uNumSymbols = symbol + 100;
        m_pu16RawSymbolCount = new U16[m_uNumSymbols+8];
        m_pu16RawCumulativeCount4 = new U16[(m_uNumSymbols>>2)+9];
        m_pu16SymbolCount = m_pu16RawSymbolCount;
        while((UPTR)m_pu16SymbolCount & (UPTR)0x0000000Fu) {
          m_pu16SymbolCount+=2;
        }
        m_pu16CumulativeCount4 = m_pu16RawCumulativeCount4;
        while((UPTR)m_pu16CumulativeCount4 & (UPTR)0x0000000Fu) {
          m_pu16CumulativeCount4+=2;
        }

        IFXASSERT(m_pu16SymbolCount);
        IFXASSERT(m_pu16CumulativeCount4);
        if(m_pu16SymbolCount && m_pu16CumulativeCount4) {
          memset(m_pu16SymbolCount,0,sizeof(U16)*m_uNumSymbols);
          memset(m_pu16CumulativeCount4,0,sizeof(U16)*((m_uNumSymbols>>2)+1));
          if(pu16OldSymbolCount && pu16OldCumulativeCount4) {
            memcpy(m_pu16SymbolCount,pu16OldSymbolCount,sizeof(U16)*uOldNumSymbols);
            memcpy(m_pu16CumulativeCount4,pu16OldCumulativeCount4,sizeof(U16)*((uOldNumSymbols>>2)+1));
          }
        }
        IFXDELETE_ARRAY(pu16OldRawSymbolCount );
        IFXDELETE_ARRAY(pu16OldRawCumulativeCount4);
      }

      if(m_pu16SymbolCount && m_pu16CumulativeCount4) {
        m_pu16SymbolCount[symbol] += 1;

        // This is the RED4 version

        U16 *pu16Start = m_pu16CumulativeCount4;

        U32 uLoopCount = ((symbol>>2)+1) >> 5;
        if(uLoopCount) {
          UpdateCumulativeCountMMX(uLoopCount, pu16Start);
          uLoopCount <<= 5;
        }
        for(i = uLoopCount; i < (symbol>>2)+1 ; i++) {
          pu16Start[i]++;
        }

      } else {
        throw IFXException(IFX_E_OUT_OF_MEMORY);
      }
    }
  }

  catch(...)
  {
    throw;
  }
}

// AddSymbol - Updates the model to include an occurence of the symbol
void IFXHistogramDynamic::AddSymbolRef(U32 symbol)
{
  U32 i=0;
  try
  {
    if(symbol <= m_uMaximumSymbolInHistogram) {

      // if necessary, scale down the counts
      if(m_pu16SymbolCount && m_pu16CumulativeCount4) {
        if(m_pu16CumulativeCount4[0] >= m_uElephant) {
          memset(m_pu16CumulativeCount4,0,sizeof(U16)*((m_uNumSymbols>>2)+1));
          for(i=m_uNumSymbols;i>0;i--) {
            m_pu16SymbolCount[i-1] >>= 1;
            m_pu16CumulativeCount4[(i-1)/4] += m_pu16SymbolCount[i-1];
          }
          for(i=(m_uNumSymbols>>2);i>0;i--) {
            m_pu16CumulativeCount4[i-1] += m_pu16CumulativeCount4[i];
          }

          // but don't lose the escape
          m_pu16SymbolCount[0] += 1;
          m_pu16CumulativeCount4[0] += 1;
        }
      }

      // if necessary, reallocate the arrays
      if(symbol >= m_uNumSymbols) {
        U16 *pu16OldSymbolCount = m_pu16SymbolCount;
        U16 *pu16OldCumulativeCount4 = m_pu16CumulativeCount4;
        U16 *pu16OldRawSymbolCount = m_pu16RawSymbolCount;
        U16 *pu16OldRawCumulativeCount4 = m_pu16RawCumulativeCount4;
        U32 uOldNumSymbols = m_uNumSymbols;

        m_uNumSymbols = symbol + 100;
        m_pu16RawSymbolCount = new U16[m_uNumSymbols+8];
        m_pu16RawCumulativeCount4 = new U16[(m_uNumSymbols>>2)+9];
        m_pu16SymbolCount = m_pu16RawSymbolCount;
        while((UPTR)m_pu16SymbolCount & (UPTR)0x0000000Fu) {
          m_pu16SymbolCount+=2;
        }
        m_pu16CumulativeCount4 = m_pu16RawCumulativeCount4;
        while((UPTR)m_pu16CumulativeCount4 & (UPTR)0x0000000Fu) {
          m_pu16CumulativeCount4+=2;
        }

        IFXASSERT(m_pu16SymbolCount);
        IFXASSERT(m_pu16CumulativeCount4);
        if(m_pu16SymbolCount && m_pu16CumulativeCount4) {
          memset(m_pu16SymbolCount,0,sizeof(U16)*m_uNumSymbols);
          memset(m_pu16CumulativeCount4,0,sizeof(U16)*((m_uNumSymbols>>2)+1));
          if(pu16OldSymbolCount && pu16OldCumulativeCount4) {
            memcpy(m_pu16SymbolCount,pu16OldSymbolCount,sizeof(U16)*uOldNumSymbols);
            memcpy(m_pu16CumulativeCount4,pu16OldCumulativeCount4,sizeof(U16)*((uOldNumSymbols>>2)+1));
          }
        }
        IFXDELETE_ARRAY(pu16OldRawSymbolCount );
        IFXDELETE_ARRAY(pu16OldRawCumulativeCount4);
      }

      if(m_pu16SymbolCount && m_pu16CumulativeCount4) {
        m_pu16SymbolCount[symbol] += 1;

        // This is the C version
        U32 i;
        for(i=0; i <= (symbol>>2); i++) {
          m_pu16CumulativeCount4[i] += 1;
        }

      } else {
        throw IFXException(IFX_E_OUT_OF_MEMORY);
      }
    }
  }

  catch(...)
  {
    throw;
  }
}

#if defined(U3D_NO_ASM) || (defined( __GNUC__ ) && !defined( __i386__ )) || defined( _WIN64 )  // ? && !defined(__x86_64__)
void UpdateCumulativeCountSSE2(U32 uLoopCount, U16 *pu16Start)
{
}

void UpdateCumulativeCountMMX(U32 uLoopCount, U16 *pu16Start)
{
}
#else
#if defined(_MSC_VER) || defined( cl ) || defined( icl )
void UpdateCumulativeCountSSE2(U32 uLoopCount, U16 *pu16Start)
{
  __asm {
    mov edx, uLoopCount
    mov ecx, pu16Start; read pointer
    // 0x66 is the Size of parameter prefix.
    // 0x66 indicates that xmm0 should be used instead of mm0.
    _emit 0x66 ;
    pxor mm0, mm0;
    _emit 0x66 ;
    pcmpeqw mm7, mm7;
    _emit 0x66 ;
    psubw mm0, mm7 ; // xmm0: 1 1 1 1 1 1 1 1

LoopStartSSE2:
    _emit 0x66
    movq mm1, [ecx]; [0-7]
    _emit 0x66
    paddw mm1, mm0
    _emit 0x66
    movq [ecx], mm1

    _emit 0x66
    movq mm2, [ecx+16]; [8-15]
    _emit 0x66
    paddw mm2, mm0
    _emit 0x66
    movq [ecx+16], mm2

    _emit 0x66
    movq mm3, [ecx+32]; [16-23]
    _emit 0x66
    paddw mm3, mm0
    _emit 0x66
    movq [ecx+32], mm3

    _emit 0x66
    movq mm4, [ecx+48]; [24-31]
    _emit 0x66
    paddw mm4, mm0
    _emit 0x66
    movq [ecx+48], mm4

    _emit 0x66
    movq mm5, [ecx+64];[32-39]
    _emit 0x66
    paddw mm5, mm0
    _emit 0x66
    movq [ecx+64], mm5

    _emit 0x66
    movq mm6, [ecx+80]; [40-47]
    _emit 0x66
    paddw mm6, mm0
    _emit 0x66
    movq [ecx+80], mm6

    _emit 0x66
    movq mm7, [ecx+96]; [48-55]
    _emit 0x66
    paddw mm7, mm0
    _emit 0x66
    movq [ecx+96], mm7

    _emit 0x66
    movq mm4, [ecx+112]; [56-63]
    _emit 0x66
    paddw mm4, mm0
    _emit 0x66
    movq [ecx+112], mm4

    add ecx, 128
    sub edx, 1 ;
    jnz LoopStartSSE2

    emms
  }
}
#endif

#if defined( __GNUC__ ) && ( defined( LINUX ) || defined( MAC32 ) || defined( __linux__ ) || defined( __APPLE__ ) || defined( __MINGW32__ ) )
void UpdateCumulativeCountSSE2(U32 uLoopCount, U16 *pu16Start)
{
  asm __volatile__ (
    // 0x66 is the Size of parameter prefix.
    // 0x66 indicates that xmm0 should be used instead of mm0.
    ".byte 102      \n\t"
    "pxor %%mm0, %%mm0    \n\t"
    ".byte 102      \n\t"
    "pcmpeqw %%mm7, %%mm7 \n\t"
    ".byte 102      \n\t"
    "psubw %%mm7, %%mm0   \n\t"//  xmm0: 1 1 1 1 1 1 1 1

"LoopStartSSE2:                 \n\t"
    ".byte 102      \n\t"
    "movq (%%ecx), %%mm1  \n\t" //[0-7]
    ".byte 102      \n\t"
    "paddw %%mm0, %%mm1   \n\t"
    ".byte 102      \n\t"
    "movq %%mm1, (%%ecx)  \n\t"

    ".byte 102      \n\t"
    "movq 16(%%ecx), %%mm2  \n\t"//[8-17]
    ".byte 102      \n\t"
    "paddw %%mm0, %%mm2   \n\t"
    ".byte 102      \n\t"
    "movq %%mm2, 16(%%ecx)  \n\t"

    ".byte 102      \n\t"
    "movq 32(%%ecx), %%mm3  \n\t"//[16-23]
    ".byte 102      \n\t"
    "paddw %%mm0, %%mm3   \n\t"
    ".byte 102      \n\t"
    "movq %%mm3, 32(%%ecx)  \n\t"

    ".byte 102      \n\t"
    "movq 48(%%ecx), %%mm4  \n\t"//[24-31]
    ".byte 102      \n\t"
    "paddw %%mm0, %%mm4   \n\t"
    ".byte 102      \n\t"
    "movq %%mm4, 48(%%ecx)  \n\t"

    ".byte 102      \n\t"
    "movq 64(%%ecx), %%mm5  \n\t"//[32-39]
    ".byte 102      \n\t"
    "paddw %%mm0, %%mm5   \n\t"
    ".byte 102      \n\t"
    "movq %%mm5, 64(%%ecx)  \n\t"

    ".byte 102      \n\t"
    "movq 80(%%ecx), %%mm6  \n\t"//[40-47]
    ".byte 102      \n\t"
    "paddw %%mm0, %%mm6   \n\t"
    ".byte 102      \n\t"
    "movq %%mm6, 80(%%ecx)  \n\t"

    ".byte 102      \n\t"
    "movq 96(%%ecx), %%mm7  \n\t"//[48-55]
    ".byte 102      \n\t"
    "paddw %%mm0, %%mm7   \n\t"
    ".byte 102      \n\t"
    "movq %%mm7, 96(%%ecx)  \n\t"

    ".byte 102      \n\t"
    "movq 112(%%ecx), %%mm4 \n\t"//[56-63]
    ".byte 102      \n\t"
    "paddw %%mm0, %%mm4   \n\t"
    ".byte 102      \n\t"
    "movq %%mm4, 112(%%ecx) \n\t"

    "add $128, %%ecx    \n\t"
    "sub $1, %%edx      \n\t"
    "jnz LoopStartSSE2    \n\t"

    "EMMS         \n\t"
    : : "d"(uLoopCount), "c"(pu16Start)
  );
}
#endif

#if defined(_MSC_VER) || defined( cl ) || defined( icl )
void UpdateCumulativeCountMMX(U32 uLoopCount, U16 *pu16Start)
{
  __asm {
    mov edx, uLoopCount
    mov ecx, pu16Start; read pointer
    pxor mm0, mm0;
    pcmpeqw mm7, mm7;
    psubw mm0, mm7 ; // mm0: 1 1 1 1

LoopStartRED4:
    movq mm1, [ecx]; [0-3]
    paddw mm1, mm0
    movq [ecx], mm1

    movq mm2, [ecx+8]; [4-7]
    paddw mm2, mm0
    movq [ecx+8], mm2

    movq mm3, [ecx+16]; [8-11]
    paddw mm3, mm0
    movq [ecx+16], mm3

    movq mm4, [ecx+24]; [12-15]
    paddw mm4, mm0
    movq [ecx+24], mm4

    movq mm5, [ecx+32];[16-19]
    paddw mm5, mm0
    movq [ecx+32], mm5

    movq mm6, [ecx+40]; [20-23]
    paddw mm6, mm0
    movq [ecx+40], mm6

    movq mm7, [ecx+48]; [24-27]
    paddw mm7, mm0
    movq [ecx+48], mm7

    movq mm4, [ecx+56]; [28-31]
    paddw mm4, mm0
    movq [ecx+56], mm4

    add ecx, 64
    sub edx, 1 ;
    jnz LoopStartRED4

    emms
  }
}
#endif

#if defined( __GNUC__ ) && ( defined( LINUX ) || defined( MAC32 ) || defined( __linux__ ) || defined( __APPLE__ ) || defined( __MINGW32__ ) )
void UpdateCumulativeCountMMX(U32 uLoopCount, U16 *pu16Start)
{
  asm __volatile__ (
    "pxor %%mm0, %%mm0    \n\t"
    "pcmpeqw %%mm7, %%mm7 \n\t"
    "psubw %%mm7, %%mm0   \n\t"//  mm0: 1 1 1 1

"LoopStartRED4:                 \n\t"
    "movq (%%ecx), %%mm1  \n\t" //[0-3]
    "paddw %%mm0, %%mm1   \n\t"
    "movq %%mm1, (%%ecx)  \n\t"

    "movq 8(%%ecx), %%mm2 \n\t"//[4-7]
    "paddw %%mm0, %%mm2   \n\t"
    "movq %%mm2, 8(%%ecx) \n\t"

    "movq 16(%%ecx), %%mm3  \n\t"//[8-11]
    "paddw %%mm0, %%mm3   \n\t"
    "movq %%mm3, 16(%%ecx)  \n\t"

    "movq 24(%%ecx), %%mm4  \n\t"//[12-15]
    "paddw %%mm0, %%mm4   \n\t"
    "movq %%mm4, 24(%%ecx)  \n\t"

    "movq 32(%%ecx), %%mm5  \n\t"//[16-19]
    "paddw %%mm0, %%mm5   \n\t"
    "movq %%mm5, 32(%%ecx)  \n\t"

    "movq 40(%%ecx), %%mm6  \n\t"//[20-23]
    "paddw %%mm0, %%mm6   \n\t"
    "movq %%mm6, 40(%%ecx)  \n\t"

    "movq 48(%%ecx), %%mm7  \n\t"//[24-27]
    "paddw %%mm0, %%mm7   \n\t"
    "movq %%mm7, 48(%%ecx)  \n\t"

    "movq 56(%%ecx), %%mm4  \n\t"//[28-31]
    "paddw %%mm0, %%mm4   \n\t"
    "movq %%mm4, 56(%%ecx)  \n\t"

    "add $64, %%ecx     \n\t"
    "sub $1, %%edx      \n\t"
    "jnz LoopStartRED4    \n\t"

    "EMMS         \n\t"
    : : "d"(uLoopCount), "c"(pu16Start)
  );
}
#endif
#endif // NO_ASM
