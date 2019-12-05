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
  if(!loaded)
	  return -1.0;
  if((xx<0) || (yy<0) || (xx>=sx) || (yy>=sy))
	  return 0.0;

  return (data[(yy * sx) + xx]);
}

int floatbuffer::setval(int xx, int yy, float val)
{
  if(!loaded)
	  return -1;
  if((xx<0) || (yy<0) || (xx>sx) || (yy>sy))
	  return -1;

  data[(yy * sx) + xx] = val;
  return 1;
}

int floatbuffer::applysobel(floatbuffer *from)
{
  int xx,yy;
  float accum;
  if(!loaded)
	  return -1;

  for(xx=0; xx<sx; xx++)
	for(yy=0; yy<sy; yy++)
		data[(yy * sx) + xx] = 0;

  for(xx=1; xx<sx-1; xx++)
	for(yy=1; yy<sy-1; yy++)
		if (from->getval(xx, yy) != 0)
			{
				accum=0;
				accum += -1.0 * from->getval(xx-1, yy-1);
				accum += -2.0 * from->getval(xx-1, yy  );
				accum += -1.0 * from->getval(xx-1, yy+1);
				accum += +1.0 * from->getval(xx+1, yy-1);
				accum += +2.0 * from->getval(xx+1, yy  );
				accum += +1.0 * from->getval(xx+1, yy+1);

				data[(yy * sx) + xx] += abs(accum);
			}
		

  for(xx=1; xx<sx-1; xx++)
	for(yy=1; yy<sy-1; yy++)
		if (from->getval(xx, yy) != 0)
			{
				accum=0;
				accum += -1.0 * from->getval(xx-1, yy-1);
				accum += -2.0 * from->getval(xx  , yy-1);
				accum += -1.0 * from->getval(xx+1, yy-1);
				accum += +1.0 * from->getval(xx-1, yy+1);
				accum += +2.0 * from->getval(xx  , yy+1);
				accum += +1.0 * from->getval(xx+1, yy+1);

				data[(yy * sx) + xx] += abs(accum);
			}
		

  return 1;
}

int floatbuffer::fillwith(float val)
{
 if(!loaded)
  return -1.0;

 for(int ii=0; ii<(sx*sy); ii++)
 {
	data[ii] = val;
 }

 return 1;
}

int floatbuffer::initborder(floatbuffer* zerofrom)
{
 float maxf,minf;
 maxf = -10000000;
 minf =  10000000;

 // setting initial data
 //
 // -99 outside object (using depthmap to decide -> if (zerofrom==0) )
 //
 //   0 on border pixel
 //
 //  -1 on pixels still to be filled


 //getting max/min for threshold
 for(int kk=0; kk< sx*sy; kk++)
 {
	{
		if (data[kk] > maxf)
			maxf = data[kk];
		if ((data[kk] < minf) && (data[kk] != 0))
			minf = data[kk];
	}
 }
/**/
 Histogram<float> myhist;

 myhist.SetRange(minf, maxf, 400);
 for(int kk=0; kk< sx*sy; kk++)
	 if(data[kk] != 0)
		myhist.Add(data[kk]);

 float bthreshold = myhist.Percentile(0.90f);

 for(int kk=0; kk< sx*sy; kk++)
 {
	if (zerofrom->data[kk] == 0)					// outside
		data[kk] = -1;
	else if(data[kk] > bthreshold)	// is border
		data[kk] = 0;
	else 											// to be filled
		data[kk] = 10000000;						
 }

 return 1;
}

int floatbuffer::distancefield()
{
	int kk;
	int xx,yy;
	int cx,cy;
	float currval;
	queue<int> todo;
  int maxval = -10000;

	// init queue.. push in all border
	for(kk=0; kk<sx*sy; kk++)
	{
		if(data[kk] == 0)
			todo.push(kk);
	}
	
	//
	while(! todo.empty())
	{
		yy = todo.front() / sx;
		xx = todo.front() % sx;
		currval = data[todo.front()] + 1;
		todo.pop();

		//---------
		cx = xx-1;
		cy = yy;

		if(cx >=0)									// inside image
			if(data[cx + (sx*cy)] != -1)			// not background
				if(data[cx + (sx*cy)] > currval)	// need update
				{
				 data[cx + (sx*cy)] = currval;
				 todo.push(cx + (sx*cy));
         if(currval > maxval)
           maxval = currval;
				}

		//---------
		cx = xx+1;
		cy = yy;

		if(cx < sx)									// inside image
			if(data[cx + (sx*cy)] != -1)			// not background
				if(data[cx + (sx*cy)] > currval)	// need update
				{
				 data[cx + (sx*cy)] = currval;
				 todo.push(cx + (sx*cy));
         if(currval > maxval)
           maxval = currval;
				}

		//---------
		cx = xx;
		cy = yy-1;

		if(cy >=0)									// inside image
			if(data[cx + (sx*cy)] != -1)			// not background
				if(data[cx + (sx*cy)] > currval)	// need update
				{
				 data[cx + (sx*cy)] = currval;
				 todo.push(cx + (sx*cy));
         if(currval > maxval)
           maxval = currval;

				}

		//---------
		cx = xx;
		cy = yy+1;

		if(cy < sy)									// inside image
			if(data[cx + (sx*cy)] != -1)			// not background
				if(data[cx + (sx*cy)] > currval)	// need update
				{
				 data[cx + (sx*cy)] = currval;
				 todo.push(cx + (sx*cy));
         if(currval > maxval)
           maxval = currval;
				}

	}

	return maxval;
}



int floatbuffer::dumppfm(QString filename)
{
 FILE* miofile;
 int res;

 miofile = fopen(filename.toUtf8().data(), "wb");

 fprintf(miofile,"PF\n");
 fprintf(miofile,"%i %i\n",sx,sy);
 fprintf(miofile,"-1.000000\n",sx,sy);

 for(int kk=0; kk< sx*sy; kk++)
 {
  res = fwrite(&(data[kk]), sizeof(float), 1, miofile);
  res = fwrite(&(data[kk]), sizeof(float), 1, miofile);
  res = fwrite(&(data[kk]), sizeof(float), 1, miofile);
 }

 fclose(miofile);

 return 1;
}

