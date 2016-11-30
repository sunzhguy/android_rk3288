/****************************************************************************
** Meta object code from reading C++ file 'wgNetwork.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qttest/wgNetwork.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wgNetwork.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_wgNetwork_t {
    QByteArrayData data[4];
    char stringdata0[39];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_wgNetwork_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_wgNetwork_t qt_meta_stringdata_wgNetwork = {
    {
QT_MOC_LITERAL(0, 0, 9), // "wgNetwork"
QT_MOC_LITERAL(1, 10, 16), // "onBtnTestClicked"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 10) // "readOutput"

    },
    "wgNetwork\0onBtnTestClicked\0\0readOutput"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_wgNetwork[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x08 /* Private */,
       3,    0,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void wgNetwork::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        wgNetwork *_t = static_cast<wgNetwork *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onBtnTestClicked(); break;
        case 1: _t->readOutput(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject wgNetwork::staticMetaObject = {
    { &DemoWidget::staticMetaObject, qt_meta_stringdata_wgNetwork.data,
      qt_meta_data_wgNetwork,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *wgNetwork::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *wgNetwork::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_wgNetwork.stringdata0))
        return static_cast<void*>(const_cast< wgNetwork*>(this));
    return DemoWidget::qt_metacast(_clname);
}

int wgNetwork::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DemoWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
