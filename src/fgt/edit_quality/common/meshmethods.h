#ifndef _MESHMETHODS_H_
#define _MESHMETHODS_H_

#include "const_types.h"
#include "transferfunction.h"
#include <vcg/math/histogram.h>
#include <meshlab/meshmodel.h>
#include <cassert>
#include <utility>

#include <QString>

using namespace std;
using namespace vcg;

pair<int,int> computeHistogramMinMaxY (Histogramf*);

// Apply colors to mesh vertexes by quality
void applyColorByVertexQuality(MeshModel& mesh, TransferFunction *transferFunction, float minQuality, float maxQuality, float midHandlePercentilePosition, float brightness);

// Opens a CSV file and gets its equalizer parameters
void loadEqualizerInfo(QString fileName, EQUALIZER_INFO *data);


#endif
