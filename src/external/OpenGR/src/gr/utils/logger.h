// Copyright 2014 Nicolas Mellado
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -------------------------------------------------------------------------- //
//
// Authors: Nicolas Mellado
//
// An implementation of the Super 4-points Congruent Sets (Super 4PCS)
// algorithm presented in:
//
// Super 4PCS: Fast Global Pointcloud Registration via Smart Indexing
// Nicolas Mellado, Dror Aiger, Niloy J. Mitra
// Symposium on Geometry Processing 2014.
//
// Data acquisition in large-scale scenes regularly involves accumulating
// information across multiple scans. A common approach is to locally align scan
// pairs using Iterative Closest Point (ICP) algorithm (or its variants), but
// requires static scenes and small motion between scan pairs. This prevents
// accumulating data across multiple scan sessions and/or different acquisition
// modalities (e.g., stereo, depth scans). Alternatively, one can use a global
// registration algorithm allowing scans to be in arbitrary initial poses. The
// state-of-the-art global registration algorithm, 4PCS, however has a quadratic
// time complexity in the number of data points. This vastly limits its
// applicability to acquisition of large environments. We present Super 4PCS for
// global pointcloud registration that is optimal, i.e., runs in linear time (in
// the number of data points) and is also output sensitive in the complexity of
// the alignment problem based on the (unknown) overlap across scan pairs.
// Technically, we map the algorithm as an 'instance problem' and solve it
// efficiently using a smart indexing data organization. The algorithm is
// simple, memory-efficient, and fast. We demonstrate that Super 4PCS results in
// significant speedup over alternative approaches and allows unstructured
// efficient acquisition of scenes at scales previously not possible. Complete
// source code and datasets are available for research use at
// http://geometry.cs.ucl.ac.uk/projects/2014/super4PCS/.

#ifndef _OPENGR_UTILS_LOGGER_H
#define _OPENGR_UTILS_LOGGER_H

#include <iostream>

namespace gr{
namespace Utils{

enum LogLevel {
    NoLog       = 0,
    ErrorReport = 1,
    Verbose     = 2
};


class Logger {
private:
    LogLevel logLevel_;


public:
    inline Logger(LogLevel loglevel = Verbose) : logLevel_(loglevel) {}

    inline void setLogLevel(LogLevel loglevel) { logLevel_ = loglevel; }
    inline LogLevel logLevel() const { return logLevel_; }

    template <LogLevel level, typename...Args>
    inline void Log(const Args&...args) const{
        switch(logLevel_) {
        case NoLog:
            LOG<NoLog, level>(args...);
            break;
        case ErrorReport:
            LOG<ErrorReport, level>(args...);
            break;
        case Verbose:
            LOG<Verbose, level>(args...);
            break;
        default:
            break;
        }
    }

protected:
    static inline void print_err_impl(){
        std::cerr << std::endl;
    }

    template<typename First, typename...Rest>
    static inline void print_err_impl(const First& param1, const Rest&...param){
        std::cerr << param1;
        print_err_impl(param...);
    }
    static inline void print_msg_impl(){
        std::cout << std::endl;
    }

    template<typename First, typename...Rest>
    static inline void print_msg_impl(const First& param1, const Rest&...param){
        std::cout << param1;
        print_msg_impl(param...);
    }

    template<LogLevel msgLevel, LogLevel appLevel, typename...Args>
    static inline void LOG( const Args&...args) {
        if(msgLevel >= appLevel){
            if (msgLevel == ErrorReport)
                print_err_impl(args...);
            else
                print_msg_impl(args...);
        }
    }
}; // class Logger
} // namespace Utils
} // namespace Super4PCS


#endif // LOGGER_H
