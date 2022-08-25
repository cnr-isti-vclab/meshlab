/*
 * GurobiHelper.cc
 *
 *  Created on: Jan 4, 2012
 *      Author: ebke
 */

#include "GurobiHelper.hh"

#if (COMISO_GUROBI_AVAILABLE && COMISO_BOOST_AVAILABLE)

#include <QTemporaryFile>
#include <QFileInfo>
#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>

#define OUTPUT_UNCOMPRESSED_WITH_CONSTANT_COMMENT 0
#define OUTPUT_CONSTANT_AS_CONT 1

namespace COMISO {

/**
 * Helper class that ensures exception safe deletion
 * of a temporary file.
 */
class TempFileGuard {
    public:
        TempFileGuard(const std::string &_filePath) : filePath_(_filePath) {
        }

        ~TempFileGuard() {
            if (boost::filesystem::exists(filePath_))
                boost::filesystem::remove(filePath_);
        }

        const boost::filesystem::path &filePath() const { return filePath_; };

    private:
        boost::filesystem::path filePath_;
};

static void moveConstantTermIntoConstrainedVariable(GRBModel &model) {
    const double constantTerm = model.getObjective().getLinExpr().getConstant();
    //tmpModel.getObjective().addConstant(-constantTerm);
    model.getObjective() -= constantTerm;
#if OUTPUT_CONSTANT_AS_CONT
    model.addVar(constantTerm, constantTerm, 1, GRB_CONTINUOUS, "MIQ_synthetic_constant");
#else
    model.addVar(1, 1, constantTerm, GRB_INTEGER, "MIQ_synthetic_constant");
#endif
}

static void copyFile(const char *from, const char *to) {
    FILE *inF = fopen(from, "r");
    FILE *outF = fopen(to, "w");

    const int bufsize = 4096;
    unsigned char buffer[bufsize];

    do {
        size_t readBytes = fread(buffer, 1, bufsize, inF);
        fwrite(buffer, 1, readBytes, outF);
    } while(!feof(inF));

    fclose(inF);
    fclose(outF);
}

/**
 * WARNING: Never call outputModelToMpsGz and importInitialSolutionIntoModel
 * on the same model. Both try to move the constant term into a variable and
 * consequently, the second attempt to do so will fail.
 */
void GurobiHelper::outputModelToMpsGz(GRBModel &model, const std::string &problem_output_path_) {
#if OUTPUT_UNCOMPRESSED_WITH_CONSTANT_COMMENT
    boost::scoped_ptr<TempFileGuard> tempFileGuard;
    {
        QTemporaryFile tempFile("XXXXXX.mps");
        tempFile.setAutoRemove(false);
        tempFile.open();

        // In order to minimize the likelihood of race conditions,
        // we initialize tempFileGuard right here.
        tempFileGuard.reset(new TempFileGuard(QFileInfo(tempFile).absoluteFilePath().toStdString()));
        tempFile.close();
    }

    const std::string fileName = tempFileGuard->filePath().string();

    model.write(fileName);
    const double constantTerm = model.getObjective().getLinExpr().getConstant();

    FILE *inF = fopen(fileName.c_str(), "r");
    FILE *outF = fopen(problem_output_path_.c_str(), "w");

    fprintf(outF, "* Constant Term: %.16e\n", constantTerm);
    const int bufsize = 4096;
    unsigned char buffer[bufsize];
    int readBytes;
    do {
        readBytes = fread(buffer, 1, bufsize, inF);
        fwrite(buffer, 1, readBytes, outF);
    } while(!feof(inF));
    fclose(inF);
    fclose(outF);
#else
    GRBModel tmpModel(model);

    moveConstantTermIntoConstrainedVariable(tmpModel);

    tmpModel.update();
    tmpModel.write(problem_output_path_);
#endif
}

/**
 * WARNING: Never call outputModelToMpsGz and importInitialSolutionIntoModel
 * on the same model. Both try to move the constant term into a variable and
 * consequently, the second attempt to do so will fail.
 */
void GurobiHelper::importInitialSolutionIntoModel(GRBModel &model, const std::string &solution_path_) {
    boost::scoped_ptr<TempFileGuard> tempFileGuard;
    {
        QTemporaryFile tempFile("XXXXXX.mst");
        tempFile.setAutoRemove(false);
        tempFile.open();

        // In order to minimize the likelihood of race conditions,
        // we initialize tempFileGuard right here.
        tempFileGuard.reset(new TempFileGuard(QFileInfo(tempFile).absoluteFilePath().toStdString()));
        tempFile.close();
    }

    const std::string fileName = tempFileGuard->filePath().string();

    copyFile(solution_path_.c_str(), fileName.c_str());

    //moveConstantTermIntoConstrainedVariable(model);
    const double constantTerm = model.getObjective().getLinExpr().getConstant();
    model.addVar(constantTerm, constantTerm, 0, GRB_CONTINUOUS, "MIQ_synthetic_constant");

    model.update();
    model.read(fileName);
    model.update();
}

void GurobiHelper::readSolutionVectorFromSOL(std::vector<double> &out_solution_, const std::string &fileName_) {
    std::ifstream solFile(fileName_.c_str());
    //if (!solFile.good())
    //    throw std::runtime_error("Unable to open file \"" + fileName + "\".");

    static const boost::regex commentRe("\\s*#", boost::regex_constants::perl);
    static const boost::regex variableRe("\\s*(\\S+)\\s+([-+]?[0-9]*\\.?[0-9]+(?:[eE][-+]?[0-9]+)?)", boost::regex_constants::perl);

    std::string line;
    while (solFile) {
        std::getline(solFile, line);
        if (boost::regex_search(line, commentRe, boost::match_continuous)) continue;
        boost::smatch match;
        if (boost::regex_search(line, match, variableRe, boost::match_continuous) && match[1] != "MIQ_synthetic_constant") {
            out_solution_.push_back(boost::lexical_cast<double>(match[2]));
        }
    }
}

} /* namespace COMISO */

#endif
