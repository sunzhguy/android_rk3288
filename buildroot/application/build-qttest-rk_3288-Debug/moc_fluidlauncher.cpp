/****************************************************************************
** Meta object code from reading C++ file 'fluidlauncher.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qttest/fluidlauncher.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fluidlauncher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FluidLauncher_t {
    QByteArrayData data[11];
    char stringdata0[142];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FluidLauncher_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FluidLauncher_t qt_meta_stringdata_FluidLauncher = {
    {
QT_MOC_LITERAL(0, 0, 13), // "FluidLauncher"
QT_MOC_LITERAL(1, 14, 17), // "launchApplication"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 5), // "index"
QT_MOC_LITERAL(4, 39, 16), // "switchToLauncher"
QT_MOC_LITERAL(5, 56, 17), // "resetInputTimeout"
QT_MOC_LITERAL(6, 74, 13), // "inputTimedout"
QT_MOC_LITERAL(7, 88, 12), // "demoFinished"
QT_MOC_LITERAL(8, 101, 19), // "switchToPictureFlow"
QT_MOC_LITERAL(9, 121, 13), // "onAnimateDone"
QT_MOC_LITERAL(10, 135, 6) // "bStart"

    },
    "FluidLauncher\0launchApplication\0\0index\0"
    "switchToLauncher\0resetInputTimeout\0"
    "inputTimedout\0demoFinished\0"
    "switchToPictureFlow\0onAnimateDone\0"
    "bStart"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FluidLauncher[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x0a /* Public */,
       4,    0,   52,    2, 0x0a /* Public */,
       5,    0,   53,    2, 0x0a /* Public */,
       6,    0,   54,    2, 0x0a /* Public */,
       7,    0,   55,    2, 0x0a /* Public */,
       8,    0,   56,    2, 0x0a /* Public */,
       9,    2,   57,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    3,   10,

       0        // eod
};

void FluidLauncher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FluidLauncher *_t = static_cast<FluidLauncher *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->launchApplication((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->switchToLauncher(); break;
        case 2: _t->resetInputTimeout(); break;
        case 3: _t->inputTimedout(); break;
        case 4: _t->demoFinished(); break;
        case 5: _t->switchToPictureFlow(); break;
        case 6: _t->onAnimateDone((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject FluidLauncher::staticMetaObject = {
    { &QStackedWidget::staticMetaObject, qt_meta_stringdata_FluidLauncher.data,
      qt_meta_data_FluidLauncher,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FluidLauncher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FluidLauncher::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FluidLauncher.stringdata0))
        return static_cast<void*>(const_cast< FluidLauncher*>(this));
    return QStackedWidget::qt_metacast(_clname);
}

int FluidLauncher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStackedWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
