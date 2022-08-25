/*
 * NProblemInterfaceAD.hpp
 *
 *  Created on: Nov 30, 2012
 *      Author: kremer
 */

#ifndef NPROBLEMINTERFACEAD_HPP_
#define NPROBLEMINTERFACEAD_HPP_

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_ADOLC_AVAILABLE
#if COMISO_EIGEN3_AVAILABLE

//== INCLUDES =================================================================

#include <vector>

#include <boost/shared_array.hpp>

#include <adolc/adolc.h>
#include <adolc/adouble.h>
#include <adolc/drivers/drivers.h>
#include <adolc/sparse/sparsedrivers.h>
#include <adolc/taping.h>

#include <Eigen/Eigen>
#if !(EIGEN_VERSION_AT_LEAST(3,1,0))
#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#endif
#include <Eigen/Sparse>

#include <CoMISo/Config/CoMISoDefines.hh>

#include "TapeIDSingleton.hh"

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

/** \class NProblemmInterfaceAD NProblemInterfaceAD.hpp <ACG/.../NProblemInterfaceAD.hh>

 Brief Description.

 Extend the problem interface with auto differentiation using ADOL-C
 */
class COMISODLLEXPORT NProblemInterfaceAD : public NProblemInterface
{
public:

    // Sparse Matrix Type
#if EIGEN_VERSION_AT_LEAST(3,1,0)
    typedef Eigen::SparseMatrix<double,Eigen::ColMajor> SMatrixNP;
#else
    typedef Eigen::DynamicSparseMatrix<double,Eigen::ColMajor> SMatrixNP;
#endif

    /// Default constructor
    NProblemInterfaceAD(int _n_unknowns) :
    n_unknowns_(_n_unknowns),
    dense_hessian_(NULL),
    function_evaluated_(false),
    use_tape_(true),
    tape_(static_cast<short int>(TapeIDSingleton::Instance()->requestId())) {

        for(size_t i = 0; i < 11; ++i) tape_stats_[i] = 0;
    }

    /// Destructor
    virtual ~NProblemInterfaceAD() {

        if(dense_hessian_ != NULL) {
            for(int i = 0; i < n_unknowns_; ++i) {
                delete[] dense_hessian_[i];
            }
            delete[] dense_hessian_;
        }

        TapeIDSingleton::Instance()->releaseId(static_cast<size_t>(tape_));
    }

    // ================================================
    // Override these three methods
    // ================================================

    virtual void initial_x(double* _x) = 0;

    virtual adouble evaluate(const adouble* _x) = 0;

    virtual void store_result(const double* _x) = 0;

    // ================================================
    // Optionally override these methods, too
    // ================================================

    /**
     * \brief Indicate whether the hessian is sparse.
     * If so, the computations (as well as the memory
     * consumption) can be performed more efficiently.
     */
    virtual bool sparse_hessian() {
        return false;
    }

    // ================================================

    virtual int n_unknowns() {

        return n_unknowns_;
    }

    virtual double eval_f(const double* _x) {

        double y = 0.0;

        if(!function_evaluated_ || !use_tape_) {

            adouble y_d = 0.0;

            boost::shared_array<adouble> x_d = x_d_ptr();

            trace_on(tape_); // Start taping

            // Fill data vector
            for(int i = 0; i < n_unknowns_; ++i) {
                x_d[i] <<= _x[i];
            }

            // Call virtual function to compute
            // functional value
            y_d = evaluate(x_d.get());

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

    virtual void eval_gradient(const double* _x, double* _g) {

        if(!function_evaluated_ || !use_tape_) {
            // Evaluate original functional
            eval_f(_x);
        }

        int ec = gradient(tape_, n_unknowns_, _x, _g);

        if(ec < 0) {
            // Retape function if return code indicates discontinuity
            function_evaluated_ = false;
#ifdef ADOLC_RET_CODES
            std::cout << __FUNCTION__ << " invokes retaping of function due to discontinuity! Return code: " << ec << std::endl;
#endif
            eval_f(_x);
            ec = gradient(tape_, n_unknowns_, _x, _g);
        }

#ifdef ADOLC_RET_CODES
        std::cout << "Info: gradient() returned code " << ec << std::endl;
#endif
    }

    virtual void eval_hessian(const double* _x, SMatrixNP& _H) {

        _H.resize(n_unknowns_, n_unknowns_);
        _H.setZero();

        if(!function_evaluated_ || !use_tape_) {
            // Evaluate original functional
            eval_f(_x);
        }

        if(sparse_hessian()) {

            int nz = 0;
            int opt[2] = {0, 0};

            unsigned int* r_ind_p = NULL;
            unsigned int* c_ind_p = NULL;
            double* val_p = NULL;

            int ec = sparse_hess(tape_, n_unknowns_, 0, _x, &nz, &r_ind_p, &c_ind_p, &val_p, opt);

            if(ec < 0) {
                // Retape function if return code indicates discontinuity
                function_evaluated_ = false;
#ifdef ADOLC_RET_CODES
                std::cout << __FUNCTION__ << " invokes retaping of function due to discontinuity! Return code: " << ec << std::endl;
#endif
                eval_f(_x);
                ec = sparse_hess(tape_, n_unknowns_, 0, _x, &nz, &r_ind_p, &c_ind_p, &val_p, opt);
            }

            assert(*nz >= 0);
            assert(r_ind_p != NULL);
            assert(c_ind_p != NULL);
            assert(val_p != NULL);

#ifdef ADOLC_RET_CODES
            std::cout << "Info: sparse_hessian() returned code " << ec << std::endl;
#endif

            for(int i = 0; i < nz; ++i) {

                _H.coeffRef(r_ind_p[i], c_ind_p[i]) = val_p[i];
            }

            delete[] r_ind_p;
            delete[] c_ind_p;
            delete[] val_p;

        } else {

            // Dense hessian data
            double** h_ptr = dense_hessian_ptr();

            int ec = hessian(tape_, n_unknowns_, const_cast<double*>(_x), h_ptr);

            if(ec < 0) {
                // Retape function if return code indicates discontinuity
                function_evaluated_ = false;
#ifdef ADOLC_RET_CODES
                std::cout << __FUNCTION__ << " invokes retaping of function due to discontinuity! Return code: " << ec << std::endl;
#endif
                eval_f(_x);
                ec = hessian(tape_, n_unknowns_, const_cast<double*>(_x), h_ptr);
            }

#ifdef ADOLC_RET_CODES
            std::cout << "Info: hessian() returned code " << ec << std::endl;
#endif

            for(int i = 0; i < n_unknowns_; ++i) {
                for(int j = 0; j <= i; ++j) {

                    _H.coeffRef(i, j) = h_ptr[i][j];

                    if(i != j) {
                        _H.coeffRef(j, i) = h_ptr[i][j];
                    }
                }
            }
        }
    }

    bool use_tape() const {
        return use_tape_;
    }

    /** \brief Use tape
     * Set this to false if the energy functional
     * is discontinuous (so that the operator tree
     * has to be re-established at each evaluation)
     */
    void use_tape(bool _b) {
        use_tape_ = _b;
    }

    /**
     * \brief Get sample point vector's address
     *
     * The objective function class allocates the
     * memory for this vector at construction.
     * Get the pointer to this vector and inject it
     * into the constraint classes in order to
     * prevent them from allocating their own vectors
     * (which can be inefficient in terms of memory
     * consumption in case there are many constraints)
     */

    boost::shared_array<adouble> x_d_ptr() {
        if(x_d_.get() == NULL) {
            x_d_.reset(new adouble[n_unknowns_]);
        }
        return x_d_;
    }

    boost::shared_array<double> grad_ptr() {
        if(grad_.get() == NULL) {
            grad_.reset(new double[n_unknowns_]);
        }
        return grad_;
    }

    double** dense_hessian_ptr() {
        if(dense_hessian_ == NULL) {
            dense_hessian_ = new double*[n_unknowns_];
            for(int i = 0; i < n_unknowns_; ++i) {
                dense_hessian_[i] = new double[i+1];
            }
        }
        return dense_hessian_;
    }

private:

    int n_unknowns_;

    // Shared data
    boost::shared_array<adouble> x_d_;

    // Gradient vector
    boost::shared_array<double> grad_;

    // Dense hessian
    double** dense_hessian_;

    size_t tape_stats_[11];

    bool function_evaluated_;
    bool use_tape_;

    const short int tape_;
};

//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // COMISO_ADOLC_AVAILABLE
//=============================================================================
#endif // COMISO_EIGEN3_AVAILABLE
//=============================================================================
#endif /* NPROBLEMINTERFACEAD_HPP_ */
