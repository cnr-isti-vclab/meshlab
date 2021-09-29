/****************************************************************************
**
** Copyright (c) 2008-2020 C.B. Barber. All rights reserved.
** $Id: //main/2019/qhull/src/libqhullcpp/QhullUser.cpp#10 $$Change: 3008 $
** $DateTime: 2020/07/30 13:54:27 $$Author: bbarber $
**
****************************************************************************/

#include "libqhullcpp/QhullUser.h"

#include "libqhullcpp/QhullError.h"

#include <iostream>
#include <stdint.h>

using std::cerr;
using std::endl;
using std::istream;
using std::ostream;
using std::ostringstream;
using std::string;
using std::vector;
using std::ws;

#ifdef _MSC_VER  // Microsoft Visual C++ -- warning level 4
#pragma warning( disable : 4996)  // function was declared deprecated(strcpy, localtime, etc.)
#endif

namespace orgQhull{

#//! QhullUser -- user-defined interface to qhull via qh_fprintf


#//!\name Constructors
QhullUser::
QhullUser(QhullQh *qqh)
    : qh_qh(qqh)
    , previous_user(NULL)
    , doubles_vector()
    , ints_vector()
    , fprintf_ints()
    , fprintf_doubles()
    , fprintf_codes()
    , fprintf_strings()
    , num_facets(0)
    , num_neighbors(0)
    , num_numbers(0)
    , num_points(0)
    , num_results(0)
    , num_ridges(0)
    , num_vectors(0)
    , num_vertices(0)
    , qhull_dim(0)
    , delaunay_dim(0)
{
    previous_user= qh()->cpp_user;
    qh()->cpp_user= NULL;
    captureOn();
}//constructor

QhullUser::
~QhullUser()
{
    captureOff();
    qh()->cpp_user= previous_user;
}//destructor

#//!\name Get/Set

//! Clear working fields of QhullUser
//! Retains qh_qh and previous_user as initialized
//! Updates qhull_dim from qh_qh (does not change)
//! Updates delaunay_dim if qh_qh.ISdelaunay
void QhullUser::
clear()
{
    doubles_vector.clear();
    ints_vector.clear();
    fprintf_ints.clear();
    fprintf_doubles.clear();
    fprintf_codes.clear();
    fprintf_strings.clear();
    num_facets= 0;
    num_neighbors= 0;
    num_numbers= 0;
    num_points= 0;
    num_results= 0;
    num_ridges= 0;
    num_vectors= 0;
    num_vertices= 0;
}//clear

#//!\name Methods

void QhullUser::
captureOff()
{
    if(qh()->cpp_user==NULL){
        throw QhullError(10080, "Qhull error: QhullUser::captureOn not call before QhullUser::captureOff for QhullUser 0x%llx", 0, 0, 0.0, this);
    }
    if(qh()->cpp_user!=this){
        throw QhullError(10081, "Qhull error: conflicting QhullUser (0x%llx) for QhullUser::captureOff().  Does not match 'this' (0x...%X)", int(0xffff&(intptr_t)this), 0, 0.0, qh()->cpp_user);
    }
    qh()->cpp_user= NULL;
}//captureOff

void QhullUser::
captureOn()
{
    if(qh()->cpp_user){
        throw QhullError(10079, "Qhull error: conflicting user of cpp_user for QhullUser::captureOn() or corrupted qh_qh 0x%llx", 0, 0, 0.0, qh());
    }
    qh()->cpp_user= this;
}//captureOn

}//namespace orgQhull

#//!\name Global functions

/*-<a                             href="qh-user.htm#TOC"
>-------------------------------</a><a name="qh_fprintf">-</a>

  qh_fprintf(qh, fp, msgcode, format, list of args )
    replacement for qh_fprintf in userprintf_r.c, which replaces fprintf for Qhull
    qh.ISqhullQh must be true, indicating that qh a subclass of QhullQh
    qh.cpp_user is an optional QhullUser for trapped MSG_OUTPUT calls
    fp may be NULL
    otherwise behaves the same as qh_fprintf in userprintf_r.c

returns:
    sets qhullQh->qhull_status if msgcode is error 6000..6999
....sets qh.last_errcode if error reported
....if qh.cpp_user defined, records results of 'qhull v Fi Fo'
        See "qvoronoi-fifo" in user_eg3_r.cpp for an example


notes:
    Only called from libqhull_r
    A similar technique is used by RboxPoints and qh_fprintf_rbox
    Do not throw errors from here.  Use qh_errexit;
    fgets() is not trapped like fprintf(), QH11008 FIX: how do users handle input?  A callback?
*/
extern "C"

//! Custom qh_fprintf for transferring Qhull output from io_r.c to QhullUser and QhullQh
//! Identify msgcodes with Qhull option 'Ta'
//! A similar technique is used by RboxPoints with a custom qh_fprintf_rbox
void qh_fprintf(qhT *qh, FILE* fp, int msgcode, const char *fmt, ... ){
    va_list args;
    int last_errcode;

    using namespace orgQhull;
    
    if(qh==NULL || !qh->ISqhullQh){
        qh_fprintf_stderr(10025, "Qhull error: qh_fprintf in QhullUser.cpp called from a Qhull instance without QhullQh defined\n");
        last_errcode= 10025;
        qh_exit(last_errcode);
    }
    va_start(args, fmt);
    if(msgcode>=MSG_OUTPUT && qh->cpp_user){
        QhullUser *out= reinterpret_cast<QhullUser *>(qh->cpp_user);
        bool isOut= false;
        switch (msgcode){
        case 9231:        /* printvdiagram, totcount (ignored) */
            out->setNumResults(va_arg(args, int));
            isOut= true;
            break;
        case 9271:        /* qh_printvnorm, count, pointId, pointId, hyperplane */
            out->appendInt(va_arg(args, int));
            out->appendInt(va_arg(args, int));
            out->appendInt(va_arg(args, int));
            out->appendAndClearInts();
            isOut= true;
            break;
        case 9272:
        case 9273:
            out->appendDouble(va_arg(args, double));
            isOut= true;
            break;
        case 9274:
            out->appendAndClearDoubles();
            isOut= true;
            break;
        default:
            // do nothing
            break;
        }
        if(isOut){
            out->appendCode(msgcode);
            va_end(args);
            return;
        }
    }/*MSG_OUTPUT, cpp_user*/

    QhullQh *qhullQh= static_cast<QhullQh *>(qh);
    char newMessage[MSG_MAXLEN];
    int msgLen= 0;
    if((qh && qh->ANNOTATEoutput) || msgcode < MSG_TRACE4){
        msgLen= snprintf(newMessage, sizeof(newMessage), "[QH%.4d]", msgcode);
    }else if(msgcode>=MSG_ERROR && msgcode < MSG_STDERR){
        msgLen= snprintf(newMessage, sizeof(newMessage), "QH%.4d ", msgcode);
    }
    if(msgLen>=0 && msgLen < (int)sizeof(newMessage)){
        vsnprintf(newMessage + msgLen, sizeof(newMessage) - msgLen, fmt, args);
    }
    if(msgcode < MSG_OUTPUT || fp == qh_FILEstderr){
        if(msgcode>=MSG_ERROR && msgcode < MSG_WARNING){
            qh->last_errcode= msgcode;
            if(qhullQh->qhull_status < MSG_ERROR || qhullQh->qhull_status>=MSG_WARNING){
                qhullQh->qhull_status= msgcode;
            }
        }
        qhullQh->appendQhullMessage(newMessage);
    }else if(qhullQh->output_stream && qhullQh->use_output_stream){
        *qhullQh->output_stream << newMessage;
        if(qh->FLUSHprint){
            qhullQh->output_stream->flush();
        }
    }else{
        qhullQh->appendQhullMessage(newMessage);
    }
    va_end(args);

    /* Place debugging traps here. Use with trace option 'Tn'
         Set qh.tracefacet_id, qh.traceridge_id, and/or qh.tracevertex_id in global_r.c
    */
    if(False){ /* in production skip test for debugging traps */
        facetT *neighbor, **neighborp;
        if(qh->tracefacet && qh->tracefacet->tested){
            if(qh_setsize(qh, qh->tracefacet->neighbors) < qh->hull_dim)
                qh_errexit(qh, qh_ERRdebug, qh->tracefacet, qh->traceridge);
            FOREACHneighbor_(qh->tracefacet){
                if(neighbor != qh_DUPLICATEridge && neighbor != qh_MERGEridge && neighbor->visible)
                    qh_errexit2(qh, qh_ERRdebug, qh->tracefacet, neighbor);
            }
        }
        if(qh->traceridge && qh->traceridge->top->id == 234342223){
            qh_errexit(qh, qh_ERRdebug, qh->tracefacet, qh->traceridge);
        }
        if(qh->tracevertex && qh_setsize(qh, qh->tracevertex->neighbors) > 3434334){
            qh_errexit(qh, qh_ERRdebug, qh->tracefacet, qh->traceridge);
        }
    }
} /* qh_fprintf */

