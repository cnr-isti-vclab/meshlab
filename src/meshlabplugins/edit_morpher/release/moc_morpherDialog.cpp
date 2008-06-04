/****************************************************************************
** Meta object code from reading C++ file 'morpherDialog.h'
**
** Created: Wed May 28 12:58:06 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../morpherDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'morpherDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MorpherDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      24,   15,   14,   14, 0x08,
      47,   15,   14,   14, 0x08,
      68,   14,   14,   14, 0x08,
      85,   14,   14,   14, 0x08,
     100,   15,   14,   14, 0x08,
     119,   14,   14,   14, 0x08,
     146,  144,  138,   14, 0x08,
     181,  157,  138,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MorpherDialog[] = {
    "MorpherDialog\0\0newValue\0startValueChanged(int)\0"
    "endValueChanged(int)\0pickSourceMesh()\0"
    "pickDestMesh()\0sliderChanged(int)\0"
    "recalculateMorph()\0float\0x\0Abs(float)\0"
    "start,end,distanceRatio\0"
    "calcNewPoint(float,float,float)\0"
};

const QMetaObject MorpherDialog::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_MorpherDialog,
      qt_meta_data_MorpherDialog, 0 }
};

const QMetaObject *MorpherDialog::metaObject() const
{
    return &staticMetaObject;
}

void *MorpherDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MorpherDialog))
	return static_cast<void*>(const_cast< MorpherDialog*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int MorpherDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: startValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: endValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: pickSourceMesh(); break;
        case 3: pickDestMesh(); break;
        case 4: sliderChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: recalculateMorph(); break;
        case 6: { float _r = Abs((*reinterpret_cast< float(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = _r; }  break;
        case 7: { float _r = calcNewPoint((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = _r; }  break;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
