/****************************************************************************
** Meta object code from reading C++ file 'morpher.h'
**
** Created: Wed May 28 12:57:58 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../morpher.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'morpher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MorpherPlugin[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_MorpherPlugin[] = {
    "MorpherPlugin\0"
};

const QMetaObject MorpherPlugin::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MorpherPlugin,
      qt_meta_data_MorpherPlugin, 0 }
};

const QMetaObject *MorpherPlugin::metaObject() const
{
    return &staticMetaObject;
}

void *MorpherPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MorpherPlugin))
	return static_cast<void*>(const_cast< MorpherPlugin*>(this));
    if (!strcmp(_clname, "MeshEditInterface"))
	return static_cast< MeshEditInterface*>(const_cast< MorpherPlugin*>(this));
    if (!strcmp(_clname, "vcg.meshlab.MeshEditInterface/1.0"))
	return static_cast< MeshEditInterface*>(const_cast< MorpherPlugin*>(this));
    return QObject::qt_metacast(_clname);
}

int MorpherPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
