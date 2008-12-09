
#include <vector>
#include <map>
// using namespace std;
//#include <vil/vil_warp.h>

class RadialDistortion // : public vil_warp_mapping
{
public:
  RadialDistortion() { }
  RadialDistortion(const RadialDistortion &obj):
      k_(obj.k_), ocx_(obj.ocx_), ocy_(obj.ocy_), ncx_(obj.ncx_), ncy_(obj.ncy_), lookup_(obj.lookup_), max_(obj.max_), resolution_(obj.resolution_) {}
  
  void SetParameters(std::vector<double>& k, double max = 2000, int resolution = 10000);
  
  std::vector<double> GetParameters() { return k_; };
  
  void ComputeNewXY(double xo, double yo, double& xn, double& yn) const;
  void ComputeOldXY(double xn, double yn, double& xo, double& yo) const;
  
  // for vil_warp, which doesn't work
  void forward_map(double x1, double y1, double* x2, double* y2) const;
  void inverse_map(double x2, double y2, double* x1, double* y1) const;
  
protected:
  std::vector<double> k_;
  double ocx_;
  double ocy_;
  double ncx_;
  double ncy_;
  
  void SetupLookupTable(double max, int resolution);
  
  std::map<double, double>  lookup_;
  
  double max_;
  int resolution_;
};
