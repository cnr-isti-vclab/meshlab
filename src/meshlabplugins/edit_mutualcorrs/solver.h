#ifndef SOLVER_H
#define SOLVER_H

#include <QTextStream>
#include "alignset.h"

#include "parameters.h"
#include "../../external/levmar-2.3/lm.h"

#include <iostream>
#include <fstream>

//These values changed from levmar definitions
#define LM_INIT_MU     1E-03
#define LM_STOP_THRESH 1E-17 
#define LM_CONV_THRESH 1E-05
#define LM_DIFF_DELTA  1E-06
#define LM_MAX_ITER    100


class AlignSet;
class MutualInfo;

class Solver {
  typedef vcg::Shot<float> Shot;
  typedef vcg::Box3<float> Box;

 public:
  AlignSet *align;
  MutualInfo *mutual;
  Parameters p;

  bool optimize_focal;  //if true also focal value is optimized
  
  double variance;   // 1/10th of the expected variance in the parameters
  double tolerance;  // newuoa terminates when minimum is closest then tolerance
  int maxiter;       // max number of function evaluations in NEWUOA

  double mIweight;  //need to weight the MI function and the error function of the correspondences

  double start, end; //starting and ending value of the function
  int f_evals;       //number of function evaluations
  int f_evals_total; //total number of function evaluations (for test)

  std::ofstream myfile;  //for debugging

  Solver();
  double operator()(int ndim, double *x);
  int optimize(AlignSet *align, MutualInfo *mutual, Shot &shot);
  
  static void value(double *p, double *x, int m, int n, void *data);
  int levmar(AlignSet *align, MutualInfo *mutual, Shot &shot); //never used

  bool tsai(AlignSet *align, Shot &shot);
  bool levmar(AlignSet *align,Shot &shot);

 private:
  double opts[5]; //0 -> initial mu              
                  //1 -> minimum JTe             
                  //2 -> minimum Dp              
                  //3 -> minimum sum(d*d)        
                  //4 -> delta for finite differe
  double info[LM_INFO_SZ];

  double calculateError2( Shot &shot);
  
};

#endif
