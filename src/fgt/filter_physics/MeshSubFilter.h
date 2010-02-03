/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef MESHSUBFILTER_H
#define MESHSUBFILTER_H

#include <common/meshmodel.h>
#include <common/interfaces.h>

#include <exception>

class MatrixNotFreezedException : public std::exception{
public:
    const char* what() const throw(){
        return "The matrix of the current selected mesh has to be frozen";
    }
};

class MeshSubFilter{
public:
    MeshSubFilter();

    virtual void initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par);
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb) = 0;
    virtual void initialize(MeshDocument&, RichParameterSet&, vcg::CallBackPos* cb);

    static void clearLastAppliedFilter();

protected:
    static int m_currentFilterType;
};

#endif // MESHSUBFILTER_H
