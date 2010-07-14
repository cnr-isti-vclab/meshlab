#ifndef PIXEL_DATA_MANAGER_H
#define PIXEL_DATA_MANAGER_H

#include "pixel_data.h"
#include "../textures/textures_manager.h"
#include <map>

using namespace std;

namespace agl
{
    namespace vrs
    {
        class PixelDataManager
        {

        public:
            PixelDataManager( void )
            {
                this->tm = tm;
            }

            ~PixelDataManager( void )
            {
                clearData();
            }

            PixelData* getData( string name )
            {
                DataIterator di = m.find( name );
                if( di != m.end() )
                {
                    return (*di).second;
                }
                return 0;
            }

            PixelData* operator[]( string name )
            {
                return getData( name );
            }

            void getData( vector< string >& names, vector< PixelData* >& target )
            {
                DataIterator di;
                for( unsigned int i=0; i<names.size(); i++ )
                {
                    di = m.find( names[i] );
                    if( di != m.end() )
                    {
                        target.push_back( (*di).second );
                    }
                    else
                    {
                        target.push_back( 0 );
                    }
                }
            }

            void saveData( string name, PixelData* pd, bool deleteTextures = false )
            {
                DataIterator di = m.find( name );
                if( di != m.end() )
                {
                    if( (*di).second != pd )
                    {
                        PixelData* toDelete = (*di).second;

                        if( deleteTextures )
                        {
                            tm.deleteTextures( &(toDelete->data), 1 );
                        }

                        delete toDelete;
                    }
                }
                m[ name ] = pd;
            }

            void clearData( void )
            {
                for( DataIterator di = m.begin(); di != m.end(); di++ )
                {
                    delete (*di).second;
                }
                m.clear();
                tm.deleteAllTextures();
            }

            void deleteData( string name )
            {
                DataIterator di = m.find( name );
                if( di != m.end() )
                {
                    delete (*di).second;
                    m.erase( di );
                }
            }

            void getData( string dataPrefix, vector< PixelData* >& target, vector< string >& names )
            {
                string name = "";
                int prefixLength = dataPrefix.length(), i = 0;
                bool equal = true;

                for( DataIterator di = m.begin(); di != m.end(); di++ )
                {
                    name = (*di).first;

                    if( name.length() < dataPrefix.length() )
                    {
                        continue;
                    }

                    equal = true;
                    i = 0;
                    while( equal && i<prefixLength )
                    {
                        equal = ( name[i] == dataPrefix[i] );
                        i++;
                    }

                    if( equal )
                    {
                        target.push_back( (*di).second );
                        names.push_back( (*di).first );
                    }
                }
            }

            void getData( string* names, int arraySize, vector< PixelData* >& target )
            {
                for( int i=0; i<arraySize; i++ )
                {
                    target.push_back( getData( names[i] ) );
                }
            }

            void printState( void )
            {
                qDebug( "PixelDataManager content\n******************\n" );
                qDebug( "Number of textures in TextureManager: %d", tm.getTexturesCount() );
                qDebug( "Number of PixelData objects: %d\n", m.size() );
                qDebug( "PixelData objects\n----------------\n" );

                PixelData* tmpData = 0;
                texture2d* t = 0;
                char buf[270];
                int elems = m.size();
                qDebug( "elements in map: %d", elems );
                map< string, PixelData* >::iterator mi;

                for( mi = m.begin(); mi != m.end(); mi++ )
                {
                    tmpData = (*mi).second;
                    t = tmpData->data;
                    sprintf( buf, "%-30s\t%7d   x   %-7d\t\t(%5d elements )",
                             (*mi).first.c_str(), t->get_width(),
                             t->get_height(), tmpData->length );
                    qDebug( buf );
                }

                qDebug( "***************" );
            }

            TexturesManager             tm;

        private:
            map< string, PixelData* >   m;
            typedef map< string, PixelData* >::iterator DataIterator;
        };
    }
}


#endif // PIXEL_DATA_MANAGER_H
