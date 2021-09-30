/****************************************************************************
**
** Copyright (c) 2008-2020 C.B. Barber. All rights reserved.
** $Id: //main/2019/qhull/src/libqhullcpp/QhullStat.h#2 $$Change: 2953 $
** $DateTime: 2020/05/21 22:05:32 $$Author: bbarber $
**
****************************************************************************/

#ifndef QHULLSTAT_H
#define QHULLSTAT_H

#include "libqhull_r/qhull_ra.h"

#include <string>
#include <vector>

namespace orgQhull {

#//!\name defined here
    //! QhullStat -- Qhull's statistics, qhstatT, as a C++ class
    //! Statistics defined with zzdef_() control Qhull's behavior, summarize its result, and report precision problems.
    class QhullStat;

class QhullStat : public qhstatT {

private:
#//!\name Fields (empty) -- POD type equivalent to qhstatT.  No data or virtual members

public:
#//!\name Constants

#//!\name class methods

#//!\name constructor, assignment, destructor, invariant
                        QhullStat();
                        ~QhullStat();

private:
    //!disable copy constructor and assignment
                        QhullStat(const QhullStat &);
    QhullStat &         operator=(const QhullStat &);
public:

#//!\name Access
};//class QhullStat

}//namespace orgQhull

#endif // QHULLSTAT_H
