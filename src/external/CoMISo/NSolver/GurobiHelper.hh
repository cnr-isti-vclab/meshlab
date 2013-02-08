/*
 * GurobiHelper.hh
 *
 *  Created on: Jan 4, 2012
 *      Author: ebke
 */


#ifndef GUROBIHELPER_HH_
#define GUROBIHELPER_HH_

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if (COMISO_GUROBI_AVAILABLE && COMISO_BOOST_AVAILABLE)
//=============================================================================

#include <gurobi_c++.h>
#include <string>
#include <vector>

namespace COMISO {

class GurobiHelper {
    public:

        /**
         * WARNING: Never call outputModelToMpsGz and importInitialSolutionIntoModel
         * on the same model. Both try to move the constant term into a variable and
         * consequently, the second attempt to do so will fail.
         */
        static void outputModelToMpsGz(GRBModel &model, const std::string &problem_output_path_);

        /**
         * WARNING: Never call outputModelToMpsGz and importInitialSolutionIntoModel
         * on the same model. Both try to move the constant term into a variable and
         * consequently, the second attempt to do so will fail.
         */
        static void importInitialSolutionIntoModel(GRBModel &model, const std::string &solution_path_);

        /**
         * Reads the solution vector from a SOL file and appends it to
         * out_solution_.
         */
        static void readSolutionVectorFromSOL(std::vector<double> &out_solution_, const std::string &fileName_);
};

} /* namespace COMISO */
#endif /* COMISO_GUROBI_AVAILABLE */
#endif /* GUROBIHELPER_HH_ */


