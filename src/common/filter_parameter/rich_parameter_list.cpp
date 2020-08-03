#include "rich_parameter_list.h"

#include "../mlexception.h"
#include <vcg/math/matrix44.h>
#include <wrap/qt/col_qt_convert.h>

using namespace vcg;

RichParameterList::RichParameterList()
{
}

RichParameterList::RichParameterList( const RichParameterList& rps )
{
	for(auto p : rps.paramList) {
		paramList.push_back(p->clone());
	}
}

RichParameterList::RichParameterList(RichParameterList&& rps)
{
	for(auto& p : rps.paramList) {
		paramList.push_back(p);
		p = nullptr;
	}
}

RichParameterList::~RichParameterList()
{
	for(RichParameter* rp : paramList)
		delete rp;
	paramList.clear();

}

bool RichParameterList::isEmpty() const
{
	return paramList.size() == 0;
}

unsigned int RichParameterList::size() const
{
	return paramList.size();
}

void RichParameterList::clear()
{
	for(RichParameter* rp : paramList)
		delete rp;
	paramList.clear();
}

bool RichParameterList::getBool(const QString& name) const
{
	return findParameter(name)->value().getBool();
}

int RichParameterList::getInt(const QString& name) const
{
	return findParameter(name)->value().getInt();
}

float RichParameterList::getFloat(const QString& name) const
{
	return findParameter(name)->value().getFloat();
}

QColor RichParameterList::getColor(const QString& name) const
{
	return findParameter(name)->value().getColor();
}

Color4b RichParameterList::getColor4b(const QString& name) const
{
	return ColorConverter::ToColor4b(findParameter(name)->value().getColor());
}

QString RichParameterList::getString(const QString& name) const
{
	return findParameter(name)->value().getString();
}

Matrix44f RichParameterList::getMatrix44(const QString& name) const
{
	return findParameter(name)->value().getMatrix44f();
}

Matrix44<MESHLAB_SCALAR> RichParameterList::getMatrix44m(const QString& name) const
{
	return Matrix44<MESHLAB_SCALAR>::Construct(findParameter(name)->value().getMatrix44f());
}

Point3f RichParameterList::getPoint3f(const QString& name) const
{
	return findParameter(name)->value().getPoint3f();
}

Point3<MESHLAB_SCALAR> RichParameterList::getPoint3m(const QString& name) const
{
	return Point3<MESHLAB_SCALAR>::Construct(findParameter(name)->value().getPoint3f());
}

Shotf RichParameterList::getShotf(const QString& name) const
{
	return findParameter(name)->value().getShotf();
}

Shot<MESHLAB_SCALAR> RichParameterList::getShotm(const QString& name) const
{
	return Shot<MESHLAB_SCALAR>::Construct(findParameter(name)->value().getShotf());
}

float RichParameterList::getAbsPerc(const QString& name) const
{
	return findParameter(name)->value().getAbsPerc();
}

int RichParameterList::getEnum(const QString& name) const
{
	return findParameter(name)->value().getEnum();
}

QList<float> RichParameterList::getFloatList(const QString& name) const
{
	return findParameter(name)->value().getFloatList();
}

MeshModel * RichParameterList::getMesh(const QString& name) const
{
	return findParameter(name)->value().getMesh();
}

float RichParameterList::getDynamicFloat(const QString& name) const
{
	return findParameter(name)->value().getDynamicFloat();
}

QString RichParameterList::getOpenFileName(const QString& name) const
{
	return findParameter(name)->value().getFileName();
}

QString RichParameterList::getSaveFileName(const QString& name) const
{
	return findParameter(name)->value().getFileName();
}

bool RichParameterList::hasParameter(const QString& name) const
{
	const RichParameter* rp = findParameter(name);
	return rp != nullptr;
}

RichParameter* RichParameterList::findParameter(const QString& name)
{
	for(RichParameter* rp : paramList) {
		if((rp != nullptr) && rp->name()==name)
			return rp;
	}
	return nullptr;
}

const RichParameter* RichParameterList::findParameter(const QString& name) const
{
	for(RichParameter* rp : paramList) {
		if((rp != nullptr) && rp->name()==name)
			return rp;
	}
	return nullptr;
}

RichParameter& RichParameterList::at(unsigned int i)
{
	if (i >= size())
		throw MLException("Index out of bound at RichParameterList::at");
	const_iterator it = begin();
	std::advance(it, i);
	return **it;
}

const RichParameter& RichParameterList::at(unsigned int i) const
{
	if (i >= size())
		throw MLException("Index out of bound at RichParameterList::at");
	const_iterator it = begin();
	std::advance(it, i);
	return **it;
}

void RichParameterList::setValue(const QString& name,const Value& newval)
{
	assert(hasParameter(name));
	findParameter(name)->setValue(newval);
}

RichParameter& RichParameterList::addParam(const RichParameter& pd )
{
	assert(!hasParameter(pd.name()));
	RichParameter* rp = pd.clone();
	paramList.push_back(rp);
	return *rp;
}

RichParameterList& RichParameterList::join( const RichParameterList& rps )
{
	for(const RichParameter* p : rps.paramList) {
		paramList.push_back(p->clone());
	}
	return (*this);
}

RichParameter* RichParameterList::pushFromQDomElement(QDomElement np)
{
	RichParameter* rp = nullptr;
	bool b = RichParameterAdapter::create(np, &rp);
	if (b)
		paramList.push_back(rp);
	return rp;
}

void RichParameterList::swap(RichParameterList& oth)
{
	std::swap(paramList, oth.paramList);
}

bool RichParameterList::operator==( const RichParameterList& rps )
{
	if (rps.paramList.size() != paramList.size())
		return false;

	bool iseq = true;

	std::list<RichParameter*>::const_iterator i = paramList.begin();
	std::list<RichParameter*>::const_iterator j = rps.paramList.begin();

	for (; i != paramList.end() && iseq; ++i, ++j){
		if (*i != *j)
			iseq = false;
	}

	return iseq;
}

RichParameterList& RichParameterList::operator=(RichParameterList rps)
{
	swap(rps);
	return *this;
}

RichParameterList::iterator RichParameterList::begin()
{
	return paramList.begin();
}

RichParameterList::iterator RichParameterList::end()
{
	return paramList.end();
}

RichParameterList::const_iterator RichParameterList::begin() const
{
	return paramList.begin();
}

RichParameterList::const_iterator RichParameterList::end() const
{
	return paramList.end();
}
