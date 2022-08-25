/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "utils.h"
#include <QFile>
#include <iostream>




std::string resourceContent( const std::string& filename )
{
    std::string content;

    QFile file( QString::fromStdString(filename) );
    if( file.open(QIODevice::ReadOnly) )
    {
        unsigned char *filemap = file.map( 0, file.size() );
        content = std::string( (char*)filemap );
        file.unmap( filemap );
        file.close();
    }

    return content;
}


bool loadShader( GPU::Shader& shader,
                 const std::string& basename,
                 std::string *logs )
{
    GPU::Shader::VertPg vpg;
    GPU::Shader::FragPg fpg;

    return vpg.CompileSrcString( resourceContent(":/shaders/"+basename+".vert").c_str(), logs ) &&
           fpg.CompileSrcString( resourceContent(":/shaders/"+basename+".frag").c_str(), logs ) &&
           shader.Attach( vpg ).AttachAndLink( fpg, logs );
}
