#include <stdio.h>
#include <assert.h>
#include <algorithm>

#include "rect_packer.h"

inline int min( const int a, const int b )
{
   return a<b ? a:b ;
}

inline int max( const int a, const int b )
{
   return a>b ? a:b ;
}

class point2iConf
{
public:
	const std::vector<point2i> & v;

	inline point2iConf( const std::vector<point2i> & nv ) : v(nv) { }

	inline bool operator() ( int a, int b )
	{
		return v[a]>v[b];
		//return v[a][0]*v[a][1]>v[b][0]*v[b][1];
	}
};

bool rect_packer::pack(const std::vector<point2i> & sizes, const point2i & max_size, std::vector<point2i> & posiz, point2i & global_size)
{
	int n = (int)(sizes.size());	
    assert(n>0);
    assert(max_size[0]>0);
    assert(max_size[1]>0);


    int gdim = max_size[0]*max_size[1];		// Size dell griglia

    int i,j,x,y;

	posiz.resize(n);
    for(i=0;i<n;i++)				// Azzero le posizioni
		posiz[i][0] = -1;


    std::vector<int> grid(gdim);			// Creazione griglia
	for(i=0;i<gdim;++i) grid[i] = 0;

#define Grid(q,w)	(grid[(q)+(w)*max_size[0]])

    std::vector<int> perm(n);			// Creazione permutazione
    for(i=0;i<n;i++) perm[i] = i;
	point2iConf conf(sizes);
	sort(perm.begin(),perm.end(),conf);

	if(sizes[perm[0]][0]>max_size[0] ||
	   sizes[perm[0]][1]>max_size[1] )
	   return false;

		// Posiziono il primo
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

		// Posiziono tutti gli altri
    for(i=1;i<n;++i)
    {
		j = perm[i];
		assert(j>=0);
		assert(j<n);
		assert(posiz[j][0]==-1);

		int bestx,besty,bestsx,bestsy,besta;

        besta = -1;

        int sx = sizes[j][0];	// Pe comodita' mi copio la dimensione
		int sy = sizes[j][1];
		assert(sx>0);
		assert(sy>0);

			// Calcolo la posizione limite
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
					// Controllo intersezione
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

				if(c)	// Salto il rettangolo
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

		posiz[j][0] = bestx;
		posiz[j][1] = besty;
		global_size[0] = bestsx;
		global_size[1] = bestsy;
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
	// Codice di debugging
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
