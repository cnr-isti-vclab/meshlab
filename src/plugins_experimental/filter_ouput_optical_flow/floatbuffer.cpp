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

#include "floatbuffer.h"

//----------------------------------------------------
//----------------------------------------------------
//  FLOAT BUFFER
//----------------------------------------------------
//----------------------------------------------------

floatbuffer::floatbuffer(void)
{
	data = NULL;
	loaded = -1;
	filename = "NONE";
	sx = 0;
	sy = 0;
}

floatbuffer::floatbuffer(floatbuffer *from)
{
	int xx,yy;

	data = NULL;
	loaded = -1;
	filename = "NONE";

	sx = from->sx;
	sy = from->sy;

	data = new float[sx * sy];

	for(xx=0; xx<sx; xx++)
		for(yy=0; yy<sy; yy++)
		{
			data[(yy * sx) + xx] = from->data[(yy * sx) + xx];
		}

	loaded = 1;
	filename = "NONE";
}
	
floatbuffer::~floatbuffer(void)
{
    if(loaded)
	{
	 delete[] data;
	}
}

int floatbuffer::init(int sizex, int sizey)
{
	if((data != NULL) && (loaded != -1))
		return -1;

	sx = sizex;
	sy = sizey;

	data = new float[sizex * sizey];

	loaded = 1;
	filename = "NONE";
	return 1;
}

int floatbuffer::destroy()
{
	if((data == NULL) && (loaded == -1))
		return -1;

	sx = 0;
	sy = 0;

	delete[] data;

	data = NULL;
	loaded = -1;
	filename = "NONE";
	return 1;
}


float floatbuffer::getval(int xx, int yy)
{
    assert( data!=NULL );
    assert( (xx<0) || (yy<0) || (xx>=sx) || (yy>=sy) );
    return data[(yy*sx) + xx];
}


void floatbuffer::applysobel( floatbuffer *from )
{
    sobelMin =  std::numeric_limits<float>::max();
    sobelMax = -std::numeric_limits<float>::max();

    for( int x=0; x<sx; ++x )
        data[x] = data[(sy-1)*sx+x] = 0.0f;
    for( int y=0; y<sy; ++y )
        data[y*sx] = data[(y+1)*sx-1] = 0.0f;


    float *row0 = from->data;
    float *row1 = row0 + sx;
    float *row2 = row1 + sx;

    for( int y=1, n=sx+1; y<sy-1; ++y, n+=2 )
    {
        for( int x=1; x<sx-1; ++x, ++n )
            if( row1[x] != 1.0f )
            {
                float dx =   row0[x+1] - row0[x-1];
                dx += 2.0f*( row1[x+1] - row1[x-1] );
                dx +=        row2[x+1] - row2[x-1];

                float dy =   row2[x-1] - row0[x-1];
                dy += 2.0f*( row2[x  ] - row0[x  ] );
                dy +=        row2[x+1] - row0[x+1];

                if( data[n] = abs(dx)+abs(dy) )
                {
                    if( data[n] < sobelMin )
                        sobelMin = data[n];
                    if( data[n] > sobelMax )
                        sobelMax = data[n];
                }
            }
            else
                data[n] = 0.0f;

        row0 += sx;
        row1 += sx;
        row2 += sx;
    }
}


// Setting inital data
//       -1 outside object (using depthmap to decide -> if (zerofrom==0) )
//        0 on border pixel
// 10000000 on pixels still to be filled
void floatbuffer::initborder( floatbuffer *zerofrom )
{
    Histogram<float> myhist;
    myhist.SetRange( sobelMin, sobelMax, 400 );

    for( int k=0; k<sx*sy; ++k )
        if( data[k] )
	        myhist.Add( data[k] );

    float threshold = myhist.Percentile( 0.9f );

    for( int k=0; k<sx*sy; ++k )
    {
#if 1
        if( zerofrom->data[k] == 1.0f )		// outside
            data[k] = -1.0f;
        else if( data[k] > threshold )	    // is border
            data[k] = 0.0f;
        else 								// to be filled
            data[k] = std::numeric_limits<float>::max();
#else
        if( zerofrom->data[k] == 1.0f )		// outside
            data[k] = 0.5f;
        else if( data[k] > threshold )	    // is border
            data[k] = 0.0f;
        else 								// to be filled
            data[k] = 1.0;
#endif
    }
}


void floatbuffer::distancefield()
{
	queue<int> todo;
    float maxval = -std::numeric_limits<float>::max();


    // init queue.. push in all border
	for( int k=0; k<sx*sy; ++k )
		if( !data[k] )
			todo.push( k );


    //
    while( !todo.empty() )
    {
        int n = todo.front();
        int y = n / sx;
        int x = n % sx;
        todo.pop();

        if( data[n] > maxval )
            maxval = data[n];
        float currval = data[n] + 1.0f;

        //---------
        if( x > 0 )
        {
            float& d = data[n-1];
            if( d!=-1.0f  &&  d>currval )
            {
                d = currval;
                todo.push( n-1 );
            }
        }

        //---------
        if( x < sx-1 )
        {
            float &d = data[n+1];
            if( d!=-1.0f  &&  d>currval)
            {
                d = currval;
                todo.push( n+1 );
            }
        }

        //---------
        if( y > 0 )
        {
            float &d = data[n-sx];
            if( d!=-1.0f  &&  d>currval )
            {
                d = currval;
                todo.push( n-sx );
            }
        }

        //---------
        if( y < sy-1 )
        {
            float &d = data[n+sx];
            if( d!=-1.0f  &&  d>currval )
            {
                d = currval;
                todo.push( n+sx );
            }
        }
    }


    float maxValInv = 1.0f / maxval;
	for( int k=0; k<sx*sy; ++k )
        data[k] *= maxValInv;
}
