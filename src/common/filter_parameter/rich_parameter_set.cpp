#include "rich_parameter_set.h"

#include <vcg/math/matrix44.h>
#include <wrap/qt/col_qt_convert.h>

using namespace vcg;

// Very similar to the findParameter but this one does not print out debugstuff.
bool RichParameterSet::hasParameter(const QString& name) const
{
    QList<RichParameter*>::const_iterator fpli;
    for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
    {
        if((*fpli != NULL) && (*fpli)->name()==name)
            return true;
    }
    return false;
}
// You should never use this one to know if a given parameter is present.
RichParameter* RichParameterSet::findParameter(const QString& name) const
{
    QList<RichParameter*>::const_iterator fpli;
    for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
    {
        if((*fpli != NULL) && (*fpli)->name()==name)
            return *fpli;
    }
    qDebug("FilterParameter Warning: Unable to find a parameter with name '%s',\n"
        "      Please check types and names of the parameter in the calling filter", qUtf8Printable(name));
    assert(0);
    return 0;
}

RichParameterSet& RichParameterSet::removeParameter(const QString& name){
    paramList.removeAll(findParameter(name));
    return (*this);
}

RichParameterSet& RichParameterSet::addParam(RichParameter* pd )
{
    assert(!hasParameter(pd->name));
    paramList.push_back(pd);
    return (*this);
}

//--------------------------------------


void RichParameterSet::setValue(const QString& name,const Value& newval){ findParameter(name)->val->set(newval); }

//- All the get<TYPE> are very similar. Nothing interesting here.

        bool RichParameterSet::getBool(const QString& name)     const { return findParameter(name)->val->getBool(); }
         int RichParameterSet::getInt(const QString& name)      const { return findParameter(name)->val->getInt();}
       float RichParameterSet::getFloat(const QString& name)    const { return findParameter(name)->val->getFloat();}
      QColor RichParameterSet::getColor(const QString& name)    const { return findParameter(name)->val->getColor();}
     Color4b RichParameterSet::getColor4b(const QString& name)  const { return ColorConverter::ToColor4b(findParameter(name)->val->getColor());}
     QString RichParameterSet::getString(const QString& name)   const { return findParameter(name)->val->getString();}
   Matrix44f RichParameterSet::getMatrix44(const QString& name) const { return findParameter(name)->val->getMatrix44f();}
   Matrix44<MESHLAB_SCALAR> RichParameterSet::getMatrix44m(const QString& name) const { return Matrix44<MESHLAB_SCALAR>::Construct(findParameter(name)->val->getMatrix44f());}
     Point3f RichParameterSet::getPoint3f(const QString& name)  const { return findParameter(name)->val->getPoint3f();}
     Point3<MESHLAB_SCALAR> RichParameterSet::getPoint3m(const QString& name)  const { return Point3<MESHLAB_SCALAR>::Construct(findParameter(name)->val->getPoint3f());}
       Shotf RichParameterSet::getShotf(const QString& name)    const { return findParameter(name)->val->getShotf();}
       Shot<MESHLAB_SCALAR> RichParameterSet::getShotm(const QString& name)    const { return Shot<MESHLAB_SCALAR>::Construct(findParameter(name)->val->getShotf());}
       float RichParameterSet::getAbsPerc(const QString& name)  const { return findParameter(name)->val->getAbsPerc();}
         int RichParameterSet::getEnum(const QString& name)     const { return findParameter(name)->val->getEnum();}
QList<float> RichParameterSet::getFloatList(const QString& name)    const { return findParameter(name)->val->getFloatList();}
 MeshModel * RichParameterSet::getMesh(const QString& name)         const { return findParameter(name)->val->getMesh();}
       float RichParameterSet::getDynamicFloat(const QString& name) const { return findParameter(name)->val->getDynamicFloat();}
     QString RichParameterSet::getOpenFileName(const QString& name) const { return findParameter(name)->val->getFileName();}
     QString RichParameterSet::getSaveFileName(const QString& name) const { return findParameter(name)->val->getFileName(); }

RichParameterSet& RichParameterSet::operator=( const RichParameterSet& rps )
{
    copy(rps);
    return *this;
}

bool RichParameterSet::operator==( const RichParameterSet& rps )
{
    if (rps.paramList.size() != paramList.size())
        return false;

    bool iseq = true;
    int ii = 0;
    while((ii < rps.paramList.size()) && iseq)
    {
        if (!(*rps.paramList.at(ii) == *paramList.at(ii)))
            iseq = false;
        ++ii;
    }

    return iseq;
}

RichParameterSet::~RichParameterSet()
{
    for(RichParameter* rp : paramList)
        delete rp;
    paramList.clear();

}

RichParameterSet& RichParameterSet::copy( const RichParameterSet& rps )
{
    if (this != &rps) {
        clear();

        RichParameterCopyConstructor copyvisitor;
        for(int ii = 0;ii < rps.paramList.size();++ii)
        {
            rps.paramList.at(ii)->accept(copyvisitor);
            paramList.push_back(copyvisitor.lastCreated);
        }
    }
    return (*this);
}


RichParameterSet::RichParameterSet( const RichParameterSet& rps )
{
    clear();

    RichParameterCopyConstructor copyvisitor;
    for(int ii = 0;ii < rps.paramList.size();++ii)
    {
        rps.paramList.at(ii)->accept(copyvisitor);
        paramList.push_back(copyvisitor.lastCreated);
    }
}

RichParameterSet::RichParameterSet()
{

}

bool RichParameterSet::isEmpty() const
{
    return paramList.isEmpty();
}

void RichParameterSet::clear()
{
    for(RichParameter* rp : paramList)
        delete rp;
    paramList.clear();
}

RichParameterSet& RichParameterSet::join( const RichParameterSet& rps )
{
    RichParameterCopyConstructor copyvisitor;
    for(int ii = 0;ii < rps.paramList.size();++ii)
    {
        rps.paramList.at(ii)->accept(copyvisitor);
        paramList.push_back(copyvisitor.lastCreated);
    }
    return (*this);
}
