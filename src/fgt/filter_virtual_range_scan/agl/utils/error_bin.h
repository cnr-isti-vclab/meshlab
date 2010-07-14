#ifndef ERROR_BIN_H
#define ERROR_BIN_H

#include <vector>
#include <GL/glew.h>
#include <string>

using namespace std;

namespace agl
{
    namespace utils
    {
        class error_bin
        {

        public:

            static error_bin* get_instance( void );

            void    clear           ( void );
            void    check           ( void );
            bool    is_empty        ( void );
            string  get_errors      ( void );
            string  get_last        ( void );
            int     get_error_count ( void );
            string  get_error_at    ( unsigned int index );
            void    reset           ( void );

        private:

            static error_bin* instance;

            typedef pair< GLenum, string > error;
            typedef vector< error >::iterator err_it;
            vector< error > errors;

            error_bin   ( void );
            ~error_bin  ( void );

        };
    }
}

#endif // ERROR_BIN_H
