

class EpochCamera
{
public:
  Matrix33d K;      // parametri intriseci camera
  Matrix33d Kinv;   

  vector<double> k;
  Matrix33d R;
  Matrix44d TR; // [R | -Rt] e.g. la matrice in cui
  Matrix44d TRinv;
  Point3d t;
  int width, height;

  RadialDistortion rd;

  void DepthTo3DPoint(double x, double y, double depth, Point3d &M) const;

  bool Open(const char * filename);
};

