//=============================================================================
//
//  CLASS NewtonSolver - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "NewtonSolver.hh"
#include <CoMISo/Solver/CholmodSolver.hh>

//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ========================================================== 


// solve
int
NewtonSolver::
solve(NProblemGmmInterface* _problem)
{
#if COMISO_SUITESPARSE_AVAILABLE  
  
  // get problem size
  int n = _problem->n_unknowns();

  // hesse matrix
  NProblemGmmInterface::SMatrixNP H;
  // gradient
  std::vector<double> x(n), x_new(n), dx(n), g(n);

  // get initial x, initial grad and initial f
  _problem->initial_x(P(x));
  double f = _problem->eval_f(P(x));

  double reg = 1e-3;
  COMISO::CholmodSolver chol;

  for(int i=0; i<max_iter_; ++i)
  {
    _problem->eval_gradient(P(x), P(g));
    // check for convergence
    if( gmm::vect_norm2(g) < convergence_eps_)
    {
      std::cerr << "Newton Solver converged after "
                << i << " iterations" << std::endl;
      _problem->store_result(P(x));
      return true;
    }

    // get current hessian
    _problem->eval_hessian(P(x), H);

    // regularize
    double reg_comp = reg*gmm::mat_trace(H)/double(n);
    for(int j=0; j<n; ++j)
      H(j,j) += reg_comp;

    // solve linear system
    bool factorization_ok = false;
    if(constant_hessian_structure_ && i != 0)
      factorization_ok = chol.update_system_gmm(H);
    else
      factorization_ok = chol.calc_system_gmm(H);

    bool improvement = false;
    if(factorization_ok)
      if(chol.solve( dx, g))
      {
        gmm::add(x, gmm::scaled(dx,-1.0),x_new);
        double f_new = _problem->eval_f(P(x_new));

        if( f_new < f)
        {
          // swap x and x_new (and f and f_new)
          x_new.swap(x);
          f = f_new;
          improvement = true;

          std::cerr << "energy improved to " << f << std::endl;
        }
      }

    // adapt regularization
    if(improvement)
    {
      if(reg > 1e-9)
        reg *= 0.1;
    }
    else
    {
      if(reg < 1e4)
        reg *= 10.0;
      else
      {
        _problem->store_result(P(x));
        std::cerr << "Newton solver reached max regularization but did not converge ... " << std::endl;
        return false;
      }
    }
  }
  _problem->store_result(P(x));
  std::cerr << "Newton Solver did not converge!!! after "
            << max_iter_ << " iterations." << std::endl;
  return false;

#else
  std::cerr << "Warning: NewtonSolver requires not-available CholmodSolver...\n";
  return false;
#endif	    
}


//-----------------------------------------------------------------------------



//=============================================================================
} // namespace COMISO
//=============================================================================
