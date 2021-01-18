//=============================================================================
//
//  CLASS NConstraintInterfaceAD
//
//=============================================================================

#ifndef COMISO_NCONSTRAINTINTERFACEAD_HH
#define COMISO_NCONSTRAINTINTERFACEAD_HH

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_ADOLC_AVAILABLE
#if COMISO_EIGEN3_AVAILABLE

//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include "SuperSparseMatrixT.hh"

#include <boost/shared_array.hpp>

#include <adolc/adolc.h>
#include <adolc/adouble.h>
#include <adolc/drivers/drivers.h>
#include <adolc/sparse/sparsedrivers.h>
#include <adolc/taping.h>

#include "NConstraintInterface.hh"
#include "NProblemInterfaceAD.hpp"

#include "TapeIDSingleton.hh"

#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <Eigen/Sparse>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

/** \class NProblemInterfaceAD NProblemInterfaceAD.hpp

    The problem interface using automatic differentiation.
 */
class COMISODLLEXPORT NConstraintInterfaceAD : public NConstraintInterface {
public:

    // Define Sparse Datatypes
    typedef NConstraintInterface::SVectorNC SVectorNC;
    typedef NConstraintInterface::SMatrixNC SMatrixNC;

    typedef NConstraintInterface::ConstraintType ConstraintType;

    /// Default constructor
    NConstraintInterfaceAD(NProblemInterfaceAD& _problem, int _n_unknowns,
                           const ConstraintType _type = NC_EQUAL, double _eps = 1e-6) :
        NConstraintInterface(_type, _eps),
        problem_(_problem),
        n_unknowns_(_n_unknowns),
        type_(_type),
        function_evaluated_(false),
        use_tape_(true),
        constant_hessian_evaluated_(false),
        tape_(static_cast<short int>(TapeIDSingleton::Instance()->requestId())) {

        for(size_t i = 0; i < 11; ++i) tape_stats_[i] = 0;
    }

    /// Destructor
    virtual ~NConstraintInterfaceAD() {
        TapeIDSingleton::Instance()->releaseId(static_cast<size_t>(tape_));
    }

    /**
     * \brief Only override this function
     */
    virtual adouble evaluate(const adouble* _x) = 0;

public:

    virtual int n_unknowns() {
        return n_unknowns_;
    }

    virtual double eval_constraint(const double* _x) {

        double y = 0.0;

        if(!function_evaluated_ || !use_tape_) {

            adouble y_d = 0.0;

            boost::shared_array<adouble> x_d_ptr = problem_.x_d_ptr();

            trace_on(tape_); // Start taping

            // Fill data vector
            for(int i = 0; i < n_unknowns_; ++i) {
                x_d_ptr[i] <<= _x[i];
            }

            // Call virtual function to compute
            // functional value
            y_d = evaluate(x_d_ptr.get());

            y_d >>= y;

            trace_off();

#ifdef ADOLC_STATS
            tapestats(tape_, tape_stats_);
            std::cout << "Status values for tape " << tape_ << std::endl;
            std::cout << "===============================================" << std::endl;
            std::cout << "Number of independent variables:\t" << tape_stats_[0] << std::endl;
            std::cout << "Number of dependent variables:\t\t" << tape_stats_[1] << std::endl;
            std::cout << "Max. number of live active variables:\t" << tape_stats_[2] << std::endl;
            std::cout << "Size of value stack:\t\t\t" << tape_stats_[3] << std::endl;
            std::cout << "Buffer size:\t\t\t\t" << tape_stats_[4] << std::endl;
            std::cout << "Total number of operations recorded:\t" << tape_stats_[5] << std::endl;
            std::cout << "Other stats [6]:\t\t\t" << tape_stats_[6] << std::endl;
            std::cout << "Other stats [7]:\t\t\t" << tape_stats_[7] << std::endl;
            std::cout << "Other stats [8]:\t\t\t" << tape_stats_[8] << std::endl;
            std::cout << "Other stats [9]:\t\t\t" << tape_stats_[9] << std::endl;
            std::cout << "Other stats [10]:\t\t\t" << tape_stats_[10] << std::endl;
            std::cout << "===============================================" << std::endl;
#endif

            function_evaluated_ = true;

        } else {

            double ay[1] = {0.0};

            int ec = function(tape_, 1, n_unknowns_, const_cast<double*>(_x), ay);

#ifdef ADOLC_RET_CODES
            std::cout << "Info: function() returned code " << ec << std::endl;
#endif

            y = ay[0];
        }

        return y;
    }

    virtual void eval_gradient(const double* _x, SVectorNC& _g) {

        if(!function_evaluated_ || !use_tape_) {
            // Evaluate original functional
            eval_constraint(_x);
        }

        boost::shared_array<double> grad_p = problem_.grad_ptr();

        _g.resize(n_unknowns_);
        _g.setZero();

        int ec = gradient(tape_, n_unknowns_, _x, grad_p.get());

        if(ec < 0) {
            // Retape function if return code indicates discontinuity
            function_evaluated_ = false;
#ifdef ADOLC_RET_CODES
            std::cout << __FUNCTION__ << " invokes retaping of function due to discontinuity! Return code: " << ec << std::endl;
#endif
            eval_constraint(_x);
            ec = gradient(tape_, n_unknowns_, _x, grad_p.get());
        }

#ifdef ADOLC_RET_CODES
        std::cout << "Info: gradient() returned code " << ec << std::endl;
#endif

        for(int i = 0; i < n_unknowns_; ++i) {
            _g.coeffRef(i) = grad_p[i];
        }
    }

    virtual void eval_hessian(const double* _x, SMatrixNC& _H) {

        _H.resize(n_unknowns_, n_unknowns_);

        if(constant_hessian() && constant_hessian_evaluated_) {
            _H = constant_hessian_;
            return;
        }

        if(!function_evaluated_ || !use_tape_) {
            // Evaluate original functional
            eval_constraint(_x);
        }

        if(sparse_hessian()) {

            int nz = 0;
            int opt[2] = {0, 0};

            unsigned int* r_ind = NULL;
            unsigned int* c_ind = NULL;
            double* val = NULL;

            int ec = sparse_hess(tape_, n_unknowns_, 0, _x, &nz, &r_ind, &c_ind, &val, opt);
            if(ec < 0) {
                // Retape function if return code indicates discontinuity
                function_evaluated_ = false;
#ifdef ADOLC_RET_CODES
                std::cout << __FUNCTION__ << " invokes retaping of function due to discontinuity! Return code: " << ec << std::endl;
#endif
                eval_constraint(_x);
                ec = sparse_hess(tape_, n_unknowns_, 0, _x, &nz, &r_ind, &c_ind, &val, opt);
            }

            assert(*nz >= 0);
            assert(r_ind != NULL);
            assert(c_ind != NULL);
            assert(val != NULL);

#ifdef ADOLC_RET_CODES
            std::cout << "Info: sparse_hessian() returned code " << ec << std::endl;
#endif

            for(int i = 0; i < nz; ++i) {

                _H(r_ind[i], c_ind[i]) = val[i];
            }

            if(constant_hessian()) {
                constant_hessian_ = _H;
                constant_hessian_evaluated_ = true;
            }

            delete[] r_ind;
            delete[] c_ind;
            delete[] val;

        } else {

            double** h_ptr = problem_.dense_hessian_ptr();

            int ec = hessian(tape_, n_unknowns_, const_cast<double*>(_x), h_ptr);

            if(ec < 0) {
                // Retape function if return code indicates discontinuity
                function_evaluated_ = false;
#ifdef ADOLC_RET_CODES
                std::cout << __FUNCTION__ << " invokes retaping of function due to discontinuity! Return code: " << ec << std::endl;
#endif
                eval_constraint(_x);
                ec = hessian(tape_, n_unknowns_, const_cast<double*>(_x), h_ptr);
            }

#ifdef ADOLC_RET_CODES
            std::cout << "Info: hessian() returned code " << ec << std::endl;
#endif

            for(int i = 0; i < n_unknowns_; ++i) {
                for(int j = 0; j <= i; ++j) {

                    _H(i, j) = h_ptr[i][j];

                    if(i != j) {
                        _H(j, i) = h_ptr[i][j];
                    }
                }
            }

            if(constant_hessian()) {
                constant_hessian_ = _H;
                constant_hessian_evaluated_ = true;
            }
        }
    }

    /** \brief Use tape
     * Set this to false if the energy functional
     * is discontinuous (so that the operator tree
     * has to be re-established at each evaluation)
     */
    bool use_tape() const {
        return use_tape_;
    }

    void use_tape(bool _b) {
        use_tape_ = _b;
    }

    /**
     * \brief Indicate whether the hessian is sparse.
     * If so, the computations (as well as the memory
     * consumption) can be performed more efficiently.
     */
    virtual bool sparse_hessian() {
        return false;
    }

private:

    // Reference to associated objective function
    NProblemInterfaceAD& problem_;

    // Number of unknowns
    int n_unknowns_;

    // Constraint type
    ConstraintType type_;

    size_t tape_stats_[11];

    bool function_evaluated_;
    bool use_tape_;

    SMatrixNC constant_hessian_;
    bool constant_hessian_evaluated_;

    const short int tape_;
};

//=============================================================================
}// namespace COMISO
//=============================================================================
#endif // COMISO_ADOLC_AVAILABLE
//=============================================================================
#endif // COMISO_EIGEN3_AVAILABLE
//=============================================================================
#endif // ACG_NCONSTRAINTINTERFACEAD_HH defined
//=============================================================================

