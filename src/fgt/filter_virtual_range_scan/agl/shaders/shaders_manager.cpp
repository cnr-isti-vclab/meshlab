#include "shaders_manager.h"

#include "../agl.h"

using namespace agl::shaders;

/* constructor and destructor */
shaders_manager::shaders_manager( void )
{
    this->shaders_path = agl::global::default_shaders_path;
}

shaders_manager::shaders_manager( string shaders_path )
{
    this->shaders_path = shaders_path;
}

shaders_manager::~shaders_manager( void )
{
    clear();
}

/* public functions */
shader* shaders_manager::get_shader( string shader_name )
{
    shaders_it i = shaders.find( shader_name );
    if( i != shaders.end() )
    {
        return (*i).second;
    }

    // try to load as a shader set in fast mode
    
    shader* s = shader_set::create_from_file_fast( shaders_path + shader_name );
    if( s != 0 )
    {
        shaders[ shader_name ] = s;
        return s;
    }
    

    return 0;
}

bool shaders_manager::register_shader( string shader_name, shader* s )
{
    shaders_it i = shaders.find( shader_name );
    if( i != shaders.end() )
    {
        return false;
    }

    shaders[ shader_name ] = s;
    return true;
}

void shaders_manager::clear( void )
{
    for( shaders_it i = shaders.begin(); i != shaders.end(); ++i )
    {
        delete ( (*i).second );
    }

    shaders.clear();
}
