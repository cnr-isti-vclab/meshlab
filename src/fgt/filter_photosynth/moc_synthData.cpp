/****************************************************************************
** Meta object code from reading C++ file 'synthData.h'
**
** Created: Thu Nov 25 13:56:03 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "synthData.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'synthData.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SynthData[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      41,   28,   10,   10, 0x08,
      73,   28,   10,   10, 0x08,
     101,   28,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SynthData[] = {
    "SynthData\0\0readWSresponse()\0httpResponse\0"
    "parseJsonString(QNetworkReply*)\0"
    "loadBinFile(QNetworkReply*)\0"
    "saveImages(QNetworkReply*)\0"
};

const QMetaObject SynthData::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SynthData,
      qt_meta_data_SynthData, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SynthData::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SynthData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SynthData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SynthData))
        return static_cast<void*>(const_cast< SynthData*>(this));
    return QObject::qt_metacast(_clname);
}

int SynthData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: readWSresponse(); break;
        case 1: parseJsonString((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 2: loadBinFile((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 3: saveImages((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
