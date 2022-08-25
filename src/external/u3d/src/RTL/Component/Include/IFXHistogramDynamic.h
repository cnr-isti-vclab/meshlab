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

/**
  @file IFXHistogramDynamic.h

      This file defines the base class for the dynamic sparse Histogram objects.
*/

#ifndef IFXHistogramDynamic_H
#define IFXHistogramDynamic_H

class IFXHistogramDynamic
{
  enum { IFXOptimization_REF, IFXOptimization_MMX, IFXOptimization_SSE2 };
public:
  // Constructor/Destructor
  IFXHistogramDynamic(U32 uElephant);
  ~IFXHistogramDynamic();


  // Model update methods

  /// AddSymbol - Updates the model to include an occurence of the symbol
  void AddSymbolRef(U32 symbol);
  void AddSymbolMMX(U32 symbol);
  void AddSymbolSSE2(U32 symbol);
  void AddSymbol(U32 symbol)
  {
#if !defined( U3D_NO_ASM ) && !defined( _WIN64 ) && ( defined( WIN32 ) || defined( cl ) || defined( icl ) || ( defined( __GNUC__ ) && ( defined( LINUX ) || defined( MAC32 ) || defined( __linux__ ) || defined( __APPLE__ ) ) ) )
    switch (m_uOptimizationMode) {
      case IFXOptimization_MMX: AddSymbolMMX(symbol); break;
      case IFXOptimization_SSE2: AddSymbolSSE2(symbol); break;
      default: AddSymbolRef(symbol);
    }
#else
      AddSymbolRef(symbol);
#endif
  }


  // Symbol / Frequency conversion methods

  /// GetSymbolFreq - Returns the approximate occurence count of this symbol
  U32 GetSymbolFreq(U32 symbol);

  /// GetCumSymbolFreq - Returns the sum of all symbol freqs for symbols less than the given symbol
  U32 GetCumSymbolFreq(U32 symbol);

  /// GetTotalSymbolFreq - Returns the approximate occurence count of all symbols
  U32 GetTotalSymbolFreq();

  /// GetSymbolFromFreq - Return the symbol that corresponds to the given cumulative frequency
  U32 GetSymbolFromFreq(U32 uCumulativeFrequency);

  U32 m_uNumSymbols;
  U32 m_uElephant;
  U16 *m_pu16SymbolCount;
  U16 *m_pu16CumulativeCount4;
  U16 *m_pu16RawSymbolCount;
  U16 *m_pu16RawCumulativeCount4;

  static const U32 m_uMaximumSymbolInHistogram;
  U32 m_uOptimizationMode;
};

#endif
