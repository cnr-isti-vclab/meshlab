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

#ifndef FILTER_OUTPUT_OPTICAL_FLOW_PLUGIN__OOCRASTER_H
#define FILTER_OUTPUT_OPTICAL_FLOW_PLUGIN__OOCRASTER_H




#include <common/meshmodel.h>


class OOCRaster
{
    unsigned int nbBind;
    bool isOOC;
	QString imgFilename;

public:
    Plane *plane;
    vcg::Shotf shot;
    int id;

    inline OOCRaster( const QString &_imgFilename,
                      const vcg::Shotf &_shot,
                      int _id ) :
        nbBind(0),
        isOOC(true),
        imgFilename(_imgFilename),
        plane(NULL),
        shot(_shot),
        id(_id)
    {}
    inline OOCRaster( RasterModel *r ) :
        nbBind(0),
        isOOC(false),
		imgFilename(r->currentPlane->shortName()),
        plane(r->currentPlane),
        shot(r->shot),
        id(r->id())
    {}
    inline ~OOCRaster()
    {
        if( isOOC )
            delete plane;
    }
    inline OOCRaster( const OOCRaster &r )
    {
        nbBind = r.nbBind;
        isOOC = r.isOOC;
        imgFilename = r.imgFilename;
        shot = r.shot;
        id = r.id;

        if( isOOC )
            plane = nbBind? new Plane( NULL, imgFilename, "RGB" ) : NULL;
        else
            plane = r.plane;
    }

    inline OOCRaster&   operator=( const OOCRaster& r )
    {
        if( &r != this )
        {
            unbind();

            nbBind = r.nbBind;
            isOOC = r.isOOC;
            imgFilename = r.imgFilename;
            shot = r.shot;
            id = r.id;

            if( isOOC )
                plane = nbBind? new Plane( NULL, imgFilename, "RGB" ) : NULL;
            else
                plane = r.plane;
        }

        return *this;
    }

    inline void         bind()
    {
        if( isOOC )
        {
            if( !nbBind )
                plane = new Plane( NULL, imgFilename, "RGB" );
            nbBind ++;
        }
    }

    inline void         unbind()
    {
        if( isOOC )
        {
            nbBind --;
            if( !nbBind )
            {
                delete plane;
                plane = NULL;
            }
        }
    }

    inline QString  shortName() const   { return QFileInfo(imgFilename).fileName(); }
};




#endif // FILTER_OUTPUT_OPTICAL_FLOW_PLUGIN__OOCRASTER_H
