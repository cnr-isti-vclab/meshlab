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

#ifndef QTSOAP_H
#define QTSOAP_H
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtXml/QtXml>
#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QUrl>
#include <QtCore/QHash>
#include <QtCore/QLinkedList>
#include <QtCore/QPointer>

#if defined(Q_WS_WIN)
#  if !defined(QT_QTSOAP_EXPORT) && !defined(QT_QTSOAP_IMPORT)
#    define QT_QTSOAP_EXPORT
#  elif defined(QT_QTSOAP_IMPORT)
#    if defined(QT_QTSOAP_EXPORT)
#      undef QT_QTSOAP_EXPORT
#    endif
#    define QT_QTSOAP_EXPORT __declspec(dllimport)
#  elif defined(QT_QTSOAP_EXPORT)
#    undef QT_QTSOAP_EXPORT
#    define QT_QTSOAP_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTSOAP_EXPORT
#endif

#define SOAPv11_ENVELOPE    "http://schemas.xmlsoap.org/soap/envelope/"
#define SOAPv11_ENCODING    "http://schemas.xmlsoap.org/soap/encoding/"
#define SOAPv11_ACTORNEXT   "http://schemas.xmlsoap.org/soap/actor/next"

#define XML_SCHEMA          "http://www.w3.org/1999/XMLSchema"
#define XML_SCHEMA_INSTANCE "http://www.w3.org/1999/XMLSchema-instance"
#define XML_NAMESPACE       "http://www.w3.org/XML/1998/namespace"

template <class T>
class QtSmartPtr
{
public:
    inline QtSmartPtr(T *data = 0)
    {
	d = data;
	r = new int;
	*r = 1;
    }

    inline QtSmartPtr(const QtSmartPtr &copy)
    {
	if (*copy.r != 0)
	    ++(*copy.r);

	r = copy.r;
	d = copy.d;
    }

    inline ~QtSmartPtr()
    {
        if ((*r) == 0)
            delete r;
	else if ((*r) != 0 && --(*r) == 0) {
	    delete r;
	    if (d) delete d;
	}
    }

    inline QtSmartPtr &operator =(const QtSmartPtr &copy)
    {
	if (*copy.r != 0)
	    ++(*copy.r);

        if ((*r) == 0)
            delete r;
	else if ((*r) != 0 && --(*r) == 0) {
	    delete r;
	    if (d) delete d;
	}

	r = copy.r;
	d = copy.d;
	return *this;
    }

    inline T &operator *() const
    {
	return *d;
    }

    inline T *operator ->() const
    {
	    return d;
    }

    inline T *ptr() const
    {
	return d;
    }

    inline T &ref() const
    {
	return *d;
    }

    inline T *releasedPtr() const
    {
	(*r) = 0;
	return d;
    }

    inline bool isNull() const
    {
	return d == 0;
    }

private:
    int *r;
    T *d;
};

class QT_QTSOAP_EXPORT QtSoapQName
{
public:
    QtSoapQName(const QString &name = QString::null, const QString &uri = QString::null);
    ~QtSoapQName();

    QtSoapQName &operator =(const QString &s);

    QString name() const;
    QString uri() const;

private:
    QString n;
    QString nuri;
};

bool operator ==(const QtSoapQName &n1, const QtSoapQName &n2);
bool operator <(const QtSoapQName &n1, const QtSoapQName &n2);

class QT_QTSOAP_EXPORT QtSoapType
{
public:
    enum Type {
	Duration, DateTime, Time, Date, GYearMonth, GYear, GMonthDay,
	GDay, GMonth, Boolean, Base64Binary, HexBinary, Float, Double,
	AnyURI, QName, NOTATION, String, NormalizedString, Token, Language,
	Name, NMTOKEN, NCName, ID, IDREF, ENTITY, Decimal, Integer,
	NonPositiveInteger, NegativeInteger, Long, Int, Short,
	Byte, NonNegativeInteger, UnsignedLong, PositiveInteger,
	UnsignedInt, UnsignedShort, UnsignedByte,
	Array, Struct, Other
    };

    QtSoapType();
    QtSoapType(const QtSoapQName &name, Type t = Other);
    QtSoapType(const QtSoapType &copy);
    QtSoapType &operator =(const QtSoapType &copy);
    virtual ~QtSoapType();

    virtual void clear();

    virtual bool parse(QDomNode);
    virtual bool isValid() const;

    virtual int count() const;
    virtual QVariant value() const;

    virtual QtSoapType &operator [](int);
    virtual QtSoapType &operator [](const QtSoapQName &s);
    virtual QtSoapType &operator [](const QString &name);

    virtual const QtSoapType &operator [](int) const;
    virtual const QtSoapType &operator [](const QtSoapQName &s) const;
    virtual const QtSoapType &operator [](const QString &name) const;

    virtual QDomElement toDomElement(QDomDocument) const;

    virtual Type type() const;
    virtual QString id() const;
    virtual QString href() const;
    virtual QString typeName() const;
    virtual QtSoapQName name() const;

    virtual QString toString() const;
    virtual int toInt() const;
    virtual bool toBool() const;

    void setName(const QtSoapQName &);
    void setId(const QString &);
    void setHref(const QString &);

    QString errorString() const;

    static QString typeToName(QtSoapType::Type t);
    static Type nameToType(const QString &);

protected:
    Type t;
    QString errorStr;
    QString i;
    QtSoapQName n;
    QString u;
    QString h;
};

class QtSoapArrayIterator;

class QT_QTSOAP_EXPORT QtSoapArray : public QtSoapType
{
public:
    QtSoapArray();
    QtSoapArray(const QtSoapQName &name, QtSoapType::Type type = Other,
		int size0 = -1, int size1 = -1, int size2 = -1, int size3 = -1, int size4 = -1);
    QtSoapArray(const QtSoapArray &copy);
    QtSoapArray &operator = (const QtSoapArray &copy);
    ~QtSoapArray();

    void clear();

    bool parse(QDomNode node);
    bool isValid() const;

    int count() const;

    QtSoapType &at(int pos0);
    QtSoapType &at(int pos0, int pos1);
    QtSoapType &at(int pos0, int pos1, int pos2);
    QtSoapType &at(int pos0, int pos1, int pos2, int pos3);
    QtSoapType &at(int pos0, int pos1, int pos2, int pos3, int pos4);
    QtSoapType &operator [](int i);
    QtSoapType &operator [](const QString &);
    QtSoapType &operator [](const QtSoapQName &);

    const QtSoapType &at(int pos) const;
    const QtSoapType &at(int pos0, int pos1) const;
    const QtSoapType &at(int pos0, int pos1, int pos2) const;
    const QtSoapType &at(int pos0, int pos1, int pos2, int pos3) const;
    const QtSoapType &at(int pos0, int pos1, int pos2, int pos3, int pos4) const;
    const QtSoapType &operator [](int i) const;
    const QtSoapType &operator [](const QString &) const;
    const QtSoapType &operator [](const QtSoapQName &) const;

    void append(QtSoapType *item);
    void insert(int pos0, QtSoapType *item);
    void insert(int pos0,int pos1, QtSoapType *item);
    void insert(int pos0,int pos1,int pos2, QtSoapType *item);
    void insert(int pos0,int pos1,int pos2,int pos3, QtSoapType *item);
    void insert(int pos0,int pos1,int pos2,int pos3,int pos4, QtSoapType *item);

    QDomElement toDomElement(QDomDocument doc) const;

    friend class QtSoapArrayIterator;

protected:
    QString arraySizeString() const;
    QString arrayTypeString() const;

    QHash<int, QtSmartPtr<QtSoapType> > array;
    int lastIndex;

private:
    Type arrayType;
    int order;
    int siz0, siz1, siz2, siz3, siz4;
};

class QT_QTSOAP_EXPORT QtSoapArrayIterator
{
public:
    QtSoapArrayIterator(QtSoapArray &);
    QtSoapArrayIterator(const QtSoapArrayIterator &copy);
    QtSoapArrayIterator &operator =(const QtSoapArrayIterator &j);
    ~QtSoapArrayIterator();

    int pos() const;
    void pos(int *pos0, int *pos1 = 0, int *pos2 = 0, int *pos3 = 0, int *pos4 = 0) const;

    QtSoapType *data();
    const QtSoapType *current() const;

    void operator ++();
    bool operator !=(const QtSoapArrayIterator &j) const;
    bool operator ==(const QtSoapArrayIterator &j) const;

    bool atEnd() const;

private:
    QHash<int, QtSmartPtr<QtSoapType> >::Iterator it;
    QtSoapArray *arr;
};

class QtSoapStructIterator;

class QT_QTSOAP_EXPORT QtSoapStruct : public QtSoapType
{
public:
    QtSoapStruct();
    QtSoapStruct(const QtSoapQName &name);
    QtSoapStruct(const QtSoapStruct &copy);
    QtSoapStruct &operator =(const QtSoapStruct &copy);
    ~QtSoapStruct();

    void clear();

    bool parse(QDomNode node);
    bool isValid() const;

    int count() const;

    QtSoapType &at(const QtSoapQName &key);
    const QtSoapType &at(const QtSoapQName &key) const;

    QtSoapType &operator [](int);
    QtSoapType &operator [](const QtSoapQName &key);
    QtSoapType &operator [](const QString &key);

    const QtSoapType &operator [](int) const;
    const QtSoapType &operator [](const QtSoapQName &key) const;
    const QtSoapType &operator [](const QString &key) const;

    void insert(QtSoapType *item);

    QDomElement toDomElement(QDomDocument doc) const;

    friend class QtSoapStructIterator;

protected:
    QList<QtSmartPtr<QtSoapType> > dict;
};

class QT_QTSOAP_EXPORT QtSoapStructIterator
{
public:
    QtSoapStructIterator(QtSoapStruct &);
    ~QtSoapStructIterator();

    QtSoapQName key() const;
    QtSoapType *data();
    const QtSoapType *current() const;

    void operator ++();
    bool operator !=(const QtSoapStructIterator &j) const;
    bool operator ==(const QtSoapStructIterator &j) const;

private:
    QList<QtSmartPtr<QtSoapType> >::Iterator it;
    QList<QtSmartPtr<QtSoapType> >::Iterator itEnd;
};

class QT_QTSOAP_EXPORT QtSoapSimpleType : public QtSoapType
{
public:
    QtSoapSimpleType();
    QtSoapSimpleType(const QtSoapQName &name);
    QtSoapSimpleType(const QtSoapQName &name, int n);
    QtSoapSimpleType(const QtSoapQName &name, bool n, int dummy);
    QtSoapSimpleType(const QtSoapQName &name, const QString &n);
    QtSoapSimpleType(const QtSoapSimpleType &copy);
    QtSoapSimpleType &operator =(const QtSoapSimpleType &copy);
    ~QtSoapSimpleType();

    void clear();

    bool parse(QDomNode node);
    bool isValid() const;

    QString toString() const;
    int toInt() const;
    bool toBool() const;
    QVariant value() const;

    QDomElement toDomElement(QDomDocument doc) const;

protected:
    QVariant v;
};

class QT_QTSOAP_EXPORT QtSoapMessage
{
    friend class QtSoapHttpServer;

public:
    QtSoapMessage();
    QtSoapMessage(const QtSoapMessage &copy);
    ~QtSoapMessage();

    QtSoapMessage &operator =(const QtSoapMessage &copy);

    bool setContent(const QByteArray &buffer);
    bool setContent(QDomDocument &d);

    void addBodyItem(QtSoapType *);
    void addHeaderItem(QtSoapType *);

    // Method and response
    const QtSoapType &method() const;
    const QtSoapType &returnValue() const;
    void setMethod(const QtSoapQName &);
    void setMethod(const QString &name, const QString &url = QString::null);
    void addMethodArgument(QtSoapType *);
    void addMethodArgument(const QString &uri, const QString &name, const QString &value);
    void addMethodArgument(const QString &uri, const QString &name, bool value, int dummy);
    void addMethodArgument(const QString &uri, const QString &name, int value);

    // Fault
    enum FaultCode {
	VersionMismatch,
	MustUnderstand,
	Client,
	Server,
	Other
    };

    bool isFault() const;
    FaultCode faultCode() const;
    const QtSoapType &faultString() const;
    const QtSoapType &faultDetail() const;
    void setFaultCode(FaultCode code);
    void setFaultString(const QString &fstring);
    void addFaultDetail(QtSoapType *detail);

    // Generating
    void clear();
    QString toXmlString(int indent = 0) const;

    // Errors
    QString errorString() const;

protected:
    enum MessageType {
	Fault,
	MethodRequest,
	MethodResponse,
	OtherType
    };

    bool isValidSoapMessage(const QDomDocument &candidate);

    QtSoapStruct &body() const;
    QtSoapStruct &header() const;

    void init();

private:
    MessageType type;

    mutable QtSoapStruct envelope;

    QtSoapQName m;
    QtSoapStruct margs;

    QString errorStr;
};

class QT_QTSOAP_EXPORT QtSoapTypeConstructorBase
{
public:
    inline QtSoapTypeConstructorBase()
    {
    }

    virtual inline ~QtSoapTypeConstructorBase()
    {
    }

    virtual QtSoapType *createObject(QDomNode) = 0;

    virtual QString errorString() const = 0;
};

template <class T>
class QT_QTSOAP_EXPORT QtSoapTypeConstructor : public QtSoapTypeConstructorBase
{
public:
    QtSoapTypeConstructor()
    {
    }

    QtSoapType *createObject(QDomNode node)
    {
	T *t = new T();
	if (t->parse(node)) {
	    return t;
	} else {
	    errorStr = t->errorString();
	    delete t;
	    return 0;
	}
    }

    QString errorString() const
    {
	return errorStr;
    }

private:
    mutable QString errorStr;
};

class QT_QTSOAP_EXPORT QtSoapTypeFactory
{
private:
    QtSoapTypeFactory();

public:
    ~QtSoapTypeFactory();

    static QtSoapTypeFactory &instance();

    bool registerHandler(const QString &name, QtSoapTypeConstructorBase *handler);

    QtSmartPtr<QtSoapType> soapType(QDomNode node) const;

    QString errorString() const;

private:
    mutable QString errorStr;
    QHash<QString, QtSoapTypeConstructorBase *> typeHandlers;
    QLinkedList<QtSoapTypeConstructorBase*> deleteList;
};

class QT_QTSOAP_EXPORT QtSoapNamespaces
{
public:
    void registerNamespace(const QString &prefix, const QString &uri);
    QString prefixFor(const QString &ns);

    static QtSoapNamespaces &instance();

private:
    QMap<QString, QString> namespaces;
    QtSoapNamespaces();
};

class QT_QTSOAP_EXPORT QtSoapHttpTransport : public QObject
{
    Q_OBJECT

public:
    QtSoapHttpTransport(QObject *parent = 0);
    ~QtSoapHttpTransport();

    void setHost(const QString &host, bool useSecureHTTP = false, int port = 0);
    void setHost(const QString &host, int port); //obsolete
    void setAction(const QString &action);
    void submitRequest(QtSoapMessage &request, const QString &path);
    const QtSoapMessage &getResponse() const;

    QNetworkAccessManager *networkAccessManager();
    QNetworkReply *networkReply();

Q_SIGNALS:
    void responseReady();
    void responseReady(const QtSoapMessage &response);

private Q_SLOTS:
    void readResponse(QNetworkReply *reply);

private:
    QNetworkAccessManager networkMgr;
    QPointer<QNetworkReply> networkRep;
    QUrl url;
    QString soapAction;
    QtSoapMessage soapResponse;
};

#endif
