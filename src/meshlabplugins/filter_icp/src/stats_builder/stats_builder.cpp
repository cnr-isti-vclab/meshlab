#include "stats_builder.h"

void ResultAlignerPlainTextFormatter::preFormat() {
    buffer = ("Iter - MinD -  Error - Sample - Used - DistR - BordR - AnglR  \n");
}

void ResultAlignerPlainTextFormatter::processStats(size_t index, vcg::AlignPair::Stat::IterInfo &iterInfo) {
    buffer.append(QString{}.sprintf(" %02zu   %6.2f  %7.4f   %05i  %05i  %5i  %5i  %5i\n",
                    index,
                    iterInfo.MinDistAbs, iterInfo.pcl50, iterInfo.SampleTested,
                    iterInfo.SampleUsed, iterInfo.DistanceDiscarded,
                    iterInfo.BorderDiscarded, iterInfo.AngleDiscarded));
}
