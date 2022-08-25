/*===========================================================================*\
 *                                                                           *
 *                               CoMISo                                      *
 *      Copyright (C) 2008-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.rwth-graphics.de                            *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of CoMISo.                                             *
 *                                                                           *
 *  CoMISo is free software: you can redistribute it and/or modify           *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  CoMISo is distributed in the hope that it will be useful,                *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with CoMISo.  If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                           *
\*===========================================================================*/ 




#ifndef VSTOOLS_HH
#define VSTOOLS_HH


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

//== DEFINITION =========================================================

/** These functions are required for Visual Studio to work around missing 
    functions. Basic equivalent functions for double exist in the float 
    header but are named different. So this wrapper makes them standard compatible.
    */
#ifdef WIN32
 #include <float.h>

 namespace std {

   inline int isnan(double x)
   {
     return _isnan(x);
   } 

   // Which idiot defines isinf as a macro somewhere?
   #ifdef isinf 
     #undef isinf
   #endif

   inline int isinf(double x)
   {
     return !_finite(x);
   } 

  }

 inline double nearbyint(double x) {
   if( x >= 0.0 )
     return int( x + 0.5 );
   else
     return int( x - 0.5 );
 }

 inline double round ( double _value ) {
   return nearbyint(_value);
 }


#endif


//=============================================================================
#endif // VSTOOLS_HH defined
//=============================================================================

