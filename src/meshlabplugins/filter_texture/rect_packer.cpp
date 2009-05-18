/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include <stdio.h>
#include <assert.h>
#include <algorithm>

#include "rect_packer.h"

using namespace std;

class point2iConf
{
public:
	const std::vector<Point2i> & v;

	inline point2iConf( const std::vector<Point2i> & nv ) : v(nv) { }

	inline bool operator() ( int a, int b )
	{
		return v[a]>v[b];
		//return v[a][0]*v[a][1]>v[b][0]*v[b][1];
	}
};

/*
1)  'sizes' is a vector of points corresponding to the width/height of each texture
2)  'max_size' is a single point that represents the maximum height/width of the texture atlas you want to create
3)  'posiz' is a vector of points that indicate for each texture in 'sizes', where to place this texture in the texture atlas (initially this would be empty)
4)  'global_size' represents the minimum height/width necessary to encompass the resulting texture atlas (initially null)
*/
bool rect_packer::pack(const std::vector<Point2i> & sizes, const Point2i & max_size, std::vector<Point2i> & posiz, Point2i & global_size)
{
	int n = (int)(sizes.size());
	if (n<=0)
		return false;
    //assert(n>0);
    assert(max_size[0]>0);
    assert(max_size[1]>0);

    int gdim = max_size[0]*max_size[1];		// grid size
    int i,j,x,y;

	posiz.resize(n);
    for(i=0;i<n;i++)				// reset initial positions
		posiz[i][0] = -1;

    std::vector<int> grid(gdim);			// grid creation
	for(i=0;i<gdim;++i) grid[i] = 0;

#define Grid(q,w)	(grid[(q)+(w)*max_size[0]])

    std::vector<int> perm(n);			// permutation creation - vector, one element for each size in sizes - this is what you want the result of
    for(i=0;i<n;i++) perm[i] = i;
	point2iConf conf(sizes);
	sort(perm.begin(),perm.end(),conf);

	if(sizes[perm[0]][0]>max_size[0] ||
	   sizes[perm[0]][1]>max_size[1] )
	   return false;

	// Find the position of the first one
    j = perm[0];
    global_size[0] = sizes[j][0];
    global_size[1] = sizes[j][1];
    posiz[j][0] = 0;
    posiz[j][1] = 0;
    for(y=0;y<global_size[1];y++)
	for(x=0;x<global_size[0];x++)
	{
	    assert(x>=0);
	    assert(x<max_size[0]);
	    assert(y>=0);
	    assert(y<max_size[1]);
	    grid[x+y*max_size[0]] = j+1;
    }

	// Lets position all the others
    for(i=1;i<n;++i)
    {
		j = perm[i];
		assert(j>=0);
		assert(j<n);
		assert(posiz[j][0]==-1);

		int bestx,besty,bestsx,bestsy,besta;

        besta = -1;

        int sx = sizes[j][0];	// it is easier to copy the sizes
		int sy = sizes[j][1];
		assert(sx>0);
		assert(sy>0);
		
		// limit positions
		int lx = min(global_size[0],max_size[0]-sx);
		int ly = min(global_size[1],max_size[1]-sy);

		assert(lx>0);
		assert(ly>0);

        int finterior = 0;

        for(y=0;y<=ly;y++)
		{
  			for(x=0;x<=lx;)
			{
				int px;
                int c;
				// intersection check
                c = Grid(x,y+sy-1);
				if(!c) c = Grid(x+sx-1,y+sy-1);
				if(!c)
				{
					for(px=x;px<x+sx;px++)
					{
						c = Grid(px,y);
						if(c) break;
					}
				}

				if(c)	// do not consider this rectangle
				{
					--c;
					assert(c>=0);
					assert(c<n);
					assert(posiz[c][0]!=-1);
					x = posiz[c][0] + sizes[c][0];
				}
				else
				{
                    int nsx = max(global_size[0],x+sx);
                    int nsy = max(global_size[1],y+sy);
					int a   = nsx*nsy;

					if(besta==-1 || besta>a)
					{
						bestx  = x;
						besty  = y;
						bestsx = nsx;
						bestsy = nsy;
						besta  = a;
						if( bestsx==global_size[0] && bestsy==global_size[1] )
							finterior = 1;
					}
					break;
				}
				if(finterior) break;
			}
			if( finterior ) break;
		}

		if(besta==-1)
		{
			return false;
		}

		posiz[j][0] = bestx;//new U offset for texture at position j
		posiz[j][1] = besty;//new V offset for texture at position j
		global_size[0] = bestsx;//holds smallest encompassing width for texture atlas
		global_size[1] = bestsy;//holds smallest encompassing height for texture atlas
		for(y=posiz[j][1];y<posiz[j][1]+sy;y++)
			for(x=posiz[j][0];x<posiz[j][0]+sx;x++)
			{
				assert(x>=0);
				assert(x<max_size[0]);
				assert(y>=0);
				assert(y<max_size[1]);
				grid[x+y*max_size[0]] = j+1;
			}	
	}

#if 0
// debugging code: it saves into a simple bitmap the computed packing.
FILE * fp = fopen("debpack.ppm","wb");
fprintf(fp,"P6\n%d %d\n255\n",global_size[0],global_size[1]);
for(j=0;j<global_size[1];++j)
for(i=0;i<global_size[0];++i)
{
	unsigned char c0[3] = {0,0,0};
	unsigned char c1[3] = {255,0,0};
	unsigned char c2[3] = {0,255,0};
	unsigned char c3[3] = {0,0,255};
	if( Grid(i,j)==0 ) fwrite(c0,1,3,fp);
	else if( Grid(i,j)==1 ) fwrite(c1,1,3,fp);
	else if( Grid(i,j)==2 ) fwrite(c2,1,3,fp);
	else if( 1/*Grid(i,j)==3*/ ) fwrite(c3,1,3,fp);
}
fclose(fp);
#endif

#undef Grid

	return true;

}
