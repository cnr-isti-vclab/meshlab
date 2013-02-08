//=============================================================================
//
//  CLASS IPOPTSolver - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_IPOPT_AVAILABLE
//=============================================================================


#include "IPOPTSolver.hh"

//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ========================================================== 


// Constructor
IPOPTSolver::
IPOPTSolver()
{
  // Create an instance of the IpoptApplication
  app_ = IpoptApplicationFactory();

  // Switch to HSL if available in Comiso
  #if COMISO_HSL_AVAILABLE
    app_->Options()->SetStringValue("linear_solver", "ma57");
  #endif

  // Restrict memory to be able to run larger problems on windows
  // with the default mumps solver
  #ifdef WIN32
    app_->Options()->SetIntegerValue("mumps_mem_percent", 5);
  #endif

  // set default parameters
  app_->Options()->SetIntegerValue("max_iter", 100);
  //  app->Options()->SetStringValue("derivative_test", "second-order");
  //  app->Options()->SetIntegerValue("print_level", 0);
  //  app->Options()->SetStringValue("expect_infeasible_problem", "yes");

}


//-----------------------------------------------------------------------------



int
IPOPTSolver::
solve(NProblemInterface* _problem, const std::vector<NConstraintInterface*>& _constraints)
{
  //----------------------------------------------------------------------------
  // 1. Create an instance of IPOPT NLP
  //----------------------------------------------------------------------------
  Ipopt::SmartPtr<Ipopt::TNLP> np = new NProblemIPOPT(_problem, _constraints);
  NProblemIPOPT* np2 = dynamic_cast<NProblemIPOPT*> (Ipopt::GetRawPtr(np));

  //----------------------------------------------------------------------------
  // 2. exploit special characteristics of problem
  //----------------------------------------------------------------------------

  std::cerr << "exploit detected special properties: ";
  if(np2->hessian_constant())
  {
    std::cerr << "*constant hessian* ";
    app().Options()->SetStringValue("hessian_constant", "yes");
  }

  if(np2->jac_c_constant())
  {
    std::cerr << "*constant jacobian of equality constraints* ";
    app().Options()->SetStringValue("jac_c_constant", "yes");
  }

  if(np2->jac_d_constant())
  {
    std::cerr << "*constant jacobian of in-equality constraints*";
    app().Options()->SetStringValue("jac_d_constant", "yes");
  }
  std::cerr << std::endl;

  //----------------------------------------------------------------------------
  // 3. solve problem
  //----------------------------------------------------------------------------

  // Initialize the IpoptApplication and process the options
  Ipopt::ApplicationReturnStatus status;
  status = app_->Initialize();
  if (status != Ipopt::Solve_Succeeded)
  {
    printf("\n\n*** Error IPOPT during initialization!\n");
  }

  status = app_->OptimizeTNLP( np);

  //----------------------------------------------------------------------------
  // 4. output statistics
  //----------------------------------------------------------------------------
  if (status == Ipopt::Solve_Succeeded || status == Ipopt::Solved_To_Acceptable_Level)
  {
    // Retrieve some statistics about the solve
    Ipopt::Index iter_count = app_->Statistics()->IterationCount();
    printf("\n\n*** IPOPT: The problem solved in %d iterations!\n", iter_count);

    Ipopt::Number final_obj = app_->Statistics()->FinalObjective();
    printf("\n\n*** IPOPT: The final value of the objective function is %e.\n", final_obj);
  }

  return status;
}


//-----------------------------------------------------------------------------



int
IPOPTSolver::
solve(NProblemInterface*                        _problem,
      const std::vector<NConstraintInterface*>& _constraints,
      const std::vector<NConstraintInterface*>& _lazy_constraints,
      const double                              _almost_infeasible,
      const int                                 _max_passes        )
{
  //----------------------------------------------------------------------------
  // 0. Initialize IPOPT Applicaiton
  //----------------------------------------------------------------------------

  StopWatch sw; sw.start();

  // Initialize the IpoptApplication and process the options
  Ipopt::ApplicationReturnStatus status;
  status = app_->Initialize();
  if (status != Ipopt::Solve_Succeeded)
  {
    printf("\n\n*** Error IPOPT during initialization!\n");
  }

  bool feasible_point_found = false;
  int  cur_pass = 0;
  double acceptable_tolerance = 0.01; // hack: read out from ipopt!!!
  // copy default constraints
  std::vector<NConstraintInterface*> constraints = _constraints;
  std::vector<bool> lazy_added(_lazy_constraints.size(),false);

  // cache statistics of all iterations
  std::vector<int> n_inf;
  std::vector<int> n_almost_inf;

  while(!feasible_point_found && cur_pass <(_max_passes-1))
  {
    ++cur_pass;
    //----------------------------------------------------------------------------
    // 1. Create an instance of current IPOPT NLP
    //----------------------------------------------------------------------------
    Ipopt::SmartPtr<Ipopt::TNLP> np = new NProblemIPOPT(_problem, constraints);
    NProblemIPOPT* np2 = dynamic_cast<NProblemIPOPT*> (Ipopt::GetRawPtr(np));
    // enable caching of solution
    np2->store_solution() = true;

    //----------------------------------------------------------------------------
    // 2. exploit special characteristics of problem
    //----------------------------------------------------------------------------

    std::cerr << "detected special properties which will be exploit: ";
    if(np2->hessian_constant())
    {
      std::cerr << "*constant hessian* ";
      app().Options()->SetStringValue("hessian_constant", "yes");
    }

    if(np2->jac_c_constant())
    {
      std::cerr << "*constant jacobian of equality constraints* ";
      app().Options()->SetStringValue("jac_c_constant", "yes");
    }

    if(np2->jac_d_constant())
    {
      std::cerr << "*constant jacobian of in-equality constraints*";
      app().Options()->SetStringValue("jac_d_constant", "yes");
    }
    std::cerr << std::endl;

    //----------------------------------------------------------------------------
    // 3. solve problem
    //----------------------------------------------------------------------------
    status = app_->OptimizeTNLP( np);

    // check lazy constraints
    n_inf.push_back(0);
    n_almost_inf.push_back(0);
    feasible_point_found = true;
    for(unsigned int i=0; i<_lazy_constraints.size(); ++i)
      if(!lazy_added[i])
      {
        NConstraintInterface* lc = _lazy_constraints[i];

        double v = lc->eval_constraint(&(np2->solution()[0]));

        bool inf        = false;
        bool almost_inf = false;

        if(lc->constraint_type() == NConstraintInterface::NC_EQUAL)
        {
          v = std::abs(v);
          if(v>acceptable_tolerance)
            inf = true;
          else
            if(v>_almost_infeasible)
              almost_inf = true;
        }
        else
          if(lc->constraint_type() == NConstraintInterface::NC_GREATER_EQUAL)
          {
            if(v<-acceptable_tolerance)
              inf = true;
            else
              if(v<_almost_infeasible)
                almost_inf = true;
          }
          else
            if(lc->constraint_type() == NConstraintInterface::NC_LESS_EQUAL)
            {
              if(v>acceptable_tolerance)
                inf = true;
              else
                if(v>-_almost_infeasible)
                  almost_inf = true;
            }

        // infeasible?
        if(inf)
        {
          constraints.push_back(lc);
          lazy_added[i] = true;
          feasible_point_found = false;
          ++n_inf.back();
        }

        // almost violated or violated? -> add to constraints
        if(almost_inf)
        {
          constraints.push_back(lc);
          lazy_added[i] = true;
          ++n_almost_inf.back();
        }
      }
  }

  // no termination after max number of passes?
  if(!feasible_point_found)
  {
    ++cur_pass;

    std::cerr << "*************** could not find feasible point after " << _max_passes-1 << " -> solving with all lazy constraints..." << std::endl;
    for(unsigned int i=0; i<_lazy_constraints.size(); ++i)
      if(!lazy_added[i])
        constraints.push_back(_lazy_constraints[i]);

    //----------------------------------------------------------------------------
    // 1. Create an instance of current IPOPT NLP
    //----------------------------------------------------------------------------
    Ipopt::SmartPtr<Ipopt::TNLP> np = new NProblemIPOPT(_problem, constraints);
    NProblemIPOPT* np2 = dynamic_cast<NProblemIPOPT*> (Ipopt::GetRawPtr(np));
    // enable caching of solution
    np2->store_solution() = true;

    //----------------------------------------------------------------------------
    // 2. exploit special characteristics of problem
    //----------------------------------------------------------------------------

    std::cerr << "exploit detected special properties: ";
    if(np2->hessian_constant())
    {
      std::cerr << "*constant hessian* ";
      app().Options()->SetStringValue("hessian_constant", "yes");
    }

    if(np2->jac_c_constant())
    {
      std::cerr << "*constant jacobian of equality constraints* ";
      app().Options()->SetStringValue("jac_c_constant", "yes");
    }

    if(np2->jac_d_constant())
    {
      std::cerr << "*constant jacobian of in-equality constraints*";
      app().Options()->SetStringValue("jac_d_constant", "yes");
    }
    std::cerr << std::endl;

    //----------------------------------------------------------------------------
    // 3. solve problem
    //----------------------------------------------------------------------------
    status = app_->OptimizeTNLP( np);
  }

  const double overall_time = sw.stop()/1000.0;

  //----------------------------------------------------------------------------
  // 4. output statistics
  //----------------------------------------------------------------------------
  if (status == Ipopt::Solve_Succeeded || status == Ipopt::Solved_To_Acceptable_Level)
  {
    // Retrieve some statistics about the solve
    Ipopt::Index iter_count = app_->Statistics()->IterationCount();
    printf("\n\n*** IPOPT: The problem solved in %d iterations!\n", iter_count);

    Ipopt::Number final_obj = app_->Statistics()->FinalObjective();
    printf("\n\n*** IPOPT: The final value of the objective function is %e.\n", final_obj);
  }

  std::cerr <<"############# IPOPT with lazy constraints statistics ###############" << std::endl;
  std::cerr << "overall time: " << overall_time << "s" << std::endl;
  std::cerr << "#passes     : " << cur_pass << "( of " << _max_passes << ")" << std::endl;
  for(unsigned int i=0; i<n_inf.size(); ++i)
    std::cerr << "pass " << i << " induced " << n_inf[i] << " infeasible and " << n_almost_inf[i] << " almost infeasible" << std::endl;

  return status;
}


//-----------------------------------------------------------------------------


int
IPOPTSolver::
solve(NProblemInterface*    _problem)
{
  std::vector<NConstraintInterface*> constraints;
  return this->solve(_problem, constraints);
}


//-----------------------------------------------------------------------------


int
IPOPTSolver::
solve(NProblemGmmInterface* _problem, std::vector<NConstraintInterface*>& _constraints)
{
  std::cerr << "****** Warning: NProblemGmmInterface is deprecated!!! -> use NProblemInterface *******\n";

  //----------------------------------------------------------------------------
  // 1. Create an instance of IPOPT NLP
  //----------------------------------------------------------------------------
  Ipopt::SmartPtr<Ipopt::TNLP> np = new NProblemGmmIPOPT(_problem, _constraints);

  //----------------------------------------------------------------------------
  // 2. solve problem
  //----------------------------------------------------------------------------

  // Initialize the IpoptApplication and process the options
  Ipopt::ApplicationReturnStatus status;
  status = app_->Initialize();
  if (status != Ipopt::Solve_Succeeded)
  {
    printf("\n\n*** Error IPOPT during initialization!\n");
  }

  //----------------------------------------------------------------------------
  // 3. solve problem
  //----------------------------------------------------------------------------
  status = app_->OptimizeTNLP(np);

  //----------------------------------------------------------------------------
  // 4. output statistics
  //----------------------------------------------------------------------------
  if (status == Ipopt::Solve_Succeeded || status == Ipopt::Solved_To_Acceptable_Level)
  {
    // Retrieve some statistics about the solve
    Ipopt::Index iter_count = app_->Statistics()->IterationCount();
    printf("\n\n*** IPOPT: The problem solved in %d iterations!\n", iter_count);

    Ipopt::Number final_obj = app_->Statistics()->FinalObjective();
    printf("\n\n*** IPOPT: The final value of the objective function is %e.\n", final_obj);
  }

  return status;
}


//== IMPLEMENTATION PROBLEM INSTANCE==========================================================


void
NProblemIPOPT::
split_constraints(const std::vector<NConstraintInterface*>& _constraints)
{
  // split user-provided constraints into general-constraints and bound-constraints
  constraints_      .clear();       constraints_.reserve(_constraints.size());
  bound_constraints_.clear(); bound_constraints_.reserve(_constraints.size());

  for(unsigned int i=0; i<_constraints.size(); ++i)
  {
    BoundConstraint* bnd_ptr = dynamic_cast<BoundConstraint*>(_constraints[i]);

    if(bnd_ptr)
      bound_constraints_.push_back(bnd_ptr);
    else
      constraints_.push_back(_constraints[i]);
  }
}


//-----------------------------------------------------------------------------


void
NProblemIPOPT::
analyze_special_properties(const NProblemInterface* _problem, const std::vector<NConstraintInterface*>& _constraints)
{
  hessian_constant_ = true;
  jac_c_constant_   = true;
  jac_d_constant_   = true;

  if(!_problem->constant_hessian())
    hessian_constant_ = false;

  for(unsigned int i=0; i<_constraints.size(); ++i)
  {
    if(!_constraints[i]->constant_hessian())
      hessian_constant_ = false;

    if(!_constraints[i]->constant_gradient())
    {
      if(_constraints[i]->constraint_type() == NConstraintInterface::NC_EQUAL)
        jac_c_constant_ = false;
      else
        jac_d_constant_ = false;
    }

    // nothing else to check?
    if(!hessian_constant_ && !jac_c_constant_ && !jac_d_constant_)
      break;
  }

  //hessian of Lagrangian is only constant, if all hessians of the constraints are zero (due to lambda multipliers)
  if(!jac_c_constant_ || !jac_d_constant_)
    hessian_constant_ = false;
}


//-----------------------------------------------------------------------------


bool NProblemIPOPT::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                         Index& nnz_h_lag, IndexStyleEnum& index_style)
{
  // number of variables
  n = problem_->n_unknowns();

  // number of constraints
  m = constraints_.size();

  // get non-zeros of hessian of lagrangian and jacobi of constraints
  nnz_jac_g = 0;
  nnz_h_lag = 0;

  // get nonzero structure
  std::vector<double> x(n);
  problem_->initial_x(P(x));

  // nonzeros in the jacobian of C_ and the hessian of the lagrangian
  SMatrixNP HP;
  SVectorNC g;
  SMatrixNC H;
  problem_->eval_hessian(P(x), HP);

  // get nonzero structure of hessian of problem
  for(int i=0; i<HP.outerSize(); ++i)
    for (SMatrixNP::InnerIterator it(HP,i); it; ++it)
      if(it.row() >= it.col())
        ++nnz_h_lag;

  // get nonzero structure of constraints
  for( int i=0; i<m; ++i)
  {
    constraints_[i]->eval_gradient(P(x),g);

    nnz_jac_g += g.nonZeros();

    // count lower triangular elements
    constraints_[i]->eval_hessian (P(x),H);

    SMatrixNC::iterator m_it = H.begin();
    for(; m_it != H.end(); ++m_it)
      if( m_it.row() >= m_it.col())
        ++nnz_h_lag;
  }

  // We use the standard fortran index style for row/col entries
  index_style = C_STYLE;

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemIPOPT::get_bounds_info(Index n, Number* x_l, Number* x_u,
                            Index m, Number* g_l, Number* g_u)
{
  // check dimensions
  if( n != (Index)problem_->n_unknowns())
    std::cerr << "Warning: IPOPT #unknowns != n " << n << problem_->n_unknowns() << std::endl;
  if( m != (Index)constraints_.size())
    std::cerr << "Warning: IPOPT #constraints != m " << m << constraints_.size() << std::endl;


  // first clear all variable bounds
  for( int i=0; i<n; ++i)
  {
    // x_l[i] = Ipopt::nlp_lower_bound_inf;
    // x_u[i] = Ipopt::nlp_upper_bound_inf;

    x_l[i] = -1.0e19;
    x_u[i] =  1.0e19;
  }

  // iterate over bound constraints and set them
  for(unsigned int i=0; i<bound_constraints_.size(); ++i)
  {
    if((Index)(bound_constraints_[i]->idx()) < n)
    {
      switch(bound_constraints_[i]->constraint_type())
      {
      case NConstraintInterface::NC_LESS_EQUAL:
      {
        x_u[bound_constraints_[i]->idx()] = bound_constraints_[i]->bound();
      }break;

      case NConstraintInterface::NC_GREATER_EQUAL:
      {
        x_l[bound_constraints_[i]->idx()] = bound_constraints_[i]->bound();
      }break;

      case NConstraintInterface::NC_EQUAL:
      {
        x_l[bound_constraints_[i]->idx()] = bound_constraints_[i]->bound();
        x_u[bound_constraints_[i]->idx()] = bound_constraints_[i]->bound();
      }break;
      }
    }
    else
      std::cerr << "Warning: invalid bound constraint in IPOPTSolver!!!" << std::endl;
  }

  // set bounds for constraints
  for( int i=0; i<m; ++i)
  {
    // enum ConstraintType {NC_EQUAL, NC_LESS_EQUAL, NC_GREATER_EQUAL};
    switch(constraints_[i]->constraint_type())
    {
      case NConstraintInterface::NC_EQUAL         : g_u[i] = 0.0   ; g_l[i] =  0.0   ; break;
      case NConstraintInterface::NC_LESS_EQUAL    : g_u[i] = 0.0   ; g_l[i] = -1.0e19; break;
      case NConstraintInterface::NC_GREATER_EQUAL : g_u[i] = 1.0e19; g_l[i] =  0.0   ; break;
      default                                     : g_u[i] = 1.0e19; g_l[i] = -1.0e19; break;
    }
  }

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemIPOPT::get_starting_point(Index n, bool init_x, Number* x,
                               bool init_z, Number* z_L, Number* z_U,
                               Index m, bool init_lambda,
                               Number* lambda)
{
  // get initial value of problem instance
  problem_->initial_x(x);

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemIPOPT::eval_f(Index n, const Number* x, bool new_x, Number& obj_value)
{
  // return the value of the objective function
  obj_value = problem_->eval_f(x);
  return true;
}


//-----------------------------------------------------------------------------


bool NProblemIPOPT::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f)
{
  problem_->eval_gradient(x, grad_f);

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemIPOPT::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g)
{
  // evaluate all constraint functions
  for( int i=0; i<m; ++i)
    g[i] = constraints_[i]->eval_constraint(x);

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemIPOPT::eval_jac_g(Index n, const Number* x, bool new_x,
                       Index m, Index nele_jac, Index* iRow, Index *jCol,
                       Number* values)
{
  if (values == NULL)
  {
    // get x for evaluation (arbitrary position should be ok)
    std::vector<double> x_rnd(problem_->n_unknowns(), 0.0);

    int gi = 0;
    SVectorNC g;
    for( int i=0; i<m; ++i)
    {
      constraints_[i]->eval_gradient(&(x_rnd[0]), g);
      SVectorNC::InnerIterator v_it(g);
      for( ; v_it; ++v_it)
      {
        iRow[gi] = i;
        jCol[gi] = v_it.index();
        ++gi;
      }
    }
  }
  else
  {
    // return the values of the jacobian of the constraints

    // return the structure of the jacobian of the constraints
    // global index
    int gi = 0;
    SVectorNC g;

    for( int i=0; i<m; ++i)
    {
      constraints_[i]->eval_gradient(x, g);

      SVectorNC::InnerIterator v_it(g);

      for( ; v_it; ++v_it)
      {
        values[gi] = v_it.value();
        ++gi;
      }
    }

    if( gi != nele_jac)
      std::cerr << "Warning: number of non-zeros in Jacobian of C is incorrect: "
                << gi << " vs " << nele_jac << std::endl;
  }

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemIPOPT::eval_h(Index n, const Number* x, bool new_x,
                   Number obj_factor, Index m, const Number* lambda,
                   bool new_lambda, Index nele_hess, Index* iRow,
                   Index* jCol, Number* values)
{
  if (values == NULL)
  {
    // return structure

    // get x for evaluation (arbitrary position should be ok)
    std::vector<double> x_rnd(problem_->n_unknowns(), 0.0);

     // global index
     int gi = 0;
     // get hessian of problem
     SMatrixNP HP;
     problem_->eval_hessian(&(x_rnd[0]), HP);

     for(int i=0; i<HP.outerSize(); ++i)
       for (SMatrixNP::InnerIterator it(HP,i); it; ++it)
       {
         // store lower triangular part only
         if(it.row() >= it.col())
         {
           //         it.value();
           iRow[gi] = it.row();
           jCol[gi] = it.col();
           ++gi;
         }
       }

    // Hessians of Constraints
    for(unsigned int j=0; j<constraints_.size(); ++j)
    {
      SMatrixNC H;
      constraints_[j]->eval_hessian(&(x_rnd[0]), H);

      SMatrixNC::iterator m_it  = H.begin();
      SMatrixNC::iterator m_end = H.end();

      for(; m_it != m_end; ++m_it)
      {
        // store lower triangular part only
        if( m_it.row() >= m_it.col())
        {
          iRow[gi] = m_it.row();
          jCol[gi] = m_it.col();
          ++gi;
        }
      }
    }

    // error check
    if( gi != nele_hess)
      std::cerr << "Warning: number of non-zeros in Hessian of Lagrangian is incorrect while indexing: "
                << gi << " vs " << nele_hess << std::endl;
  }
  else
  {
    // return values.

    // global index
    int gi = 0;
    // get hessian of problem
    SMatrixNP HP;
    problem_->eval_hessian(x, HP);

    for(int i=0; i<HP.outerSize(); ++i)
      for (SMatrixNP::InnerIterator it(HP,i); it; ++it)
      {
        // store lower triangular part only
        if(it.row() >= it.col())
        {
          values[gi] = obj_factor*it.value();
          ++gi;
        }
      }

    // Hessians of Constraints
    for(unsigned int j=0; j<constraints_.size(); ++j)
    {
      SMatrixNC H;
      constraints_[j]->eval_hessian(x, H);

      SMatrixNC::iterator m_it  = H.begin();
      SMatrixNC::iterator m_end = H.end();

      for(; m_it != m_end; ++m_it)
      {
        // store lower triangular part only
        if( m_it.row() >= m_it.col())
        {
          values[gi] = lambda[j]*(*m_it);
          ++gi;
        }
      }
    }

    // error check
    if( gi != nele_hess)
      std::cerr << "Warning: number of non-zeros in Hessian of Lagrangian is incorrect2: "
                << gi << " vs " << nele_hess << std::endl;
  }
  return true;
}


//-----------------------------------------------------------------------------


void NProblemIPOPT::finalize_solution(SolverReturn status,
                              Index n, const Number* x, const Number* z_L, const Number* z_U,
                              Index m, const Number* g, const Number* lambda,
                              Number obj_value,
                              const IpoptData* ip_data,
                              IpoptCalculatedQuantities* ip_cq)
{
  // problem knows what to do
  problem_->store_result(x);

  if(store_solution_)
  {
    x_.resize(n);
    for( Index i=0; i<n; ++i)
      x_[i] = x[i];
  }
}


//-----------------------------------------------------------------------------


bool NProblemIPOPT::hessian_constant() const
{
  return hessian_constant_;
}


//-----------------------------------------------------------------------------


bool NProblemIPOPT::jac_c_constant() const
{
  return jac_c_constant_;
}


//-----------------------------------------------------------------------------


bool NProblemIPOPT::jac_d_constant() const
{
  return jac_d_constant_;
}


//== IMPLEMENTATION PROBLEM INSTANCE==========================================================


bool NProblemGmmIPOPT::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                         Index& nnz_h_lag, IndexStyleEnum& index_style)
{
  // number of variables
  n = problem_->n_unknowns();

  // number of constraints
  m = constraints_.size();

  // get nonzero structure
  std::vector<double> x(n);
  problem_->initial_x(&(x[0]));
  // ToDo: perturb x

  // nonzeros in the jacobian of C_ and the hessian of the lagrangian
  SMatrixNP HP;
  SVectorNC g;
  SMatrixNC H;
  problem_->eval_hessian(&(x[0]), HP);
  nnz_jac_g = 0;
  nnz_h_lag = 0;

  // clear old data
  jac_g_iRow_.clear();
  jac_g_jCol_.clear();
  h_lag_iRow_.clear();
  h_lag_jCol_.clear();

  // get non-zero structure of initial hessian
  // iterate over rows
  for( int i=0; i<n; ++i)
  {
    SVectorNP& ri = HP.row(i);

    SVectorNP_citer v_it  = gmm::vect_const_begin(ri);
    SVectorNP_citer v_end = gmm::vect_const_end  (ri);

    for(; v_it != v_end; ++v_it)
    {
      // store lower triangular part only
      if( i >= (int)v_it.index())
      {
        h_lag_iRow_.push_back(i);
        h_lag_jCol_.push_back(v_it.index());
        ++nnz_h_lag;
      }
    }
  }


  // get nonzero structure of constraints
  for( int i=0; i<m; ++i)
  {
    constraints_[i]->eval_gradient(&(x[0]),g);
    constraints_[i]->eval_hessian (&(x[0]),H);

    // iterate over sparse vector
    SVectorNC::InnerIterator v_it(g);
    for(; v_it; ++v_it)
    {
      jac_g_iRow_.push_back(i);
      jac_g_jCol_.push_back(v_it.index());
      ++nnz_jac_g;
    }

    // iterate over superSparseMatrix
    SMatrixNC::iterator m_it  = H.begin();
    SMatrixNC::iterator m_end = H.end();
    for(; m_it != m_end; ++m_it)
      if( m_it.row() >= m_it.col())
      {
        h_lag_iRow_.push_back(m_it.row());
        h_lag_jCol_.push_back(m_it.col());
        ++nnz_h_lag;
      }
  }

  // store for error checking...
  nnz_jac_g_ = nnz_jac_g;
  nnz_h_lag_ = nnz_h_lag;

  // We use the standard fortran index style for row/col entries
  index_style = C_STYLE;

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemGmmIPOPT::get_bounds_info(Index n, Number* x_l, Number* x_u,
                            Index m, Number* g_l, Number* g_u)
{
  // first clear all variable bounds
  for( int i=0; i<n; ++i)
  {
    // x_l[i] = Ipopt::nlp_lower_bound_inf;
    // x_u[i] = Ipopt::nlp_upper_bound_inf;

    x_l[i] = -1.0e19;
    x_u[i] =  1.0e19;
  }

  // set bounds for constraints
  for( int i=0; i<m; ++i)
  {
    // enum ConstraintType {NC_EQUAL, NC_LESS_EQUAL, NC_GREATER_EQUAL};
    switch(constraints_[i]->constraint_type())
    {
      case NConstraintInterface::NC_EQUAL         : g_u[i] = 0.0   ; g_l[i] =  0.0   ; break;
      case NConstraintInterface::NC_LESS_EQUAL    : g_u[i] = 0.0   ; g_l[i] = -1.0e19; break;
      case NConstraintInterface::NC_GREATER_EQUAL : g_u[i] = 1.0e19; g_l[i] =  0.0   ; break;
      default                                     : g_u[i] = 1.0e19; g_l[i] = -1.0e19; break;
    }
  }

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemGmmIPOPT::get_starting_point(Index n, bool init_x, Number* x,
                               bool init_z, Number* z_L, Number* z_U,
                               Index m, bool init_lambda,
                               Number* lambda)
{
  // get initial value of problem instance
  problem_->initial_x(x);

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemGmmIPOPT::eval_f(Index n, const Number* x, bool new_x, Number& obj_value)
{
  // return the value of the objective function
  obj_value = problem_->eval_f(x);
  return true;
}


//-----------------------------------------------------------------------------


bool NProblemGmmIPOPT::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f)
{
  problem_->eval_gradient(x, grad_f);

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemGmmIPOPT::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g)
{
  // evaluate all constraint functions
  for( int i=0; i<m; ++i)
    g[i] = constraints_[i]->eval_constraint(x);

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemGmmIPOPT::eval_jac_g(Index n, const Number* x, bool new_x,
                       Index m, Index nele_jac, Index* iRow, Index *jCol,
                       Number* values)
{
  if (values == NULL)
  {
    // return the (cached) structure of the jacobian of the constraints
    gmm::copy(jac_g_iRow_, VectorPTi(iRow, jac_g_iRow_.size()));
    gmm::copy(jac_g_jCol_, VectorPTi(jCol, jac_g_jCol_.size()));
  }
  else
  {
    // return the values of the jacobian of the constraints

    // return the structure of the jacobian of the constraints
    // global index
    int gi = 0;
    SVectorNC g;

    for( int i=0; i<m; ++i)
    {
      constraints_[i]->eval_gradient(x, g);

      SVectorNC::InnerIterator v_it(g);

      for( ; v_it; ++v_it)
      {
        if(gi < nele_jac)
          values[gi] = v_it.value();
        ++gi;
      }
    }

    if( gi != nele_jac)
      std::cerr << "Warning: number of non-zeros in Jacobian of C is incorrect: "
                << gi << " vs " << nele_jac << std::endl;
  }

  return true;
}


//-----------------------------------------------------------------------------


bool NProblemGmmIPOPT::eval_h(Index n, const Number* x, bool new_x,
                   Number obj_factor, Index m, const Number* lambda,
                   bool new_lambda, Index nele_hess, Index* iRow,
                   Index* jCol, Number* values)
{
  if (values == NULL)
  {
    // return the (cached) structure of the hessian
    gmm::copy(h_lag_iRow_, VectorPTi(iRow, h_lag_iRow_.size()));
    gmm::copy(h_lag_jCol_, VectorPTi(jCol, h_lag_jCol_.size()));
  }
  else
  {
    // return values.

    // global index
    int gi = 0;

    // get hessian of problem
    problem_->eval_hessian(x, HP_);

    for( int i=0; i<n; ++i)
    {
      SVectorNP& ri = HP_.row(i);

      SVectorNP_citer v_it  = gmm::vect_const_begin(ri);
      SVectorNP_citer v_end = gmm::vect_const_end  (ri);

      for(; v_it != v_end; ++v_it)
      {
        // store lower triangular part only
        if( i >= (int)v_it.index())
        {
          if( gi < nele_hess)
            values[gi] = obj_factor*(*v_it);
          ++gi;
        }
      }
    }

    // Hessians of Constraints
    for(unsigned int j=0; j<constraints_.size(); ++j)
    {
      SMatrixNC H;
      constraints_[j]->eval_hessian(x, H);

      SMatrixNC::iterator m_it  = H.begin();
      SMatrixNC::iterator m_end = H.end();

      for(; m_it != m_end; ++m_it)
      {
        // store lower triangular part only
        if( m_it.row() >= m_it.col())
        {
          if( gi < nele_hess)
            values[gi] = lambda[j]*(*m_it);
          ++gi;
        }
      }
    }

    // error check
    if( gi != nele_hess)
      std::cerr << "Warning: number of non-zeros in Hessian of Lagrangian is incorrect: "
                << gi << " vs " << nele_hess << std::endl;
  }
  return true;
}


//-----------------------------------------------------------------------------


void NProblemGmmIPOPT::finalize_solution(SolverReturn status,
                              Index n, const Number* x, const Number* z_L, const Number* z_U,
                              Index m, const Number* g, const Number* lambda,
                              Number obj_value,
                              const IpoptData* ip_data,
                              IpoptCalculatedQuantities* ip_cq)
{
  // problem knows what to do
  problem_->store_result(x);
}



//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // COMISO_IPOPT_AVAILABLE
//=============================================================================
