#ifndef VCG_SCALARIMAGE_H
#define VCG_SCALARIMAGE_H
#include <assert.h>
#include <vcg/space/color4.h>
/*
Very simple class to store a bitmap of floating point values.
*/
template <class ScalarType> 
class ScalarImage
{
public:
  std::vector<ScalarType> v;
  int w,h;
  void resize(int _w, int _h) { w=_w; h=_h; v.resize(w*h);}
  bool Open(const char *filename);
  ScalarType &Val(int x,int y) {
    assert(x>=0 && x<w);
    assert(y>=0 && y<h);
    return v[y*w+x];
  };

  QImage convertToQImage();
  ScalarImage(QImage img);
  ScalarImage(){};
  bool Subsample(const int factor, ScalarImage<ScalarType> &fli);
  static QPixmap colorizedScaledToHeight(const int desiredH, ScalarImage<ScalarType> &fli, float colormax=10)
  {
    assert(fli.h>desiredH);
    int factor =  fli.h / desiredH;
     
    int newW=fli.w/factor -1;
    int newH=fli.h/factor -1;
    QImage newImage(newW,newH,QImage::Format_RGB32);
     
    for(int i=0;i<newImage.height();++i)
        for(int j=0;j<newImage.width();++j)
        {
          float sum=0;
          for(int si=0;si<factor;++si)
            for(int sj=0;sj<factor;++sj)
                sum+=  fli.Val(j*factor+sj,i*factor+si);
          sum/=factor*factor;
					sum=std::min(sum,colormax);
					vcg::Color4b avgcolor; avgcolor.ColorRamp(0,colormax,sum);
          newImage.setPixel(j,i,qRgb(avgcolor.V(0),avgcolor.V(1),avgcolor.V(2)));
        }
        return QPixmap::fromImage(newImage);
};

};

typedef ScalarImage<float> FloatImage;
typedef ScalarImage<unsigned char> CharImage;


#endif
