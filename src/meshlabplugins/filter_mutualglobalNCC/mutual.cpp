#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <iostream>
#include <QImage> /*debug*/
#include "mutual.h"

using namespace std;
MutualInfo::MutualInfo(unsigned int _nbins, int _bweight, bool _use_background):
  bweight(_bweight), use_background(_use_background),
  histo2D(NULL), histoA(NULL), histoB(NULL) {

  setBins(_nbins);
}

MutualInfo::~MutualInfo() {
  delete []histo2D;
  delete []histoA;
  delete []histoB;
}

void MutualInfo::setBins(unsigned int _nbins) {
  nbins = _nbins;
  assert(!(nbins & (nbins-1)));

  if(histo2D) delete []histo2D;
  if(histoA) delete []histoA;
  if(histoB) delete []histoB;
  histo2D = new unsigned int[nbins*nbins];
  histoA = new unsigned int[nbins];
  histoB = new unsigned int[nbins];
}

double MutualInfo::infoNCC(int width, int height,
	                         unsigned char *target, unsigned char *render, 
													 QImage & rendered , QImage & combined,
													 int startx, int endx,
													 int starty, int endy)
{
	double ncc = 0.0;

	float r1,g1,b1,r2,g2,b2;
	float r1mean, g1mean, b1mean, r2mean, g2mean, b2mean;
	int offset;

	if (endx == 0)
		endx = width;

	if (endy == 0)
		endy = height;

	r1mean = g1mean = b1mean = 0.0f;
	r2mean = g2mean = b2mean = 0.0f;
	int Npixels=0;
	for (int y = starty; y < endy; y++)
	{
		for (int x = startx; x < endx; x++)
		{
			if (rendered.pixel(x,y) != combined.pixel(x,y))
			{
				offset = (x + y * width)*3;
				r1mean += static_cast<float>(target[offset]);
				g1mean += static_cast<float>(target[offset+1]);
				b1mean += static_cast<float>(target[offset+2]);
				r2mean += static_cast<float>(render[offset]);
				g2mean += static_cast<float>(render[offset+1]);
				b2mean += static_cast<float>(render[offset+2]);
				Npixels++;
			}
		}
	}

	if (Npixels == 0)
	{
		return 0.0;
	}
	else
	{
		r1mean /= Npixels;
		g1mean /= Npixels;
		b1mean /= Npixels;
		r2mean /= Npixels;
		g2mean /= Npixels;
		b2mean /= Npixels;

		float sum = 0.0f;
		float sum1r,sum1g,sum1b;
		sum1r = sum1g = sum1b = 0.0f;
		float sum2r, sum2g, sum2b;
		sum2r = sum2g = sum2b = 0.0f;
		for (int y = starty; y < endy; y++)
		{
			for (int x = startx; x < endx; x++)
			{
				if (rendered.pixel(x,y) != combined.pixel(x,y))
				{
					offset = (x + y * width)*3;
					r1 = target[offset];
					g1 = target[offset+1];
					b1 = target[offset+2];
					r2 = render[offset];
					g2 = render[offset+1];
					b2 = render[offset+2];

					sum += (r1-r1mean)*(r2-r2mean) + (g1-g1mean)*(g2-g2mean) + (b1-b1mean)*(b2-b2mean);
					sum1r += (r1-r1mean)*(r1-r1mean);
					sum1g += (g1-g1mean)*(g1-g1mean);
					sum1b += (b1-b1mean)*(b1-b1mean);
					sum2r += (r2-r2mean)*(r2-r2mean);
					sum2g += (g2-g2mean)*(g2-g2mean);
					sum2b += (b2-b2mean)*(b2-b2mean);
				}
			}
		}

		ncc = sum / ((sum1r * sum2r) + (sum1g * sum2g) + (sum1b * sum2b));

		return ncc;
	}

}

double MutualInfo::info(int width, int height, 
                        unsigned char *target, unsigned char *render, 
                        int startx, int endx, 
                        int starty, int endy) {
  histogram(width, height, target, render, startx, endx, starty, endy);

  memset(histoA, 0, nbins*sizeof(int));
  memset(histoB, 0, nbins*sizeof(int));
  double n = 0.0;

  int i = 0;
  for(unsigned int y = 0; y < nbins; y++) {
    unsigned int &b = histoB[y];
    for(unsigned int x = 0; x < nbins; x++) {
      int ab = histo2D[i++];
      histoA[x] += ab;
      b += ab;
//      if(ab != 0) cout << "1";
//      else cout << "0";
    }
//    cout << endl;
    n += b;
  }
  //cout << endl;
  double ILOG2 = 1/log(2.0);
  //assert(n > 0);
  if(n == 0) n = 1;
  double m = 0.0;
  for(unsigned int y = 0; y < nbins; y++) {
    double b = histoB[y];
    if(b == 0) continue;
    for(unsigned int x = 0; x < nbins; x++) {
      double ab = histo2D[x + nbins*y];
      if(ab == 0) continue;
      double a = histoA[x];
      m += ab * log((n*ab)/(a*b))*ILOG2;
    }
  }
  m /= n;

  #ifdef USE_MUTUAL
  for(int x = 0; x < bins; x++) {
    double a = histoA[x]/n;
    double b = histoB[x]/n;
    if(a > 0)
    m -= a*log(a);
    if(b > 0) m -= b*log(b);
  }
  #endif
  return m;
}

void MutualInfo::histogram(int width, int height,
                           unsigned char *target, unsigned char *render, 
                           int startx, int endx,
                           int starty, int endy) {
  if(endx == 0) endx = width;
  if(endy == 0) endy = height;
  memset(histo2D, 0, nbins*nbins*sizeof(int));
  int side = 256/nbins;
  assert(!(side & (side-1)));

  //k is the log2 of side
  int k = 0;
  while ( side>>=1 ) { ++k; }

  int bins = nbins;
  int s = 0; 
  while ( bins>>=1) { ++s; }

  for(int y = starty; y < endy; y++) {
    int offset = width*y + startx;
    for(int x = startx; x < endx; x++, offset++) {
      unsigned char a = target[offset]>>k; //instead of /side;
      unsigned char b = render[offset]>>k; //instead of /side;
      histo2D[a + (b<<s)] += 2;//bweight; //instead of nbins*s
    }
  }
  //weight of background is divided.
  //background is when b = 0 -> first row of histo2D
  if(bweight != 0) {
    for(unsigned int i = 0; i < nbins; i++)
      histo2D[i] /= bweight;
  } else
    memset(histo2D, 0, nbins*sizeof(int));

  /*unsigned char buffer[nbins*nbins*4];
  for(int i = 0; i < nbins*nbins; i++) {
    buffer[i*4] = buffer[i*4+1] = buffer[i*4+2] = (unsigned char)(10*log(histo2D[i]));
    buffer[i*4+3] = 0;
  }
  QImage im(buffer, nbins, nbins, QImage::Format_ARGB32);
  im.save("prova.jpg");*/
}

