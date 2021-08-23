#ifndef MESHLAB_STATSBUILDER_H
#define MESHLAB_STATSBUILDER_H

#include <QString>
#include <vcglib/vcg/complex/algorithms/align_pair.h>

class AlignerStatsFormatter {

public:

    const QString& format() {
        preFormat();
        size_t index = 0;
        for (auto& statsInfo: this->statsInfoIterator) {
            processStats(index++, statsInfo);
        }
        postFormat();
        return buffer;
    }

protected:

    QString buffer = {};
    std::vector<vcg::AlignPair::Stat::IterInfo>& statsInfoIterator;

    virtual void preFormat() { };
    virtual void processStats(size_t index, vcg::AlignPair::Stat::IterInfo &iterInfo) = 0;
    virtual void postFormat() { };

    explicit AlignerStatsFormatter(std::vector<vcg::AlignPair::Stat::IterInfo>& statsInfoIterator)
            : statsInfoIterator{statsInfoIterator} {}
};

class ResultAlignerPlainTextFormatter : public AlignerStatsFormatter {

    void preFormat() override;
    void processStats(size_t index, vcg::AlignPair::Stat::IterInfo &iterInfo) override;

public:
    explicit ResultAlignerPlainTextFormatter(std::vector<vcg::AlignPair::Stat::IterInfo>& statsInfoIterator)
            : AlignerStatsFormatter(statsInfoIterator) {
    }
};

#endif //MESHLAB_STATSBUILDER_H
