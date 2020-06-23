#include "gr/io/io.h"

#include <Eigen/Geometry>

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace gr;

////////////////////////////////////////////////////////////////////////////////
/// Write
////////////////////////////////////////////////////////////////////////////////

bool IOManager::WriteMatrix(
        const string &name,
        const Eigen::Ref<const Eigen::Matrix<double, 4, 4> >& mat,
        IOManager::MATRIX_MODE mode)
{
    std::ofstream sstr;
    sstr.open(name, std::ofstream::out | std::ofstream::trunc);

    bool status = false;

    switch (mode) {
    case POLYWORKS:
        formatPolyworksMatrix(mat, sstr);
        status = true;
        break;
    default:
        break;
    }

    sstr.close();

    return status;
}

std::ofstream &
IOManager::formatPolyworksMatrix(
        const Eigen::Ref<const Eigen::Matrix<double, 4, 4> > &mat,
        std::ofstream &sstr) {

    auto formatValue = [](const double& v){
        return (v >= 0.
                ? std::string(" ") + std::to_string(v)
                : std::to_string(v));
    };

    sstr << "VERSION\t=\t1\n";
    sstr << "MATRIX\t=\n";
    for (int j = 0; j!= 4; ++j){
        sstr << formatValue(mat(j, 0)) << "  "
             << formatValue(mat(j, 1)) << "  "
             << formatValue(mat(j, 2)) << "  "
             << formatValue(mat(j, 3)) << "\n";
    }

    return sstr;
}

////////////////////////////////////////////////////////////////////////////////
/// Wrapped STBI functions to be used inside template methods
/// Limits dependency on stb just to compilation of the library
////////////////////////////////////////////////////////////////////////////////
unsigned char*
IOManager::stbi_load_(char const *filename, int *x, int *y, int *comp, int req_comp)
{
    return stbi_load(filename, x, y, comp, req_comp);
}

void
IOManager::stbi_image_free_(void *retval_from_stbi_load)
{
    return stbi_image_free(retval_from_stbi_load);
}
  
const char *
IOManager::stbi_failure_reason_(void)
{
    return stbi_failure_reason();
}