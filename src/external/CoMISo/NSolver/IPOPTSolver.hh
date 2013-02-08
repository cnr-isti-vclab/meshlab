//=============================================================================
//
//  CLASS IPOPTSolver
//
//=============================================================================


#ifndef COMISO_IPOPTSOLVER_HH
#define COMISO_IPOPTSOLVER_HH


//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_IPOPT_AVAILABLE

//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include <CoMISo/Utils/StopWatch.hh>
#include <vector>
#include <cstddef>
#include <gmm/gmm.h>
#include "NProblemGmmInterface.hh"
#include "NProblemInterface.hh"
#include "NConstraintInterface.hh"
#include "BoundConstraint.hh"
#include <IpTNLP.hpp>
#include <IpIpoptApplication.hpp>
#include <IpSolveStatistics.hpp>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      

/** \class NewtonSolver NewtonSolver.hh <ACG/.../NewtonSolver.hh>

    Brief Description.
  
    A more elaborate description follows.
*/
class COMISODLLEXPORT IPOPTSolver
{
public:
   
  /// Default constructor -> set up IpOptApplication
  IPOPTSolver();
 
  /// Destructor
  ~IPOPTSolver() {}

  // ********** SOLVE **************** //
  // solve -> returns ipopt status code
//------------------------------------------------------
//  enum ApplicationReturnStatus
//    {
//      Solve_Succeeded=0,
//      Solved_To_Acceptable_Level=1,
//      Infeasible_Problem_Detected=2,
//      Search_Direction_Becomes_Too_Small=3,
//      Diverging_Iterates=4,
//      User_Requested_Stop=5,
//      Feasible_Point_Found=6,
//
//      Maximum_Iterations_Exceeded=-1,
//      Restoration_Failed=-2,
//      Error_In_Step_Computation=-3,
//      Maximum_CpuTime_Exceeded=-4,
//      Not_Enough_Degrees_Of_Freedom=-10,
//      Invalid_Problem_Definition=-11,
//      Invalid_Option=-12,
//      Invalid_Number_Detected=-13,
//
//      Unrecoverable_Exception=-100,
//      NonIpopt_Exception_Thrown=-101,
//      Insufficient_Memory=-102,
//      Internal_Error=-199
//    };
//------------------------------------------------------

  int solve(NProblemInterface*    _problem, const std::vector<NConstraintInterface*>& _constraints);

  // same as above with additional lazy constraints that are only added iteratively to the problem if not satisfied
  int solve(NProblemInterface*                        _problem,
            const std::vector<NConstraintInterface*>& _constraints,
            const std::vector<NConstraintInterface*>& _lazy_constraints,
            const double                              _almost_infeasible = 0.5,
            const int                                 _max_passes        = 5   );


  // for convenience, if no constraints are given
  int solve(NProblemInterface*    _problem);

  // deprecated interface for backwards compatibility
  int solve(NProblemGmmInterface* _problem, std::vector<NConstraintInterface*>& _constraints);

  // ********* CONFIGURATION ********************* //
  // access the ipopt-application (for setting parameters etc.)
  // example: app().Options()->SetIntegerValue("max_iter", 100);
  Ipopt::IpoptApplication& app() {return (*app_); }


protected:
  double* P(std::vector<double>& _v)
  {
    if( !_v.empty())
      return ((double*)&_v[0]);
    else
      return 0;
  }

private:

  // smart pointer to IpoptApplication to set options etc.
  Ipopt::SmartPtr<Ipopt::IpoptApplication> app_;
};


//== CLASS DEFINITION PROBLEM INSTANCE=========================================================


class NProblemIPOPT : public Ipopt::TNLP
{
public:

  // Ipopt Types
  typedef Ipopt::Number                    Number;
  typedef Ipopt::Index                     Index;
  typedef Ipopt::SolverReturn              SolverReturn;
  typedef Ipopt::IpoptData                 IpoptData;
  typedef Ipopt::IpoptCalculatedQuantities IpoptCalculatedQuantities;

  // sparse matrix and vector types
  typedef NConstraintInterface::SVectorNC SVectorNC;
  typedef NConstraintInterface::SMatrixNC SMatrixNC;
  typedef NProblemInterface::SMatrixNP    SMatrixNP;

  /** default constructor */
  NProblemIPOPT(NProblemInterface* _problem, const std::vector<NConstraintInterface*>& _constraints)
   : problem_(_problem), store_solution_(false) { split_constraints(_constraints); analyze_special_properties(_problem, _constraints);}

  /** default destructor */
  virtual ~NProblemIPOPT() {};

  /**@name Overloaded from TNLP */
  //@{
  /** Method to return some info about the nlp */
  virtual bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                            Index& nnz_h_lag, IndexStyleEnum& index_style);

  /** Method to return the bounds for my problem */
  virtual bool get_bounds_info(Index n, Number* x_l, Number* x_u,
                               Index m, Number* g_l, Number* g_u);

  /** Method to return the starting point for the algorithm */
  virtual bool get_starting_point(Index n, bool init_x, Number* x,
                                  bool init_z, Number* z_L, Number* z_U,
                                  Index m, bool init_lambda,
                                  Number* lambda);

  /** Method to return the objective value */
  virtual bool eval_f(Index n, const Number* x, bool new_x, Number& obj_value);

  /** Method to return the gradient of the objective */
  virtual bool eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f);

  /** Method to return the constraint residuals */
  virtual bool eval_g(Index n, const Number* x, bool new_x, Index m, Number* g);

  /** Method to return:
   *   1) The structure of the jacobian (if "values" is NULL)
   *   2) The values of the jacobian (if "values" is not NULL)
   */
  virtual bool eval_jac_g(Index n, const Number* x, bool new_x,
                          Index m, Index nele_jac, Index* iRow, Index *jCol,
                          Number* values);

  /** Method to return:
   *   1) The structure of the hessian of the lagrangian (if "values" is NULL)
   *   2) The values of the hessian of the lagrangian (if "values" is not NULL)
   */
  virtual bool eval_h(Index n, const Number* x, bool new_x,
                      Number obj_factor, Index m, const Number* lambda,
                      bool new_lambda, Index nele_hess, Index* iRow,
                      Index* jCol, Number* values);

  //@}

  /** @name Solution Methods */
  //@{
  /** This method is called when the algorithm is complete so the TNLP can store/write the solution */
  virtual void finalize_solution(SolverReturn status,
                                 Index n, const Number* x, const Number* z_L, const Number* z_U,
                                 Index m, const Number* g, const Number* lambda,
                                 Number obj_value,
                                 const IpoptData* ip_data,
                                 IpoptCalculatedQuantities* ip_cq);
  //@}

  // special properties of problem
  bool hessian_constant() const;
  bool jac_c_constant() const;
  bool jac_d_constant() const;

  bool&                 store_solution()  {return store_solution_; }
  std::vector<double>&  solution()        {return x_;}

private:
  /**@name Methods to block default compiler methods.
   * The compiler automatically generates the following three methods.
   *  Since the default compiler implementation is generally not what
   *  you want (for all but the most simple classes), we usually
   *  put the declarations of these methods in the private section
   *  and never implement them. This prevents the compiler from
   *  implementing an incorrect "default" behavior without us
   *  knowing. (See Scott Meyers book, "Effective C++")
   *
   */
  //@{
  //  MyNLP();
  NProblemIPOPT(const NProblemIPOPT&);
  NProblemIPOPT& operator=(const NProblemIPOPT&);
  //@}

  // split user-provided constraints into general-constraints and bound-constraints
  void split_constraints(const std::vector<NConstraintInterface*>& _constraints);

  // determine if hessian_constant, jac_c_constant or jac_d_constant
  void analyze_special_properties(const NProblemInterface* _problem, const std::vector<NConstraintInterface*>& _constraints);


protected:
  double* P(std::vector<double>& _v)
  {
    if( !_v.empty())
      return ((double*)&_v[0]);
    else
      return 0;
  }

private:

  // pointer to problem instance
  NProblemInterface* problem_;
  // reference to constraints vector
  std::vector<NConstraintInterface*> constraints_;
  std::vector<BoundConstraint*>      bound_constraints_;

  bool hessian_constant_;
  bool jac_c_constant_;
  bool jac_d_constant_;

  bool store_solution_;
  std::vector<double> x_;
};


//== CLASS DEFINITION PROBLEM INSTANCE=========================================================


class NProblemGmmIPOPT : public Ipopt::TNLP
{
public:

  // Ipopt Types
  typedef Ipopt::Number                    Number;
  typedef Ipopt::Index                     Index;
  typedef Ipopt::SolverReturn              SolverReturn;
  typedef Ipopt::IpoptData                 IpoptData;
  typedef Ipopt::IpoptCalculatedQuantities IpoptCalculatedQuantities;

  // sparse matrix and vector types
  typedef NConstraintInterface::SVectorNC SVectorNC;
  typedef NConstraintInterface::SMatrixNC SMatrixNC;
  typedef gmm::wsvector<double>           SVectorNP;
  typedef NProblemGmmInterface::SMatrixNP SMatrixNP;

  typedef gmm::array1D_reference<       double* > VectorPT;
  typedef gmm::array1D_reference< const double* > VectorPTC;

  typedef gmm::array1D_reference<       Index* > VectorPTi;
  typedef gmm::array1D_reference< const Index* > VectorPTCi;

  typedef gmm::linalg_traits<SVectorNP>::const_iterator SVectorNP_citer;
  typedef gmm::linalg_traits<SVectorNP>::iterator       SVectorNP_iter;

  /** default constructor */
  NProblemGmmIPOPT(NProblemGmmInterface* _problem, std::vector<NConstraintInterface*>& _constraints)
   : problem_(_problem), constraints_(_constraints), nnz_jac_g_(0), nnz_h_lag_(0) 
   {}

  /** default destructor */
  virtual ~NProblemGmmIPOPT() {};

  /**@name Overloaded from TNLP */
  //@{
  /** Method to return some info about the nlp */
  virtual bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                            Index& nnz_h_lag, IndexStyleEnum& index_style);

  /** Method to return the bounds for my problem */
  virtual bool get_bounds_info(Index n, Number* x_l, Number* x_u,
                               Index m, Number* g_l, Number* g_u);

  /** Method to return the starting point for the algorithm */
  virtual bool get_starting_point(Index n, bool init_x, Number* x,
                                  bool init_z, Number* z_L, Number* z_U,
                                  Index m, bool init_lambda,
                                  Number* lambda);

  /** Method to return the objective value */
  virtual bool eval_f(Index n, const Number* x, bool new_x, Number& obj_value);

  /** Method to return the gradient of the objective */
  virtual bool eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f);

  /** Method to return the constraint residuals */
  virtual bool eval_g(Index n, const Number* x, bool new_x, Index m, Number* g);

  /** Method to return:
   *   1) The structure of the jacobian (if "values" is NULL)
   *   2) The values of the jacobian (if "values" is not NULL)
   */
  virtual bool eval_jac_g(Index n, const Number* x, bool new_x,
                          Index m, Index nele_jac, Index* iRow, Index *jCol,
                          Number* values);

  /** Method to return:
   *   1) The structure of the hessian of the lagrangian (if "values" is NULL)
   *   2) The values of the hessian of the lagrangian (if "values" is not NULL)
   */
  virtual bool eval_h(Index n, const Number* x, bool new_x,
                      Number obj_factor, Index m, const Number* lambda,
                      bool new_lambda, Index nele_hess, Index* iRow,
                      Index* jCol, Number* values);

  //@}

  /** @name Solution Methods */
  //@{
  /** This method is called when the algorithm is complete so the TNLP can store/write the solution */
  virtual void finalize_solution(SolverReturn status,
                                 Index n, const Number* x, const Number* z_L, const Number* z_U,
                                 Index m, const Number* g, const Number* lambda,
                                 Number obj_value,
                                 const IpoptData* ip_data,
                                 IpoptCalculatedQuantities* ip_cq);
  //@}

private:
  /**@name Methods to block default compiler methods.
   * The compiler automatically generates the following three methods.
   *  Since the default compiler implementation is generally not what
   *  you want (for all but the most simple classes), we usually
   *  put the declarations of these methods in the private section
   *  and never implement them. This prevents the compiler from
   *  implementing an incorrect "default" behavior without us
   *  knowing. (See Scott Meyers book, "Effective C++")
   *
   */
  //@{
  //  MyNLP();
  NProblemGmmIPOPT(const NProblemGmmIPOPT&);
  NProblemGmmIPOPT& operator=(const NProblemGmmIPOPT&);
  //@}


private:

  // pointer to problem instance
  NProblemGmmInterface* problem_;
  // reference to constraints vector
  std::vector<NConstraintInterface*>& constraints_;

  int nnz_jac_g_;
  int nnz_h_lag_;

  // constant structure of jacobian_of_constraints and hessian_of_lagrangian
  std::vector<Index> jac_g_iRow_;
  std::vector<Index> jac_g_jCol_;
  std::vector<Index> h_lag_iRow_;
  std::vector<Index> h_lag_jCol_;

  // Sparse Matrix of problem (don't initialize every time!!!)
  SMatrixNP HP_;
};


//=============================================================================
} // namespace COMISO

//=============================================================================
#endif // COMISO_IPOPT_AVAILABLE
//=============================================================================
#endif // ACG_IPOPTSOLVER_HH defined
//=============================================================================

