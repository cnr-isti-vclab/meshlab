#include "shared_resources.h"

using namespace agl::vrs;

SharedResources* SharedResources::instance = 0;

SharedResources::SharedResources( void )
{
    ;
}

SharedResources::~SharedResources( void )
{
    clearResources();
}

SharedResources* SharedResources::getInstance( void )
{
    if( !instance )
    {
        instance = new SharedResources();
    }
    return instance;
}

void SharedResources::deleteInstance( void )
{
    if( instance )
    {
        delete instance;
        instance = 0;
    }
}

void SharedResources::clearResources( void )
{
    fbo.setConfiguration( 0 );
    pdm.clearData();
    fbo.clearConfigurations();
}
