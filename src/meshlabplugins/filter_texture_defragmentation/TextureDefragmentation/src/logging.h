/*******************************************************************************
    Copyright (c) 2021, Andrea Maggiordomo, Paolo Cignoni and Marco Tarini

    This file is part of TextureDefrag, a reference implementation for
    the paper ``Texture Defragmentation for Photo-Reconstructed 3D Models''
    by Andrea Maggiordomo, Paolo Cignoni and Marco Tarini.

    TextureDefrag is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TextureDefrag is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TextureDefrag. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <vector>
#include <map>

// logging macros

#define LOG_INIT(level) (logging::Logger::Init(level))
#define LOG_SET_THREAD_NAME(name) (logging::Logger::RegisterName(name))
#define LOG_GET_THREAD_NAME (logging::Logger::GetName())
#define LOG(level) (level > logging::Logger::GetLogLevel()) ? ((void) 0) : logging::V_() & logging::Buffer(level)

#define LOG_ERR     LOG(logging::Level::Error)
#define LOG_WARN    LOG(logging::Level::Warning)
#define LOG_INFO    LOG(logging::Level::Info)
#define LOG_VERBOSE LOG(logging::Level::Verbose)
#define LOG_DEBUG   LOG(logging::Level::Debug)

namespace logging {

enum Level {
    Error   = -2,
    Warning = -1,
    Info    =  0,
    Verbose =  1,
    Debug   =  2
};

class Buffer {

    std::ostringstream os;

public:

    Buffer(int level);
    ~Buffer();

    template <typename T>
    Buffer& operator<<(const T& t)
    {
        os << t;
        return *this;
    }

    /*
    Buffer& operator<<(std::ostream& (*f)(std::ostream&))
    {
        f(os);
        return *this;
    }
    */

};

struct V_ {
    void operator &(const Buffer&) { }
};

class Logger {

    static int logLevel;
    static std::vector<std::ostream *> streamVec;
    static std::map<std::thread::id, std::string> threadNames;

    static std::mutex singletonMtx;

public:

    static void Init(int level);

    static int GetLogLevel();
    static std::string GetName();
    static void RegisterStream(std::ostream *os);
    static void RegisterName(const std::string& threadName);
    static void Log(const std::string& s);

};

}

#endif // LOGGING_H

