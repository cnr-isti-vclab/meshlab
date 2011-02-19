#ifndef __VSCAN_H__
#define __VSCAN_H__

class Vscan
{
  public:
    
    Vscan(int _sx, int _sy);

    ~Vscan();
    
    bool ready(){ return initDone; }
    
    bool init();
    
    // gets parameters from current GL status
    void getFromGl();
    
    // renders to screen as a 1:1 texutre
    void splash(bool depth=false);

    // set as comparison texture
    void useAsSource();

    // set as destination for renderings
    void useAsDest();
    
    // fills a buffer (must be allocated with sizeX() * sizeY() floats )
    void fillDepthBuffer(float* buf) const;

    int sizeX() const { return sx; }
    int sizeY() const { return sy; }

  private:

    int    qualityID; // index of quality map
    int    depthID;   // index of depth map
    int    frameID;

    bool checkValid();
    bool initDone;
    int sx, sy; // size

};

// utility functions;
void useScreenAsDest();
void useDefaultShader();
void setDepthPeelingTolerance(float t);
void setDepthPeelingSize(const Vscan & scan);

#endif
