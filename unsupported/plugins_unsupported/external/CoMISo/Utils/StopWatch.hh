//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 3468 $
//   $Author: moebius $
//   $Date: 2008-10-17 14:58:52 +0200 (Fri, 17 Oct 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS StopWatch
//
//=============================================================================


#ifndef COMISO_STOPWATCH_HH
#define COMISO_STOPWATCH_HH


//== INCLUDES =================================================================


#ifdef _WIN32

#include <windows.h>

#else // Linux

#include <sys/time.h>

#endif

#include <CoMISo/Config/CoMISoDefines.hh>


//== NAMESPACES ===============================================================


namespace COMISO {


//== CLASS DEFINITION =========================================================



/** \class StopWatch StopWatch.hh <COMISO/Utils/StopWatch.hh>

    This class can be used for measuring time, providing milli-second
    precision by using the gettimeofday() funtion.  It is e.g. used in
    the class COMISO::TimedTracing.
**/

class COMISODLLEXPORT StopWatch
{
public:

  /// Constructor
  StopWatch() {
    #ifdef _WIN32
      QueryPerformanceFrequency(&freq_);
    #else
      starttime_.tv_sec = starttime_.tv_usec = 0;
      endtime_.tv_sec   = endtime_.tv_usec = 0;
    #endif
  }

  /// Destructor
  ~StopWatch() {}

  /// Start time measurement
  void start() {
    #ifdef _WIN32
      QueryPerformanceCounter(&starttime_);
    #else
      starttime_ = current_time();
    #endif
  }

  /// Restart, return time elapsed until now.
  double restart() {
    #ifdef _WIN32
      QueryPerformanceCounter(&endtime_);
    #else
      endtime_ = current_time();
    #endif

    double t = elapsed();
    start();
    return t;
  }

  /// Stop time measurement, return time.
  double stop() {
    #ifdef _WIN32
      QueryPerformanceCounter(&endtime_);
    #else
      endtime_ = current_time();
    #endif

    return elapsed();
  }

  /// Get the total time in milli-seconds (watch has to be stopped).
  double elapsed() const {
    #ifdef _WIN32
      return (double)(endtime_.QuadPart - starttime_.QuadPart)
	   / (double)freq_.QuadPart * 1000.0f;
    #else
      return ((endtime_.tv_sec  - starttime_.tv_sec )*1000.0 +
	      (endtime_.tv_usec - starttime_.tv_usec)*0.001);
    #endif
  }


private:

  #ifdef _WIN32
    LARGE_INTEGER starttime_, endtime_;
    LARGE_INTEGER freq_;
  #else // Linux
    timeval current_time() const {
      struct timeval tv;
      gettimeofday(&tv, 0);
      return tv;
    }

    timeval starttime_, endtime_;
  #endif

};


//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // COMISO_STOPWATCH_HH defined
//=============================================================================

