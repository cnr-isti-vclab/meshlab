/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

#include "qtsoap.h"
#include <QtCore/QSet>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

/*! \page qtsoap-overview.html

    \title Qt SOAP component

    \tableofcontents

    \target overview
    \section1 Overview of the SOAP support in the QtSoap classes

    QtSoapMessage provides an interface for creating, inspecting and
    modifying SOAP messages. It has convenience functions for
    generating method requests and inspecting method response
    messages, and also functions that provide easy access to SOAP
    Fault messages.

    The QtSoapType class allows you to inspect SOAP messages with no
    knowledge of XML or DOM. Header and body items are all derived
    from QtSoapType, and through easy accessors and iterators, this
    class and its derivatives make it easy to build arrays
    (QtSoapArray), structs (QtSoapStruct) and simple types like
    String, Integer and Boolean (QtSoapSimpleType).

    Finally, QtSoapHttpTransport provides a convenient way to submit
    SOAP messages to a host via the HTTP protocol.

    \target classes
    \section1 The SOAP classes

    \table
    \header \i Class \i Short description
    \row    \i \l QtSoapMessage
	    \i Represents a SOAP message
    \row    \i \l QtSoapQName
	    \i Represents qualified names (QNames)
    \row    \i \l QtSoapType
	    \i A superclass for all data constructs in a SOAP message.
    \row    \i \l QtSoapArray
	    \i Represents a SOAP array
    \row    \i \l QtSoapArrayIterator
	    \i Lets you iterate over all the values in a SOAP array
    \row    \i \l QtSoapStruct
	    \i Represents a SOAP struct
    \row    \i \l QtSoapStructIterator
	    \i Lets you iterate over all the values in a SOAP array
    \row    \i \l QtSoapSimpleType
	    \i Represents simple SOAP types such as String, Integer and Boolean.
    \row    \i \l QtSoapHttpTransport
	    \i Provides a method for transmitting SOAP messages to an
	    HTTP server and for getting the SOAP reply.
    \endtable

    \target partial
    \section1 Status of the SOAP component

    This is a partial implementation of the SOAP v1.1 protocol.

    \list
    \i Server side SOAP is not supported.
    \i References to values (id and href attributes) are not supported.
    \i Only arrays with less than 5 dimensions are supported.
    \i Namespaces for types are not checked. Only the type names are used.
    \i The encodingStyle attribute is ignored. The serialization and
       encoding rules from section 5 in the SOAP v1.1 specification are
       assumed regardless of the value of the encodingStyle attribute.
    \i QtSoapType does not have accessors for attributes, which means
       for example that actor, mustUnderstand and so on are not
       accessible in headers.
    \i The SOAP root attribute is not supported.
    \endlist
*/

namespace {
    QString localName(const QString &tagName)
    {
	int pos;
	if ((pos = tagName.indexOf(':')))
	    return tagName.right(tagName.length() - pos - 1);

	return tagName;
    }

    QString prefix(const QString &tagName)
    {
	int pos;
	if ((pos = tagName.indexOf(':')))
	    return tagName.left(pos);

	return tagName;
    }

}

/*! \class QtSoapQName qtsoap.h

    \brief The QtSoapQName class provides a wrapper for QNames (names with
    namespaces).

    This class is used extensively in QtSoap to define and identify
    header and body elements, including method and argument names.

    The QtSoapQName consists of a name and a URI. The URI is used as
    the name's namespace, i.e. the name is qualified (hence 'Q'-Name)
    by the URI. The name() and uri() functions return the QNames's
    name and URI.

    The QtSoapQName can be empty. It can also have just a name with no
    URI. Special handling is often applied to a QtSoapQName that has
    no URI. Typically, if a QName with no namespace is used in an
    element in a SOAP document that already has a default namespace
    defined, then that namespace will be applied to the QName.

    \code
    QtSoapMessage message;

    message.setMethod(QtSoapQName("sendMessage", "http://messenging.example.com/"));
    message.addMethodArgument(QtSoapSimpleType(QtSoapQName("a"), 15));
    \endcode
*/


/*!
    Constructs a QName. Sets the QName name to \a name and the URI to
    \a uri.
*/
QtSoapQName::QtSoapQName(const QString &name, const QString &uri)
    : n(name), nuri(uri)
{
}

/*!
    Destructs the QtSoapQName.
*/
QtSoapQName::~QtSoapQName()
{
}

/*!
    Returns QName's name.
*/
QString QtSoapQName::name() const
{
    return n;
}

/*!
    Returns the QName's URI.
*/
QString QtSoapQName::uri() const
{
    return nuri;
}

/*!
    Sets the QName's name to \a s, and sets the URI to an empty string.
*/
QtSoapQName &QtSoapQName::operator =(const QString &s)
{
    n = s;
    nuri = "";

    return *this;
}

/*!
    \fn bool operator==(const QtSoapQName &s1, const QtSoapQName &s2)

    \relates QtSoapQName

    If \a s2 has a non-empty URI, this function returns true if the
    merge of the URI and the name of \a s1 is equal to that of \a s2;
    otherwise it returns false.

    If \a s2 has an empty URI, this function returns true if the name
    of \a s1 is equal to the name of \a s2; otherwise it returns false.

    The comparison is case-insensitive.
*/
bool operator ==(const QtSoapQName &s1, const QtSoapQName &s2)
{
    if (s2.uri() == "")
	return s1.name().toLower() == s2.name().toLower();

    return s1.name().toLower() == s2.name().toLower()
	  && s1.uri().toLower() == s2.uri().toLower();
}

/*!
    \fn bool operator<(const QtSoapQName &s1, const QtSoapQName &s2)

    \relates QtSoapQName

    If \a s2 has a non-empty URI, this function returns true if the
    merge of the URI and the name of \a s1 is lexically less than that
    of \a s2; otherwise it returns false.

    If \a s2 has an empty URI, this function returns true if the name
    of \a s1 is lexically less than the name of \a s2; otherwise
    it returns false.

    The comparison is case-insensitive.
*/
bool operator <(const QtSoapQName &s1, const QtSoapQName &s2)
{
    if (s2.uri() == "")
	return s1.name().toLower() < s2.name().toLower();

    return (s1.uri().toLower()+s1.name().toLower()) < (s2.uri().toLower()+s2.name().toLower());
}

/*! \class QtSoapType qtsoap.h
    \brief The QtSoapType class is the base class for all SOAP types
    in QtSoap.

    Although it is not used to create data types, a QtSoapType
    reference can be conveniently used to inspect a tree of QtSoapType
    subclasses. Accessors from this base class, such as operator[](),
    allow safe navigation.

    \code
    const QtSoapType &root = message.returnValue();
    if (root["fault"].isValid()) {
	qWarning("Warning: %s", root["fault"]["faultstring"].toString().toLatin1().constData());
    }
    \endcode

    This class holds the name(), type(), id() and href() of all its
    derived types.

    If a QtSoapType reference or pointer points to a QtSoapStruct, a
    QtSoapArray or a QtSoapSimpleType, isValid() returns true.

    \sa QtSoapStruct, QtSoapArray, QtSoapSimpleType
*/

/*!
    \class QtSoapStruct qtsoap.h
    \brief The QtSoapStruct class is an implementation of the SOAP
    struct type.

    A SOAP struct is a dictionary of QtSoapTypes where entries are
    looked up by name. Entries in a struct can be of any QtSoapType
    type, such as QtSoapArray, QtSoapStruct or QtSoapSimpleType.

    The struct can be created in several ways. parse() generates a
    struct by analyzing a QDomNode. insert() is used to add items
    manually.

    \code
    QtSoapStruct myStruct(QtSoapQName("myStruct"));

    myStruct.insert(new QtSoapSimpleType(QtSoapQName("item1"), 5));
    myStruct.insert(new QtSoapSimpleType(QtSoapQName("item2"), "hello"));
    myStruct.insert(new QtSoapSimpleType(QtSoapQName("item3"), true));
    \endcode

    Use the operator[]() or at() when looking up entries in a struct
    by name. If the names are unknown, QtSoapStructIterator lets you
    iterate through all the items.

    \code
    QtSoapType &helloItem = myStruct["Hello"];
    \endcode

    toDomElement() converts the QtSoapStruct to a DomElement.

    \sa QtSoapStructIterator, QtSoapType, QtSoapArray, QtSoapSimpleType
*/

/*! \class QtSoapStructIterator
    \brief The QtSoapStructIterator class provides an iterator for
    traversing the items in a QtSoapStruct.

    The iterator is created by passing a QtSoapStruct to the
    constructor. It it not defined which item the iterator initially
    will point to. Neither is the order in which the items are
    processed.

    key() returns the name of the current item. data() and current()
    return a pointer to the current item, or 0 if there is none.
    operator++() navigates to the next item in the struct.

    \code
    for (QtSoapStructIterator it(myStruct); it.current(); ++it) {
	QtSoapType *item = it.data();
	// process item
    }
    \endcode

    \sa QtSoapArrayIterator
*/

/*! \class QtSoapArray qtsoap.h
    \brief The QtSoapArray class is an implementation of the SOAP
    array type.

    A SOAP array is a sequence of QtSoapType objects such as
    QtSoapArrays, QtSoapStructs or QtSoapSimpleTypes, and they are
    accessible through their ordinal position. An array can be
    consecutive (0, 1, 2, 3, ...) or sparse (1, 5, 6, 10, ...) and
    they can be multidimensional ((0, 0), (0, 1), ...). QtSoapArray
    supports arrays of up to 5 dimensions. The size and dimensions of
    the array are set in the constructor. The default constructor
    produces a one dimensional array, with an unset size, meaning that
    the array can grow as required.

    All elements in a SOAP array must be of the same type. Inserting
    different types will result in a run time error.

    The parse() function builds the array by analyzing a QDomNode from
    a SOAP document. count() returns the number of items in the array.

    Retrieve items in the array with at() or operator[](). Note that
    operator[]() only works with one dimensional arrays, but there are
    at() functions for arrays of up to 5 dimensions.
    QtSoapArrayIterator lets you iterate through all items in an
    array.

    Use insert() or append() to insert items into an array manually.
    append() only works with one dimensional arrays.

    toDomElement() returns a QDomElement representation of the SOAP
    array.

    \code
    QtSoapArray array;

    array.insert(0, new QtSoapSimpleType(QtSoapQName("Peter"), "peter"));
    array.insert(1, new QtSoapSimpleType(QtSoapQName("Lisa"), "lisa"));
    array.insert(2, new QtSoapSimpleType(QtSoapQName("Paul"), "paul"));
    array.insert(3, new QtSoapSimpleType(QtSoapQName("Heather"), "heather"));
    \endcode

    \sa QtSoapType, QtSoapStruct, QtSoapSimpleType
*/

/*! \class QtSoapArrayIterator qtsoap.h
    \brief The QtSoapArrayIterator class provides an iterator for
    traversing the items in a QtSoapArray.

    The items are traversed in ascending order of index position,
    depth first.

    \code
    // Construct a 2x2 array of Strings.
    QtSoapArray array("Array of strings", String, 2, 2);
    array.insert(0, 0, new QtSoapSimpleType(Q4SoapQName("top left"), "top left");
    array.insert(0, 1, new QtSoapSimpleType(Q4SoapQName("top right"), "top right");
    array.insert(1, 0, new QtSoapSimpleType(Q4SoapQName("bottom left"), "bottom left");
    array.insert(1, 1, new QtSoapSimpleType(Q4SoapQName("bottom right"), "bottom right");

    // Traverse all items.
    QtSoapArrayIterator it(array);
    while (!it.current()) {

      // Find the position of the current element.
      int pos1, pos2;
      it.pos(&pos1, &pos2);

      qDebug() << "Position (" << pos1 << ", " << pos2 << ") is "
               << "the " << it.current()->toString() << " coordinate of the grid." << endl;

      ++it;
    }
    \endcode
*/

/*! \class QtSoapSimpleType qtsoap.h
    \brief The QtSoapSimpleType class provides a container for all
    simple SOAP types, such as strings, integers and booleans.



    \sa QtSoapType, QtSoapStruct, QtSoapArray
*/

/*! \enum QtSoapType::Type

    SOAP supports the types described in XML Schema Part 2: Datatypes
    listed at \l http://www.w3.org/TR/xmlschema-2/. The following
    values are represented using QtSoapSimpleType, except where noted.

    \value AnyURI
    \value Array Represented by QtSoapArray
    \value Base64Binary
    \value Boolean
    \value Byte
    \value Date
    \value DateTime
    \value Decimal
    \value Double
    \value Duration
    \value ENTITY
    \value Float
    \value GDay
    \value GMonth
    \value GMonthDay
    \value GYear
    \value GYearMonth
    \value HexBinary
    \value ID
    \value IDREF
    \value Int
    \value Integer
    \value Language
    \value Long
    \value NCName
    \value NMTOKEN
    \value NOTATION
    \value Name
    \value NegativeInteger
    \value NonNegativeInteger
    \value NonPositiveInteger
    \value NormalizedString
    \value Other
    \value PositiveInteger
    \value QName
    \value Short
    \value String
    \value Struct Represented by QtSoapStruct
    \value Time
    \value Token
    \value UnsignedByte
    \value UnsignedInt
    \value UnsignedLong
    \value UnsignedShort
*/

/*!
    Constructs a QtSoapType.
*/
QtSoapType::QtSoapType()
{
    t = Other;
    errorStr = "Unknown error";
}

/*!
    Constructs a QtSoapType whose name is \a name and type is \a type.
    This contructor is usually only called by constructors in
    subclasses.
*/
QtSoapType::QtSoapType(const QtSoapQName &name, Type type)
    : t(type), n(name)
{
    errorStr = "Unknown error";
}

/*!
    Creates a QtSoapType copy of \a copy.
*/
QtSoapType::QtSoapType(const QtSoapType &copy)
    : t(copy.t), errorStr(copy.errorStr), i(copy.i),
      n(copy.n), u(copy.u), h(copy.h)
{
}

/*!
    Destructs a QtSoapType.
*/
QtSoapType::~QtSoapType()
{
}

/*!
    Clears any contents. In this base implementation, clear() does
    nothing.
*/
void QtSoapType::clear()
{
}

/*!
    Makes this QtSoapType equal to \a copy.
*/
QtSoapType &QtSoapType::operator =(const QtSoapType &copy)
{
    t = copy.t;
    errorStr = copy.errorStr;
    i = copy.i;
    n = copy.n;
    u = copy.u;
    h = copy.h;
    return *this;
}

/*!
    Returns true if this object is of type QtSoapStruct, QtSoapArray
    or QtSoapSimpletype; otherwise returns false.
*/
bool QtSoapType::isValid() const
{
    return false;
}

/*!
    Returns the QString equivalent of type \a t.
*/
QString QtSoapType::typeToName(QtSoapType::Type t)
{
    switch (t) {
    case Duration: return "duration";
    case DateTime: return "dateTime";
    case Time: return "time";
    case Date: return "date";
    case GYearMonth: return "gYearMonth";
    case GYear: return "gYear";
    case GMonthDay: return "gMonthDay";
    case GDay: return "gDay";
    case GMonth: return "gMonth";
    case Boolean: return "boolean";
    case Base64Binary: return "base64Binary";
    case HexBinary: return "hexBinary";
    case Float: return "float";
    case Double: return "double";
    case AnyURI: return "anyURI";
    case QName: return "QName";
    case NOTATION: return "NOTATION";
    case String: return "string";
    case NormalizedString: return "normalizedString";
    case Token: return "token";
    case Language: return "language";
    case Name: return "name";
    case NMTOKEN: return "NMToken";
    case NCName: return "NCName";
    case ID: return "ID";
    case IDREF: return "IDREF";
    case ENTITY: return "ENTITY";
    case Decimal: return "decimal";
    case Integer: return "integer";
    case NonPositiveInteger: return "nonPositiveInteger";
    case NegativeInteger: return "negativeInteger";
    case Long: return "long";
    case Int: return "int";
    case Short: return "short";
    case Byte: return "byte";
    case NonNegativeInteger: return "nonNegativeInteger";
    case UnsignedLong: return "unsignedLong";
    case PositiveInteger: return "positiveInteger";
    case UnsignedInt: return "unsignedInt";
    case UnsignedShort: return "unsignedShort";
    case UnsignedByte: return "unsignedByte";
    case Array: return "array";
    case Struct: return "struct";
    default: return "other";
    }
}

/*!
    Returns the QtSoapType::Type called \a name.
*/
QtSoapType::Type QtSoapType::nameToType(const QString &name)
{
    const QString type = name.trimmed().toLower();

    if (type == "string")
	return String;
    else if (type == "normalizedstring")
	return NormalizedString;
    else if (type == "token")
	return Token;
    else if (type == "language")
	return Language;
    else if (type == "name")
	return Name;
    else if (type == "ncname")
	return NCName;
    else if (type == "nmtoken")
	return NMTOKEN;
    else if (type == "id")
	return ID;
    else if (type == "idref")
	return IDREF;
    else if (type == "entity")
	return ENTITY;
    else if (type == "base64binary")
	return Base64Binary;
    else if (type == "hexBinary")
	return HexBinary;
    else if (type == "anyuri")
	return AnyURI;
    else if (type == "qname")
	return QName;
    else if (type == "notation")
	return NOTATION;
    else if (type == "duration")
	return Duration;
    else if (type == "datetime")
	return DateTime;
    else if (type == "time")
	return Time;
    else if (type == "date")
	return Date;
    else if (type == "gyearmonth")
	return GYearMonth;
    else if (type == "gyear")
	return GYear;
    else if (type == "gmonthday")
	return GMonthDay;
    else if (type == "gday")
	return GDay;
    else if (type == "gmonth")
	return GMonth;
    else if (type == "decimal")
	return Decimal;
    else if (type == "integer")
	return Integer;
    else if (type == "nonPositiveinteger")
	return NonPositiveInteger;
    else if (type == "negativeinteger")
	return NegativeInteger;
    else if (type == "long")
	return Long;
    else if (type == "int")
	return Int;
    else if (type == "short")
	return Short;
    else if (type == "byte")
	return Byte;
    else if (type == "nonnegativeinteger")
	return NonNegativeInteger;
    else if (type == "unsignedlong")
	return UnsignedLong;
    else if (type == "unsignedint")
	return UnsignedInt;
    else if (type == "unsignedshort")
	return UnsignedShort;
    else if (type == "unsignedbyte")
	return UnsignedByte;
    else if (type == "positiveinteger")
	return PositiveInteger;
    else if (type == "float")
	return Float;
    else if (type == "double")
	return Double;
    else if (type == "boolean")
	return Boolean;
    else
	return Other;
}

/*!
    Returns QString::null.
*/
QString QtSoapType::toString() const
{
    return QString::null;
}

/*!
    Returns 0.
*/
int QtSoapType::toInt() const
{
    return 0;
}

/*!
    Returns false.
*/
bool QtSoapType::toBool() const
{
    return false;
}

/*!
    Returns the QDomElement representation of this QtSoapType
    as a child of \a document.
*/
QDomElement QtSoapType::toDomElement(QDomDocument document) const
{
    Q_UNUSED(document);
    return QDomElement();
}

/*!
    Returns the QString representation of this QtSoapType's type.
*/
QString QtSoapType::typeName() const
{
    return QtSoapType::typeToName(type());
}

/*!
    Returns the type as a QtSoapType::Type.
*/
QtSoapType::Type QtSoapType::type() const
{
    return t;
}

/*!
    Returns the QName (qualified name) of this QtSoapType.
*/
QtSoapQName QtSoapType::name() const
{
    return n;
}

/*!
    Returns the ID of this QtSoapType.
*/
QString QtSoapType::id() const
{
    return i;
}

/*!
    Returns the href attribute of this QtSoapType.
*/
QString QtSoapType::href() const
{
    return h;
}

/*!
    Sets the QName (qualified name) of this QtSoapType to \a name.
*/
void QtSoapType::setName(const QtSoapQName &name)
{
    this->n = name;
}

/*!
    Sets the ID of this QtSoapType to \a i.
*/
void QtSoapType::setId(const QString &i)
{
    this->i = i;
}

/*!
    Sets the href attribute of this QtSoapType to \a h.
*/
void QtSoapType::setHref(const QString &h)
{
    this->h = h;
}

/*!
    Returns the value of this QtSoapType. In the base implementation,
    an invalid QVariant() is returned.
*/
QVariant QtSoapType::value() const
{
    return QVariant();
}

/*!
    Returns a human readable explanation of the most recent error.
*/
QString QtSoapType::errorString() const
{
    return errorStr;
}

/*!
    Returns the number of child items in this QtSoapType. In the base
    implementation, this returns 0.
*/
int QtSoapType::count() const
{
    return 0;
}

/*!
    Returns a reference to the child item at ordinal position \a pos.
    If no item exists at this position, returns an empty QtSoapType.
*/
QtSoapType &QtSoapType::operator [](int /* pos */ )
{
    static QtSoapType NIL;
    return NIL;
}

/*!
    \overload

    Returns a reference to the child item whose QName (qualified name)
    is \a key. If no item exists with this key an empty QtSoapType is
    returned.
*/
QtSoapType &QtSoapType::operator [](const QtSoapQName & /* key */)
{
    static QtSoapType NIL;
    return NIL;
}

/*!
    \overload

    Returns a reference to the child item whose QName (qualified name)
    is \a key, regardless of the qualified name's URI. If no item
    exists with this key, an empty QtSoapType is returned.
*/
QtSoapType &QtSoapType::operator [](const QString & /* key */)
{
    static QtSoapType NIL;
    return NIL;
}

/*!
    \overload

    Returns a reference to the child item at ordinal position \a pos.
    If no item exists at this position, returns an empty QtSoapType.
*/
const QtSoapType &QtSoapType::operator [](int /* pos */) const
{
    static QtSoapType NIL;
    return NIL;
}

/*!
    \overload

    Returns a reference to the child item whose QName (qualified name)
    is \a key. If no item exists with this key, returns an empty
    QtSoapType.
*/
const QtSoapType &QtSoapType::operator [](const QtSoapQName & /* key */) const
{
    static QtSoapType NIL;
    return NIL;
}

/*!
    \overload

    Returns a reference to the child item whose QName (qualified name)
    is \a key, regardless of the qualified name's URI. If no item
    exists with this key, returns an empty QtSoapType.
*/
const QtSoapType &QtSoapType::operator [](const QString & /* key */) const
{
    static QtSoapType NIL;
    return NIL;
}

/*!
    Attempts to interpret \a node as a QtSoapType, and returns true if
    successful. This base implementation always returns false.
*/
bool QtSoapType::parse(QDomNode /* node */)
{
    return false;
}

/*!
    \overload

    Constructs an empty one dimensional QtSoapArray whose element type
    is undefined. The first insert will decide what type of elements
    the array can contain.
*/
QtSoapArray::QtSoapArray()
    : QtSoapType(QtSoapQName(), Array), arrayType(Other), order(1)
{
    lastIndex = 0;
    siz0 = 0;
    siz1 = 0;
    siz2 = 0;
    siz3 = 0;
    siz4 = 0;
}

/*!
    Constructs an empty QtSoapArray whose QName (qualified name) is \a
    name, that contains elements of type \a type, and whose dimensions
    are given by \a size0, \a size1, \a size2, \a size3 and \a size4.

    To construct a one dimensional array of size 5, set \a size0 = 5. To
    create a two dimensional array of size 5x10, set \a size0 = 5 and \a
    size1 = 10. The maximum dimension of a QtSoapArray is 5.
*/
QtSoapArray::QtSoapArray(const QtSoapQName &name, QtSoapType::Type type, int size0,
			 int size1, int size2, int size3, int size4)
    : QtSoapType(name, Array), lastIndex(0), arrayType(type),
      siz0(size0), siz1(size1), siz2(size2), siz3(size3),
      siz4(size4)
{
    if (size4 != -1) order = 5;
    else if (size3 != -1) order = 4;
    else if (size2 != -1) order = 3;
    else if (size1 != -1) order = 2;
    else order = 1;
}

/*!
    Create a QtSoapArray that is a copy of \a copy.
*/
QtSoapArray::QtSoapArray(const QtSoapArray &copy)
    : QtSoapType(copy)
{
    *this = copy;
}

/*!
    Destructs the QtSoapArray.
*/
QtSoapArray::~QtSoapArray()
{
}

/*!
    Clears the contents, and the dimensions of the QtSoapArray.
*/
void QtSoapArray::clear()
{
    array.clear();
    lastIndex = 0;
    arrayType = Other;
    siz0 = siz1 = siz2 = siz3 = siz4 = 0;
    order = -1;
}

/*!
    Makes this QtSoapArray a copy of \a copy.
*/
QtSoapArray &QtSoapArray::operator = (const QtSoapArray &copy)
{
    if (this == &copy)
        return *this;
    t = copy.t;
    errorStr = copy.errorStr;
    i = copy.i;
    n = copy.n;
    u = copy.u;
    h = copy.h;
    lastIndex = copy.lastIndex;
    order = copy.order;
    siz0 = copy.siz0;
    siz1 = copy.siz1;
    siz2 = copy.siz2;
    siz3 = copy.siz3;
    siz4 = copy.siz4;
    array = copy.array;

    return *this;
}

/*!
    Appends the QtSoapType \a item to the end of this array, which must
    be one dimensional.

    \sa insert()
*/
void QtSoapArray::append(QtSoapType *item)
{
    if (order != 1) {
	qWarning("Attempted to insert item at position (%i) in %i-dimensional QtSoapArray.",
		 lastIndex, order);
	return;
    }

    if (array.count() == 0) {
	array.insert(0, item);
    } else {
	array.insert(lastIndex + 1, item);
	++lastIndex;
    }
}

/*!
    Inserts the QtSoapType \a item at the absolute position \a pos in
    the array. Note that this function can be used to insert items
    into arrays with any number of dimensions.

    If the array is one dimensional, then \a pos is simply the index
    position in the array. But if the array is multi-dimensional then
    \a pos is the absolute position. For example, if we have a two
    dimensional array [['a', 'b'], ['c', 'd'], ['e', 'f']], the
    element at position 0 is 'a', at position 1 is 'b', at position 2
    is 'c', and so on. (There are other insert() overloads that allow
    for each dimension to be specified individually.)
*/
void QtSoapArray::insert(int pos, QtSoapType *item)
{
    if (arrayType == Other)
	arrayType = item->type();

    if (item->type() != arrayType) {
	qWarning("Attempted to insert item of type \"%s\" in QtSoapArray of type \"%s\".",
		 item->typeName().toLatin1().constData(), QtSoapType::typeToName(arrayType).toLatin1().constData());
	return;
    }

    if (order == -1)
	order = 1;
    else if (order == 1 && pos > lastIndex)
	lastIndex = pos;

    array.insert(pos, item);
}

/*!
    \overload

    Insert the QtoSoapType \a item at position \a pos0 x \a pos1 in a
    two dimensional array.
*/
void QtSoapArray::insert(int pos0, int pos1, QtSoapType *item)
{
    if (order != 2) {
	qWarning("Attempted to insert item at position (%i, %i)"
		 " in %i-dimensional QtSoapArray.",
		 pos0, pos1, order);
	return;
    }

    if (pos0 < 0 || pos0 >= siz0 || pos1 < 0 || pos1 >= siz1) {
	qWarning("Attempted to insert item at position (%i, %i)"
		 " when range of QtSoapArray is (0..%i, 0..%i)",
		 pos0, pos1, siz0 - 1, siz1 - 1);
	return;
    }

    insert((pos0 * siz1) + pos1, item);
}

/*!
    \overload

    Insert the QtoSoapType \a item at position \a pos0 x \a pos1 x \a
    pos2 in a three dimensional array.
*/
void QtSoapArray::insert(int pos0, int pos1, int pos2, QtSoapType *item)
{
    if (order != 3) {
	qWarning("Attempted to insert item at position (%i, %i, %i)"
		 " in %i-dimensional QtSoapArray.",
		 pos0, pos1, pos2, order);
	return;
    }

    if (pos0 < 0 || pos0 >= siz0 || pos1 < 0 || pos1 >= siz1 || pos2 < 0 || pos2 >= siz2) {
	qWarning("Attempted to insert item at position (%i, %i, %i)"
		 " when range of QtSoapArray is (0..%i, 0..%i, 0..%i)",
		 pos0, pos1, pos2, siz0 - 1, siz1 - 1, siz2 - 1);
	return;
    }

    insert((pos0 * siz2 * siz1) + (pos1 * siz2) + pos2, item);
}

/*!
    \overload

    Insert the QtoSoapType \a item at position \a pos0 x \a pos1 x \a
    pos2 x \a pos3 in a four dimensional array.
*/
void QtSoapArray::insert(int pos0, int pos1, int pos2, int pos3, QtSoapType *item)
{
    if (order != 4) {
	qWarning("Attempted to insert item at position (%i, %i, %i, %i)"
		 " in %i-dimensional QtSoapArray.",
		 pos0, pos1, pos2, pos3, order);
	return;
    }

    insert((pos0 * siz3 * siz2 * siz1)
	   + (pos1 * siz3 * siz2)
	   + (pos2 * siz3)
	   + pos3,
	   item);
}

/*!
    \overload

    Insert the QtoSoapType \a item at position \a pos0 x \a pos1 x \a
    pos2 x \a pos3 x \a pos4 in a five dimensional array.
*/
void QtSoapArray::insert(int pos0, int pos1, int pos2, int pos3, int pos4,
			 QtSoapType *item)
{
    if (order != 5) {
	qWarning("Attempted to insert item at position (%i, %i, %i, %i, %i)"
		 " in %i-dimensional QtSoapArray.",
		 pos0, pos1, pos2, pos3, pos4, order);
	return;
    }

    insert((pos0 * siz4 * siz3 * siz2 * siz1)
	   + (pos1 * siz4 * siz3 * siz2)
	   + (pos2 * siz4 * siz3)
	   + (pos3 * siz4)
	   + pos4,
	   item);
}

/*! \internal

    Given the size and dimensions of the array, generates a string
    used to represent the array in XML. For example, a 1-dimensional
    array of size 5 would get [5], a 2-dimensional array of size 5x10
    would get [5,10].
*/
QString QtSoapArray::arraySizeString() const
{
    QString arraySize = "[";
    if (siz0 != -1) {
	arraySize += QString::number(siz0);
	if (order > 1) arraySize += "," + QString::number(siz1);
	if (order > 2) arraySize += "," + QString::number(siz2);
	if (order > 3) arraySize += "," + QString::number(siz3);
	if (order > 4) arraySize += "," + QString::number(siz4);
    }

    arraySize += "]";
    return arraySize;
}

/*! \internal

    Recursively inspects the items and any child arrays' items to
    generate the aggregate type of items in this array. It the array
    contains ints, returns "int[5]", but if the array is of arrays of
    arrays of ints, the function returns "int[][][5]".
*/
QString QtSoapArray::arrayTypeString() const
{
    if (arrayType != Array)
	return QtSoapType::typeToName(arrayType);

    QString atString;
    QtSoapArray *ar = const_cast<QtSoapArray *>(this);
    do {
	if (ar->count()	== 0)
	    break;

	atString += ar->arraySizeString();

	QtSoapArrayIterator it(*const_cast<QtSoapArray *>(this));
	if (it.data()->type() != Array)
	    break;

	ar = (QtSoapArray *)it.data();
    } while (ar);


    QtSoapArrayIterator it(*const_cast<QtSoapArray *>(this));
    if (ar->count() == 0)
	atString = QtSoapSimpleType::typeToName(Int) + atString;
    else
	atString = it.data()->typeName() + atString;

    return atString;
}

/*!
    Returns the QDomElement representation of this QtSoapArray. The
    returned QDomElement is created using \a doc.
*/
QDomElement QtSoapArray::toDomElement(QDomDocument doc) const
{
    QString prefix = QtSoapNamespaces::instance().prefixFor(n.uri());
    QDomElement a = n.uri() == ""
		    ? doc.createElement( n.name())
		    : doc.createElementNS(n.uri(), prefix + ":" + n.name());

    QString schemaprefix = QtSoapNamespaces::instance().prefixFor(XML_SCHEMA_INSTANCE);
    a.setAttributeNS(XML_SCHEMA_INSTANCE, schemaprefix + ":type", "xsd:Array");

    QString encprefix = QtSoapNamespaces::instance().prefixFor(SOAPv11_ENCODING);
    a.setAttributeNS(SOAPv11_ENCODING, encprefix + ":arrayType", "xsd:" + arrayTypeString());

    for (QtSoapArrayIterator i(*const_cast<QtSoapArray *>(this)); !i.atEnd(); ++i) {
	QDomElement item = i.data()->toDomElement(doc);
	item.setTagName("item");

	int pos0, pos1, pos2, pos3, pos4;
	i.pos(&pos0, &pos1, &pos2, &pos3, &pos4);

	QString position = "[" + QString::number(pos0);
	if (order > 1) position += "," + QString::number(pos1);
	if (order > 2) position += "," + QString::number(pos2);
	if (order > 3) position += "," + QString::number(pos3);
	if (order > 4) position += "," + QString::number(pos4);
	position += "]";

	QString envprefix = QtSoapNamespaces::instance().prefixFor(SOAPv11_ENVELOPE);
	item.setAttributeNS(SOAPv11_ENVELOPE, envprefix + ":position", position);
	a.appendChild(item);
    }

    return a;
}

/*! \reimp

    For this class, always returns true.
*/
bool QtSoapArray::isValid() const
{
    return true;
}

/*!
    Inspects \a node and builds the content of the QtSoapArray if \a
    node qualifies as a SOAP array. Returns true if it does; otherwise
    returns false.
*/
bool QtSoapArray::parse(QDomNode node)
{
    if (node.isNull() || !node.isElement())
	return false;

    QDomElement e = node.toElement();
    QDomAttr typeattr = e.attributeNode("type");
    if (!typeattr.isNull() && (localName(typeattr.value()).toLower() != "array"))
	return false;


    QDomNodeList children = e.childNodes();
    int c = children.count();
    array.clear();
    // ### array.resize(c);

    int pos = 0;
    for (int i = 0; i < c; ++i) {
	QDomNode n = children.item(i);
        if (n.isComment())
            continue;
	if (!n.isElement()){
	    // ### An error in the soap document.
	    return false;
	}

	QDomElement elem = n.toElement();

	QtSmartPtr<QtSoapType> type = QtSoapTypeFactory::instance().soapType(elem);
	if (!type.ptr()) {
	    // ### An error in the soap document.
	    return false;
	}

	// ### Check namespace
	QDomAttr posattr = elem.attributeNode("position");
	if (!posattr.isNull())
	    pos = posattr.value().toInt();

	array.insert(pos, type);
	++pos;
    }

    setName(QtSoapQName(localName(e.tagName()), e.namespaceURI()));
    return true;
}

/*!
    Returns the number of items in the array. Note that this is not
    the same as the size of the array, because the array may be sparse.
*/
int QtSoapArray::count() const
{
    return array.count();
}

/*!
    Returns a reference to the item at ordinal position \a pos. If
    there is no item at position \a pos, returns an empty QtSoapType.
*/
QtSoapType &QtSoapArray::operator [](int pos)
{
    return at(pos);
}

/*!
    \overload

    Returns a reference to the child item whose local name is \a s. If
    there is no item with this local name, returns an empty
    QtSoapType.
*/
QtSoapType &QtSoapArray::operator [](const QString &s)
{
    return QtSoapType::operator[](s);
}

/*!
    \overload

    Returns a reference to the child item whose QName (qualified name)
    is \a s. If there is no item with this name, returns an empty
    QtSoapType.
*/
QtSoapType &QtSoapArray::operator [](const QtSoapQName &s)
{
    return QtSoapType::operator[](s);
}

/*!
    \overload

    Returns a reference to the item at ordinal position \a pos. If
    there is no item at position \a pos, returns an empty QtSoapType.
*/
const QtSoapType &QtSoapArray::operator [] (int pos) const
{
    return at(pos);
}

/*!
    \overload

    Returns a reference to the child item whose local name is \a s. If
    there is no item with this local name, returns an empty
    QtSoapType.
*/
const QtSoapType &QtSoapArray::operator [](const QString &s) const
{
    return QtSoapType::operator[](s);
}

/*!
    \overload

    Returns a reference to the child item whose QName (qualified name)
    is \a s. If there is no item with this name, returns an empty
    QtSoapType.
*/
const QtSoapType &QtSoapArray::operator [](const QtSoapQName &s) const
{
    return QtSoapType::operator[](s);
}

/*!
    Returns a reference to the item at ordinal position \a pos. If
    there is no item at position \a pos, returns an empty QtSoapType.
*/
QtSoapType &QtSoapArray::at(int pos)
{
    static QtSoapType NIL;

    if (array.find(pos) != array.end())
	return *array[pos];
    else
	return NIL;
}

/*!
    \overload

    Returns a reference to the item at ordinal position \a pos0 x \a
    pos1 in a two dimensional array. If there is no such item, returns
    an empty QtSoapType.
*/
QtSoapType &QtSoapArray::at(int pos0, int pos1)
{
    return at(pos0 * siz1 + pos1);
}

/*!
    \overload

    Returns a reference to the item at ordinal position \a pos0 x \a
    pos1 x \a pos2 in a three dimensional array. If there is no such
    item, returns an empty QtSoapType.
*/
QtSoapType &QtSoapArray::at(int pos0, int pos1, int pos2)
{
    return at((pos0 * siz2 * siz1) + (pos1 * siz2) + pos2);
}

/*!
    \overload

    Returns a reference to the item at ordinal position \a pos0 x \a
    pos1 x \a pos2 x \a pos3 in a four dimensional array. If there is no
    such item, returns an empty QtSoapType.
*/
QtSoapType &QtSoapArray::at(int pos0, int pos1, int pos2, int pos3)
{
    return at((pos0 * siz3 * siz2 * siz1)
	      + (pos1 * siz3 * siz2)
	      + (pos2 * siz3)
	      + pos3);
}

/*!
    \overload

    Returns a reference to the item at ordinal position \a pos0 x \a
    pos1 x \a pos2 x \a pos3 x \a pos4 in a five dimensional array. If
    there is no such item, returns an empty QtSoapType.
*/
QtSoapType &QtSoapArray::at(int pos0, int pos1, int pos2, int pos3, int pos4)
{
    return at((pos0 * siz4 * siz3 * siz2 * siz1)
	      + (pos1 * siz4 * siz3 * siz2)
	      + (pos2 * siz4 * siz3)
	      + (pos3 * siz4)
	      + pos4);
}

/*!
    \overload

    Returns a reference to the item at ordinal position \a pos. If
    there is no item at position \a pos, returns an empty QtSoapType.
*/
const QtSoapType &QtSoapArray::at(int pos) const
{
    static QtSoapType NIL;

    if (array.find(pos) != array.end())
	return *array[pos];
    else
	return NIL;
}

/*!
    \overload

    Returns a reference to the item at ordinal position \a pos0 x \a
    pos1 in a two dimensional array. If there is no such item, returns
    an empty QtSoapType.
*/
const QtSoapType &QtSoapArray::at(int pos0, int pos1) const
{
    return at(pos0 * siz1 + pos1);
}

/*!
    \overload

    Returns a reference to the item at ordinal position \a pos0 x \a
    pos1 x \a pos2 in a three dimensional array. If there is no such
    item, returns an empty QtSoapType.
*/
const QtSoapType &QtSoapArray::at(int pos0, int pos1, int pos2) const
{
    return at((pos0 * siz2 * siz1) + (pos1 * siz2) + pos2);
}

/*!
    \overload

    Returns a reference to the item at ordinal position \a pos0 x \a
    pos1 x \a pos2 x \a pos3 in a four dimensional array. If there is no
    such item, returns an empty QtSoapType.
*/
const QtSoapType &QtSoapArray::at(int pos0, int pos1, int pos2, int pos3) const
{
    return at((pos0 * siz3 * siz2 * siz1)
	      + (pos1 * siz3 * siz2)
	      + (pos2 * siz3)
	      + pos3);
}

/*!
    \overload

    Returns a reference to the item at ordinal position \a pos0 x \a
    pos1 x \a pos2 x \a pos3 x \a pos4 in a five dimensional array. If
    there is no such item, returns an empty QtSoapType.
*/
const QtSoapType &QtSoapArray::at(int pos0, int pos1, int pos2, int pos3, int pos4) const
{
    return at((pos0 * siz4 * siz3 * siz2 * siz1)
	      + (pos1 * siz4 * siz3 * siz2)
	      + (pos2 * siz4 * siz3)
	      + (pos3 * siz4)
	      + pos4);
}

/*!
    Constructs a QtSoapArrayIterator on \a array, initializing
    the iterator to point to the first element.
*/
QtSoapArrayIterator::QtSoapArrayIterator(QtSoapArray &array)
    : it(array.array.begin()), arr(&array)
{
}

/*!
    Constructs a QtSoapArrayIterator that is a copy of \a copy.
*/
QtSoapArrayIterator::QtSoapArrayIterator(const QtSoapArrayIterator &copy)
    : it(copy.it), arr(copy.arr)
{
}

/*!
    Returns false if this iterator points to an item in the array, otherwise true.
*/
bool QtSoapArrayIterator::atEnd() const
{
    return (it == arr->array.end());
}

/*!
    Assignment operator of QtSoapArrayIterator. Makes this iterator a
    copy of \a copy.
*/
QtSoapArrayIterator &QtSoapArrayIterator::operator =(const QtSoapArrayIterator &copy)
{
    it = copy.it;
    arr = copy.arr;

    return *this;
}

/*!
    Destructs the QtSoapArrayIterator.
*/
QtSoapArrayIterator::~QtSoapArrayIterator()
{
}

/*!
    \overload

    Returns the ordinal position of the iterator. Works for arrays of
    any dimension, but is only useful for one dimensional arrays.
*/
int QtSoapArrayIterator::pos() const
{
    return it.key();
}

/*!
    Populates the arguments \a pos0, \a pos1, \a pos2, \a pos3 and \a
    pos4 with the coordinate of the current position of the iterator.
    For a one dimensional array, only \a pos0 is populated. For a two
    dimensional array, \a pos0 and \a pos1 are populated, and so on.

    Any of the arguments that are 0-pointers are ignored.
*/
void QtSoapArrayIterator::pos(int *pos0, int *pos1, int *pos2,
			      int *pos3, int *pos4) const
{
    const int key = it.key();

    switch (arr->order) {
    case 1:
	if (pos0) *pos0 = key;
	break;
    case 2: {
	const int tmp = key / arr->siz1;
	if (pos0) *pos0 = tmp;
	if (pos1) *pos1 = key - (tmp * arr->siz1);
    }
	break;
    case 3: {
	const int tmp0 = key / (arr->siz2 * arr->siz1);
	const int tmp1 = key - (tmp0 * (arr->siz2 * arr->siz1));
	const int tmp2 = tmp1 / arr->siz2;
	if (pos0) *pos0 = tmp0;
	if (pos1) *pos1 = tmp2;
	if (pos2) *pos2 = tmp1 - (tmp2 * arr->siz2);
    }
	break;
    case 4: {
	const int tmp0 = key / (arr->siz3 * arr->siz2 * arr->siz1);
	const int tmp1 = key - (tmp0 * (arr->siz3 * arr->siz2 * arr->siz1));
	const int tmp2 = tmp1 / (arr->siz3 * arr->siz2);
	const int tmp3 = tmp1 - (tmp2 * (arr->siz3 * arr->siz2));
	const int tmp4 = tmp3 / arr->siz3;
	const int tmp5 = tmp3 - (tmp4 * arr->siz3);
	if (pos0) *pos0 = tmp0;
	if (pos1) *pos1 = tmp2;
	if (pos2) *pos2 = tmp4;
	if (pos3) *pos3 = tmp5;
    }
	break;
    case 5: {
	const int tmp0 = key / (arr->siz4 * arr->siz3 * arr->siz2 * arr->siz1);
	const int tmp1 = key - (tmp0 * (arr->siz4 * arr->siz3 * arr->siz2 * arr->siz1));
	const int tmp2 = tmp1 / (arr->siz4 * arr->siz3 * arr->siz2);
	const int tmp3 = tmp1 - (tmp2 * (arr->siz4 * arr->siz3 * arr->siz2));
	const int tmp4 = tmp3 / (arr->siz4 * arr->siz3);
	const int tmp5 = tmp3 - (tmp4 * arr->siz4 * arr->siz3);
	const int tmp6 = tmp5 / arr->siz3;
	const int tmp7 = tmp5 - (tmp6 * arr->siz3);
	if (pos0) *pos0 = tmp0;
	if (pos1) *pos1 = tmp2;
	if (pos2) *pos2 = tmp4;
	if (pos3) *pos3 = tmp6;
	if (pos4) *pos4 = tmp7;
    }
	break;
    default:
	break;
    }
}

/*!
    Returns a reference to the item that the iterator is currently
    pointing to.
*/
QtSoapType *QtSoapArrayIterator::data()
{
    if (it == arr->array.end())
        return 0;
    return it.value().ptr();
}

/*!
    Returns a reference to the item that the iterator is currently
    pointing to.
*/
const QtSoapType *QtSoapArrayIterator::current() const
{
    if (it == arr->array.end())
        return 0;
    return it.value().ptr();
}

/*!
    Moves the iterator position to the next item in the array.
*/
void QtSoapArrayIterator::operator ++()
{
    ++it;
}

/*!
    Returns true if this Iterator's position is not equal to
    the position of \a j; otherwise returns false.
*/
bool QtSoapArrayIterator::operator != (const QtSoapArrayIterator &j) const
{
    return it != j.it;
}

/*!
    Returns true if this Iterator's position is equal to the position
    of \a j; otherwise returns false.
*/
bool QtSoapArrayIterator::operator == (const QtSoapArrayIterator &j) const
{
    return it == j.it;
}

/*!
    Constructs an empty QtSoapStruct.
*/
QtSoapStruct::QtSoapStruct()
    : QtSoapType(QtSoapQName(), Struct)
{
}

/*!
    Constructs an empty QtSoapStruct and sets its QName (qualified
    name) to \a name.
*/
QtSoapStruct::QtSoapStruct(const QtSoapQName &name)
    : QtSoapType(name, Struct)
{
}

/*!
    Constructs a QtSoapStruct that is a copy of \a copy.
*/
QtSoapStruct::QtSoapStruct(const QtSoapStruct &copy)
    : QtSoapType(copy)
{
    *this = copy;
}

/*!
    Destructs the QtSoapStruct.
*/
QtSoapStruct::~QtSoapStruct()
{
}

/*!
    Removes all items from the struct.
*/
void QtSoapStruct::clear()
{
    dict.clear();
}

/*!
    Makes this struct a copy of \a copy.
*/
QtSoapStruct &QtSoapStruct::operator =(const QtSoapStruct &copy)
{
    if (this == &copy)
        return *this;
    t = copy.t;
    errorStr = copy.errorStr;
    i = copy.i;
    n = copy.n;
    u = copy.u;
    h = copy.h;
    i = copy.i;
    dict = copy.dict;

    return *this;
}

/*!
    Inserts the QtSoapType \a item into this struct. Any existing
    item with the same QName (qualified name) will be erased.
*/
void QtSoapStruct::insert(QtSoapType *item)
{
    dict.append(item);
}

/*!
    Generates the QDomElement representation of this struct.  The
    returned QDomElement is created using \a doc.
*/
QDomElement QtSoapStruct::toDomElement(QDomDocument doc) const
{
    QString prefix = QtSoapNamespaces::instance().prefixFor(n.uri());
    QDomElement a = n.uri() == ""
		    ? doc.createElement(n.name())
		    : doc.createElementNS(n.uri(), prefix + ":" + n.name());

    for (QtSoapStructIterator i(*const_cast<QtSoapStruct *>(this)); i.data(); ++i)
	a.appendChild(i.data()->toDomElement(doc));

    return a;
}

/*! \reimp
 */
bool QtSoapStruct::isValid() const
{
    return true;
}

/*!
    Inspects \a node and constructs the equivalent QtSoapStruct if \a
    node qualifies as a SOAP struct. Returns true if it does;
    otherwise returns false.
*/
bool QtSoapStruct::parse(QDomNode node)
{
    if (node.isNull() || !node.isElement())
	return false;

    QDomElement e = node.toElement();
    QDomNodeList children = e.childNodes();
    int c = children.count();
    dict.clear();

    for (int i = 0; i < c; ++i) {
	QDomNode n = children.item(i);
        if (n.isComment())
            continue;
	if (!n.isElement()){
	    errorStr = "In the struct element " + e.tagName();
	    errorStr += ", the " + QString::number(i) + "th child ";
	    errorStr += "is not an element.";
	    return false;
	}

	QtSmartPtr<QtSoapType> type = QtSoapTypeFactory::instance().soapType(n.toElement());
	if (!type.ptr()) {
	    errorStr = "In the struct element " + e.tagName();
	    errorStr += ", child #" + QString::number(i) + ", ";
	    errorStr += n.toElement().tagName() + ", was not recognized as a SOAP type.";
	    return false;
	}

	dict.append(type);
    }

    setName(QtSoapQName(localName(e.tagName()), e.namespaceURI()));
    return true;
}

/*!
    Returns the number of items in this struct.
*/
int QtSoapStruct::count() const
{
    return dict.count();
}

/*!
    Returns a reference to the item in this struct whose QName
    (qualified name) is \a key. If no such item exists, an empty
    QtSoapType is returned.
*/
QtSoapType &QtSoapStruct::operator [](const QtSoapQName &key)
{
    return at(key);
}

/*!
    \overload

    Returns a reference to the item in this struct whose QName
    (qualified name) is \a key. If no such item exists, an empty
    QtSoapType is returned.
*/
const QtSoapType &QtSoapStruct::operator [](const QtSoapQName &key) const
{
    return at(key);
}

/*!
    \overload

    Returns a reference to the item in this struct whose QName
    (qualified name) is \a key. If no such item exists, an empty
    QtSoapType is returned.
*/
QtSoapType &QtSoapStruct::operator [](const QString &key)
{
    return at(QtSoapQName(key, ""));
}

/*!
    \overload

    Returns a reference to the item in this struct whose QName
    (qualified name) is \a key. If no such item exists, an empty
    QtSoapType is returned.
*/
const QtSoapType &QtSoapStruct::operator [](const QString &key) const
{
    return at(QtSoapQName(key, ""));
}

/*!
    \overload

    Returns a reference to item number \a i in this struct. If no such
    item exists, an empty QtSoapType is returned.

    The items are ordered in the sequence in which they were inserted,
    starting from 0.
*/
QtSoapType &QtSoapStruct::operator [](int i)
{
    static QtSoapType NIL;
    if (i < 0 || i >= dict.count())
        return NIL;

    return *dict[i].ptr();
}

/*!
    \overload

    Returns a reference to item number \a i in this struct. If no such
    item exists, an empty QtSoapType is returned.

    The items are ordered in the sequence in which they were inserted,
    starting from 0.
*/
const QtSoapType &QtSoapStruct::operator [](int i) const
{
    static QtSoapType NIL;
    if (i < 0 || i >= dict.count())
        return NIL;

    return *dict[i].ptr();
}

/*!
    Returns a reference to the item in this struct whose QName
    (qualified name) is \a key. If no such item exists, an empty
    QtSoapType is returned.
*/
QtSoapType &QtSoapStruct::at(const QtSoapQName &key)
{
    static QtSoapType NIL;

    QListIterator<QtSmartPtr<QtSoapType> > it(dict);
    while (it.hasNext()) {
        QtSoapType *ret = it.next().ptr();
        if (ret->name() == key)
            return *ret;
    }

    return NIL;
}

/*!
    \overload

    Returns a reference to the item in this struct whose QName
    (qualified name) is \a key. If no such item exists, an empty
    QtSoapType is returned.
*/
const QtSoapType &QtSoapStruct::at(const QtSoapQName &key) const
{
    static QtSoapType NIL;

    for (QtSoapStructIterator i(*const_cast<QtSoapStruct *>(this)); i.current(); ++i)
	if (i.key() == key)
	    return *i.current();

    return NIL;
}

/*!
    Constructs a QtSoapStructIterator and initializes it to point to
    the first element in the struct \a s.
*/
QtSoapStructIterator::QtSoapStructIterator(QtSoapStruct &s) : it(s.dict.begin()), itEnd(s.dict.end())
{
}

/*!
    Destructs the QtSoapStructIterator.
*/
QtSoapStructIterator::~QtSoapStructIterator()
{
}


/*!
    Returns the QName (qualified name) of the current item.
*/
QtSoapQName QtSoapStructIterator::key() const
{
    if (it == itEnd)
        return QtSoapQName();
    return (*it)->name();
}

/*!
    Returns a pointer to the current item, or 0 if there is none.
*/
QtSoapType *QtSoapStructIterator::data()
{
    if (it == itEnd)
        return 0;
    return it->ptr();
}

/*!
    Returns a pointer to the current item, or 0 if there is none.
*/
const QtSoapType *QtSoapStructIterator::current() const
{
    if (it == itEnd)
        return 0;
    return it->ptr();
}

/*!
    Moves the iterator to the next item in the struct.
*/
void QtSoapStructIterator::operator ++()
{
    if (it == itEnd)
        return;
    ++it;
}

/*!
    Returns true if this iterator's position is not equal to that of
    \a j; otherwise returns false.
*/
bool QtSoapStructIterator::operator !=(const QtSoapStructIterator &j) const
{
    return it != j.it;
}

/*!
    Returns true if this iterator's position is equal to that of \a
    j; otherwise returns false.
*/
bool QtSoapStructIterator::operator ==(const QtSoapStructIterator &j) const
{
    return it == j.it;
}

/*!
    Constructs an empty QtSoapSimpleType.
*/
QtSoapSimpleType::QtSoapSimpleType()
{
}

/*!
    \overload

    Constructs an empty QtSoapSimpleType, and sets its QName
    (qualified name) to \a name.
*/
QtSoapSimpleType::QtSoapSimpleType(const QtSoapQName &name)
    : QtSoapType(name)
{
}

/*!
    \overload

    Constructs a QtSoapSimpleType of type Int. Sets its QName
    (qualified name) to \a name and its value to \a n.
*/
QtSoapSimpleType::QtSoapSimpleType(const QtSoapQName &name, int n)
    : QtSoapType(name, Int), v(QVariant(n))
{
}

/*!
    \overload

    Constructs a QtSoapSimpleType of type Boolean. Sets its QName
    (qualified name) to \a name and its value to \a n.

    \a dummy is an unused variable that should be set to 0; it is
    needed for older compilers that cannot distinguish between bool
    and int.
*/
QtSoapSimpleType::QtSoapSimpleType(const QtSoapQName &name, bool n, int)
    : QtSoapType(name, Boolean), v(QVariant(n))
{
}

/*!
    \overload

    Constructs a QtSoapSimpleType of type String. Sets its QName
    (qualified name) to \a name and its value to \a n.
*/
QtSoapSimpleType::QtSoapSimpleType(const QtSoapQName &name, const QString &n)
    : QtSoapType(name, String), v(QVariant(n))
{
}

/*!
    Constructs a QtSoapSimpleType that is a copy of \a copy.
*/
QtSoapSimpleType::QtSoapSimpleType(const QtSoapSimpleType &copy)
    : QtSoapType(copy), v(copy.v)
{
}

/*!
    Destructs the QtSoapSimpleType.
*/
QtSoapSimpleType::~QtSoapSimpleType()
{
}

/*!
    Erases the value of this QtSoapSimpleType.
*/
void QtSoapSimpleType::clear()
{
    v.clear();
}

/*!
    Returns the QDomElement representation of this QtSoapSimpleType.
    The returned QDomElement is created using \a doc.
*/
QDomElement QtSoapSimpleType::toDomElement(QDomDocument doc) const
{
    QString prefix = QtSoapNamespaces::instance().prefixFor(n.uri());
    QDomElement a = n.uri() == ""
		    ? doc.createElement(n.name())
		    : doc.createElementNS(n.uri(), prefix + ":" + n.name());

    QString schemaprefix = QtSoapNamespaces::instance().prefixFor(XML_SCHEMA_INSTANCE);
    a.setAttributeNS(XML_SCHEMA_INSTANCE, schemaprefix + ":type", "xsd:" + typeName());
    a.appendChild(doc.createTextNode(v.toString()));

    return a;
}

/*! \reimp
*/
bool QtSoapSimpleType::isValid() const
{
    return true;
}

/*!
    Makes this QtSoapSimpleType a copy of \a copy.
*/
QtSoapSimpleType &QtSoapSimpleType::operator =(const QtSoapSimpleType &copy)
{
    t = copy.t;
    errorStr = copy.errorStr;
    i = copy.i;
    n = copy.n;
    u = copy.u;
    h = copy.h;
    v = copy.v;

    return *this;
}

/*!
    Inspects \a node and constructs the QtSoapSimpleType content if \a
    node qualifies as a SOAP simple type. Returns true if it does;
    otherwise returns false.
*/
bool QtSoapSimpleType::parse(QDomNode node)
{
    if (node.isNull() || !node.isElement())
	return false;

    QDomElement e = node.toElement();

    QDomAttr typeattr = e.attributeNode("type");
    QString type = typeattr.isNull() ? QString("string") : localName(typeattr.value()).toLower();

    t = QtSoapType::nameToType(type);
    switch (t) {
    case Duration:
    case DateTime:
    case Time:
    case Date:
    case GYearMonth:
    case GYear:
    case GMonthDay:
    case GDay:
    case GMonth:
    case Base64Binary:
    case HexBinary:
    case AnyURI:
    case QName:
    case NOTATION:
    case String:
    case NormalizedString:
    case Token:
    case Language:
    case Name:
    case NMTOKEN:
    case NCName:
    case ID:
    case IDREF:
    case ENTITY:
	v = QVariant(e.text());
	break;
    case Float:
	v = QVariant(e.text().toFloat());
	break;
    case Double:
	v = QVariant(e.text().toDouble());
	break;
    case Decimal:
    case Integer:
    case NonPositiveInteger:
    case NegativeInteger:
    case Long:
    case Int:
    case Short:
    case Byte:
    case NonNegativeInteger:
    case UnsignedLong:
    case PositiveInteger:
    case UnsignedInt:
    case UnsignedShort:
    case UnsignedByte:
	if (e.text() == "" || (e.text() != "" && (e.text()[0].isNumber() || e.text()[0] == '-')))
	    v = QVariant(e.text().toInt());
	else {
	    errorStr = "Type error at element \"" + e.tagName() + "\"";
	    return false;
	}

	break;
    case Boolean: {
	QString val = e.text().trimmed().toLower();
	if (val == "false")
	    v = QVariant(false);
	else if (val == "true")
	    v = QVariant(true);
    }
	break;
    default:
	v = e.text();
	break;
    }

    setName(QtSoapQName(localName(e.tagName()), e.namespaceURI()));
    return true;
}

/*!
    Returns the value of the simple type as a QString.
*/
QString QtSoapSimpleType::toString() const
{
    return v.toString();
}

/*!
    Returns the value of the simple type as an int.
*/
int QtSoapSimpleType::toInt() const
{
    return v.toInt();
}

/*!
    Returns the value of the simple type as a bool.
*/
bool QtSoapSimpleType::toBool() const
{
    return v.toBool();
}


/*!
    Returns the QVariant value of this QtSoapSimpleType.
*/
QVariant QtSoapSimpleType::value() const
{
    return v;
}

/*! \class QtSoapMessage qtsoap.h
    \brief The QtSoapMessage class provides easy access to SOAP
    messages.

    With this class, you can create and inspect any SOAP message.
    There are convenience functions available for generating the most
    common types of SOAP messages, and any other messages can be
    constructed manually using addBodyItem().

    Use setMethod() and addMethodArgument() to construct a method
    request. The return value of a method response is available
    from returnValue().

    Use setFaultCode(), setFaultString() and addFaultDetail() to
    construct a Fault message. To inspect a Fault message, use
    faultCode(), faultString() and faultDetail().

    To add items to the body part of the SOAP message, use
    addBodyItem(). To add items to the header, use addHeaderItem().

    toXmlString() returns a QString XML representation of the SOAP
    message. clear() resets all content in the message, creating an
    empty SOAP message.

    \code
    QtSoapMessage message;

    message.setMethod("getTemperature", "http://weather.example.com/temperature");
    message.addMethodArgument("city", "Oslo");

    // Get the SOAP message as an XML string.
    QString xml = message.toXmlString();
    \endcode

    QtSoap provides a partial implementation of version 1.1 of the
    SOAP protocol as defined in \l http://www.w3.org/TR/SOAP/.

    \list
    \i Server side SOAP is not supported.
    \i References to values (id and href attributes) are not supported.
    \i Arrays support a maximum of five dimensions.
    \i Namespaces for types are not checked. Only the type names are used.
    \i The encodingStyle attribute is ignored. The serialization and
       encoding rules from section 5 in the SOAP v1.1 specification
       are assumed regardless of the value of the encodingStyle
       attribute.
    \i QtSoapType does not have accessors for attributes, which means
       for example that actor, mustUnderstand and so on are not
       accessible in headers.
    \i The SOAP root attribute is not supported.
    \endlist

    \sa QtSoapType, QtSoapQName, QtSoapHttpTransport

*/

/*! \enum QtSoapMessage::FaultCode

    This enum describes all the supported SOAP Fault codes:

    \value VersionMismatch
    The namespace for the Envelope element was unrecognized by the
    remote SOAP server. This usually means that the remote server does
    not support version 1.1 of the SOAP protocol.

    \value MustUnderstand
    One of the header items in the SOAP message with a
    "MustUnderstand" attribute was not recognized by the remote
    server.

    \value Client
    An error in the SOAP message or transport prevents further
    processing by the remote SOAP server.

    \value Server
    An error in the remote SOAP server prevents it from processing the
    SOAP message.

    \omitvalue Other
*/

/*! \enum QtSoapMessage::MessageType

    \value Fault
    \value MethodRequest
    \value MethodResponse
    \value OtherType

*/

/*!
    Constructs an empty QtSoapMessage. The message only contains the
    Envelope element, with no header and no body.
*/
QtSoapMessage::QtSoapMessage()
    : type(OtherType), envelope(QtSoapQName("Envelope", SOAPv11_ENVELOPE))
{
    init();
}

/*!
    Constructs a copy of \a copy.
*/
QtSoapMessage::QtSoapMessage(const QtSoapMessage &copy)
    : type(copy.type), envelope(copy.envelope), m(copy.m), margs(copy.margs),
      errorStr(copy.errorStr)
{
    init();
}

/*!
    Destructs a QtSoapMessage.
*/
QtSoapMessage::~QtSoapMessage()
{
}

/*! \internal

    Registers the standard SOAP namespaces with prefixes.
*/
void QtSoapMessage::init()
{
    QtSoapNamespaces::instance().registerNamespace("SOAP-ENV", SOAPv11_ENVELOPE);
    QtSoapNamespaces::instance().registerNamespace("SOAP-ENC", SOAPv11_ENCODING);
    QtSoapNamespaces::instance().registerNamespace("xsi", XML_SCHEMA_INSTANCE);
    QtSoapNamespaces::instance().registerNamespace("xsd", XML_SCHEMA);
}

/*!
    Clears the content of the SOAP message.
*/
void QtSoapMessage::clear()
{
    type = OtherType;
    envelope.clear();
    m = QtSoapQName();
    margs.clear();
    errorStr = "Unknown error";
}

/*!
    Makes this message a copy of \a copy.
*/
QtSoapMessage &QtSoapMessage::operator =(const QtSoapMessage &copy)
{
    envelope = copy.envelope;
    m = copy.m;
    margs = copy.margs;
    errorStr = copy.errorStr;
    return *this;
}

/*!
    Imports the QDomDocument \a d if it validates as a SOAP
    message. Any existing message content is replaced.

    If the import fails, this message becomes a Fault message.

    Returns true if the import succeeds, otherwise false.
*/
bool QtSoapMessage::setContent(QDomDocument &d)
{
    if (isValidSoapMessage(d)) {
	clear();
        QDomNode node = d.firstChild();
        if (!node.isElement())
            node = node.nextSibling();

	if (envelope.parse(node))
	    return true;
    }

    return false;
}

/*!
    \overload

    Parses the XML document in \a buffer. Imports the document if it
    validates as a SOAP message. Any existing message content is
    replaced.

    If the import fails, this message becomes a Fault message.

    Returns true if the import succeeds, otherwise false.
*/
bool QtSoapMessage::setContent(const QByteArray &buffer)
{
    int errorLine, errorColumn;
    QString errorMsg;

    QDomDocument doc;
    if (!doc.setContent(buffer, true, &errorMsg,
			&errorLine, &errorColumn)) {
	QString s;
	s.sprintf("%s at line %i, column %i", errorMsg.toLatin1().constData(),
		  errorLine, errorColumn);
	setFaultCode(VersionMismatch);
	setFaultString("XML parse error");
	addFaultDetail(new QtSoapSimpleType(QtSoapQName("ParseError"), s));
	return false;
    }

    if (!isValidSoapMessage(doc))
	return false;

    QDomNode node = doc.firstChild();
    if (!node.isElement())
	node = node.nextSibling();
    bool res = envelope.parse(node);
    if (!res)
	qDebug("QtSoapMessage::setContent(), parsing failed: %s", envelope.errorString().toLatin1().constData());
    return res;
}

/*!
    Validates the QDomDocument \a candidate using some simple
    heuristics. Returns true if the document is a valid SOAP message;
    otherwise returns false.
*/
bool QtSoapMessage::isValidSoapMessage(const QDomDocument &candidate)
{
    QDomNode tmp = candidate.firstChild();
    if (tmp.isNull())
	return false;

    // Skip the initial processing instruction if there is one. Most
    // likely this isn't actually a processing instruction, but rather
    // the initial xml declaration <?xml...
    if (tmp.isProcessingInstruction()) {
	tmp = tmp.nextSibling();

	if (tmp.isNull() || !tmp.isElement())
	    return false;
    }

    QDomElement tmpe = tmp.toElement();

    if (localName(tmpe.tagName()).toUpper() != "ENVELOPE") {
	setFaultCode(VersionMismatch);
	setFaultString("SOAP structure invalid");
	addFaultDetail(new QtSoapSimpleType(QtSoapQName("extra"), "root element \"" + tmpe.localName()
					+ "\"/\"" + tmpe.tagName() + "\" is not envelope"));
	return false;
    }

    tmp = tmp.firstChild();
    if (tmp.isNull() || !tmp.isElement()) {
	setFaultCode(VersionMismatch);
	setFaultString("SOAP structure invalid");
	addFaultDetail(new QtSoapSimpleType(QtSoapQName("extra"), "mandatory body element missing"));
	return false;
    }

    QDomElement tmpe2 = tmp.toElement();

    bool foundHeader = false;
    if (localName(tmpe2.tagName()).toUpper() == "HEADER") {
	foundHeader = true;
	tmp = tmp.nextSibling();
    }

    if (!foundHeader && (tmp.isNull() || !tmp.isElement())) {
	setFaultCode(VersionMismatch);
	setFaultString("SOAP structure invalid");
	addFaultDetail(new QtSoapSimpleType(QtSoapQName("extra"), "mandatory body element missing"));
	return false;
    }

    QDomElement tmpe3 = tmp.toElement();

    if (localName(tmpe3.tagName()).toUpper() != "BODY") {
	setFaultCode(VersionMismatch);
	setFaultString("SOAP structure invalid");
	addFaultDetail(new QtSoapSimpleType(QtSoapQName("extra"), "mandatory body element missing"));
	return false;
    }

    // At this point, check that the version of the envelope is
    // correct.
    if (tmpe.namespaceURI() != SOAPv11_ENVELOPE) {
	setFaultCode(VersionMismatch);
	setFaultString("SOAP structure invalid");
	addFaultDetail(new QtSoapSimpleType(QtSoapQName("extra"), "Unsupported namespace for envelope element"));
	return false;
    }

    return true;
}

/*!
    Returns the XML representation of the SOAP message as a QString,
    optionally indenting using \a indent spaces.
*/
QString QtSoapMessage::toXmlString(int indent) const
{
    QDomImplementation impl;
    QDomDocument doc = impl.createDocument(QString(), QString("placeholder"),
                                           QDomDocumentType());
    doc.removeChild(doc.firstChild());
    doc.appendChild(envelope.toDomElement(doc));

    QDomElement env = doc.firstChild().toElement();

    env.setAttribute(QtSoapNamespaces::instance().prefixFor(SOAPv11_ENVELOPE)
		     + ":" + "encodingStyle",
		     SOAPv11_ENCODING);

    env.setAttribute("xmlns:" + QtSoapNamespaces::instance().prefixFor(XML_SCHEMA),
		     XML_SCHEMA);

    return doc.toString(indent);
}

/*!
    Returns a human readable explanation of the most recent error that
    occurred in the QtSoapMessage.
*/
QString QtSoapMessage::errorString() const
{
    return errorStr;
}

/*!
    Adds \a item to the body in the SOAP message. The item is added
    after the last existing item in the body.
*/
void QtSoapMessage::addBodyItem(QtSoapType *item)
{
    body().insert(item);
}

/*!
    Adds \a item to the header in the SOAP message. The item is added
    after the last existing item in the header.
*/
void QtSoapMessage::addHeaderItem(QtSoapType *item)
{
    QtSoapType &headerTmp = envelope[QtSoapQName("Header", SOAPv11_ENVELOPE)];
    if (!headerTmp.isValid())
	envelope.insert(new QtSoapStruct(QtSoapQName("Header", SOAPv11_ENVELOPE)));

    QtSoapStruct &header = (QtSoapStruct &)envelope[QtSoapQName("Header", SOAPv11_ENVELOPE)];
    header.insert(item);
}
/*!
    Returns the return value of a SOAP method response as a
    QtSoapType.
*/
const QtSoapType &QtSoapMessage::returnValue() const
{
    static QtSoapType NIL;

    const QtSoapType &meth = method();

    if (!meth.isValid() || meth.type() != QtSoapType::Struct)
	return NIL;

    QtSoapStruct &m = (QtSoapStruct &) meth;
    if (m.count() == 0)
	return NIL;

    QtSoapStructIterator mi(m);
    return *mi.data();
}

/*!
    Returns the Fault detail element of a SOAP Fault message.
*/
const QtSoapType &QtSoapMessage::faultDetail() const
{
    return body()[QtSoapQName("Fault", SOAPv11_ENVELOPE)][QtSoapQName("Detail")];
}

/*!
    Returns the Fault faultstring element of a SOAP Fault message.
*/
const QtSoapType &QtSoapMessage::faultString() const
{
    return body()[QtSoapQName("Fault", SOAPv11_ENVELOPE)][QtSoapQName("Faultstring")];
}

/*!
    Returns true if the message is a SOAP Fault message; otherwise
    returns false.
*/
bool QtSoapMessage::isFault() const
{
    return faultCode() != Other;
}

/*!
    Returns the Fault faultcode element of a SOAP Fault message.
*/
QtSoapMessage::FaultCode QtSoapMessage::faultCode() const
{
    QtSoapType &code = body()[QtSoapQName("Fault")][QtSoapQName("Faultcode")];
    if (!code.isValid() || (code.type() != QtSoapType::String
                            && code.type() != QtSoapType::QName))
	return Other;

    QtSoapSimpleType &fcode = (QtSoapSimpleType &)code;
    QString fcodestr = fcode.value().toString();

    // Only test string before first .
    int pos;
    if ((pos = fcodestr.indexOf('.')) != -1)
        fcodestr.truncate(pos);

    if (localName(fcodestr.toLower()) == "versionmismatch")
	return VersionMismatch;

    if (localName(fcodestr.toLower()) == "mustunderstand")
	return MustUnderstand;

    if (localName(fcodestr.toLower()) == "client")
	return Client;

    if (localName(fcodestr.toLower()) == "server")
	return Server;

    return Other;
}

/*!
    Returns a reference to the body of the SOAP message.
*/
QtSoapStruct &QtSoapMessage::body() const
{
    const QtSoapQName bodyName("Body", SOAPv11_ENVELOPE);

    QtSoapType &bodyTmp = envelope[bodyName];
    if (!bodyTmp.isValid())
	envelope.insert(new QtSoapStruct(bodyName));

    return (QtSoapStruct &)envelope[bodyName];
}

/*!
    Returns a reference to the header of the SOAP message.
*/
QtSoapStruct &QtSoapMessage::header() const
{
    const QtSoapQName headerName("Header", SOAPv11_ENVELOPE);

    QtSoapType &headerTmp = envelope[headerName];
    if (!headerTmp.isValid())
	envelope.insert(new QtSoapStruct(headerName));

    return (QtSoapStruct &)envelope[headerName];
}

/*!
    Sets the fault code of the SOAP Fault message to \a code.
*/
void QtSoapMessage::setFaultCode(FaultCode code)
{
    if (type != Fault && type != OtherType) {
	clear();
	type = Fault;
    }

    if (!body()[QtSoapQName("Fault", SOAPv11_ENVELOPE)].isValid())
	addBodyItem(new QtSoapStruct(QtSoapQName("Fault", SOAPv11_ENVELOPE)));

    QString codeStr;
    switch (code) {
    case VersionMismatch:
	codeStr = "SOAP-ENV:VersionMismatch";
	break;
    case MustUnderstand:
	codeStr = "SOAP-ENV:MustUnderstand";
	break;
    case Client:
	codeStr = "SOAP-ENV:Client";
	break;
    case Server:
	codeStr = "SOAP-ENV:Server";
	break;
    case Other:
	codeStr = "Other";
	break;
    }

    QtSoapType &node = body()[QtSoapQName("Fault", SOAPv11_ENVELOPE)];
    QtSoapStruct &fault = reinterpret_cast<QtSoapStruct &>(node);
    fault.insert(new QtSoapSimpleType(QtSoapQName("Faultcode"), codeStr));
}

/*!
    Sets the Fault faultstring of the SOAP Fault message to \a s.
*/
void QtSoapMessage::setFaultString(const QString &s)
{
    if (type != Fault && type != OtherType) {
	clear();
	type = Fault;
    }

    if (!body()[QtSoapQName("Fault", SOAPv11_ENVELOPE)].isValid())
	addBodyItem(new QtSoapStruct(QtSoapQName("Fault", SOAPv11_ENVELOPE)));

    QtSoapType &node = body()[QtSoapQName("Fault", SOAPv11_ENVELOPE)];
    QtSoapStruct &fault = reinterpret_cast<QtSoapStruct &>(node);
    fault.insert(new QtSoapSimpleType(QtSoapQName("Faultstring"), s));
}

/*!
   Adds the QtSoapType \a detail to the end of the list of faultdetail
   items in a SOAP Fault message.
*/
void QtSoapMessage::addFaultDetail(QtSoapType *detail)
{
    if (type != Fault && type != OtherType) {
	clear();
	type = Fault;
    }

    if (!body()[QtSoapQName("Fault", SOAPv11_ENVELOPE)].isValid())
	addBodyItem(new QtSoapStruct(QtSoapQName("Fault", SOAPv11_ENVELOPE)));

    QtSoapType &node = body()[QtSoapQName("Fault", SOAPv11_ENVELOPE)];
    QtSoapStruct &fault = reinterpret_cast<QtSoapStruct &>(node);
    if (!fault[QtSoapQName("Faultdetail", SOAPv11_ENVELOPE)].isValid())
	fault.insert(new QtSoapStruct(QtSoapQName("Faultdetail", SOAPv11_ENVELOPE)));

    QtSoapType &node2 = fault[QtSoapQName("Faultdetail", SOAPv11_ENVELOPE)];
    QtSoapStruct &fdetail = reinterpret_cast<QtSoapStruct &>(node2);

    fdetail.insert(detail);
}

/*!
    Returns the method of a SOAP method request or response
    as a QtSoapType.
*/
const QtSoapType &QtSoapMessage::method() const
{
    static QtSoapType NIL;

    if (body().count() == 0)
	return NIL;

    QtSoapStructIterator it(body());

    return *it.data();
}

/*!
    Sets the QName (qualified name) of the method to call in a SOAP
    method request to \a meth.

    This function \e must be called before calling
    addMethodArgument().
*/
void QtSoapMessage::setMethod(const QtSoapQName &meth)
{
    if (type != MethodRequest && type != OtherType) {
	clear();
	type = MethodRequest;
    }

    addBodyItem(new QtSoapStruct(meth));
}

/*! \overload

    Sets the method name to \a name and uri to \a uri.
*/
void QtSoapMessage::setMethod(const QString &name, const QString &uri)
{
    setMethod(QtSoapQName(name, uri));
}

/*!
   Adds argument \a arg to the list of arguments that are passed in a
   SOAP method request.

   \warning setMethod() must be called before calling this function.
*/
void QtSoapMessage::addMethodArgument(QtSoapType *arg)
{
    if (body().count() == 0) {
	qWarning("Attempted to add argument (%s:%s) without first setting method",
		 arg->name().uri().toLatin1().constData(), arg->name().name().toLatin1().constData());
	return;
    }

    QtSoapStructIterator it(body());
    QtSoapType &node = *it.data();
    QtSoapStruct &meth = static_cast<QtSoapStruct &>(node);
    meth.insert(arg);
}

/*! \overload

    Adds an argument called \a name with a uri of \a uri. The type
    of the argument is QtSoapType::String and its value is \a value.
*/
void QtSoapMessage::addMethodArgument(const QString &name, const QString &uri, const QString &value)
{
    addMethodArgument(new QtSoapSimpleType(QtSoapQName(name, uri), value));
}

/*! \overload

    Adds an argument called \a name with a uri of \a uri. The type
    of the argument is QtSoapType::Boolean and its value is \a value.

    The \a dummy argument is used to distinguish this function from
    the overload which takes an int.
*/
void QtSoapMessage::addMethodArgument(const QString &name, const QString &uri, bool value, int dummy)
{
    addMethodArgument(new QtSoapSimpleType(QtSoapQName(name, uri), value, dummy));
}

/*! \overload

    Adds an argument called \a name with a uri of \a uri. The type
    of the argument is QtSoapType::Integer and its value is \a value.
*/
void QtSoapMessage::addMethodArgument(const QString &name, const QString &uri, int value)
{
    addMethodArgument(new QtSoapSimpleType(QtSoapQName(name, uri), value));
}

/*!
    Constructs a QtSoapTypeFactory and initializes it with all the
    known SOAP types.
*/
QtSoapTypeFactory::QtSoapTypeFactory()
{
    QtSoapTypeConstructor<QtSoapStruct> *structConstructor = new QtSoapTypeConstructor<QtSoapStruct>();
    deleteList.append(structConstructor);
    QtSoapTypeConstructor<QtSoapArray> *arrayConstructor = new QtSoapTypeConstructor<QtSoapArray>();
    deleteList.append(arrayConstructor);
    QtSoapTypeConstructor<QtSoapSimpleType> *basicTypeConstructor = new QtSoapTypeConstructor<QtSoapSimpleType>();
    deleteList.append(basicTypeConstructor);

    registerHandler("struct", structConstructor);
    registerHandler("array", arrayConstructor);
    registerHandler("string", basicTypeConstructor);
    registerHandler("normalizedstring", basicTypeConstructor);
    registerHandler("token", basicTypeConstructor);
    registerHandler("language", basicTypeConstructor);
    registerHandler("name", basicTypeConstructor);
    registerHandler("ncname", basicTypeConstructor);
    registerHandler("id", basicTypeConstructor);
    registerHandler("idref", basicTypeConstructor);
    registerHandler("entity", basicTypeConstructor);
    registerHandler("nmtoken", basicTypeConstructor);
    registerHandler("nmtokens", basicTypeConstructor);
    registerHandler("boolean", basicTypeConstructor);
    registerHandler("decimal", basicTypeConstructor);
    registerHandler("integer", basicTypeConstructor);
    registerHandler("nonpositiveinteger", basicTypeConstructor);
    registerHandler("negativeinteger", basicTypeConstructor);
    registerHandler("int", basicTypeConstructor);
    registerHandler("long", basicTypeConstructor);
    registerHandler("short", basicTypeConstructor);
    registerHandler("byte", basicTypeConstructor);
    registerHandler("nonnegativeinteger", basicTypeConstructor);
    registerHandler("unsignedlong", basicTypeConstructor);
    registerHandler("unsignedint", basicTypeConstructor);
    registerHandler("unsignedshort", basicTypeConstructor);
    registerHandler("unsignedbyte", basicTypeConstructor);
    registerHandler("positiveinteger", basicTypeConstructor);
    registerHandler("float", basicTypeConstructor);
    registerHandler("double", basicTypeConstructor);
    registerHandler("other", structConstructor);
}

/*!
    Destructs the QtSoapTypeFactory. This destructor is called when
    the application exits.
*/
QtSoapTypeFactory::~QtSoapTypeFactory()
{
    QLinkedList<QtSoapTypeConstructorBase*>::ConstIterator it = deleteList.begin();
    while (it != deleteList.end()) {
        delete *it;
        ++it;
    }
}

/*!
    Returns a reference to the QtSoapTypeFactory singleton.
*/
QtSoapTypeFactory &QtSoapTypeFactory::instance()
{
    static QtSoapTypeFactory factory;
    return factory;
}

/*!
    Registers a handler \a handler for a QtSoapType called \a name.
*/
bool QtSoapTypeFactory::registerHandler(const QString &name, QtSoapTypeConstructorBase *handler)
{
    if (typeHandlers.find(name) != typeHandlers.end()) {
	errorStr = "A handler for " + name + " is already registered.";
	return false;
    }

    typeHandlers.insert(name, handler);
    return true;
}

/*! \internal
*/
QtSmartPtr<QtSoapType> QtSoapTypeFactory::soapType(QDomNode node) const
{
    if (node.isNull() || !node.isElement())
	return QtSmartPtr<QtSoapType>();

    QDomElement elem = node.toElement();

    QDomAttr attr = elem.attributeNode("type");
    QtSoapTypeConstructorBase *constructor = 0;
    if (!attr.isNull()) {
        QHash<QString, QtSoapTypeConstructorBase *>::ConstIterator it;
	it = typeHandlers.find(localName(attr.value().toLower()));
        if (it != typeHandlers.end())
            constructor = *it;
    }

    if (attr.isNull() || !constructor) {
        QHash<QString, QtSoapTypeConstructorBase *>::ConstIterator it;
	if (node.firstChild().isElement()) {
            if (localName(node.nodeName().toLower()) == "array") {
                it = typeHandlers.find("array");
            } else
                it = typeHandlers.find("struct");
        } else
	    it = typeHandlers.find("string");
        if (it != typeHandlers.end())
            constructor = *it;
    }

    if (!constructor) {
	return QtSmartPtr<QtSoapType>();
    }

    QtSoapType *type = constructor->createObject(node);

    if (!type)
	errorStr = constructor->errorString();

    return QtSmartPtr<QtSoapType>(type);
}

/*!
    Returns a human readable interpretation of the last error
    that occurred.
*/
QString QtSoapTypeFactory::errorString() const
{
    return errorStr;
}

/*! \class QtSoapHttpTransport

    \brief The QtSoapHttpTransport class provides a mechanism for
    transporting SOAP messages to and from other hosts using the
    HTTP protocol.

    Use this class to submit SOAP messages to a web service.
    Set the hostname of the SOAP server with setHost(). Some servers
    also require the SOAPAction header to be set, and you can do this
    with setAction(). Next, submit the request with submitRequest(),
    passing the message to submit together with the path that you want
    to submit the message to. The responseReady() signal is emitted
    when a response has been received. Call getResponse() to get the
    reponse from the service.

    QtSoapHttpTransport usage example: If a SOAP weather service was
    running on the host weather.example.com, the following code might
    be used to find the temperature in any given city:

    \code
    void WeatherFetcher::findTemperature(const QString &city)
    {
        QtSoapMessage message;
        message.setMethod("getTemperature", "http://weather.example.com/temperature");
        message.setMethodArgument("city", "", city);

        // transport is a private member of WeatherFetcher, of type QtSoapHttpTransport
        transport.setHost("www.example.com");
        connect(&transport, SIGNAL(responseReady()), SLOT(readResponse()));

        transport.submitRequest(message, "/weatherfetcher/fetch.asp");
    }
    \endcode

    This is an example implementation of the readResponse() slot in
    the WeatherFetcher class:

    \code
    void WeatherFetcher::readResponse()
    {
        const QtSoapMessage &response = transport.getResponse();
        if (response.isFault()) {
            cout << response.faultString().toString().toLatin1().constData() << endl;
            return;
        }

        const QtSoapType &returnValue = response.returnValue();
        if (returnValue["temperature"].isValid()) {
        cout << "The current temperature is "
             << returnValue["temperature"].toString().toLatin1().constData()
             << " degrees Celcius." << endl;
    }
    \endcode

    \sa QtSoapMessage, QtSoapType
*/

/*! \fn void QtSoapHttpTransport::responseReady()

    This signal is emitted when a SOAP response is received from a
    remote peer.

    \sa getResponse()
*/

/*! \fn void QtSoapHttpTransport::responseReady(const QtSoapMessage &response)

    This signal is emitted when a SOAP response is received from a
    remote peer. The received response is available in \a
    response. This signal is emitted in tandem with the argument-less
    responseReady() signal.

    \sa responseReady()
*/

/*!
    Constructs a QtSoapHttpTransport object. Passes \a parent to
    QObject's constructor.
*/

QtSoapHttpTransport::QtSoapHttpTransport(QObject *parent)
    : QObject(parent), networkMgr(this)
{
    connect(&networkMgr, SIGNAL(finished(QNetworkReply *)),
            SLOT(readResponse(QNetworkReply *)));
}

/*!
    Destructs a QtSoapHttpTransport.
*/
QtSoapHttpTransport::~QtSoapHttpTransport()
{
}

/*!
  \obsolete
*/

void QtSoapHttpTransport::setHost(const QString &host, int port)
{
    setHost(host, false, port);
}

/*!
  Sets the \a host this transport should connect to. The transport
  mode will be HTTP, unless \a useSecureHTTP is set, in which case it
  will be HTTPS. This transport will connect to the well-known ports
  by default (80 for HTTP, 443 for HTTPS), unless a different,
  non-zero port is specified in \a port.
*/
void QtSoapHttpTransport::setHost(const QString &host, bool useSecureHTTP, int port)
{
    url.setHost(host);
    url.setScheme(useSecureHTTP ? QLatin1String("https") : QLatin1String("http"));
    if (port)
        url.setPort(port);
    else
        url.setPort(useSecureHTTP ? 443 : 80);
}

/*!
    Sets the HTTP header SOAPAction to \a action.
*/
void QtSoapHttpTransport::setAction(const QString &action)
{
    soapAction = action;
}

/*!
    Submits the SOAP message \a request to the path \a path on the
    HTTP server set using setHost().
*/
void QtSoapHttpTransport::submitRequest(QtSoapMessage &request, const QString &path)
{
    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", soapAction.toAscii());
    url.setPath(path);
    networkReq.setUrl(url);

    soapResponse.clear();
    networkRep = networkMgr.post(networkReq, request.toXmlString().toUtf8().constData());
}


/*!
    Returns the most recently received response SOAP message. This
    message could be a Fault message, so it is wise to check using
    QtSoapMessage::isFault() before processing the response.
*/
const QtSoapMessage &QtSoapHttpTransport::getResponse() const
{
    return soapResponse;
}


/*!
    Returns a pointer to the QNetworkAccessManager object used by this
    transport. This is useful if the application needs to connect to its
    signals, or set or read its cookie jar, etc.
*/

QNetworkAccessManager *QtSoapHttpTransport::networkAccessManager()
{
    return &networkMgr;
}


/*!
    Returns a pointer to the QNetworkReply object of the current (or last)
    request, or 0 if no such object is currently available.

    This is useful if the application needs to access the raw header
    data etc.
*/

QNetworkReply *QtSoapHttpTransport::networkReply()
{
    return networkRep;
}

/*!

*/

void QtSoapHttpTransport::readResponse(QNetworkReply *reply)
{
    networkRep = reply;
    switch (reply->error()) {
    case QNetworkReply::NoError:
    case QNetworkReply::ContentAccessDenied:
    case QNetworkReply::ContentOperationNotPermittedError:
    case QNetworkReply::ContentNotFoundError:
    case QNetworkReply::UnknownContentError:
        {
            soapResponse.setContent(reply->readAll());

            int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (httpStatus != 200 && httpStatus != 100) {
                if (soapResponse.faultCode() == QtSoapMessage::Other)
                    soapResponse.setFaultCode(QtSoapMessage::Client);
                /*
                QString httpReason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
                soapResponse.setFaultString(QString("HTTP status %2 (%3).\n%1").arg(soapResponse.faultString().toString()).arg(httpStatus).arg(httpReason));
                */
            }
        }
        break;
    default:
        {
            soapResponse.setFaultCode(QtSoapMessage::Client);
            soapResponse.setFaultString(QString("Network transport error (%1): %2").arg(reply->error()).arg(reply->errorString()));
        }
        break;
    }

    emit responseReady();
    emit responseReady(soapResponse);

    reply->deleteLater();
}

/*! \class QtSoapNamespaces qtsoap.h

    \brief The QtSoapNamespaces class provides a registry for XML
    namespaces and prefixes for use in QtSoap.

    When a QtSoapMessage is converted to XML via
    QtSoapMessage::toXmlString(), this class is used to find
    appropriate XML namespace prefixes for the QNames (qualified
    names) in the message.

    To register a namespace with a prefix, call register().
    prefixFor() will then return the prefix that is registered for the
    given namespace, if any.

    To access the QtSoapNamespaces registry, call
    QtSoapNamespaces::instance().

    \code
    QtSoapNamespaces &registry = QtSoapNamespaces::instance();

    registry.register("pre", "http://www.example.com/");
    QString prefix = registry.prefixFor("http://www.example.com/"); // returns "pre"
    \endcode

    \sa QtSoapMessage
*/

/*!
    Returns a reference to the QtSoapNamespaces singleton.
*/
QtSoapNamespaces &QtSoapNamespaces::instance()
{
    static QtSoapNamespaces ns;
    return ns;
}

/*! \internal

    Constructs a QtSoapNamespaces object.
*/
QtSoapNamespaces::QtSoapNamespaces()
{
}

/*!
    Registers the namespace \a uri with the prefix \a prefix.
*/
void QtSoapNamespaces::registerNamespace(const QString &prefix, const QString &uri)
{
    namespaces.insert(uri, prefix);
}

/*!
    Returns the prefix for the namespace \a uri, or an empty string if
    no prefix has been registered for \a uri.
*/
QString QtSoapNamespaces::prefixFor(const QString &uri)
{
    return namespaces.value(uri);
}
