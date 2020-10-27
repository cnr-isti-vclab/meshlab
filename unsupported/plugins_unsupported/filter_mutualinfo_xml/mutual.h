#ifndef MUTUAL_INFORMATION_H
#define MUTUAL_INFORMATION_H

class MutualInfo {
 public:
  int bweight;
  bool use_background;
  //bweight: weithg of boundary pixels in mutual information (1/bweight is weighting function)
  MutualInfo(unsigned int nbins = 128, int bweight = 2, bool use_background = true);
  ~MutualInfo();

  void setBins(unsigned int nbins);
  double info(int width, int height, unsigned char *target, unsigned char *render, 
              int startx = 0, int endx = 0, int starty = 0, int endy = 0);
  void histogram(int width, int height, unsigned char *target, unsigned char *render, 
                 int startx = 0, int endx = 0, int starty = 0, int endy = 0);

 private:
  unsigned int nbins;
  unsigned int *histo2D; //matrix nbisXnbins
  unsigned int *histoA;  //vector nbins
  unsigned int *histoB;
};


#endif
