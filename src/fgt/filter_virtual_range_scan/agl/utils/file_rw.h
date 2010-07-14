#ifndef FILE_RW_H
#define FILE_RW_H

#include <string>

using namespace std;

namespace agl
{
    namespace utils
    {
        class file_rw
        {

        public:

            static bool read_file   ( string filename, string& target );
            static bool write_file  ( string filename, string& content );

        };
    }
}

#endif // FILE_RW_H
