#include "MeshSubFilter.h"

const float MeshSubFilter::m_stepSize = 0.02;
const unsigned int MeshSubFilter::m_stepsPerSecond = 1.0f/m_stepSize;
//MeshSubFilter::FilterType MeshSubFilter::m_currentFilterType = FILTERTYPE_UNDEFINED;
int MeshSubFilter::m_currentFilterType = 0;
