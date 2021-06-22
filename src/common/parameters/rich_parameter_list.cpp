/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2021                                           \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "rich_parameter_list.h"

#include "../mlexception.h"
#include "../ml_document/mesh_document.h"
#include <vcg/math/matrix44.h>
#include <wrap/qt/col_qt_convert.h>

using namespace vcg;

/**
 * @brief Creates an empty RichParameterList
 */
RichParameterList::RichParameterList()
{
}

/**
 * @brief Creates a copy of the RichParameterList rps
 */
RichParameterList::RichParameterList( const RichParameterList& rps )
{
	for(auto p : rps.paramList) {
		paramList.push_back(p->clone());
	}
}

/**
 * @brief Moves rps in this RichParameterList
 */
RichParameterList::RichParameterList(RichParameterList&& rps)
{
	for(auto& p : rps.paramList) {
		paramList.push_back(p);
		p = nullptr;
	}
}

/**
 * @brief delete the RichParameterList
 */
RichParameterList::~RichParameterList()
{
	clear();
}

/**
 * @return true if the number of RichParameters is zero, false otherwise
 */
bool RichParameterList::isEmpty() const
{
	return paramList.size() == 0;
}

/**
 * @return the number of RichParameters contained on the list
 */
unsigned int RichParameterList::size() const
{
	return paramList.size();
}

/**
 * @brief clears the RichParameterList
 */
void RichParameterList::clear()
{
	for(RichParameter* rp : paramList)
		delete rp;
	paramList.clear();
}

/**
 * @return the bool of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
bool RichParameterList::getBool(const QString& name) const
{
	return getParameterByName(name).value().getBool();
}

/**
 * @return the int of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
int RichParameterList::getInt(const QString& name) const
{
	return getParameterByName(name).value().getInt();
}

/**
 * @return the float of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
Scalarm RichParameterList::getFloat(const QString& name) const
{
	return getParameterByName(name).value().getFloat();
}

/**
 * @return the color of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
QColor RichParameterList::getColor(const QString& name) const
{
	return getParameterByName(name).value().getColor();
}

/**
 * @return the color of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
Color4b RichParameterList::getColor4b(const QString& name) const
{
	return ColorConverter::ToColor4b(getParameterByName(name).value().getColor());
}

/**
 * @return the string of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
QString RichParameterList::getString(const QString& name) const
{
	return getParameterByName(name).value().getString();
}

/**
 * @return the matrix of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
Matrix44m RichParameterList::getMatrix44(const QString& name) const
{
	return getParameterByName(name).value().getMatrix44f();
}

/**
 * @return the point of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
Point3m RichParameterList::getPoint3m(const QString& name) const
{
	return getParameterByName(name).value().getPoint3f();
}

/**
 * @return the shot of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
Shot<Scalarm> RichParameterList::getShotf(const QString& name) const
{
	return Shot<Scalarm>::Construct(getParameterByName(name).value().getShotf());
}

/**
 * @return the float of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
Scalarm RichParameterList::getAbsPerc(const QString& name) const
{
	return getParameterByName(name).value().getFloat();
}

/**
 * @return the enum of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
int RichParameterList::getEnum(const QString& name) const
{
	return getParameterByName(name).value().getInt();
}

/**
 * @return the mesh of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
unsigned int RichParameterList::getMeshId(const QString& name) const
{
	return getParameterByName(name).value().getInt();
}

/**
 * @return the float of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
Scalarm RichParameterList::getDynamicFloat(const QString& name) const
{
	return getParameterByName(name).value().getFloat();
}

/**
 * @return the filename of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
QString RichParameterList::getOpenFileName(const QString& name) const
{
	return getParameterByName(name).value().getString();
}

/**
 * @return the filename of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
QString RichParameterList::getSaveFileName(const QString& name) const
{
	return getParameterByName(name).value().getString();
}

/**
 * @return true if the RichParameter exists
 */
bool RichParameterList::hasParameter(const QString& name) const
{
	const_iterator it = findParameter(name);
	return it != end();
}

/**
 * @return the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
RichParameter& RichParameterList::getParameterByName(const QString& name)
{
	for(RichParameter* rp : paramList) {
		if((rp != nullptr) && rp->name()==name)
			return *rp;
	}
	throw MLException("No parameter with name " + name + " found in RichParameterList");
}

/**
 * @return the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
const RichParameter& RichParameterList::getParameterByName(const QString& name) const
{
	for(RichParameter* rp : paramList) {
		if((rp != nullptr) && rp->name()==name)
			return *rp;
	}
	throw MLException("No parameter with name " + name + " found in RichParameterList");
}

/**
 * @return the iterator pointing to the RichParameter having the given name.
 * an iterator equal to RichParameter::end() is returned if a RichParameter with
 * the given name is not found.
 */
RichParameterList::iterator RichParameterList::findParameter(const QString& name)
{
	for(std::list<RichParameter*>::iterator it = paramList.begin(); it != paramList.end(); ++it) {
		if((*it != nullptr) && (*it)->name()==name)
			return iterator(it);
	}
	return iterator(paramList.end());
}

/**
 * @return the iterator pointing to the RichParameter having the given name.
 * an iterator equal to RichParameter::end() is returned if a RichParameter with
 * the given name is not found.
 */
RichParameterList::const_iterator RichParameterList::findParameter(const QString& name) const
{
	for(std::list<RichParameter*>::const_iterator it = paramList.begin(); it != paramList.end(); ++it) {
		if((*it != nullptr) && (*it)->name()==name)
			return const_iterator(it);
	}
	return const_iterator(paramList.end());
}

/**
 * @return the RichParameter having the given position.
 * @throws an MLException if the index exceeds the size of the list.
 */
RichParameter& RichParameterList::at(unsigned int i)
{
	if (i >= size())
		throw MLException("Index out of bound at RichParameterList::at");
	std::list<RichParameter*>::iterator it = paramList.begin();
	std::advance(it, i);
	return **it;
}

/**
 * @return the RichParameter having the given position.
 * @throws an MLException if the index exceeds the size of the list.
 */
const RichParameter& RichParameterList::at(unsigned int i) const
{
	if (i >= size())
		throw MLException("Index out of bound at RichParameterList::at");
	std::list<RichParameter*>::const_iterator it = paramList.begin();
	std::advance(it, i);
	return **it;
}

/**
 * @brief sets the value of the RichParameter having the given name.
 * @throws an MLException if the name is not found in the list
 */
void RichParameterList::setValue(const QString& name,const Value& newval)
{
	getParameterByName(name).setValue(newval);
}

/**
 * @brief adds a RichParameter to the list.
 * @return a reference to the added parameter
 */
RichParameter& RichParameterList::addParam(const RichParameter& pd )
{
	assert(!hasParameter(pd.name()));
	RichParameter* rp = pd.clone();
	paramList.push_back(rp);
	return *rp;
}

/**
 * @brief joins this RichParameterList to rps
 */
void RichParameterList::join( const RichParameterList& rps )
{
	for(const RichParameter* p : rps.paramList) {
		paramList.push_back(p->clone());
	}
}

/**
 * @brief extracts a RichParameter from the given QDomElement and
 * pushes it in the list
 */
void RichParameterList::pushFromQDomElement(QDomElement np)
{
	RichParameter* rp = nullptr;
	bool b = RichParameterAdapter::create(np, &rp);
	if (b)
		paramList.push_back(rp);
}

/**
 * @brief swaps this RichParameterList with oth
 * @param oth
 */
void RichParameterList::swap(RichParameterList& oth)
{
	std::swap(paramList, oth.paramList);
}

/**
 * @return true if two RichParameterList are equal.
 * @see operator== of RichParameter
 */
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

/**
 * @brief Assignment operator
 */
RichParameterList& RichParameterList::operator=(RichParameterList rps)
{
	swap(rps);
	return *this;
}

/**
 * @return the begin iterator of the list
 */
RichParameterList::iterator RichParameterList::begin()
{
	return iterator(paramList.begin());
}

/**
 * @return the end iterator of the list
 */
RichParameterList::iterator RichParameterList::end()
{
	return iterator(paramList.end());
}

/**
 * @return the begin iterator of the list
 */
RichParameterList::const_iterator RichParameterList::begin() const
{
	return const_iterator(paramList.begin());
}

/**
 * @return the end iterator of the list
 */
RichParameterList::const_iterator RichParameterList::end() const
{
	return const_iterator(paramList.end());
}
